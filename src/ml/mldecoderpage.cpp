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

#include "mldecoderpage.h"

// torch
#include "Allocator.h"
#include "EditDistance.h"
#include "time.h"
#include "string_stuff.h"
#include "DiskXFile.h"

using namespace Torch;

MlDecoderPage::MlDecoderPage( char *datafiles_filename , char *expected_results_file , 
							   BeamSearchDecoder *decoder_ , bool remove_sil, 
							   bool output_res , char *out_fname )
{
    clock_t start_time , end_time ;

    if ( decoder_ == NULL )
        error("DBT::DBT - decoder_ is NULL\n") ;
    if ( (datafiles_filename == NULL) || (strcmp(datafiles_filename,"")==0) )
        error("DBT::DBT - datafiles_filename is undefined\n") ;
        
    total_time = 0.0 ;
    start_time = clock() ;
    decoder = decoder_ ;
    vocabulary = decoder->vocabulary ;
    n_tests = 0 ;
    tests = NULL ;
    archive_fd = NULL ;
	
	print_by_page = false;
	print_by_part = false;
	print_by_book = false;

    if ( (expected_results_file == NULL) || (strcmp(expected_results_file,"")==0) )
        have_expected_results = false ;
    else
        have_expected_results = true ;

    // Open the results file
    if ( (out_fname != NULL) && (strcmp(out_fname,"")!=0) )
    {
        if ( (output_fd=fopen( out_fname , "w" )) == NULL )
            error("DBT::DBT - error opening output file\n") ;
    }
    else
        output_fd = NULL ;

 
    // We have a separate input file for each test (eg. separate lna file for 
    //   each sentence).  'datafiles_filename' should contain a list of absolute 
    //   pathnames of the input files for all tests (1 abs filename per line). 
    // 'expected_results_file' is the ground truth transcriptions for each test
    //   in HTK MLF format (see below) (filenames must be absolute and must have
    //   a 3 letter extension (the extension gets ignored)).
    configureWithIndividualInputs( datafiles_filename , expected_results_file , remove_sil, output_res  );

    end_time = clock() ;
    total_time = (real)(end_time-start_time) / CLOCKS_PER_SEC ;
}


MlDecoderPage::~MlDecoderPage()
{
    if ( archive_fd != NULL )
        fclose( archive_fd ) ;

    if ( output_fd != NULL )
        fclose( output_fd ) ;

    if ( tests != NULL )
    {
        for ( int i=0 ; i<n_tests ; i++ )
            delete tests[i] ;
        free( tests ) ;
    }
}


void MlDecoderPage::setOutputOptions( bool print_by_page_, bool print_by_part_, bool print_by_book_ )
{
	print_by_page = print_by_page_;
	print_by_part = print_by_part_;
	print_by_book = print_by_book_;	
}


