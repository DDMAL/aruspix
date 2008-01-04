const char *help = "\
HMM Adaptation (c) Laurent Pugin 2007\n";

#include "CmdLine.h"
#include "FileListCmdOption.h"
#include "time.h"

#ifdef WIN32
	#include <io.h>
#else
	#include <unistd.h>
#endif

#include "AruspixHMM.h"

using namespace Torch;

//#include "Common.h"
/*
void newMAPSpeechHMM( SpeechHMM *shmm, real* thresh, real prior, Allocator* allocator, int max )
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
			if ( i < max )
			{
				//gmm->setBOption("learn variances",true);
				//gmm->setBOption("learn weights",true);			
			}
			//gmm->setBOption("learn variances",true);
			//gmm->setBOption("learn weights",true);
			gmms[i][j] = gmm;
		}

		gmms[i][0] = NULL;
		gmms[i][states-1] = NULL;

		shmm->models[i]->states = (Distribution**)gmms[i];
		
		}
}
*/

int main(int argc, char **argv)
{
	int n_gaussians;
	int n_states;
	bool is_symbol;
	char* file_states;

	char* model_name1=NULL;
	char* model_name2=NULL;
	char* model_name3=NULL;
	char* spacing_model;
	
	char* lex_name1=NULL;
	char* lex_name2=NULL;
	char* lex_name3=NULL;
	char *sent_start_symbol=NULL;
	char *sent_end_symbol=NULL;
	char *spacing_symbol=NULL;
	
	char *model_file_in;
	char *model_file_out;
	char *dir_name;
	char *log_fname;

	real accuracy;
	real threshold;
	int max_iter;
	real prior;

	int max_load;

	bool add_space_to_targets;
	bool viterbi;
	bool train_separate;
	bool disk;
	
	bool no_learn_means;
	bool learn_var;
	bool learn_weights;
	real map_factor;
	bool adapt_separate;
	bool adapt_separate_set_data;

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
	cmd.setBOption("write log", false );

	// Put the help line at the beginning
	cmd.info(help);

	// Adapt mode
	cmd.addText("\nArguments:");
	cmd.addSCmdArg("model_name1", &model_name1, "the list of the model models file");
	cmd.addSCmdArg("lex_name1", &lex_name1, "the model lexicon file");
	cmd.addSCmdArg("model_name2", &model_name2, "the list of the data models file");
	cmd.addSCmdArg("lex_name2", &lex_name2, "the data lexicon file");
	cmd.addSCmdArg("model_name3", &model_name3, "the list of the output models file");
	cmd.addSCmdArg("lex_name3", &lex_name3, "the output lexicon file");
	cmd.addCmdOption(&input_file_list);
	cmd.addCmdOption(&target_file_list);
	cmd.addSCmdArg("input model file", &model_file_in, "the input model file to be adapted");
	cmd.addSCmdArg("output model file", &model_file_out, "the adapted output model file");
	
    // Data Initialization
    cmd.addText("\nInitialization Options:") ;	
	cmd.addICmdOption("-n_gaussians", &n_gaussians, 10, "number of Gaussians");
	cmd.addICmdOption("-n_states", &n_states, 5, "number of states");
	cmd.addSCmdOption("-file_states", &file_states, "", "file containing n_states per model");
	cmd.addBCmdOption("-train_separate", &train_separate, false, "first train separate models");

    // Data and Model Parameters
    cmd.addText("\nData and Model Options:") ;
	cmd.addBCmdOption("-symbol", &is_symbol, false, "targets are in symbol format");
	cmd.addBCmdOption("-disk", &disk, false, "keep data on disk");
	cmd.addICmdOption("-load", &max_load, -1, "max number of examples to load for train");
	cmd.addSCmdOption("-spacing_model", &spacing_model,"", "name of silence phone");
	cmd.addBCmdOption("-add_sil_to_targets", &add_space_to_targets, false, "add silence at begining of targets");	
    cmd.addSCmdOption("-sent_start_symbol" , &sent_start_symbol , SP_START , "symbol that will start every sentence" ) ;
    cmd.addSCmdOption("-sent_end_symbol" , &sent_end_symbol , SP_END , "symbol that will end every sentence" ) ;
    cmd.addSCmdOption("-spacing_symbol" , &spacing_symbol , SP_WORD ,"the silence dictionary symbol" ) ;

	// Learning Options
	cmd.addText("\nLearning Options:");
	cmd.addBCmdOption("-viterbi", &viterbi, false, "viterbi training (instead of EM)");
	cmd.addRCmdOption("-threshold", &threshold, 0.001, "variance threshold");
	cmd.addRCmdOption("-prior", &prior, 0.001, "prior on the weights");
	cmd.addICmdOption("-iter", &max_iter, 25, "max number of iterations of HMM");
	cmd.addRCmdOption("-e", &accuracy, 0.00001, "end accuracy");
	cmd.addBCmdOption("-adapt_separate", &adapt_separate, false, "adapt models separately");
	cmd.addBCmdOption("-adapt_separate_set_data", &adapt_separate_set_data, false, "set data to non represented models when adapting  models separately");
	
	// MAP Options
	cmd.addText("\nMAP Options:");
	cmd.addRCmdOption("-map", &map_factor, 0.5, "adaptation factor [0-1]");
	cmd.addBCmdOption("-no_means", &no_learn_means, false, "no enroll means");
	cmd.addBCmdOption("-learn_var", &learn_var, false, "enroll var");
	cmd.addBCmdOption("-learn_weights", &learn_weights, false, "enroll weights");

	// Misc Options
	cmd.addText("\nMisc Options:");
	cmd.addSCmdOption("-dir", &dir_name, ".", "directory to save measures");
	cmd.addSCmdOption( "-log_fname" , &log_fname , "", "the log output file, standard output if none" ) ;

	// Read the command line
	cmd.read(argc, argv);
	cmd.setWorkingDirectory(dir_name);

	DiskXFile::setBigEndianMode();
	
	//==================================================================== 
	//=================== Log file  ======================================
	//====================================================================
	
	int saved_stdout = 0;
	FILE *out_fd = NULL;
	if ( strlen( log_fname ) )
	{
		// redirect stdout if output file is supplied
		saved_stdout = dup( fileno(stdout) );
		out_fd = freopen( log_fname, "a", stdout );
		if ( out_fd == NULL )
		{
			dup2(saved_stdout, fileno(stdout) );
			close(saved_stdout);
			warning("Opening log file failed") ;
		}
	}

	//==================================================================== 
	//=================== Data preparation ===============================
	//====================================================================
	
	Random::seed();
	
    clock_t start_time , end_time ;
    real total_time = 0.0 ;
    start_time = clock() ;
	
	AruspixHMM ahmm;
	
	//==================================================================== 
	//=================== Set Options ====================================
	//====================================================================
	
	// initialization
	ahmm.setIOption("number of gaussians", n_gaussians );
	ahmm.setIOption("number of states", n_states );
	ahmm.setBOption("train separate", train_separate );
	
	// data
	ahmm.setBOption("is symbol", is_symbol );
	ahmm.setBOption("add spacing", add_space_to_targets );
	ahmm.setIOption("max load", max_load );
	ahmm.setBOption("disk", disk );

	// training
	ahmm.setROption("accuracy", accuracy );
	ahmm.setROption("threshold", threshold );
	ahmm.setIOption("iterations", max_iter );
	ahmm.setROption("prior", prior );
	ahmm.setBOption("viterbi", viterbi );
	
	// map
	ahmm.setROption("map factor", map_factor );
	ahmm.setBOption("no means", no_learn_means );
	ahmm.setBOption("learn var", learn_var );
	ahmm.setBOption("learn weights", learn_weights );
	ahmm.setBOption("adapt separate", adapt_separate );
	ahmm.setBOption("adapt separate set data", adapt_separate_set_data );
	
	ahmm.setLexicon( model_name1, spacing_model, lex_name1, add_space_to_targets ? sent_start_symbol : NULL );

	ahmm.mapSetDataLexicon( model_name2, lex_name2, model_name3, lex_name3 );

	//==================================================================== 
	//=================== Create the DataSet ... =========================
	//==================================================================== 
	
	ahmm.setData( input_file_list.file_names, input_file_list.n_files, target_file_list.file_names, target_file_list.n_files );
	
	ahmm.mapInit( file_states );
	
	char* n_nll = strConcat(2,"hmm_train_val_",viterbi ? "viterbi" : "em");
	allocator->retain(n_nll);
	ahmm.mapAdapt( model_file_in, model_file_out, cmd.getXFile(n_nll) );
	
	end_time = clock() ;
    total_time = (real)(end_time-start_time) / CLOCKS_PER_SEC ;
	
    printf("\nTotal time spent adapting = %.2f secs\n", total_time) ;
	
	if ( out_fd )
		fclose( out_fd );

	delete allocator;
	return(0);
}
