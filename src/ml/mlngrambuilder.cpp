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

//#include "Allocator.h"
#include "MlNgramBuilder.h"

#include <stdlib.h>
#include <stdio.h>

using namespace Torch;


MlNgramBuilder::MlNgramBuilder( int order_ ,  Vocabulary *vocab_ )
{
#ifdef DEBUG
    if ( order_ < 1 )
        error("::MlNgramBuilder - order_ cannot be < 1\n") ;
    if ( order_ > MAX_ORDER)
        error("::MlNgramBuilder - order_ cannot be > MAX_ORDER\n") ;
#endif
	
	int i;
	
    order = order_ ;
    vocab = vocab_ ;
	root = NULL;

	root = (MlNgramEntry*)allocator->alloc( sizeof(MlNgramEntry) );
	root->n_entries = 0;
	root->count = 0;
	root->level = 0;
	root->entries = NULL;

	// static values
	// array of int for count at each level
	MlNgramEntry::ngram_count = (int*)allocator->alloc( order * sizeof(int) );
	memset( MlNgramEntry::ngram_count, 0, order * sizeof(int) );
	// NGram order (highest level)
	MlNgramEntry::order = order;
	// root
	MlNgramEntry::root = root;

	// Init with all vocabulary entries
	root->n_entries = vocab->n_words;
    root->entries = (MlNgramEntry*)allocator->alloc( (vocab->n_words)*sizeof(MlNgramEntry) ) ;
	MlNgramEntry empty( 1 ); // for values initilization
	for (i = 0; i < root->n_entries; i++)
	{
		(MlNgramEntry::ngram_count[0])++;
		root->entries[i] = empty; // initilization is not done by the allocator
		root->entries[i].word = i;
	}
}


void MlNgramBuilder::loadFile( const char *wrdtrns_fname, const char *output_fname, const char *data_fname  )
{
	char line[1000], res_word[1000];
	int size;
	int words[MAX_ORDER];
	FILE *wrdtrns_fd=NULL;
	bool sentence;

    wrdtrns_fd = fopen( wrdtrns_fname , "r" ) ;
    if ( wrdtrns_fd == NULL )
        error("MlNgramBuilder::MlNgramBuilder - error opening wrdtrns file") ;

	fgets( line , 1000 , wrdtrns_fd ) ;
    if ( !strstr( line , "MLF" ) )
		error("MlNgramBuilder::MlNgramBuilder - error opening wrdtrns file is not a HTK MLF file") ;


	size = 0;
	sentence = false;
	memset( words, 0, MAX_ORDER * sizeof( int ) );
    while ( fgets( line , 1000 , wrdtrns_fd ) != NULL )
    {
		// skip line
        if ( (line[0] == '#') || (line[0] == '\n') ||
             (line[0] == '\r')|| (line[0] == '\t') || (line[0] == '"') )
			 continue;

		if (line[0] == '.') 
		{
			// This is the end of a sentence
			// First add END word if it exists in vocabulary
			if ( vocab->sent_end_index != -1 )
			{
				root->count++;
				words[size] = vocab->sent_end_index;
				root->addWords( words, size, allocator );
				memmove( words, words+1, (MAX_ORDER - 1) * sizeof(int)  );
			}
			// Add the end of the words chain
			// Wn-2 Wn-1 Wn END
			// Wn-1 Wn END
			// ....
			while ( size > 0 )
			{
				size--;
				root->addWords( words, size, allocator );
				memmove( words, words+1, (MAX_ORDER - 1) * sizeof(int)  );	
			}
			// clear
			size = 0;
			sentence = false;
			memset( words, 0, MAX_ORDER * sizeof( int ) );
            continue ;
		}

		
		if ( !sentence )
		{
			// New sentence
			sentence = true;
			// First add START word if it exists in vocabulary
			if ( vocab->sent_start_index != -1 )
			{
				root->count++;
				words[size] = vocab->sent_start_index;
				if ( size == order - 1 ) // This will only happen if order == 1
					root->addWords( words, size, allocator );
				else
					size++;
			}
		}

		sscanf( line , "%s" , res_word ) ;
		int word = vocab->getIndex( res_word );
		root->count++;
		words[size] = word;

		if ( size == order - 1)
		{
			// This size of the words chain is correct
			root->addWords( words, size, allocator );
			memmove( words, words+1, (MAX_ORDER - 1) * sizeof(int) );
		}
		else
			size++;
    }

	fclose( wrdtrns_fd );

	//outputText();
	saveData( data_fname );

	// discount
	root->discount();
	//outputText();

	// backoff 
	memset( words, 0, MAX_ORDER * sizeof(int) );
	root->backoff( words, 0 );

	//outputText();
	//checkGram();

	printGram( output_fname );
}


