// Copyright (C) 2003--2004 Darren Moore (moore@idiap.ch)
//
// Modified 2005 Laurent Pugin (laurent.pugin@lettres.unige.ch)
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

#include "ClassModel.h"

// torch
#include "Allocator.h"
#include "string_stuff.h"
#include "DiskXFile.h"

using namespace Torch;


// !!!! Changer aussi ces valeurs dans Aruspix dans iomlfclass.h !!!!
#define MLF_INTERVAL 0x00FF	// définit la hauteur maximal en valeur absolu d'une note
							// dépend de l'encodage (par note, par 1/2 ton, 41, ...
#define MLF_INTERVAL_V1 0x0F00	// définit la hauteur maximal en valeur absolu d'une note
							// dépend de l'encodage (par note, par 1/2 ton, 41, ...
//#define MLF_INTERVAL_V2 0xF000	// définit la hauteur maximal en valeur absolu d'une note
							// dépend de l'encodage (par note, par 1/2 ton, 41, ...
#define MLF_INTERVAL_SHIFT	8	// bit shift correspondant
//#define MLF_INTERVAL_SHIFT_V	12	// bit shift correspondant


ClassModel::ClassModel( Vocabulary *general_vocabulary_ , 
                        char *voc_fname,  char *lm_fname, char *map_fname, 
						int order_ , real lm_scaling_factor_,
						bool interval_model_, char *voc_notes_fname_ )
{
    FILE *lm_fd ;
    
    if ( general_vocabulary_ == NULL )
        error("ClassModel::ClassModel - no vocabulary defined\n") ;
    if ( order_ <= 0 )
        error("ClassModel::ClassModel - LM order must be > 0\n") ;
    if ( (lm_fname == NULL) || (strcmp(lm_fname,"")==0) )
        error("ClassModel::ClassModel - no LM filename specified\n") ;
    if ( interval_model_ && ((voc_notes_fname_ == NULL) || (strcmp(voc_notes_fname_,"")==0)) )
        error("ClassModel::ClassModel - no notes vocabulary filename specified for pitch model\n") ;
        
    order = order_ ;
	interval_model = interval_model_;
    vocabulary = ::new Vocabulary ( voc_fname , 
		general_vocabulary_->getWord( general_vocabulary_->sent_start_index ) ,
		general_vocabulary_->getWord( general_vocabulary_->sent_end_index  ),
		NULL ) ; // NULL is very important - this vocabulary cannot have a SP_INDEX != 1 
				 // since SP (-1) is used to remove all symbols not mapped in this class model
				 // for example a key will be SP (-1) in a duration model
	

    n_words = vocabulary->n_words ;
    lm_scaling_factor = lm_scaling_factor_ ;
    ngram = new LMNGram( order , vocabulary ) ;

    lm_has_start_word = true ;
    lm_has_end_word = true ;

	if ( interval_model ) // load notes vocabulary for mapping, 
					   // because vocabulary and LM contains intervals
	{
		notes_vocabulary = ::new Vocabulary ( voc_notes_fname_ , 
		general_vocabulary_->getWord( general_vocabulary_->sent_start_index ) ,
		general_vocabulary_->getWord( general_vocabulary_->sent_end_index  ),
		NULL ) ; // NULL is very important - this vocabulary cannot have a SP_INDEX != 1 
				 // since SP (-1) is used to remove all symbols not mapped in this class model
				 // for example a key will be SP (-1) in a duration model
		mapper = new ClassMapper( map_fname, general_vocabulary_, notes_vocabulary );
	}
	else
	{
		notes_vocabulary = NULL;
		mapper = new ClassMapper( map_fname, general_vocabulary_, vocabulary );
	}


    // Open the LM file
    if ( (lm_fd = fopen( lm_fname , "r" )) == NULL )
        error("ClassModel::ClassModel - error opening LM file\n") ;

    // Assume that the file is in ARPA format.
    readARPA( lm_fd ) ;

    fclose( lm_fd ) ;
 
#ifdef DEBUG
    //outputText() ;
#endif
}



