// Copyright (C) 2005 Laurent Pugin (laurent.pugin@lettres.unige.ch)
//
// All rights reserved.

#ifdef WIN32
	#include <io.h>
#else
	#include <unistd.h>
#endif

#include "Vocabulary.h"
#include "DiskXFile.h"
#include "CmdLine.h"


#include "../ml/ml.h"
#include "../ml/mldecoderpage.h"

using namespace Torch ;

// Lexicon Parameters
char *lex_dict_fname=NULL ;
char *lex_sent_start_word=NULL ;
char *lex_sent_end_word=NULL;
char *lex_sil_word=NULL ;

// output options
bool print_by_page=false;
bool print_by_part=false;
bool print_by_book=false;

// Batch Test Parameters
char *wrdtrns_fname=NULL ;
char *results_fname=NULL ;



void processCmdLine( CmdLine *cmd , int argc , char *argv[] )
{
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
						
	 // Beam Search Decoder Parameters
	cmd->addBCmdOption( "-print_by_page" , &print_by_page , false ,
                        "indicates whether results are printed for each page separately (based on file names print_partX_pageX_staffX)" ) ;
	cmd->addBCmdOption( "-print_by_part" , &print_by_part , false ,
                        "indicates whether results are printed for each part separately (based on file names print_partX_pageX_staffX)" ) ;
	cmd->addBCmdOption( "-print_by_book" , &print_by_book , false ,
                        "indicates whether results are printed for each print separately (based on file names print_partX_pageX_staffX)" ) ;


    // General Parameters
    cmd->addText("\nGeneral Options:") ;
    cmd->addSCmdOption( "-results_fname" , &results_fname , "" ,
                        "the file where decoded results were written" ) ;
    cmd->addSCmdOption( "-wrdtrns_fname" , &wrdtrns_fname , "" ,
                        "the file containing word-level reference transcriptions" ) ;
	
    cmd->read( argc , argv ) ;
        
    // Basic parameter checks
    if ( strcmp( wrdtrns_fname , "" ) == 0 )
        error("wrdtrns_fname undefined\n") ;
    if ( strcmp( results_fname , "" ) == 0 )
        error("result_fname undefined\n") ;
    if ( strcmp( lex_dict_fname , "" ) == 0 )
        error("lex_dict_fname undefined\n") ;
}

int main( int argc , char *argv[] )
{
    CmdLine cmd ;
	cmd.setBOption("write log",false);
    DiskXFile::setBigEndianMode() ;
	
	Allocator *allocator = new Allocator;

    processCmdLine( &cmd , argc , argv ) ;
						  
	Vocabulary voc( lex_dict_fname , lex_sent_start_word , lex_sent_end_word , lex_sil_word );

	MlDecoderPage batch_tester( &voc, wrdtrns_fname ) ;
	
	batch_tester.setOutputOptions( print_by_page, print_by_part, print_by_book ); 

	batch_tester.eval( results_fname ) ;
	
	delete allocator;
	
    return(0) ;
}