void MlNgramBuilder::reloadFile( const char *fname )
{
	
	char line[1000], res_word[1000];
	int level, number;
	int words[MAX_ORDER];
	FILE *fname_fd=NULL;
	
    fname_fd = fopen( fname , "r" ) ;
    if ( fname_fd == NULL )
        error("MlNgramBuilder::relaodFile - error opening wrdtrns file") ;

	fgets( line , 1000 , fname_fd ) ;
    if ( !strstr( line , "NGRAM" ) )
		error("MlNgramBuilder::relaodFile - error opening wrdtrns file is not a NGRAM file") ;
		
    sscanf( line , "# %s %d" , res_word, &level ) ;
	if ( level < order )
	{
		warning("MlNgramBuilder::relaodFile - order of previous model is smaller, order reduced to it") ;
		order = level;
	}

	level = 0;
	number = 0;
	memset( words, 0, MAX_ORDER * sizeof( int ) );
    while ( fgets( line , 1000 , fname_fd ) != NULL )
    {
		// skip line
        if ( (line[0] == '#') || (line[0] == '\n') ||
             (line[0] == '\r')|| (line[0] == '\t') || (line[0] == '"') )
			 continue;

		if ( sscanf( line , "%d %s %d" , &level , res_word, &number ) != 3 )
			continue;
			
		if ( level == 0 )
			root->count += number; // number of words in the data to be reloaded
			
		int word = vocab->getIndex( res_word );
		words[level] = word;
		root->addWords( words, level, allocator, number );
    }

	fclose( fname_fd );

	//outputText();
}


MlNgramBuilder::~MlNgramBuilder()
{
}


void MlNgramBuilder::saveData( const char *data_fname )
{
	FILE *out_fd = NULL;
	int i;

	if ( !root )
		return;

	out_fd = fopen( data_fname, "w" );
	if ( out_fd == NULL )
		error("MlNgramBuilder::saveData - error opening data file") ;
	
	// header
	fprintf( out_fd, "# NGRAM %d\n", order );

	for (i = 0; i < root->n_entries; i++)
	{
		root->entries[i].outputData( out_fd, vocab );
	}
	
	fclose( out_fd );
}

void MlNgramBuilder::printGram( const char *output_fname )
{	
	FILE *out_fd = NULL;
	int i, j;
	char output[MAX_WORD_LEN * 2];

	if ( !root )
		return;

	out_fd = fopen( output_fname, "w" );
	if ( out_fd == NULL )
		error("MlNgramBuilder::printGram - error opening output file") ;
	
	// ARPA header
	fprintf( out_fd, "\\data\\\n");
	for (j = 0; j < order; j++)
		fprintf( out_fd, "ngram %d=%d\n", j+1, MlNgramEntry::ngram_count[j] );
	fprintf( out_fd, "\n");

	for (j = 0; j < order; j++)
	{
		// each order
		fprintf( out_fd, "\\%d-grams:\n", j+1 );
		for (i = 0; i < root->n_entries; i++)
		{
			// each entry
			output[0] = 0;
			root->entries[i].printValues( out_fd, j+1, output, vocab );
		}
		fprintf( out_fd, "\n");
	}

	fprintf( out_fd, "\\end\\\n\n");
	
	fclose( out_fd );
}

#ifdef DEBUG
void MlNgramBuilder::checkGram( )
{

	if ( !root )
		return;

	char output[MAX_WORD_LEN * 2];
	output[0] = 0;

	int words[MAX_ORDER];
	memset( words, 0, MAX_ORDER * sizeof(int) );

	for (int j = 0; j < order - 1; j++)
	{
		// each order
		print("\\%d-grams:\n", j+1 );
		for (int i = 0; i < root->n_entries; i++)
		{
			words[0] = root->entries[i].word;
			root->entries[i].checkValues( j+1, output, words, root, vocab );
		}
		print("\n");
	}

}
#endif

#ifdef DEBUG
void MlNgramBuilder::outputText( )
{
	if ( !root )
		return;

	int i;

	for (i = 0; i < root->n_entries; i++)
	{
		root->entries[i].outputText( vocab );
	}
}
#endif

