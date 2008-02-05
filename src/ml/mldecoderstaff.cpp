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

#include "mldecoderstaff.h"

// torch
#include "Allocator.h"
#include "IOHTK.h"
#include "Sequence.h"
#include "DiskXFile.h"
#include "time.h"
#include "sys/types.h"
#include "sys/stat.h"

using namespace Torch;


MlDecoderStaff::MlDecoderStaff()
{
    test_filename = NULL ;
    test_id = -1 ;
    expected_words = NULL ;
    n_expected_words = 0 ;
    actual_words = NULL ;
    actual_words_times = NULL ;
    n_actual_words = 0 ;
    decode_time = 0.0 ;
    output_result = false ;
    remove_sent_marks = false ;
    archive_offset = -1 ;
    output_fd=NULL ;

    n_frames=0 ;
    n_features=0 ;
    n_emission_probs=0 ;
    decoder_input = NULL ;
    phone_models = NULL ;
}


MlDecoderStaff::~MlDecoderStaff()
{
    if ( test_filename != NULL )
        free( test_filename ) ;
    if ( expected_words != NULL )
        free( expected_words ) ;
    if ( actual_words != NULL )
        free( actual_words ) ;
    if ( actual_words_times != NULL )
        free( actual_words_times ) ;
    if ( decoder_input != NULL )
    {
        for ( int i=0 ; i<n_frames ; i++ )
            free( decoder_input[i] ) ;
        free( decoder_input ) ;
    }
}


void MlDecoderStaff::configure( int test_id_ , char *test_filename_ , int n_expected_words_ ,
                             int *expected_words_ ,PhoneModels *phone_models_ , bool remove_sent_marks_ , 
                             bool output_result_ , FILE *out_fd )
{
    test_id = test_id_ ;
    archive_offset = -1 ;
    if ( (phone_models = phone_models_) == NULL )
        error("MlDecoderStaff::configure - phone_models is NULL\n") ;
    
    // Allocate memory to hold the filename of the test data and copy the string.
    test_filename = (char *)Allocator::sysAlloc( (strlen(test_filename_)+1) * sizeof(char) ) ;
    strcpy( test_filename , test_filename_ ) ;
    
    // Allocate memory to hold the array of word indices that constitute the
    //   expected result of the test and copy the results.
    if ( (n_expected_words_>0) && (expected_words_!=NULL) )
    {
        n_expected_words = n_expected_words_ ;
        expected_words = (int *)Allocator::sysAlloc( n_expected_words * sizeof(int) ) ;
        memcpy( expected_words , expected_words_ , n_expected_words*sizeof(int) ) ;
    }
    else
    {
        n_expected_words = 0 ;
        expected_words = NULL ;
    }

    // If there are existing actual results - delete them
    if ( actual_words != NULL )
    {
        free( actual_words ) ;
        actual_words = NULL ;
    }
    if ( actual_words_times != NULL )
    {
        free( actual_words_times ) ;
        actual_words_times = NULL ;
    }
    n_actual_words = 0 ;

    remove_sent_marks = remove_sent_marks_ ;
    output_result = output_result_ ;
    if ( (output_fd = out_fd) == NULL )
        output_fd = stdout ;
}


void MlDecoderStaff::configure( int test_id_ , long archive_offset_ , int n_expected_words_ ,
                              int *expected_words_ , PhoneModels *phone_models_ , bool remove_sent_marks_ , 
                              bool output_result_ , FILE *out_fd )
{
    test_id = test_id_ ;
    test_filename = NULL ;
    if ( (phone_models = phone_models_) == NULL )
        error("MlDecoderStaff::configure(2) - phone_models is NULL\n") ;
    archive_offset = archive_offset_ ;
    
    // Allocate memory to hold the array of word indices that constitute the
    //   expected result of the test and copy the results.
    if ( (n_expected_words_>0) && (expected_words_!=NULL) )
    {
        n_expected_words = n_expected_words_ ;
        expected_words = (int *)Allocator::sysAlloc( n_expected_words * sizeof(int) ) ;
        memcpy( expected_words , expected_words_ , n_expected_words*sizeof(int) ) ;
    }
    else
    {
        n_expected_words = 0 ;
        expected_words = NULL ;
    }

    // If there are existing actual results - delete them
    if ( actual_words != NULL )
    {
        free( actual_words ) ;
        actual_words = NULL ;
    }
    if ( actual_words_times != NULL )
    {
        free( actual_words_times ) ;
        actual_words_times = NULL ;
    }
    n_actual_words = 0 ;

    remove_sent_marks = remove_sent_marks_ ;
    output_result = output_result_ ;
    if ( (output_fd = out_fd) == NULL )
        output_fd = stdout ;
}