void MlDecoderPage::printStatistics( int i_cost , int d_cost , int s_cost )
{
    // Calculates the insertions, deletions, substitutions, accuracy
    //   and outputs. Also calculates the total time taken to decode (not including
    //   loading of datafiles).
    real decode_time=0.0 ;
    
    EditDistance totalRes , singleRes ;
    totalRes.setCosts( i_cost , d_cost , s_cost ) ;
    singleRes.setCosts( i_cost , d_cost , s_cost ) ;
	
	// Statistics by page and by print
	// Specifique au formattage des noms de fichier...
	EditDistance pageRes , partRes, bookRes ;
    pageRes.setCosts( i_cost , d_cost , s_cost ) ;
	partRes.setCosts( i_cost , d_cost , s_cost ) ;
    bookRes.setCosts( i_cost , d_cost , s_cost ) ;
	
	char page_name[1000];
	char part_name[1000];
	char book_name[1000];
	char current_name[1000];
	page_name[0]=0;
	part_name[0]=0;
	book_name[0]=0;
	
    for (int i=0 ; i<n_tests ; i++ )
    {
        decode_time += tests[i]->decode_time ;
        if ( tests[i]->actual_words != NULL )
        {
			singleRes.distance( tests[i]->actual_words , tests[i]->n_actual_words ,
                                tests[i]->expected_words , tests[i]->n_expected_words ) ;
			totalRes.add( &singleRes ) ;
		
			char *ptr;
			strcpy( current_name, tests[i]->test_filename );
			ptr = strrchr( current_name, '_' );
			if ( ptr )
				(*ptr) = 0; // page name in current_name
			
			if ( print_by_page )
			{
				if ( strcmp( current_name, page_name )!=0 )
				// new page
				{
					// print last page
					if ( page_name[0] )
						printResults( &pageRes, "PAGE", page_name );
					
					strcpy( page_name, current_name );
					pageRes.n_delete = 0;
					pageRes.n_insert = 0;
					pageRes.n_seq = 0;
					pageRes.n_subst = 0;
				}
				pageRes.add( &singleRes ) ;
			}
			
			
			ptr = strrchr( current_name, '_' );
			if ( ptr )
				(*ptr) = 0; // part name in current_name
				
			if ( print_by_part )
			{
				if ( strcmp( current_name, part_name )!=0 )
				// new part
				{
					// print last part
					if ( part_name[0] )
					{
						printf("-----------------------------------------\n"); 
						printResults( &partRes, "PART", part_name );
						printf("-----------------------------------------\n"); 
					}
					
					strcpy( part_name, current_name );
					partRes.n_delete = 0;
					partRes.n_insert = 0;
					partRes.n_seq = 0;
					partRes.n_subst = 0;
				}
				partRes.add( &singleRes ) ;
			}				
				
			ptr = strrchr( current_name, '_' );
			if ( ptr )
				(*ptr) = 0; // book name in current_name
				
			if ( print_by_book )
			{	
				if ( strcmp( current_name, book_name )!=0 )
				// new book
				{
					// print last book
					if ( book_name[0] )
					{
						printf("=========================================\n"); 
						printResults( &bookRes, "BOOK", book_name );
						printf("=========================================\n");
						printf("\n"); 
					}
					
					strcpy( book_name, current_name );
					bookRes.n_delete = 0;
					bookRes.n_insert = 0;
					bookRes.n_seq = 0;
					bookRes.n_subst = 0;
				}
				bookRes.add( &singleRes ) ;
			}
            
        }
    }
	
	if ( page_name[0] )
	{
		printResults( &pageRes, "PAGE", page_name );
	}
			
	if ( part_name[0] )
	{
		printf("-----------------------------------------\n"); 
		printResults( &partRes, "PART", part_name );
		printf("-----------------------------------------\n"); 
	}
			
	if ( book_name[0] )
	{
		printf("=========================================\n"); 
		printResults( &bookRes, "BOOK", book_name );
		printf("=========================================\n");
	}

    printf("\nTotal time spent actually decoding = %.2f secs\n", decode_time) ;
    printf("Total time spent configuring and running batch test = %.2f secs\n\n",total_time) ;
    DiskXFile xf(stdout);
    //totalRes.print( &xf ) ;
    //totalRes.printRatio( &xf ) ;
	printResults( &totalRes, "REC" );
    printf("\n") ;
}

void MlDecoderPage::printResults( EditDistance *distance, char *prefix_, char *name_ )
{
	if ( name_ && strrchr(name_, '/' ) )
		name_ = strrchr(name_, '/' ) + 1; // try to remove path
		
	char *name = "";
	if ( name_ )
		name = name_;

	printf("%s %5.2f | ACC %5.2f | I %5.2f | D %5.2f | S %5.2f || N %d | I %d | D %d | S %d | %s\n",
		prefix_,
		(distance->n_seq - distance->n_delete - distance->n_subst) * 100. / (real)distance->n_seq,
		distance->accuracy,
		distance->n_insert*100./distance->n_seq,
		distance->n_delete*100./distance->n_seq,
		distance->n_subst*100./distance->n_seq,
		distance->n_seq,
		distance->n_insert,
		distance->n_delete,
		distance->n_subst,
		name);
}

void MlDecoderPage::run()
{
    clock_t start_time , end_time ;
    real decode_time=0.0 ;

    start_time = clock() ;
	fprintf( output_fd , "#!MLF!#\n") ;
    for ( int i=0 ; i<n_tests ; i++ )
    {
		//fprintf( output_fd , "\"*/test_001.%d.0.lab\"\n", i) ;
        tests[i]->run( decoder , archive_fd ) ;
		decode_time += tests[i]->decode_time ;
    }
    end_time = clock() ;
    total_time += (real)(end_time-start_time) / CLOCKS_PER_SEC ;

    printf("Recognition total time = %.2f secs\n",decode_time) ;
	printf("Recognition time per staff = %.2f secs\n",decode_time / n_tests) ;

    if ( have_expected_results == true )
        printStatistics(7,7,10) ;   // HTK settings for Ins, Sub, Del calculations
}


