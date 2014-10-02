/////////////////////////////////////////////////////////////////////////////
// Name:        mldecoder.cpp
// Author:      Laurent Pugin
// Created:     2008
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#include "mldecoder.h"
#include "mldecoderpage.h"

#include <unistd.h>

// torch
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

#include "wx/log.h"

using namespace Torch;

// WDR: class implementations

//----------------------------------------------------------------------------
// MlDecoder
//----------------------------------------------------------------------------

MlDecoder::MlDecoder( wxString input_fname_, wxString am_models_fname_, wxString lex_dic_fname_ )
{
    
	am_models_fname = am_models_fname_;
	am_sil_phone = "";
	am_pause_phone = "";
	am_phone_del_pen = 1.0 ;
	am_apply_pause_del_pen = false ;

	// Lexicon Parameters
	lex_dict_fname = lex_dic_fname_ ;
	lex_sent_start_word =  SP_START;
	lex_sent_end_word = SP_END;
	lex_sil_word = SP_WORD;

	// Music Model Parameters
	lm_ngram_order = 0;
	lm_fname = "";
	lm_scaling_factor = 1.0;

	// Beam Search Decoder Parameters
	dec_int_prune_window = 75  /*LOG_ZERO*/ ; // plus petit = pruning -> plus rapide, mais plus d'erreurs (si trop petit)
	dec_end_prune_window = LOG_ZERO ;
	dec_word_entr_pen = 0.0;
	dec_verbose = false;
	dec_delayed_lm = true;
	
	// output options
	print_by_page = false;
	print_by_part = false;
	print_by_book = false;

	// Batch Test Parameters
	input_fname = input_fname_;
	wrdtrns_fname = "";
	output_fname = "";

	// Log
	log_fname= "";
}

MlDecoder::~MlDecoder()
{
}

void MlDecoder::Run()
{

    // Basic parameter checks
    if ( input_fname.IsEmpty() || am_models_fname.IsEmpty() || lex_dict_fname.IsEmpty() )
	{ 
		wxLogError("Missing input files");
		return  ;
	}
    
    if ( (lm_ngram_order > 0) && lm_fname.IsEmpty() )
	{
        wxLogWarning("lm_ngram_order > 0 but no LM file specified") ;
		lm_ngram_order = 0;
	}
	
	DiskXFile::setBigEndianMode() ;
	
	//==================================================================== 
	//=================== Log file  ======================================
	//====================================================================
	
	int saved_stdout = 0;
	FILE *out_fd = NULL;
	if ( !log_fname.IsEmpty() )
	{
		// redirect stdout if output file is supplied
		saved_stdout = dup( fileno( stdout) );
		out_fd = freopen( log_fname.c_str(), "a", stdout );
		if ( out_fd == NULL )
		{
			dup2( saved_stdout, fileno( stdout ) );
			close( saved_stdout );
			wxLogWarning( "Opening log file failed %s", log_fname.c_str() ) ;
		}
	}
	
	//==================================================================== 
	//=================== Lexicon ========================================
	//====================================================================
	
    LexiconInfo lex_info( am_models_fname.c_str() , am_sil_phone.c_str() , am_pause_phone.c_str() , 
		lex_dict_fname.c_str() , lex_sent_start_word.c_str() , lex_sent_end_word.c_str() , lex_sil_word.c_str() ) ;

    char am_models_fname_c[2048];
    strcpy( am_models_fname_c, am_models_fname.c_str() );
	PhoneModels phone_models( lex_info.phone_info , am_models_fname_c ,
                               true , (real)am_phone_del_pen , 
                               am_apply_pause_del_pen , "" , "" , 
                               9 , "" , false , 
                               (real)0.005 , (real)0.005 ) ;

    LinearLexicon lexicon( &lex_info , &phone_models ) ;

	LanguageModel *lang_model ;
	if ( lm_ngram_order <= 0 )
		lang_model = NULL ;
	else
	{
        char lm_fname_c[2048];
        strcpy( lm_fname_c, lm_fname.c_str() );
		lang_model = new LanguageModel( 2 , lex_info.vocabulary , 
											lm_fname_c, lm_scaling_factor ) ;
	}
	
	//==================================================================== 
	//=================== Decoder ========================================
	//====================================================================
	
	BeamSearchDecoder bs_decoder( &lexicon , lang_model , dec_word_entr_pen ,
									  dec_int_prune_window , dec_end_prune_window , 
									  dec_delayed_lm , dec_verbose ) ;
	
    char input_fname_c[2048];
    strcpy( input_fname_c, input_fname.c_str() );
    char wrdtrns_fname_c[2048];
    strcpy( wrdtrns_fname_c, wrdtrns_fname.c_str() );
    char output_fname_c[2048];
    strcpy( output_fname_c, output_fname.c_str() );
    
	MlDecoderPage page_decoder( input_fname_c , wrdtrns_fname_c ,
		&bs_decoder , true , true , output_fname_c ) ;
    
    page_decoder.setOutputOptions( print_by_page, print_by_part, print_by_book );
    
	page_decoder.run();
    
    wxLogMessage("Recognition done");
	
	if ( out_fd )
		fclose( out_fd );
		
	return;
}