ClassModel::ClassModel( Vocabulary *vocabulary_ , char *lm_fname ,  
						int order_ , real lm_scaling_factor_ )
{
    FILE *lm_fd ;
    
    if ( vocabulary_ == NULL )
        error("ClassModel::ClassModel - no vocabulary defined\n") ;
    if ( order_ <= 0 )
        error("ClassModel::ClassModel - LM order must be > 0\n") ;
    if ( (lm_fname == NULL) || (strcmp(lm_fname,"")==0) )
        error("ClassModel::ClassModel - no LM filename specified\n") ;
       
    order = order_ ;
	interval_model = false;
    vocabulary = vocabulary_;	

    n_words = vocabulary->n_words ;
    lm_scaling_factor = lm_scaling_factor_ ;
    ngram = new LMNGram( order , vocabulary ) ;

    lm_has_start_word = true ;
    lm_has_end_word = true ;

	notes_vocabulary = NULL;
	mapper = new ClassMapper( vocabulary );

    // Open the LM file
    if ( (lm_fd = fopen( lm_fname , "r" )) == NULL )
        error("ClassModel::ClassModel - error opening LM file\n") ;

    // Assume that the file is in ARPA format.
    readARPA( lm_fd ) ;

    fclose( lm_fd ) ;
 
#ifdef DEBUG
    //outputText() ;
#endif
}


ClassModel::~ClassModel()
{
    if ( ngram != NULL )
        delete ngram ;

	if ( mapper != NULL )
		delete mapper;

	if ( vocabulary != NULL )
		delete vocabulary;

	if ( notes_vocabulary != NULL )
		delete notes_vocabulary;
}

Vocabulary *ClassModel::getVocabulary( )
{
	if ( interval_model )
		return notes_vocabulary;
	else
		return vocabulary;
}

void ClassModel::getPitch( int map_wrd, int *pitch, int *value )
{
	// first get the note code from the notes_vocabulary
	int code = atoi( notes_vocabulary->getWord( map_wrd ) );

	*pitch = code & MLF_INTERVAL; 
	code = code & MLF_INTERVAL_V1;
	*value = (code >> MLF_INTERVAL_SHIFT);
}

void ClassModel::getInterval( char *interval, int pitch, int value, int last_pitch, int last_value )
{
	// calculate the code : value (4bits) | last_value (4bits) | pitch - last_pitch (8bits)
	/*int code = pitch - last_pitch + 
		( last_value << MLF_INTERVAL_SHIFT ) + 
		( value << MLF_INTERVAL_SHIFT_V );*/

	int code = pitch - last_pitch + // one value only
		( value << MLF_INTERVAL_SHIFT );

	/*int code = pitch - last_pitch; */ // if interval only 
	
	//printf( "Last %d %d - Next %d %d\n", last_pitch, last_value, pitch, value );
	
	sprintf( interval, "%d", code );
}

int ClassModel::getIndexFromNote( int note_index )
{
	if ( interval_model == false )
		return note_index;
	else
		return	vocabulary->getIndex( notes_vocabulary->getWord( note_index ) );
}

real ClassModel::getLogProbBackoff( int order, int *words )
{
	/*
	// replace *words by mapped values
	for (int i=0; i < order; i++)
	{
		words[i] = mapper->map[ words[i] ];
		//printf("%d ", words[i] );
	}
	//printf("\n");

	// remove values that are not in dictionnary (eg -1) which correspond to SP
	for (int i=0; i < order; i++)
	{
		if ( words[i] == -1 )
		{

			//printf("removing SP (order == %d, i == %d)...\n", order, i);
			memmove( words + i, words + i + 1, (order - i - 1) * sizeof(int) );
			order--; 
			i--;

			//for (int j=0; j < order; j++)
			//	printf("%d ", words[j] );
			//printf("\n");

		}
		else
		{
			//printf("%d (order == %d, i == %d) ",  words[i], order, i );
			//printf("%s ", vocabulary->getWord( words[i] ) );
		}
	}

	if ( order == 0 )
	{
		printf("order == 0\n");
		return 0.0;
	}*/

	real prob = ngram->getLogProbBackoff( order , words ) ;
	//printf("%f\n", prob );
	return prob;
}


