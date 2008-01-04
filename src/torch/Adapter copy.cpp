const char *help = "\
HMM Adaptation (c) Laurent Pugin 2007\n";


#include "LexiconInfo.h"
#include "EMTrainer.h"
#include "HMM.h"
#include "DiagonalGMM.h"
#include "MAPDiagonalGMM.h"
#include "MatDataSet.h"
#include "CmdLine.h"
#include "NLLMeasurer.h"
#include "Random.h"
#include "DiskHTKDataSet.h"
#include "HTKDataSet.h"
#include "FileListCmdOption.h"
#include "string_utils.h"

#include "AruspixHMM.h"

using namespace Torch;

#include "Common.h"

void merge_lexicons( LexiconInfo *lex1, LexiconInfo *lex2, char *fname )
{
	char line[1000];
	FILE* f=fopen(fname,"w");
	if (!f)
		error("file %s cannot be opened",fname);
		
	// copy all entries in the first lex1
    for ( int i=0 ; i<lex1->n_entries ; i++ )
    {
		fprintf(f,"%s ", lex1->vocabulary->getWord( lex1->entries[i].vocab_index ) ) ;
			for ( int j=0 ; j<lex1->entries[i].n_phones ; j++ )
        		fprintf(f," %s" , lex1->phone_info->getPhone( lex1->entries[i].phones[j] )) ;
			fprintf(f,"\n") ;
    }

	// go through all entries in the second lex2 and check if they exist in the first lex1
    for ( int i=0 ; i<lex2->n_entries ; i++ )
    {
		bool found = false;
		char *word = lex2->vocabulary->getWord( lex2->entries[i].vocab_index );
		int word_index1 = lex1->vocabulary->getIndex( word );
		if  ( word_index1 != -1 ) 
		// check the prounciation only if the word exists
		{	
			for ( int j=0; (j < lex1->vocab_to_lex_map[word_index1].n_pronuns) && !found ; j++ )
			// go through all pronounciation in the first lex1
			{
				int p = lex1->vocab_to_lex_map[word_index1].pronuns[j];
				if ( lex2->entries[i].n_phones != lex1->entries[p].n_phones )
					continue; // not the same length, false anyway
				else
				{
					for ( int k=0 ; (k < lex2->entries[i].n_phones) && (k < lex1->entries[p].n_phones); k++ )
					{
						if ( strcmp( lex2->phone_info->getPhone( lex2->entries[i].phones[k] ) , lex1->phone_info->getPhone( lex1->entries[p].phones[k] ) ) )
							break;
						else if ( k == lex2->entries[i].n_phones - 1 ) // last phone
							found = true;
					}
				}
			}
		}
		if ( !found )
		{
			sprintf(line,"%s", word ) ;
			for ( int j=0 ; j<lex2->entries[i].n_phones ; j++ )
			{
				strcat(line," ");
        		strcat(line, lex2->phone_info->getPhone( lex2->entries[i].phones[j] ) ) ;
			}
			fprintf(f, "%s\n", line );
#ifdef __AXDEBUG__
			printf( "New symbol: %s\n", line );
#endif
		}
    }

	//fprintf(f,"\n");
	fclose(f);
}


void newMAPSpeechHMM( SpeechHMM *shmm, real* thresh, real prior, Allocator* allocator)
{
	MAPDiagonalGMM*** gmms = (MAPDiagonalGMM ***)allocator->alloc(sizeof(MAPDiagonalGMM**)*shmm->n_models);
	DiagonalGMM** priors = NULL;

	for (int i=0;i<shmm->n_models;i++) 
	{
		int states = shmm->models[i]->n_states;
		priors = (DiagonalGMM**)shmm->models[i]->states;
		gmms[i] = (MAPDiagonalGMM**)allocator->alloc(sizeof(MAPDiagonalGMM*)*states);
		for (int j=1;j<states-1;j++) 
		{
			MAPDiagonalGMM* gmm = new(allocator)MAPDiagonalGMM(priors[j]);
			// set the training options
			if (thresh)
				gmm->setVarThreshold(thresh);
			if (prior>0)
				gmm->setROption("prior weights",prior);
		
			//gmm->setROption("weight on prior",map_factor);
			//gmm->setBOption("learn means",!no_learn_means);
			//gmm->setBOption("learn variances",learn_var);
			//gmm->setBOption("learn weights",learn_weights);
			gmm->setROption("weight on prior", 0.5 );
			gmm->setBOption("learn means",true);
			//gmm->setBOption("learn variances",true);
			//gmm->setBOption("learn weights",true);
			gmms[i][j] = gmm;
		}

		gmms[i][0] = NULL;
		gmms[i][states-1] = NULL;

		shmm->models[i]->states = (Distribution**)gmms[i];
	}
}


