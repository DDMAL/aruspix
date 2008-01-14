// Copyright (C) 2005 Laurent Pugin (laurent.pugin@lettres.unige.ch)
//
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

#include "RecDecoderPage.h"
#include "RecBeamSearchDecoder.h"

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

#define SP_START "SP_START"
#define SP_END "SP_END"
#define SP_WORD "SP"

// Music Model Parameters
int lm_ngram_order=0 ;
char *lm_fname=NULL ;
real lm_scaling_factor=1.0 ;
// Classes models
int cm_symb_order=-1;
int cm_duration_order=-1;
int cm_pitch_order=-1;
int cm_interval_order=-1;
real cm_symb_factor=1.0;
real cm_duration_factor=1.0;
real cm_pitch_factor=1.0;
real cm_interval_factor=1.0;
bool cm_no_symb=false;
bool cm_no_duration=false;
bool cm_no_pitch=false;
bool cm_no_interval=false;
// Bigram model
real cm_bigram_factor=1.0;
bool cm_no_bigram=false;

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

// Varia
bool use_torch_decoder=false ;

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
	// Classes Model
    cmd->addRCmdOption( "-cm_symb_factor" , &cm_symb_factor , 1.0 ,
                        "the factor by which log symbols models probs are scaled during decoding" ) ;
    cmd->addRCmdOption( "-cm_duration_factor" , &cm_duration_factor , 1.0 ,
                        "the factor by which log durations models probs are scaled during decoding" ) ;
    cmd->addRCmdOption( "-cm_pitch_factor" , &cm_pitch_factor , 1.0 ,
                        "the factor by which log pitches models probs are scaled during decoding" ) ;
    cmd->addRCmdOption( "-cm_interval_factor" , &cm_interval_factor , 1.0 ,
                        "the factor by which log intervals models probs are scaled during decoding" ) ;
	//
    cmd->addICmdOption( "-cm_symb_order" , &cm_symb_order , -1 , 
                        "the order of the n-gram symbols class model (if different from lm_ngram_order)" ) ;
    cmd->addICmdOption( "-cm_duration_order" , &cm_duration_order , -1 , 
                        "the order of the n-gram durations class model (if different from lm_ngram_order)" ) ;
    cmd->addICmdOption( "-cm_pitch_order" , &cm_pitch_order , -1 , 
                        "the order of the n-gram pitches class model (if different from lm_ngram_order)" ) ;
    cmd->addICmdOption( "-cm_interval_order" , &cm_interval_order , -1 , 
                        "the order of the n-gram intervals  class model (if different from lm_ngram_order)" ) ;
	//
    cmd->addBCmdOption( "-cm_no_symb" , &cm_no_symb , false ,
                        "don't use symbols class model" ) ;
    cmd->addBCmdOption( "-cm_no_duration" , &cm_no_duration , false ,
                        "don't use durations class model" ) ;
    cmd->addBCmdOption( "-cm_no_pitch" , &cm_no_pitch , false ,
                        "don't use pitch  class model" ) ;
    cmd->addBCmdOption( "-cm_no_interval" , &cm_no_interval , false ,
                        "don't use intervals  class model" ) ;
    // Bigram Model Parameters
    cmd->addBCmdOption( "-cm_no_bigram" , &cm_no_bigram , false ,
                        "don't use bigram class model" ) ;
    cmd->addRCmdOption( "-cm_bigram_factor" , &cm_bigram_factor , 1.0 ,
                        "the factor by which log bigram model probs are scaled during decoding" ) ;

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
    cmd->addBCmdOption( "-use_torch_decoder" , &use_torch_decoder , false ,
                        "User original torch decoder an language model" ) ;
	cmd->addSCmdOption( "-log_fname" , &log_fname , "", 
						"the log output file, standard output if none" ) ;
	
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
	// cm_orders
	if ( cm_symb_order == -1 )
		cm_symb_order = lm_ngram_order;
	if ( cm_duration_order == -1 )
		cm_duration_order = lm_ngram_order;
	if ( cm_pitch_order == -1 )
		cm_pitch_order = lm_ngram_order;
	if ( cm_interval_order == -1 )
		cm_interval_order = lm_ngram_order;
}


