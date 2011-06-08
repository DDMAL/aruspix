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

#include "ClassMapper.h"
#include "ctype.h"
#include "log_add.h"
#include "DiskXFile.h"


using namespace Torch;


ClassMapper::ClassMapper(  const char *map_fname , Vocabulary *vocabulary1_ , Vocabulary *vocabulary2_  )
{

    if ( (map_fname == NULL) || (strcmp(map_fname,"")==0) )
        error("ClassMapper::ClassMapper - map filename undefined\n") ;
    if ( (vocabulary1_ == NULL) )
        error("ClassMapper::ClassMapper - vocabulary1 is NULL\n") ;
    if ( (vocabulary2_ == NULL) )
        error("ClassMapper::ClassMapper - vocabulary2 is NULL\n") ;

    DiskXFile lex_fd( map_fname , "r" ) ;
    char line[1000], *line_ptr, *wrd, *map_wrd;

    sent_start_index = -1 ;
    sent_end_index = -1 ;
    sil_index = -1 ;
    
    // Allocate the 'map' array
	n_map = vocabulary1_->n_words;
    map = (int *)Allocator::sysAlloc( n_map * sizeof(int) ) ;

	// Allocate the word_in_class_prob
    word_in_class_prob = (real *)Allocator::sysAlloc( n_map * sizeof(real) ) ;

    // Return to the start of the file.
    lex_fd.seek( 0 , SEEK_SET ) ;
    
    // Add words to the vocabulary.
    // Do not add duplicates.
    // Maintain alphabetical order.
	line_ptr = line;
	int n_expected_words = 0 ;
    while ( lex_fd.gets( line , 1000 ) != NULL )
    {
        if ( line[0] == '#') 
            continue ;

		wrd = strtok( line, "\r\n\t " );
		map_wrd = strtok( NULL, "\r\n\t " );
		map[ vocabulary1_->getIndex( wrd ) ] = vocabulary2_->getIndex( map_wrd );
		// word probability given the class
		real prob = 0.0;
		char *next = strtok( NULL, "\r\n\t " );
		if ( next )
			prob = (real)atof( next );
		word_in_class_prob[ vocabulary1_->getIndex( wrd ) ] = prob;

		n_expected_words++;

		//printf("%d %s %s (%d)\n", n_expected_words, wrd, map_wrd, vocabulary2_->getIndex( map_wrd ));
	}
    
   if ( vocabulary1_->n_words != n_expected_words )
        error("ClassMapper::ClassMapper - all words not mapped.\n") ;

   /*

    sent_start_index = -1 ;
    if ( (sent_start_word != NULL) && (strcmp(sent_start_word,"") != 0) )
    {
        for ( int i=0 ; i<n_words ; i++ )
        {
            if ( strcmp( words[i] , sent_start_word ) == 0 )
            {
                if ( sent_start_index >= 0 )
                    error("ClassMapper::ClassMapper - duplicate start words\n") ;
                sent_start_index = i ;
            }
        }
    }
    sent_end_index = -1 ;
    if ( (sent_end_word != NULL) && (strcmp(sent_end_word,"") != 0) )
    {
        for ( int i=0 ; i<n_words ; i++ )
        {
            if ( strcmp( words[i] , sent_end_word ) == 0 )
            {
                if ( sent_end_index >= 0 )
                    error("ClassMapper::ClassMapper - duplicate end words\n") ;
                sent_end_index = i ;
            }
        }
    }
    sil_index = -1 ;
    if ( (sil_word != NULL) && (strcmp(sil_word,"") != 0) )
    {
        for ( int i=0 ; i<n_words ; i++ )
        {
            if ( strcmp( words[i] , sil_word ) == 0 )
            {
                if ( sil_index >= 0 )
                    error("ClassMapper::ClassMapper - duplicate end words\n") ;
                sil_index = i ;
            }
        }
    }

    if ( n_words != total_n_words )
        error("ClassMapper::ClassMapper - did not get expected n_words\n") ;

	*/
}