void MlDecoderPage::configureWithIndividualInputs( char *datafiles_filename , char *expected_results_file , 
												   bool remove_sil, bool output_res )
{    
    FILE *datafiles_fd=NULL , *results_fd=NULL ;
    char line[1000] , fname[1000] , result_fname[1000] , res_word[1000] , *ptr ;
    int temp_result_list[1000] , n_sentence_words=0 , i=0 , test_index ;
    char **filenames=NULL ;
    bool have_mlf=false ;

    // Open the file containing the names of the data files we want to run tests for.
    datafiles_fd = fopen( datafiles_filename , "r" ) ;
    if ( datafiles_fd == NULL )
        error("RecDecoderBatch::configure - error opening datafiles file") ;

    // Open the file containing the expected results for each test.
    // We assume that the format is as per HTK MLF format.
    // Note that the filename line must be enclosed in "". 
    if ( have_expected_results == true )
    {
        if ( (results_fd = fopen( expected_results_file , "r" )) == NULL )
            error("RecDecoderBatch::configureWII - error opening results file") ;

        // Read the first line of the results file to determine its type
        fgets( line , 1000 , results_fd ) ;
        if ( strstr( line , "MLF" ) )
            have_mlf = true ;
        else
			error("MlDecoderPage::configureWII - non MLF datafiles file") ;
    }
    
    // Determine the number of filenames present in the datafiles file
    n_tests=0 ;
    while ( fgets( line , 1000 , datafiles_fd ) != NULL )
    {
        if ( (sscanf(line,"%s",fname)==0) || (line[0] == '#') || (line[0] == '\n') ||
             (line[0] == '\r') || (line[0] == ' ') || (line[0] == '\t') )
            continue ;
        n_tests++ ;
        tests = (MlDecoderStaff **)Allocator::sysRealloc( tests ,
                                                   n_tests * sizeof(MlDecoderStaff *) ) ;
        tests[n_tests-1] = NULL ;
        filenames = (char **)Allocator::sysRealloc( filenames , n_tests * sizeof(char *) ) ;
        filenames[n_tests-1] = (char *)Allocator::sysAlloc( (strlen(fname)+1)*sizeof(char) ) ;
        strcpy( filenames[n_tests-1] , fname ) ;
    }

    if ( have_expected_results == true )
    {
        // Read each entry in the expected results file, find its corresponding
        //   filename in the temporary list of filename, create a MlDecoderStaff
        //   instance and add it to the list of tests.
        test_index = 0 ;
        while ( fgets( line , 1000 , results_fd ) != NULL )
        {
			if ( sscanf(line,"\"%[^\"]",result_fname) != 0 )
            {
                // remove the extension and path from the filename
                if ( (ptr=strrchr( result_fname , '/' )) != NULL )
                    memmove( result_fname , ptr+1 , strlen(ptr)+1 ) ;
                if ( (ptr=strrchr( result_fname , '.' )) != NULL )
                    *ptr = '\0' ;

                // find the filename in the temporary list of filenames
                for ( i=0 ; i<n_tests ; i++ )
                {
                    if ( strstr( filenames[i] , result_fname ) )
                    {
                        // We found the filename in the temporary list of filenames.
                        // Read the expected words.
                        n_sentence_words = 0 ;
                        fgets( line , 1000 , results_fd ) ;
                        while ( line[0] != '.' )
                        {
							int x, y;
							if ( strstr( line, " " ) == NULL )
								sscanf( line , "%s" , res_word );
							else
								sscanf( line , "%d %d %s" , &x, &y , res_word );
                            temp_result_list[n_sentence_words]=vocabulary->getIndex(res_word) ;
                            if ( temp_result_list[n_sentence_words] >= 0 )
                                n_sentence_words++ ;
                            fgets( line , 1000 , results_fd ) ;
                        }

                        // Now configure the next MlDecoderStaff instance 
                        //   with the details of the test.
                        if ( tests[i] != NULL )
                            error("MlDecoderStaff::configureWII - duplicate exp results\n");
                        tests[i] = new MlDecoderStaff() ;
                        tests[i]->configure( i , filenames[i] , n_sentence_words , 
                                temp_result_list , decoder->phone_models , 
                                remove_sil, output_res , output_fd ) ;
                        break ;
                    }
                }
            }
        }
        
        // Check that each element of 'tests' has been configured
        for ( i=0 ; i<n_tests ; i++ )
        {
            if ( tests[i] == NULL )
            {
                error( "MlDecoderStaff::configureWII - exp res not found for file %s\n" ,
                       filenames[i] ) ;
            }

            // this is useful to generate an in-order ref transcription file from an MLF file.
            //for ( int j=0 ; j<tests[i]->n_expected_words ; j++ )
            //    printf("%s ",vocabulary->getWord( tests[i]->expected_words[j] ) ) ;
            //printf("\n");
        }
    }
    else
    {
        for ( i=0 ; i<n_tests ; i++ )
        {
            tests[i] = new MlDecoderStaff() ;
            tests[i]->configure( i , filenames[i] , 0 , NULL ,
                                 decoder->phone_models , remove_sil, output_res , output_fd ) ;
        }
    }

    // Free the temporary list of filenames
    for ( i=0 ; i<n_tests ; i++ )
        free( filenames[i] ) ;
    free( filenames ) ; 
                    
    if ( have_expected_results == true )
        fclose( results_fd ) ;
    fclose( datafiles_fd ) ;
}


#ifdef DEBUG
void MlDecoderPage::outputText()
{
    printf("Number of tests = %d\n",n_tests) ;
    for ( int i=0 ; i<n_tests ; i++ )
    {
        printf("\nTest %d\n********\n",i) ;
        tests[i]->outputText() ;
        printf("\n") ;
    }
}
#endif

