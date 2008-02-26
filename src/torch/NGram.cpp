// Copyright (C) 2005 Laurent Pugin (laurent.pugin@lettres.unige.ch)
//
// All rights reserved.

#include "Vocabulary.h"
#include "CmdLine.h"

#include "../ml/ml.h"
#include "../ml/mlngrambuilder.h"

#ifdef WIN32
	#include <io.h>
#else
	#include <unistd.h>
#endif

using namespace Torch ;

// Lexicon Parameters
char *lex_sent_start_word=NULL ;
char *lex_sent_end_word=NULL ;
char *lex_sil_word=NULL ;

// Language Model Parameters
int lm_ngram_order=0 ;
char *input_wrdtrns_fname=NULL ;
char *input_dict_fname=NULL ;
char *output_fname=NULL ;
char *data_fname=NULL ;
char *reload_data_fname=NULL ;

bool dec_verbose=false ;


void processCmdLine( CmdLine *cmd , int argc , char *argv[] )
{
    // Lexicon Parameters
    cmd->addText("\nGrammar Options:") ;
    cmd->addSCmdArg( "-input_wrdtrns_fname" , &input_wrdtrns_fname ,
                        "the file containing word-level reference transcriptions" ) ;
    cmd->addSCmdArg( "-input_dict_fname" , &input_dict_fname ,
                        "the dictionary file" ) ;
    cmd->addICmdArg( "-lm_ngram_order" , &lm_ngram_order ,
                        "the ngram order" ) ;
	cmd->addSCmdArg( "-output_fname" , &output_fname ,
                        "the output file" ) ;

    // START and END words
	cmd->addSCmdOption( "-lex_sent_start_word" , &lex_sent_start_word , SP_START ,
                        "the name of the dictionary word that will start every sentence" ) ;
    cmd->addSCmdOption( "-lex_sent_end_word" , &lex_sent_end_word , SP_END ,
                        "the name of the dictionary word that will end every sentence" ) ;
					
	// reload file
    cmd->addSCmdOption( "-reload_data_fname" , &reload_data_fname , "" ,
                        "the name of the file containing data that have to be reloaded" ) ;
    
	// output
	cmd->addSCmdOption( "-data_fname" , &data_fname , "" ,
                        "the data output file" ) ;

    cmd->read( argc , argv ) ;
        
    // Basic parameter checks
    if ( lm_ngram_order < 1 )
        error("invalid ngram order\n") ;
}


int main( int argc , char *argv[] )
{
    CmdLine cmd ;
    DiskXFile::setBigEndianMode() ;

    processCmdLine( &cmd , argc , argv ) ;

    Vocabulary vocabulary ( input_dict_fname , lex_sent_start_word , 
                                            lex_sent_end_word , lex_sil_word ) ;

	MlNgramBuilder ngram( lm_ngram_order, &vocabulary );
	
	if ( strlen( reload_data_fname ) )
		ngram.reloadFile( reload_data_fname );
		
	ngram.loadFile( input_wrdtrns_fname, output_fname, data_fname);
	
	printf( SUCCESS );
 
    return(0) ;
}