// file names buffer
char dir[1000];
char gram[1000];
char dic[1000];
char dic_interval[1000];
char map[1000];

void formatFilenames( const char *directory, const char *basename, bool interval=false )
{
	// base filename
	strcpy(dir, directory);
	strcat(dir, basename);
	// dic
	strcpy(dic, dir);
	strcat(dic, ".dic");
	// interval dic
	strcpy(dic_interval, dir);
	strcat(dic_interval, ".notes.dic");
	// map
	strcpy(map, dir);
	if ( interval )
		strcat(map, ".notes.map");
	else
		strcat(map, ".map");
	// gram
	strcpy(gram, dir );
	strcat(gram, ".gram");	
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

	if ( use_torch_decoder == false )
	{
		MusicModel *mus_model ;
		if ( lm_ngram_order <= 0 )
			mus_model = NULL ;
		else
		{
			strcpy(dir, lm_fname);
			strcat(dir, "ngram.dic");
			printf("dir");
			Vocabulary mus_vocabulary( dir , lex_sent_start_word , lex_sent_end_word , lex_sil_word ) ;
		
			strcpy( dir, lm_fname );
			//mus_model = new MusicModel( lex_info.vocabulary, lm_scaling_factor ) ;
			mus_model = new MusicModel( &mus_vocabulary, lm_scaling_factor ) ;

			if ( cm_no_bigram == false )
			{
				formatFilenames( lm_fname, "ngram" );
				mus_model->addClassModel( gram, lm_ngram_order, cm_bigram_factor);
			}
		
			if ( cm_no_symb == false )
			{
				formatFilenames( lm_fname, "symb" );
				mus_model->addClassModel( dic,
					gram, map, cm_symb_order, cm_symb_factor);
			}	
			
			if ( cm_no_duration == false )
			{
				formatFilenames( lm_fname, "duration" );
				mus_model->addClassModel( dic,
					gram, map, cm_duration_order, cm_duration_factor);
			}

			if ( cm_no_pitch == false )
			{
				formatFilenames( lm_fname, "pitch" );
				mus_model->addClassModel( dic,
					gram, map, cm_pitch_order, cm_pitch_factor);
			}

			if ( cm_no_interval == false )
			{
				formatFilenames( lm_fname, "interval", true );
				mus_model->addClassModel( dic,
					gram, map, 3, cm_interval_factor, true, dic_interval );
					//gram, map, cm_interval_order, cm_interval_factor, true, dic_interval );
			}
		}

		RecBeamSearchDecoder bs_decoder( &lexicon , mus_model , dec_word_entr_pen ,
									  dec_int_prune_window , dec_end_prune_window , 
									  dec_delayed_lm , dec_verbose ) ;

		RecDecoderPage batch_tester( input_fname , wrdtrns_fname , &bs_decoder , 
									   true , true , output_fname ) ;
		
		batch_tester.setOutputOptions( print_by_page, print_by_part, print_by_book ); 

		batch_tester.run() ;
	}
	else
	{
		LanguageModel *lang_model ;
		if ( lm_ngram_order <= 0 )
			lang_model = NULL ;
		else
		{
			char gram[1000];
			strcpy( gram, lm_fname );
			strcat( gram, "ngram.gram");
			lang_model = new LanguageModel( 2 , lex_info.vocabulary , 
											gram, lm_scaling_factor ) ;
		}
		BeamSearchDecoder bs_decoder( &lexicon , lang_model , dec_word_entr_pen ,
									  dec_int_prune_window , dec_end_prune_window , 
									  dec_delayed_lm , dec_verbose ) ;
		

		DecoderBatchTest batch_tester( input_fname , DST_FEATS_HTK , wrdtrns_fname , &bs_decoder , 
									   true , true , output_fname , false , 10.0 ) ;
		
		batch_tester.run() ;	
	}
	
	if ( out_fd )
		fclose( out_fd );
	
    return(0) ;
}