void MlDecoderStaff::run( BeamSearchDecoder *decoder , FILE *archive_fd )
{
    clock_t start_time , end_time ;
    int start_index = 0 ;

    // The data file hasn't been loaded yet - load it
    loadDataFile( archive_fd ) ;

    // Now look at the type of data that was in the file and compare it with the
    //   type expected by the phone set.
    if ( ((n_emission_probs == 0) && (phone_models->input_vecs_are_features == false)) ||
         ((n_features == 0) && (phone_models->input_vecs_are_features == true)) )
    {
        // We've got feature vectors (or nothing), but the phone_models is expecting
        //   vectors of emission probabilities (or vice versa).
        error("MlDecoderStaff::run - datafile format does not agree with phone_models\n") ;
    }

    if ( (n_features != phone_models->n_features) && 
         (n_emission_probs != phone_models->n_emission_probs) )
    {
        error("MlDecoderStaff::run - input vector size does not agree with phone_models\n") ;
    }
   
    // If the input vectors are features and we are calculating emission probs
    //   using an MLP, we need to initialise the context window of the MLP.
    if ( (phone_models->input_vecs_are_features == true) && (phone_models->mlp != NULL) )
        start_index = phone_models->mlp->initContextWindow( decoder_input ) ;
    n_frames -= start_index ;
    
    // invoke the decoder
    start_time = clock() ;
    decoder->decode( decoder_input+start_index , n_frames , &n_actual_words , 
                     &actual_words , &actual_words_times ) ;
    end_time = clock() ;
    decode_time = (real)(end_time-start_time) / CLOCKS_PER_SEC ;
    
    // process the decoding result
    if ( remove_sent_marks == true )
        removeSentMarksFromActual( decoder->vocabulary ) ;
    if ( output_result == true )
	{
		char label[1000];
		char *ptr;
		strcpy( label, test_filename );
        if ( (ptr=strrchr( label , '/' )) != NULL )
			memmove( label , ptr+1 , strlen(ptr)+1 ) ;
        if ( (ptr=strrchr( label , '.' )) != NULL )
             *ptr = '\0' ;
		strcat( label, ".lab" );

		fprintf( output_fd , "\"*/%s\"\n", label) ;
        outputText( decoder->vocabulary ) ;
	}

    // Free up some memory
    for( int i=0 ; i<(n_frames+start_index) ; i++ )
        free( decoder_input[i] ) ;
    free( decoder_input ) ;
    decoder_input = NULL ;
    n_emission_probs = 0 ;
    n_features = 0 ;
}


void MlDecoderStaff::removeSentMarksFromActual( Vocabulary *vocabulary )
{
    if ( (n_actual_words == 0) || (vocabulary == NULL) )
        return ;
  
    // remove the sentence start word
    if ( actual_words[0] == vocabulary->sent_start_index )
    {
        for ( int j=1 ; j<n_actual_words ; j++ )
        {
            actual_words[j-1] = actual_words[j] ;
            actual_words_times[j-1] = actual_words_times[j] ;
        }
        n_actual_words-- ;
    }
    
    // remove the sentence end word
    if ( actual_words[n_actual_words-1] == vocabulary->sent_end_index )
        n_actual_words-- ;

    // remove any instances of silence
    if ( vocabulary->sil_index >= 0 )
    {
        for ( int j=0 ; j<n_actual_words ; j++ )
        {
            while ( (j<n_actual_words) && (actual_words[j] == vocabulary->sil_index) )
            {
                for ( int k=(j+1) ; k<n_actual_words ; k++ )
                {
                    actual_words[k-1] = actual_words[k] ;
                    actual_words_times[k-1] = actual_words_times[k] ;
                }
                n_actual_words-- ;
            }
        }
    }
}


void MlDecoderStaff::loadDataFile( FILE *archive_fd )
{
    // Make sure that the test_filename and data_format member variables
    //   have been configured.
    if ( (test_filename == NULL) && (archive_fd == NULL) )
        error("MlDecoderStaff::loadDataFile - test_filename/archive_fd not configured\n") ;

    // Free any existing data and reset the size-related member variables
    if ( decoder_input != NULL )
    {
        for( int i=0 ; i<n_frames ; i++ )
            free( decoder_input[i] ) ;
        free( decoder_input ) ;
        decoder_input = NULL ;
    }
    n_frames=0 ;
    n_features=0 ;
    n_emission_probs=0 ;
    
    // load the input htk data (ie. feature vectors) into a new IOHtk instance
    IOHTK *htk_data = ::new IOHTK( test_filename , true ) ; // ENZ do not use torch new operator

    // reorganise the IOHtk 1-dimesional data into a 2D array
    n_frames = htk_data->n_total_frames ;
    n_features = htk_data->frame_size ;
    decoder_input = (real **)Allocator::sysAlloc( n_frames * sizeof(real *) ) ;

    // read the HTK data into a sequence
    Sequence *temp_seq = ::new Sequence( n_frames , n_features ) ;  // ENZ do not use torch new operator
    htk_data->getSequence( 0 , temp_seq ) ;
        
    // copy the sequence data into the decoder_input array
    for ( int i=0 ; i<n_frames ; i++ )
    {
        decoder_input[i] = (real *)Allocator::sysAlloc( n_features * sizeof(real) ) ;
        memcpy( decoder_input[i] , temp_seq->frames[i] , n_features * sizeof(real) ) ;
    }    

    delete htk_data ;
    delete temp_seq ;

#ifdef DEBUG
    if ( (n_features==0) && (n_emission_probs==0) )
        error("MlDecoderStaff::loadDataFile - no data loaded\n") ;
#endif
}


void MlDecoderStaff::outputText( Vocabulary *vocab )
{
    if ( (test_filename == NULL) && (archive_offset<0) )
        return ;
    
    // We just want to output the actual result words - nothing more or less
    for ( int i=0 ; i<n_actual_words ; i++ )
        fprintf( output_fd , "%d %d %s\n" , 2*(actual_words_times[i]+1), 2*(actual_words_times[i]+1), vocab->getWord(actual_words[i]) ) ;
    fprintf( output_fd , ".\n") ;
    fflush( output_fd ) ;
}