ClassMapper::ClassMapper( Vocabulary *vocabulary_  )
{
    if ( (vocabulary_ == NULL) )
        error("ClassMapper::ClassMapper - vocabulary is NULL\n") ;

    sent_start_index = -1 ;
    sent_end_index = -1 ;
    sil_index = -1 ;
    
    // Allocate the 'map' array
	n_map = vocabulary_->n_words;
    map = (int *)Allocator::sysAlloc( n_map * sizeof(int) ) ;

	// Allocate the word_in_class_prob
    word_in_class_prob = (real *)Allocator::sysAlloc( n_map * sizeof(real) ) ;

	// just fill the map
	int i;
	for(i = 0; i < n_map; i++)
	{
		map[i] = i;
		word_in_class_prob[i] = 0.0;
	}

}

ClassMapper::~ClassMapper()
{
	if ( map )
		free( map );
}

/*
void ClassMapper::addWord( char *word )
{
    int cmp_result=0 ;

	if ( word[0] == '#' )
	{
		// The string is a comment so don't add to vocabulary
		return ;
	}
   
    if ( n_words > 0 )
        cmp_result = strcmp( words[n_words-1] , word ) ;
        
    if ( (cmp_result < 0) || (n_words == 0) )
    {
        // The new word belongs at the end of the list
        // Allocate memory in the list of words for the new word   
        words[n_words] = (char *)allocator->alloc( (strlen(word)+1) * sizeof(char) ) ;
        strcpy( words[n_words] , word ) ;
        n_words++ ;
        return ;
    }
    else if ( cmp_result > 0 )
    {
        // Find the place in the list of words where we want to insert the new word
        for ( int i=0 ; i<n_words ; i++ )
        {
            cmp_result = strcmp( words[i] , word ) ;
            if ( cmp_result > 0 )
            {
                // Shuffle down all words from i onwards and place the
                //   new word in position i.

                // Allocate memory in the list of words for the new word   
                for ( int j=n_words ; j>i ; j-- )
                    words[j] = words[j-1] ;
                words[i] = (char *)allocator->alloc( (strlen(word)+1) * sizeof(char) ) ;
                strcpy( words[i] , word ) ;
                n_words++ ;
                return ;
            }
            else if ( cmp_result == 0 )
            {
                // the word is already in our vocab - don't duplicate
                return ;
            }
        }
    }
    else
    {
        // The word is already at the end of the list - don't duplicate
        return ;
    }

    // If we make it here there is a problem
    return ;
}


char *ClassMapper::getWord( int index )
{
    if ( (index<0) || (index>=n_words) )
        error("ClassMapper::getWord - index out of range\n") ;
	else
		return words[index] ;
    return NULL ;
}


int ClassMapper::getIndex( char *word , int guess )
{
    // We assume that the list of words is in ascending order so 
    //   that we can do a binary search.
    int min=0 , max=(n_words-1) , curr_pos=0 ;
    int cmp_result=0 ;
   
    // If guess is valid, do a quick check to see if the word is where
    //   the caller expects it to be - either at guess or at guess+1
    if ( (guess >= 0) && (guess<n_words) )
    {
        if ( strcmp(word,words[guess]) == 0 ) 
            return guess ;
        else if ( ((guess+1)<n_words) && (strcmp(word,words[guess+1])==0) )
            return guess+1 ;
    }
        
    while (1)
    {
        curr_pos = min+(max-min)/2 ;
        cmp_result = strcmp( word , words[curr_pos] ) ;
        if ( cmp_result < 0 )
            max = curr_pos-1 ;
        else if ( cmp_result > 0 )
            min = curr_pos+1 ;
        else
            return curr_pos ;
            
        if ( min > max )
            return -1 ;
    }

    return -1 ;
}

*/


#ifdef DEBUG
void ClassMapper::outputText()
{
    //printf("** START VOCABULARY - n_words=%d start_index=%d end_index=%d sil_index=%d**\n" , 
    //                                 n_words , sent_start_index , sent_end_index , sil_index ) ;
    //for ( int i=0 ; i<n_words ; i++ )
    //    printf("%s\n",words[i]) ;
    //printf("** END VOCABULARY **\n") ;
}
#endif
