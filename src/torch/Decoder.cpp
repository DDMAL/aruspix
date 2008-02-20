// Copyright (C) 2005 Laurent Pugin (laurent.pugin@lettres.unige.ch)
//
// All rights reserved.

#ifdef WIN32
	#include <io.h>
#else
	#include <unistd.h>
#endif

#include "LinearLexicon.h"
#include "LanguageModel.h"
#include "DecoderBatchTest.h"
#include "BeamSearchDecoder.h"
#include "Vocabulary.h"
#include "DiskXFile.h"
#include "CmdLine.h"
#include "PhoneInfo.h"
#include "PhoneModels.h"
#include "LexiconInfo.h"

#include "../ml/ml.h"
#include "../ml/mldecoderpage.h"

using namespace Torch ;

// Acoustic Modelling Parameters
char *am_models_fname=NULL ;
char *am_sil_phone=NULL ;
char *am_pause_phone=NULL ;
real am_phone_del_pen=1.0 ;
bool am_apply_pause_del_pen=false ;

// Lexicon Parameters
char *lex_dict_fname=NULL ;
char *lex_sent_start_word=NULL ;
char *lex_sent_end_word=NULL;
char *lex_sil_word=NULL ;

// Music Model Parameters
int lm_ngram_order=0 ;
char *lm_fname=NULL ;
real lm_scaling_factor=1.0 ;

// Beam Search Decoder Parameters
real dec_int_prune_window= LOG_ZERO ; // plus petit = pruning -> plus rapide, mais plus d'erreurs (si trop petit)
real dec_end_prune_window= LOG_ZERO ;
real dec_word_entr_pen=0.0 ;
bool dec_verbose=false ;
bool dec_delayed_lm=true ;
// output options
bool print_by_page=false;
bool print_by_part=false;
bool print_by_book=false;

// Batch Test Parameters
char *input_fname=NULL ;
char *input_format_s=NULL ;
char *wrdtrns_fname=NULL ;
char *output_fname=NULL ;

// Log
char *log_fname=NULL ;
char *end_fname=NULL ; // file to notify the end of the process


