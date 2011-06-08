const char *help = "\
speech_hmm_init (c) Samy Bengio & Co 2001\n\
\n\
This program is used to do speech recognition \n";

#include "LexiconInfo.h"
#include "EMTrainer.h"
#include "HMM.h"
#include "KMeans.h"
#include "MatDataSet.h"
#include "CmdLine.h"
#include "NLLMeasurer.h"
#include "Random.h"
#include "DiskHTKDataSet.h"
#include "HTKDataSet.h"
#include "SpeechHMM.h"
#include "string_utils.h"
#include "FileListCmdOption.h"

using namespace Torch;

#include "Common.h"

int main(int argc, char **argv)
{
  char* phoneme_name;
  char* lex_name;

  int n_gaussians;
  int n_states;
  char* file_states;

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
  bool htk_model;
  bool train_separate;
  bool disk;
  bool is_word;
  bool add_sil_to_targets;
  bool full_connect;

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

  cmd.addText("\nModel Options:");
  cmd.addBCmdOption("-word", &is_word, false, "targets are in word format");
  cmd.addICmdOption("-n_gaussians", &n_gaussians, 10, "number of Gaussians");
  cmd.addICmdOption("-n_states", &n_states, 5, "number of states");
  cmd.addSCmdOption("-file_states", &file_states, "", "file containing n_states per model");
  cmd.addSCmdOption("-silence_word", &silence_name,"sil", "name of silence word");
  cmd.addSCmdOption("-silence_phone", &silence_phone,"h#", "name of silence phone");
  cmd.addSCmdOption("-sp_word", &sp_name,"", "name of short pause word");
  cmd.addSCmdOption("-sp_phone", &sp_phone,"", "name of short pause phoneme");
  cmd.addBCmdOption("-add_sil_to_targets", &add_sil_to_targets, false, "add silence at begining of targets");
  cmd.addBCmdOption("-full_connect", &full_connect, false, "models are full connect");

  cmd.addText("\nLearning Options:");
  cmd.addBCmdOption("-train_separate", &train_separate, false, "first train separate models");
  cmd.addRCmdOption("-threshold", &threshold, 0.001, "variance threshold");
  cmd.addRCmdOption("-prior", &prior, 0.001, "prior on the weights");
  cmd.addICmdOption("-iter", &max_iter, 25, "max number of init iterations");
  cmd.addRCmdOption("-e", &accuracy, 0.00001, "end accuracy");

  cmd.addText("\nMisc Options:");
  cmd.addICmdOption("-seed", &the_seed, -1, "the random seed");
  cmd.addICmdOption("-load", &max_load, -1, "max number of examples to load for train");
  cmd.addSCmdOption("-dir", &dir_name, ".", "directory to save measures");
  cmd.addSCmdOption("-save", &model_file, "", "the model file");
  cmd.addBCmdOption("-htk_model", &htk_model, false, "save in HTK model format");
  cmd.addBCmdOption("-disk", &disk, false, "keep data on disk");

  // Read the command line
  cmd.read(argc, argv);
  cmd.setWorkingDirectory(dir_name);

  DiskXFile::setBigEndianMode();

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
    data = (DataSet*)new(allocator) DiskHTKDataSet(input_file_list.file_names,target_file_list.file_names, input_file_list.n_files, true, max_load, &lex,is_word);
  else
    data = (DataSet*)new(allocator) HTKDataSet(input_file_list.file_names,target_file_list.file_names, input_file_list.n_files, true, max_load, &lex,is_word);

  //=================== Create the HMM... =========================

  real* thresh = (real*)allocator->alloc(data->n_inputs*sizeof(real));
  initializeThreshold(data,thresh,threshold);  

  int n_phonemes = lex.phone_info->n_phones;
  int* states = read_states(file_states,n_phonemes,n_states,
    lex.phone_info->pause_index,allocator);

  // create the GMM
  DiagonalGMM*** gmms = (DiagonalGMM ***)allocator->alloc(sizeof(DiagonalGMM**)*n_phonemes);
  EMTrainer** hmm_trainer = (EMTrainer **)allocator->alloc(sizeof(EMTrainer*)*n_phonemes);
  HMM** hmm = (HMM**)allocator->alloc(sizeof(HMM*)*n_phonemes);
  KMeans*** kmeans = (KMeans***)allocator->alloc(sizeof(KMeans**)*n_phonemes);
  EMTrainer*** kmeans_trainer = (EMTrainer***)allocator->alloc(sizeof(EMTrainer**)*n_phonemes);
  real*** transitions = (real***)allocator->alloc(n_phonemes*sizeof(real**));
  for (int i=0;i<n_phonemes;i++) {
	//printf( lex.phone_info->phone_names[i] );
	//printf( "\n" );
    gmms[i] = (DiagonalGMM**)allocator->alloc(sizeof(DiagonalGMM*)*states[i]);
    kmeans[i] = (KMeans**)allocator->alloc(sizeof(KMeans*)*states[i]);
    kmeans_trainer[i] = (EMTrainer**)allocator->alloc(sizeof(EMTrainer*)*states[i]);
    for (int j=1;j<states[i]-1;j++) {
      // create a KMeans object to initialize the GMM
      kmeans[i][j] = new(allocator)KMeans(data->n_inputs, n_gaussians);
      kmeans[i][j]->setVarThreshold(thresh);
      // the kmeans trainer
      kmeans_trainer[i][j] = new(allocator) EMTrainer(kmeans[i][j]);
      kmeans_trainer[i][j]->setROption("end accuracy", accuracy);
      kmeans_trainer[i][j]->setIOption("max iter", max_iter);

      gmms[i][j] = new(allocator)DiagonalGMM(data->n_inputs,n_gaussians,kmeans_trainer[i][j]);

      // set the training options
      gmms[i][j]->setVarThreshold(thresh);
      gmms[i][j]->setROption("prior weights",prior);
    }

    gmms[i][0] = NULL;
    gmms[i][states[i]-1] = NULL;

    // the transition table probability: left-right topology or full connect
    transitions[i] = (real**)allocator->alloc(states[i]*sizeof(real*));
    for (int j=0;j<states[i];j++) {
      transitions[i][j] = (real*)allocator->alloc(states[i]*sizeof(real));
    }
    setTransitions(transitions[i], states[i], i, lex.phone_info->pause_index, lex.phone_info->sil_index, full_connect);

    hmm[i] = new (allocator)HMM(states[i],(Distribution**)gmms[i],transitions[i]);
    hmm[i]->setROption("prior transitions",prior);
    hmm[i]->setBOption("linear segmentation", true); // can be changed ?
    hmm_trainer[i] = new(allocator)EMTrainer(hmm[i]);
    hmm_trainer[i]->setIOption("max iter", max_iter);
    hmm_trainer[i]->setROption("end accuracy", accuracy);
  }

  EMTrainer** hmm_trainer_ = NULL;
  if (train_separate)
    hmm_trainer_ = hmm_trainer;

  SpeechHMM shmm(n_phonemes,hmm,&lex,hmm_trainer_);
  shmm.setBOption("targets are phonemes",!is_word);
  shmm.setDataSet(data);

  if(strcmp(model_file, "")) {
    if (htk_model) {
      save_htk_model(cmd.getPath(model_file),&shmm,lex.phone_info->phone_names);
    } else {
      DiskXFile model_(cmd.getPath(model_file), "w");
      cmd.saveXFile(&model_);
      model_.taggedWrite(&n_gaussians, sizeof(int), 1, "n_gaussians");
      model_.taggedWrite(&n_phonemes, sizeof(int), 1, "n_phonemes");
      model_.taggedWrite(states, sizeof(int), n_phonemes, "n_states");
      model_.taggedWrite(&data->n_inputs, sizeof(int), 1, "n_inputs");
      shmm.saveXFile(&model_);
    }
  }
  delete allocator;
  
  //printf( SUCCESS );
  
  return(0);
}

