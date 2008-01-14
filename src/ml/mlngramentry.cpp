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

#include "Allocator.h"
#include "LMNGramEntry.h"

namespace Torch {

int *LMNGramEntry::ngram_count = NULL ;
int LMNGramEntry::order = 1;
LMNGramEntry *LMNGramEntry::root = NULL ;


LMNGramEntry::LMNGramEntry( int level_ )
{
#ifdef DEBUG
    if ( level_ < 1 )
        error("LMNGramEntry::LMNGramEntry - level_ cannot be < 1\n") ;
#endif

	level = level_;
	bow = 0.0;
	prob = 0.0;
	entries = NULL;
	n_entries = 0;
	count = 0;
}


LMNGramEntry::~LMNGramEntry()
{
}

real LMNGramEntry::getProb( int *words_, int level_ )
{
	int insert;
	if ( n_entries == 0 )
		return 0;

	LMNGramEntry *entry ;
	if ( (entry = this->findWord( words_[0], &insert )) == NULL )
		return 0;
		
	if ( level == level_ )
		return entry->prob;
	else
		return entry->getProb( words_ + 1, level_ );
}


real LMNGramEntry::getBow( int *words_, int level_ )
{
	int insert;
	if ( n_entries == 0 )
		return 0;

	LMNGramEntry *entry ;
	if ( (entry = this->findWord( words_[0], &insert )) == NULL )
		return 0;
		
	if ( level == level_ )
		return entry->bow;
	else
		return entry->getBow( words_ + 1, level_ );
}


void LMNGramEntry::discount()
{
	int i;

	for (i = 0; i < this->n_entries; i++)
	{	// first Witten-Bell discounting
		if ( entries[i].count == 0 )
			// no entry : Pbackoff = T / (N + T)
			// this may happen only at level one since all voc entries are added to the root
			entries[i].prob = (real)n_entries / (this->count + n_entries);
		else
			// entries : Pbackoff = c(W1W2...Wn) / (c(W1W2...Wn-1) + T(W1W2...Wn-1))
			entries[i].prob = (real)entries[i].count / (this->count + n_entries);

		// discount next level
		entries[i].discount( );
	}
}

void LMNGramEntry::backoff( int *words_, int level_ )
{
	int i, j;

	for (i = 0; i < this->n_entries; i++)
	{
		// We are not at the end
		// Add/Replace the current word in the chain
		words_[level_] = entries[i].word;
		if ( entries[i].n_entries > 0 )
		{
			// For all next words, get the probability | Wn-1 ... Wn-N+2 
			real sum_prob = 0;
			real sum_prob2 = 0;
			for (j = 0; j <	entries[i].n_entries; j++)
			{	
				// Add/Replace next word
				words_[level_+1] = entries[i].entries[j].word;
				// Get Probability from root for chain WITHOUT first word
				// -> words_ + 1
				sum_prob += root->getProb( words_ + 1, level_ );
				// Get Probability from root for chain WITH first word but one level less
				sum_prob2 += root->getProb( words_, level_ + 1 );
			}
			entries[i].bow = 1 - sum_prob2;
			//entries[i].bow = ((real)entries[i].n_entries / ((real)entries[i].count + (real)entries[i].n_entries));
			entries[i].bow /= (1 - sum_prob);
		}	
		entries[i].backoff( words_, level_ + 1 );
	}
}


void LMNGramEntry::printValues( FILE *out_fd_, int order_, char *output_ , Vocabulary *vocab_ )
{
	int i;
	int len = MAX_WORD_LEN * (level + 1);
	char *output = (char*)Allocator::sysAlloc( len );
	output[0]=0;

	strcat( output, output_ );
	strcat( output, " " );
	strcat( output, vocab_->getWord( word ) );

	if ( level == order_ )
	{
		if ( level == LMNGramEntry::order )
			// Last level - no bow
			fprintf( out_fd_, "%.09f %s\n", log10( prob ), output);
			//print( "%.09f %s\n", ( prob ), output);
		else if ( bow != 0 )
			fprintf( out_fd_, "%.09f %s %.09f\n", log10( prob ), output, log10(bow) );
			//print( "%.09f %s %.09f\n", ( prob ), output, (bow) );
		else
			// Add bow dummy value
			fprintf( out_fd_, "%.09f %s -99.9\n", log10( prob ), output );
			//print( "%.09f %s -99.9\n", ( prob ), output );
	}
	else
	{
		for (i = 0; i < this->n_entries; i++)
		{
			entries[i].printValues( out_fd_, order_, output, vocab_ );
		}
	}
	free( output );
}

#ifdef DEBUG
void LMNGramEntry::checkValues( int order_, char *output_ , int *words_, LMNGramEntry *previous , Vocabulary *vocab_ )
{
	if ( order_ == 0 )
		return; // don't check first level - nothing to check

	int i;
	int len = MAX_WORD_LEN * (level + 1);
	char *output = (char*)Allocator::sysAlloc( len );
	output[0]=0;

	strcat( output, output_ );
	strcat( output, " " );
	strcat( output, vocab_->getWord( word ) );

	if ( level == order_)
	{
		real sum_prob = 0.0;
		for (i = 0; i < vocab_->n_words; i++)
		{
			words_[ 1 ] = i;
			int insert;
			LMNGramEntry *entry ;
			if ( (entry = this->findWord( i, &insert )) == NULL )
			{
				real p = previous->getProb( words_ + 1, level );
				real bow = previous->getBow( words_ + 1, 0 ); 
				sum_prob += previous->getProb( words_ + 1, 0 ) *
					previous->getBow( words_ + 1, 0 );
				//print( "\t prob %.09f \tbow %.09f\n", p, bow);
			}
			else
				sum_prob += this->getProb( words_ + 1, level );
		}
		print( "%s %.09f\n", output, sum_prob);
	}
	else
	{
		for (i = 0; i < this->n_entries; i++)
		{
			words_[ 1 ] = entries[i].word;
			entries[i].checkValues( order_, output, words_ + 1, &entries[i], vocab_ );
		}
	}
	free( output );
}
#endif

void LMNGramEntry::addWords( int *words_, int size, Allocator *allocator, int number )
{
	int insert;
	LMNGramEntry *new_entry ;
	if ( (new_entry = this->findWord( words_[0], &insert )) == NULL )
    {
        // No we don't so we need to create an entry and add it to our 'entries' array.
        entries = (LMNGramEntry*)allocator->realloc( entries , 
                                         (n_entries+1)*sizeof(LMNGramEntry) ) ;

		//if ( size == 0)
			(LMNGramEntry::ngram_count[level])++;

        LMNGramEntry tmp_entry( level + 1 );
		tmp_entry.word = words_[0];

        // If the list is empty, just insert it straight off.  Also
        //   do an initial check to see if the word belongs at the end of the array
        if ( (n_entries == 0) || (tmp_entry.word > entries[n_entries-1].word) )
        {
            entries[n_entries] = tmp_entry;
			new_entry = &entries[n_entries];
        }
        else
        {
            // Place in the list of words at the insert position retruned by findWord
            memmove( entries+insert+1 , entries+insert , (n_entries-insert)*sizeof(LMNGramEntry) ) ;
            entries[insert] = tmp_entry ;
			new_entry = &entries[insert];
        }
        n_entries++ ;
    }
	if ( number == -1 )
		new_entry->count++;
	else if ( size == 0 )
		new_entry->count += number;

	if ( size > 0 )
	{
		new_entry->addWords( words_ + 1, size - 1, allocator, number );
	}
}

LMNGramEntry *LMNGramEntry::findWord( int word_ , int *insert_pos_ )
{
    // We assume that the list of words is in ascending order so 
    //   that we can do a binary search.
    int min=0 , max=(n_entries-1) , curr_pos=0 ;
	int i = 0;

	*insert_pos_ = 0;
    
    if ( n_entries == 0 )
        return NULL;

    if ( n_entries <= 10 )
    {
		if ( insert_pos_ )
			*insert_pos_ = i;

        // just do a linear search
        for (i=0 ; i<n_entries ; i++ )
        {
            if ( word_ < entries[i].word )
			{
				*insert_pos_ = i;
                return NULL ;
			}
            else if ( word_ == entries[i].word )
                return &entries[i] ;
        }
		*insert_pos_ = i;
		return NULL ;
    }
    else
    {
        // do a binary search
        while (1)
        {
            curr_pos = min+(max-min)/2 ;
            if ( word_ < entries[curr_pos].word )
                max = curr_pos-1 ;
            else if ( word_ > entries[curr_pos].word )
                min = curr_pos+1 ;
            else
                return &entries[curr_pos] ;

            if ( min > max )
			{
				*insert_pos_ = min;
                return NULL ;
			}
        }
    }
}

void LMNGramEntry::outputData( FILE *out_fd_, Vocabulary *vocab_ )
{
	int i;

	fprintf( out_fd_, "%d ", level - 1 );

	fprintf( out_fd_, "%s %d\n", vocab_->getWord( word ), count );

	for (i = 0; i < n_entries; i++)
		entries[i].outputData( out_fd_, vocab_ );

}

#ifdef DEBUG
void LMNGramEntry::outputText( Vocabulary *vocab_ )
{
	int i;

	print("%d\t", level - 1 );
	for (i = 0; i < level-1; i++)
		print("\t");

	print("%s %d\n", vocab_->getWord( word ), count );

	for (i = 0; i < n_entries; i++)
		entries[i].outputText( vocab_ );

}
#endif


}