void ClassModel::readARPA( FILE *arpa_fd )
{
    int n_exp_entries[30] ; // n_exp_entries[0] is the expected number of unigram entries
                            // n_exp_entries[0] is the expected number of bigram entries
    int n_act_entries[30] ; // the actual number of entries read from the file.
    int words[30] ;    // holds the predecessor words for a given word.
    real curr_prob=0.0 , curr_bow=0.0 ;
    char *curr_word=NULL ;
    int curr_index ;
    real ln_10 = (real)log(10.0) ;
    int tempn=0 , tempn_entries=0 , max_n_in_file=0 ;
    char line[1000] ;
    bool got_begin_data_mark=false , expecting_end=false , got_end=false , error_flag ;
    int curr_gram_data=0 ;
    
    if ( arpa_fd == NULL )
        error("ClassModel::readARPA - arpa_fd is NULL\n") ;
        
    // discard lines until we get the "beginning of data mark".
    while ( fgets( line , 1000 , arpa_fd ) != NULL )
    {
        // if the new line is empty, get the next line
        if ( (line[0]==' ') || (line[0]=='\r') || (line[0]=='\n') || (line[0]=='\t') ||
             (line[0]=='#') )
            continue ;
            
        if ( line[0] == '\\' )
        {
            strtoupper( line ) ;
            if ( strstr( line , "\\END\\" ) != NULL )
            {
                if ( curr_gram_data < order )
                {
                    // we haven't encountered the n-grams we expected
                    error("ClassModel::readARPA - not enough data in file\n") ;
                }
            
                // we've reached the end of the ARPA file - we're done
                got_end = true ;
                break ;
            }
            else if ( expecting_end == true )
            {
                // we're expecting the end marker and didn't get it - get the next line
                continue ;
            }
            else if ( strstr( line , "\\DATA\\" ) != NULL )
            {
                if ( got_begin_data_mark == true )
                {
                    // we have already seen the beginning of data marker - error !
                    error("ClassModel::readARPA - duplicate beginning of data marker\n") ;
                }
                got_begin_data_mark = true ;
            }
            else if ( strstr( line , "-GRAMS:" ) != NULL )
            {
                if ( got_begin_data_mark == true )
                {
                    // find out whether we are at the start of the 1-gram, 
                    //   2-gram, 3-gram, etc data.
                    if ( (curr_gram_data+1) != ( line[1]-0x30 ) )
                        error("ClassModel::readARPA - N-Gram N out of order\n") ;

                    curr_gram_data = line[1]-0x30 ;
                    n_act_entries[curr_gram_data-1] = 0 ;
                    if ( curr_gram_data > order )
                    {
                        // the order has exceeded the order of our LM - we're done 
                        //   reading probabilties.
                        expecting_end = true ;
                    }
                }
            }
            else
            {
                // we got something else that started with a '\' - error !!
                error("ClassModel::readARPA - unrecognised marker\n%s\n",line) ;
            }
        }
        else
        {
            if ( (got_begin_data_mark == false) || (expecting_end == true) )
                continue ;

            if ( strstr( line , "<UNK>") != NULL )
            {
                n_exp_entries[curr_gram_data-1]-- ;
                continue ;
            }

            if ( curr_gram_data == 0 )
            {
                // we are just below the \data\ - therefore expecting ngram x=y lines
                sscanf( line , "%*s %d=%d" , &tempn , &tempn_entries ) ;
                if ( tempn != (max_n_in_file+1) )
                    error("ClassModel::readARPA - ngram n=y -> unexpected n\n") ;
                max_n_in_file = tempn ;
                if ( tempn <= order )
                    n_exp_entries[tempn-1] = tempn_entries ;
            }    
            else if ( (curr_gram_data > 0) && (curr_gram_data < max_n_in_file) )
            {
                // The line should contain (curr_gram_data+2) fields.
                // eg. for 2-gram entry -> p wd_1 wd_2 bo_wt_2
            
                // Read the probability from the first field (in log10 format) and convert
                //   to ln format.
#ifdef USE_DOUBLE
                if ( sscanf( line , "%lf" , &curr_prob ) != 1 )
#else
                if ( sscanf( line , "%f" , &curr_prob ) != 1 )
#endif
                    error("ClassModel::readARPA - error reading prob\n") ;
                if ( curr_prob < -90.0 )
                    curr_prob = LOG_ZERO/2 ;
                else
                    curr_prob *= ln_10 ;
                
                // get past the prob field so we can read the words
                strtok( line , " \n\r\t" ) ;
                                
                // read wd_1 , ... , wd_n (ie. all predecessor words of wd_n)
                error_flag = false ;
                for ( int i=0 ; i<curr_gram_data ; i++ )
                {
                    // Extract the next word from the line
                    curr_word = strtok( NULL , " \n\r\t" ) ;

                    // determine the index of the word in the vocabulary
                    curr_index = vocabulary->getIndex( curr_word ) ;
                    if ( curr_index < 0 ) 
                    {
                        // The word is not in our vocab - don't add the entry to our LM
                        error_flag = true ;
                        n_exp_entries[curr_gram_data-1]-- ;
                        break ;
                        //error("ClassModel::readARPA - %s in ARPA file not in vocab\n" ,
                        //                           curr_word ) ;
                    }
                    else
                    {
                        if ( curr_index == vocabulary->sent_start_index )
                            lm_has_start_word = true ;
                        if ( curr_index == vocabulary->sent_end_index )
                            lm_has_end_word = true ;
                    }
                    
                    // Place the word index into the array of predecessor words in
                    //   oldest-word-first order.
                    words[i] = curr_index ;
                }

                if ( error_flag == true )
                    continue ;

                // Extract the back off weight from the last field in the line and
                //   convert from log10 to ln.
#ifdef USE_DOUBLE
                if ( sscanf( strtok( NULL , " \n\r\t" ) , "%lf" , &curr_bow ) != 1 )
#else
                if ( sscanf( strtok( NULL , " \n\r\t" ) , "%f" , &curr_bow ) != 1 )
#endif
                    error("ClassModel::readARPA - back off weight not found\n") ;
                if ( curr_bow < -90.0 )
                    curr_bow = 0.0 ;
                else
                    curr_bow *= ln_10 ;
                
                // add the entry to the curr_gram_data-gram for the new word
                ngram->addEntry( curr_gram_data, words, curr_prob, curr_bow ) ;
                n_act_entries[curr_gram_data-1]++ ;
            }
            else if ( curr_gram_data == max_n_in_file )
            {
                // The line should contain (curr_gram_data+1) fields because
                //   backoff weights are only required for N-grams that form a prefix of
                //   longer N-grams in the model file (ie. not this one - the longest).
                // eg. for 4-gram entry -> p wd_1 wd_2 wd_3 wd_4
                //    (where 4-gram probabilities are the maximum in the file.
            
                // read the probability from the first field
#ifdef USE_DOUBLE
                sscanf( line , "%lf" , &curr_prob ) ;
#else
                sscanf( line , "%f" , &curr_prob ) ;
#endif
                if ( curr_prob < -90.0 )
                    curr_prob = LOG_ZERO/2 ;
                else
                    curr_prob *= ln_10 ;
                
                // get past the prob field so we can read the words
                strtok( line , " \n\r\t" ) ;
                                
                // read wd_1 , ... , wd_n and insert indices in 'words' array.
                error_flag = false ;
                for ( int i=0 ; i<curr_gram_data ; i++ )
                {
                    // Extract the next word from the line
                    curr_word = strtok( NULL , " \n\r\t" ) ;

                    // determine the index of the word in the vocabulary
                    curr_index = vocabulary->getIndex( curr_word ) ;
                    if ( curr_index < 0 ) 
                    {
                        // The word is not in our vocab - don't add the entry to our LM
                        error_flag = true ;
                        n_exp_entries[curr_gram_data-1]-- ;
                        break ;
                        //error("ClassModel::readARPA - %s in ARPA file not in vocab\n" ,
                        //                           curr_word ) ;
                    }
                    else
                    {
                        if ( curr_index == vocabulary->sent_start_index )
                            lm_has_start_word = true ;
                        if ( curr_index == vocabulary->sent_end_index )
                            lm_has_end_word = true ;
                    }
                    
                    // Place the word index into the array of predecessor words in
                    //   oldest-word-first order.
                    words[i] = curr_index ;
                }
                    
                if ( error_flag == true )
                    continue ;

                // add the entry to the curr_gram_data-gram for the new word
                n_act_entries[curr_gram_data-1]++ ;
                ngram->addEntry( curr_gram_data , words , curr_prob ) ;
            }
        }
    }   
    
    // make sure that we got the /end/ marker
    if ( got_end == false )
        error("ClassModel::readARPA - EOF but no end marker\n") ;
        
    // Issue warnings if the number of expected entries for each n-gram did
    //   not match the actual number read from the file
    for ( int i=0 ; i<order ; i++ )
    {
        if ( n_exp_entries[i] != n_act_entries[i] )
        {
            warning("ClassModel::readARPA - warning - %d-gram entry count mismatch\n" , i+1 ) ;
            warning("\t%d expected != %d actual\n",n_exp_entries[i],n_act_entries[i]) ;
        }
    }
}


#ifdef DEBUG
void ClassModel::outputText()
{
    //ngram->outputText() ;
}
#endif