void processCmdLine( CmdLine *cmd , int argc , char *argv[] )
{
    // Phoneset Options
    cmd->addText("\nPhoneset Options:") ;
    cmd->addSCmdOption( "-am_models_fname" , &am_models_fname , "" ,
                        "the file with the HMM definitions for the phone models" ) ;
    cmd->addSCmdOption( "-am_sil_phone" , &am_sil_phone , "" ,
                        "the name of silence phoneme" ) ;
    cmd->addSCmdOption( "-am_pause_phone" , &am_pause_phone , "" ,
                        "the name of pause phoneme" ) ;
    cmd->addRCmdOption( "-am_phone_del_pen" , &am_phone_del_pen , 1.0 , 
                        "the (non-log) phone deletion penalty" ) ;
    cmd->addBCmdOption( "-am_apply_pause_del_pen" , &am_apply_pause_del_pen , false ,
                        "indicates whether the phone deletion penalty is applied to pause phone" ) ;

    // Lexicon Parameters
    cmd->addText("\nLexicon Options:") ;
    cmd->addSCmdOption( "-lex_dict_fname" , &lex_dict_fname , "" ,
                        "the dictionary file" ) ;
    cmd->addSCmdOption( "-lex_sent_start_word" , &lex_sent_start_word , SP_START ,
                        "the name of the dictionary word that will start every sentence" ) ;
    cmd->addSCmdOption( "-lex_sent_end_word" , &lex_sent_end_word , SP_END ,
                        "the name of the dictionary word that will end every sentence" ) ;
    cmd->addSCmdOption( "-lex_sil_word" , &lex_sil_word , SP_WORD ,
                        "the name of the silence dictionary word" ) ;

    // Music Model Parameters
    cmd->addText("\nLanguage Model Options:") ;
    cmd->addSCmdOption( "-lm_fname" , &lm_fname , "" ,
                        "the file (ARPA LM format) containing the LM probabilities" ) ;
    cmd->addICmdOption( "-lm_ngram_order" , &lm_ngram_order , 0 , 
                        "the order of the n-gram language model" ) ;
    cmd->addRCmdOption( "-lm_scaling_factor" , &lm_scaling_factor , 1.0 ,
                        "the factor by which log LM probs are scaled during decoding" ) ;

    // Beam Search Decoder Parameters
    cmd->addText("\nBeam Search Decoding Options:") ;
    cmd->addRCmdOption( "-dec_int_prune_window" , &dec_int_prune_window , 75 /* LOG_ZERO */,
                        "the (+ve log) window used for pruning word-interior state hypotheses" ) ;
    cmd->addRCmdOption( "-dec_end_prune_window" , &dec_end_prune_window , LOG_ZERO ,
                        "the (+ve log) window used for pruning word-end state hypotheses" ) ;
    cmd->addRCmdOption( "-dec_word_entr_pen" , &dec_word_entr_pen , 0.0 ,
                        "the log word entrance penalty" ) ;
    cmd->addBCmdOption( "-dec_delayed_lm" , &dec_delayed_lm , false ,
                        "indicates whether LM probabilities are applied in a delayed fashion" ) ;
    cmd->addBCmdOption( "-dec_verbose" , &dec_verbose , false ,
                        "indicates whether frame-by-frame decoding info is printed (to stderr)" ) ;
	 // Beam Search Decoder Parameters
	cmd->addBCmdOption( "-print_by_page" , &print_by_page , false ,
                        "indicates whether results are printed for each page separately (based on file names print_partX_pageX_staffX)" ) ;
	cmd->addBCmdOption( "-print_by_part" , &print_by_part , false ,
                        "indicates whether results are printed for each part separately (based on file names print_partX_pageX_staffX)" ) ;
	cmd->addBCmdOption( "-print_by_book" , &print_by_book , false ,
                        "indicates whether results are printed for each print separately (based on file names print_partX_pageX_staffX)" ) ;

    // General Parameters
    cmd->addText("\nGeneral Options:") ;
    cmd->addSCmdOption( "-input_fname" , &input_fname , "" ,
                        "the file containing the list of files to be decoded (or an archive)" ) ;
    cmd->addSCmdOption( "-output_fname" , &output_fname , "" ,
                        "the file where decoding results are written" ) ;
    cmd->addSCmdOption( "-wrdtrns_fname" , &wrdtrns_fname , "" ,
                        "the file containing word-level reference transcriptions" ) ;

	// Varia
	cmd->addSCmdOption( "-log_fname" , &log_fname , "", 
						"the log output file, standard output if none" ) ;
	cmd->addSCmdOption( "-end_fname" , &end_fname , "", 
						"File used to notify the end of the process. Used to avoid a bug in Mac 10.5 that cannot be fixed" ) ;
	
    cmd->read( argc , argv ) ;
        
    // Basic parameter checks
    if ( strcmp( input_fname , "" ) == 0 )
        error("input_fname undefined\n") ;
    if ( strcmp( am_models_fname , "" ) == 0 )
        error("am_models_fname undefined\n") ;
    if ( strcmp( lex_dict_fname , "" ) == 0 )
        error("lex_dict_fname undefined\n") ;
    
    // Some parameter dependencies
    if ( (lm_ngram_order > 0) && (strcmp(lm_fname,"") == 0) )
        error("lm_ngram_order > 0 but no LM file specified\n") ;
}

int main( int argc , char *argv[] )
{
    CmdLine cmd ;
	cmd.setBOption("write log",false);
    DiskXFile::setBigEndianMode() ;

    processCmdLine( &cmd , argc , argv ) ;
	
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
	
    LexiconInfo lex_info( am_models_fname , am_sil_phone , am_pause_phone , lex_dict_fname , 
                          lex_sent_start_word , lex_sent_end_word , lex_sil_word ) ;

    PhoneModels phone_models ( lex_info.phone_info , am_models_fname ,
                               true , am_phone_del_pen , 
                               am_apply_pause_del_pen , "" , "" , 
                               9 , "" , false , 
                               (real)0.005 , (real)0.005 ) ;

    LinearLexicon lexicon( &lex_info , &phone_models ) ;


	LanguageModel *lang_model ;
	if ( lm_ngram_order <= 0 )
		lang_model = NULL ;
	else
	{
		lang_model = new LanguageModel( 2 , lex_info.vocabulary , 
										lm_fname, lm_scaling_factor ) ;
	}
	BeamSearchDecoder bs_decoder( &lexicon , lang_model , dec_word_entr_pen ,
								  dec_int_prune_window , dec_end_prune_window , 
								  dec_delayed_lm , dec_verbose ) ;
	

	MlDecoderPage batch_tester( input_fname , wrdtrns_fname , &bs_decoder , 
								   true , true , output_fname ) ;
	
	batch_tester.setOutputOptions( print_by_page, print_by_part, print_by_book ); 

	batch_tester.run() ;
	
	if ( out_fd )
		fclose( out_fd );
		
	if ( strlen( end_fname ) )
	{
		out_fd = fopen( end_fname, "w"  );
		fclose( out_fd );
	}

    return(0) ;
}