int main(int argc, char **argv)
{
	int n_gaussians;
	int n_states;
	bool is_word;
	char* file_states;

	char* phoneme_name1=NULL;
	char* phoneme_name2=NULL;
	char* phoneme_name3=NULL;
	char* silence_phone;
	
	char* lex_name1=NULL;
	char* lex_name2=NULL;
	char* lex_name3=NULL;
	char *sent_start_word=NULL;
	char *sent_end_word=NULL;
	char *sil_word=NULL;
	
	char *model_file_in;
	char *model_file_out;
	char *dir_name;

	real accuracy;
	real threshold;
	int max_iter;
	real prior;

	int max_load;

	bool add_sil_to_targets;
	bool viterbi;
	bool train_separate;
	bool disk;
	
	bool no_learn_means;
	bool learn_var;
	bool learn_weights;
	real map_factor;

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

	// Adapt mode
	cmd.addText("\nArguments:");
	cmd.addSCmdArg("phoneme_name1", &phoneme_name1, "the list of the model phonemes file");
	cmd.addSCmdArg("lex_name1", &lex_name1, "the model lexicon file");
	cmd.addSCmdArg("phoneme_name2", &phoneme_name2, "the list of the data phonemes file");
	cmd.addSCmdArg("lex_name2", &lex_name2, "the data lexicon file");
	cmd.addSCmdArg("phoneme_name3", &phoneme_name3, "the list of the output phonemes file");
	cmd.addSCmdArg("lex_name3", &lex_name3, "the output lexicon file");
	cmd.addCmdOption(&input_file_list);
	cmd.addCmdOption(&target_file_list);
	cmd.addSCmdArg("input model file", &model_file_in, "the input model file to be adapted");
	cmd.addSCmdArg("output model file", &model_file_out, "the adapted output model file");
	cmd.addBCmdOption("-word", &is_word, false, "targets are in word format");
	cmd.addICmdOption("-n_gaussians", &n_gaussians, 10, "number of Gaussians");
	cmd.addICmdOption("-n_states", &n_states, 5, "number of states");
	cmd.addSCmdOption("-file_states", &file_states, "", "file containing n_states per model");
	
    // Lexicon Parameters
    cmd.addText("\nLexicon Options:") ;
    cmd.addSCmdOption( "-sent_start_word" , &sent_start_word , SP_START , "word that will start every sentence" ) ;
    cmd.addSCmdOption( "-sent_end_word" , &sent_end_word , SP_END , "word that will end every sentence" ) ;
    cmd.addSCmdOption( "-sil_word" , &sil_word , SP_WORD ,"the silence dictionary word" ) ;

	cmd.addText("\nModel Options:");
	cmd.addSCmdOption("-silence_phone", &silence_phone,"", "name of silence phone");
	cmd.addBCmdOption("-add_sil_to_targets", &add_sil_to_targets, false, "add silence at begining of targets");

	cmd.addText("\nLearning Options:");
	cmd.addBCmdOption("-train_separate", &train_separate, false, "first train separate models");
	cmd.addBCmdOption("-viterbi", &viterbi, false, "viterbi training (instead of EM)");
	cmd.addRCmdOption("-threshold", &threshold, 0.001, "variance threshold");
	cmd.addRCmdOption("-prior", &prior, 0.001, "prior on the weights");
	cmd.addICmdOption("-iter", &max_iter, 25, "max number of iterations of HMM");
	cmd.addRCmdOption("-e", &accuracy, 0.00001, "end accuracy");
	
	cmd.addText("\nMAP Options:");
	cmd.addRCmdOption("-map", &map_factor, 0.5, "adaptation factor [0-1]");
	cmd.addBCmdOption("-no_means", &no_learn_means, false, "no enroll means");
	cmd.addBCmdOption("-learn_var", &learn_var, false, "enroll var");
	cmd.addBCmdOption("-learn_weights", &learn_weights, false, "enroll weights");

	cmd.addText("\nMisc Options:");
	cmd.addICmdOption("-load", &max_load, -1, "max number of examples to load for train");
	cmd.addBCmdOption("-disk", &disk, false, "keep data on disk");
	cmd.addSCmdOption("-dir", &dir_name, ".", "directory to save measures");

	// Read the command line
	cmd.read(argc, argv);
	cmd.setWorkingDirectory(dir_name);

	DiskXFile::setBigEndianMode();

	//==================================================================== 
	//=================== Data preparation ===============================
	//====================================================================
	
	PhoneInfo phones1( phoneme_name1 ) ;
	PhoneInfo phones2( phoneme_name2 ) ;
	PhoneInfo phones3( phoneme_name1 ) ; // the new list of phones is phone1 + those missing in phones2...
	int* missing = (int*)allocator->alloc(phones2.n_phones*sizeof(int));
	int n_missing = 0;
	memset( missing, 0, phones2.n_phones*sizeof(int) );
	
	// get missing phones in the model to be adapted
	for ( int i=0; i < phones2.n_phones; i++ )
		if ( phones1.getIndex( phones2.getPhone(i) ) == -1 )
		{
			missing[n_missing]=i;
			n_missing++;
			phones3.addPhone( phones2.getPhone(i) );
#ifdef __AXDEBUG__
			printf("New internal symbol %s\n", phones2.getPhone(i) ) ;
#endif
			
		}
	save_phones( phoneme_name3, &phones3 );
	
	LexiconInfo lex1(phoneme_name1,silence_phone, NULL,lex_name1, add_sil_to_targets ? sent_start_word : NULL, NULL, NULL);
	LexiconInfo lex2(phoneme_name2,silence_phone, NULL,lex_name2, add_sil_to_targets ? sent_start_word : NULL, NULL, NULL);
	merge_lexicons( &lex1, &lex2, lex_name3 );
	

	//==================================================================== 
	//=================== Training Mode  =================================
	//==================================================================== 

	Random::seed();

	// read lexicon
	if (strlen(silence_phone) == 0)
		silence_phone = NULL;
	LexiconInfo lex3(phoneme_name3,silence_phone, NULL,lex_name3, add_sil_to_targets ? sent_start_word : NULL, NULL, NULL);

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
		data = (DataSet*)new(allocator) DiskHTKDataSet(input_file_list.file_names,target_file_list.file_names, input_file_list.n_files, true, max_load, &lex2, is_word);
	else
		data = (DataSet*)new(allocator) HTKDataSet(input_file_list.file_names,target_file_list.file_names, input_file_list.n_files, true, max_load, &lex2, is_word);

	//=================== Create the HMM... =========================

	real* thresh = (real*)allocator->alloc(data->n_inputs*sizeof(real));
	initializeThreshold(data,thresh,threshold);
	//initializeThreshold(data,thresh,1.0);
	
	//=================== Create the HMM of the new symbols =========================

	int n_phonemes = lex2.phone_info->n_phones;
	int* states = read_states(file_states,n_phonemes,n_states,
		lex2.phone_info->pause_index,allocator);

	

	/*

	// create the GMM
	DiagonalGMM*** gmms = (DiagonalGMM ***)allocator->alloc(sizeof(DiagonalGMM**)*n_phonemes);
	EMTrainer** hmm_trainer = (EMTrainer **)allocator->alloc(sizeof(EMTrainer*)*n_phonemes);
	HMM** hmm = (HMM**)allocator->alloc(sizeof(HMM*)*n_phonemes);
	KMeans*** kmeans = (KMeans***)allocator->alloc(sizeof(KMeans**)*n_phonemes);
	EMTrainer*** kmeans_trainer = (EMTrainer***)allocator->alloc(sizeof(EMTrainer**)*n_phonemes);
	real*** transitions = (real***)allocator->alloc(n_phonemes*sizeof(real**));
	for (int i=0;i<n_phonemes;i++) 
	{
		//printf( lex.phone_info->phone_names[i] );
		//printf( "\n" );
		gmms[i] = (DiagonalGMM**)allocator->alloc(sizeof(DiagonalGMM*)*states[i]);
		kmeans[i] = (KMeans**)allocator->alloc(sizeof(KMeans*)*states[i]);
		kmeans_trainer[i] = (EMTrainer**)allocator->alloc(sizeof(EMTrainer*)*states[i]);
		for (int j=1;j<states[i]-1;j++) 
		{
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
		for (int j=0;j<states[i];j++) 
		{
			transitions[i][j] = (real*)allocator->alloc(states[i]*sizeof(real));
		}
		setTransitions(transitions[i], states[i], i, lex2.phone_info->pause_index, lex2.phone_info->sil_index, false );

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

	SpeechHMM mshmm(n_phonemes,hmm,&lex2,hmm_trainer_);*/
	AruspixHMM *mshmm = newAruspixHMMWithTrainer( states, n_gaussians, n_phonemes, data->n_inputs, &lex2, data, thresh, prior, max_iter, accuracy, train_separate, allocator );
	mshmm->setBOption("targets are phonemes",!is_word);
	mshmm->initSubset(data, missing, n_missing );
	//mshmm->setDataSet( data );
	
	/*
	
  //=================== Measurers and Trainer  ===============================

  // Measurers on the training dataset
	MeasurerList measurers1;
	char* n_nll1 = strConcat(2,"hmm_train_val_",viterbi ? "viterbi" : "em");
	allocator->retain(n_nll1);
	NLLMeasurer nll_meas1(mshmm->log_probabilities, data, cmd.getXFile(n_nll1));
	measurers1.addNode(&nll_meas1);

	// The Gradient Machine Trainer
	EMTrainer trainer1(mshmm);
	trainer1.setIOption("max iter", max_iter);
	trainer1.setROption("end accuracy", accuracy);
	if (viterbi) 
		trainer1.setBOption("viterbi", true);

	//=================== Let's go... ===============================


	trainer1.train(data, &measurers1);
	*/
	
	FILE* fin=fopen(model_file_in,"r");
	if (!fin)
		error("file %s cannot be opened",model_file_in);	
		
	FILE* fout=fopen(model_file_out,"w");
	if (!fout)
		error("file %s cannot be opened",model_file_out);	
	
	int c;
	while((c=getc(fin))!=EOF)
		putc(c,fout);
  
	fclose(fin);
	
    for (int i=0;i<n_missing;i++) 
	{
		int m = missing[i];
		write_htk_model( fout, mshmm->models[m], lex2.phone_info->phone_names[m] );
	}
	fclose(fout);
	
	//=================== MAP  ===============================
	
	DataSet* data3;
	if (disk)
		data3 = (DataSet*)new(allocator) DiskHTKDataSet(input_file_list.file_names,target_file_list.file_names, input_file_list.n_files, true, max_load, &lex3, is_word );
	else
		data3 = (DataSet*)new(allocator) HTKDataSet(input_file_list.file_names,target_file_list.file_names, input_file_list.n_files, true, max_load, &lex3, is_word );
		
	thresh = (real*)allocator->alloc(data3->n_inputs*sizeof(real));
	initializeThreshold(data3,thresh,threshold); 
	
	SpeechHMM* shmm = NULL;
	shmm = newSpeechHMMFromHTK(cmd.getPath(model_file_out),&lex3,thresh,prior,allocator);
  
	//=================== Measurers and Trainer  ===============================
  
	newMAPSpeechHMM( shmm, thresh, prior, allocator);

  // Measurers on the training dataset
  MeasurerList measurers;
  char* n_nll = strConcat(2,"hmm_train_val_",viterbi ? "viterbi" : "em");
  allocator->retain(n_nll);
  NLLMeasurer nll_meas(shmm->log_probabilities, data3, cmd.getXFile(n_nll));
  measurers.addNode(&nll_meas);

  // The Gradient Machine Trainer
  EMTrainer trainer(shmm);
  trainer.setIOption("max iter", max_iter);
  trainer.setROption("end accuracy", accuracy);
  if (viterbi) 
    trainer.setBOption("viterbi", true);

  //=================== Let's go... ===============================

  trainer.train(data3, &measurers);

  if(strcmp(model_file_out, "")) {
      save_htk_model(cmd.getPath(model_file_out),shmm,lex3.phone_info->phone_names);
  }

  delete allocator;
  return(0);
}
