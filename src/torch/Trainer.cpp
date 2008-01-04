const char *help = "\
speech_hmm_train (c) Samy Bengio & Co 2001\n\
\n\
This program is used to do speech recognition \n";

#include "LexiconInfo.h"
#include "EMTrainer.h"
#include "HMM.h"
#include "DiagonalGMM.h"
#include "MatDataSet.h"
#include "CmdLine.h"
#include "NLLMeasurer.h"
#include "Random.h"
#include "DiskHTKDataSet.h"
#include "HTKDataSet.h"
#include "SpeechHMM.h"
#include "FileListCmdOption.h"
#include "string_utils.h"

using namespace Torch;

#include "Common.h"

int main(int argc, char **argv)
{
  char* phoneme_name;
  char* lex_name;


  char* silence_name;
  char* silence_phone;
  char* sp_name;
  char* sp_phone;

  real accuracy;
  real threshold;
  int max_iter;
  real prior;

  int max_load;
  int the_seed;

  char *dir_name;
  char *model_file;
  char *init_model;
  bool add_sil_to_targets;
  bool htk_model;
  bool viterbi;
  bool disk;

  Allocator *allocator = new Allocator;

	FileListCmdOption input_file_list("file name", "the list of inputs files or one data file");
	input_file_list.isArgument(true);
  
	FileListCmdOption target_file_list("file name", "the list of target files or one target file");
	target_file_list.isArgument(true);

  //=============================================================== 
  //=================== The command-line ==========================
  //=============================================================== 

  // Construct the command line
  CmdLine cmd;

  // Put the help line at the beginning
  cmd.info(help);

  // Train mode
  cmd.addText("\nArguments:");
  cmd.addSCmdArg("phoneme_name", &phoneme_name, "the list of phonemes file");
  cmd.addSCmdArg("lex_name", &lex_name, "the lexicon file");
	cmd.addCmdOption(&input_file_list);
	cmd.addCmdOption(&target_file_list);
  cmd.addSCmdArg("init model file", &init_model, "the initialized model file");

  cmd.addText("\nModel Options:");
  cmd.addSCmdOption("-silence_word", &silence_name,"sil", "name of silence word");
  cmd.addSCmdOption("-silence_phone", &silence_phone,"h#", "name of silence phone");
  cmd.addSCmdOption("-sp_word", &sp_name,"", "name of short pause word");
  cmd.addSCmdOption("-sp_phone", &sp_phone,"", "name of short pause phoneme");
  cmd.addBCmdOption("-add_sil_to_targets", &add_sil_to_targets, false, "add silence at begining of targets");

  cmd.addText("\nLearning Options:");
  cmd.addBCmdOption("-viterbi", &viterbi, false, "viterbi training (instead of EM)");
  cmd.addRCmdOption("-threshold", &threshold, 0.001, "variance threshold");
  cmd.addRCmdOption("-prior", &prior, 0.001, "prior on the weights");
  cmd.addICmdOption("-iter", &max_iter, 25, "max number of iterations of HMM");
  cmd.addRCmdOption("-e", &accuracy, 0.00001, "end accuracy");

  cmd.addText("\nMisc Options:");
  cmd.addICmdOption("-seed", &the_seed, -1, "the random seed");
  cmd.addICmdOption("-load", &max_load, -1, "max number of examples to load for train");
  cmd.addSCmdOption("-dir", &dir_name, ".", "directory to save measures");
  cmd.addSCmdOption("-save", &model_file, "", "the model file");
  cmd.addBCmdOption("-htk_model", &htk_model, false, "load and save in HTK model format");
  cmd.addBCmdOption("-disk", &disk, false, "keep data on disk");

  // Read the command line
  cmd.read(argc, argv);
  cmd.setWorkingDirectory(dir_name);

  DiskXFile::setBigEndianMode();
  
  clock_t start_time , end_time ;
  real total_time = 0.0 ;
  start_time = clock() ;

  //==================================================================== 
  //=================== Training Mode  =================================
  //==================================================================== 

  if(the_seed == -1)
    Random::seed();
  else
    Random::manualSeed((long)the_seed);

  // read lexicon
  if (strlen(sp_phone) == 0)
    sp_phone = NULL;
  if (strlen(silence_phone) == 0)
    silence_phone = NULL;
  LexiconInfo lex(phoneme_name,silence_phone,sp_phone,lex_name,add_sil_to_targets ? silence_name : NULL,NULL);

  //==================================================================== 
  //=================== Create the DataSet ... =========================
  //==================================================================== 

  // some basic tests on the files
  if (input_file_list.n_files != target_file_list.n_files) {
    error("the input and target files should have the same number of files (%d != %d)\n",input_file_list.n_files,target_file_list.n_files);
  }
  for (int i=0;i<input_file_list.n_files;i++) {
    char* si = strRemoveSuffix(input_file_list.file_names[i]);
    char* st = strRemoveSuffix(target_file_list.file_names[i]);
    if (strcmp(strBaseName(si),strBaseName(st)))
      warning("input file (%s) do not correspond to target file (%s)",input_file_list.file_names[i],target_file_list.file_names[i]);
    free(si);
    free(st);
  }


  DataSet* data;
  if (disk)
    data = (DataSet*)new(allocator) DiskHTKDataSet(input_file_list.file_names,target_file_list.file_names, input_file_list.n_files, true, max_load, &lex);
  else
    data = (DataSet*)new(allocator) HTKDataSet(input_file_list.file_names,target_file_list.file_names, input_file_list.n_files, true, max_load, &lex);

  //=================== Create the HMM... =========================

  real* thresh = (real*)allocator->alloc(data->n_inputs*sizeof(real));
  initializeThreshold(data,thresh,threshold);  

  SpeechHMM* shmm = NULL;
  if (htk_model)
    shmm = newSpeechHMMFromHTK(cmd.getPath(init_model),&lex,thresh,prior,allocator);
  else
    shmm = newSpeechHMMFromTorch(cmd.getPath(init_model),&lex,thresh,prior,allocator,&cmd);
  
  //=================== Measurers and Trainer  ===============================

  // Measurers on the training dataset
  MeasurerList measurers;
  char* n_nll = strConcat(2,"hmm_train_val_",viterbi ? "viterbi" : "em");
  allocator->retain(n_nll);
  NLLMeasurer nll_meas(shmm->log_probabilities, data, cmd.getXFile(n_nll));
  measurers.addNode(&nll_meas);

  // The Gradient Machine Trainer
  EMTrainer trainer(shmm);
  trainer.setIOption("max iter", max_iter);
  trainer.setROption("end accuracy", accuracy);
  if (viterbi) 
    trainer.setBOption("viterbi", true);

  //=================== Let's go... ===============================

  trainer.train(data, &measurers);

  if(strcmp(model_file, "")) {
    if (htk_model) {
      save_htk_model(cmd.getPath(model_file),shmm,lex.phone_info->phone_names);
    } else {
      DiskXFile model_(cmd.getPath(model_file), "w");
      cmd.saveXFile(&model_);
      int n_models = shmm->n_models;
      int n_gaussians = ((DiagonalGMM*)((HMM*)shmm->models[n_models-2])->states[1])->n_gaussians;
      int* states = (int*)allocator->alloc(sizeof(int)*n_models);
      for (int i=0;i<n_models;i++)
        states[i] = shmm->models[i]->n_states;
      model_.taggedWrite(&n_gaussians, sizeof(int), 1, "n_gaussians");
      model_.taggedWrite(&n_models, sizeof(int), 1, "n_phonemes");
      model_.taggedWrite(states,sizeof(int), n_models, "n_states");
      model_.taggedWrite(&data->n_inputs, sizeof(int), 1, "n_inputs");
      shmm->saveXFile(&model_);
    }
  }
  
  end_time = clock() ;
  total_time = (real)(end_time-start_time) / CLOCKS_PER_SEC ;
	
  printf("\nTotal time spent training = %.2f secs\n", total_time) ;

  delete allocator;
  return(0);
}
