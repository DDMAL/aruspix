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

#ifndef __CLASS_MAPPER_H__
#define __CLASS_MAPPER_H__

#include "general.h"
#include "Vocabulary.h"

using namespace Torch;

/** 
*/

class ClassMapper
{
public:
	int n_map ;
    int *map;
	real *word_in_class_prob;
    int sent_start_index ;
    int sent_end_index ;
    int sil_index ;
	
	/* Constructors / Destructor */

    /// Creates the vocabulary.
    /// 'lex_fname' is the name of the lexicon file containing the pronunciations to be
    ///   recognised.  The format is the standard "word(prior) ph ph ph" format
    ///   where the (prior) is optional.
    /// 'sent_start_word' and 'sent_end_word' are the words that will start and
    ///   end every recognised utterance. 
	ClassMapper( const char *map_fname , Vocabulary *vocabulary1_ , Vocabulary *vocabulary2_ ) ;
	// dummy mapper for class model without mapping i.e a normal model
	ClassMapper( Vocabulary *vocabulary_ ) ;
	virtual ~ClassMapper() ;

	/* Methods */
    
    /// Adds a word to the vocabulary. Maintains alphabetic order. Does not add
    ///   duplicate entries.
	void addWord( char *word ) ;

    /// Returns the word given the index into the vocabulary
	char *getWord( int index ) ;
    
    /// Returns the index of a given word.  If 'guess' is defined, then the
    ///   words at indices of 'guess' and 'guess+1' are checked for a match
    ///   before the rest of the vocab is searched.
    int getIndex( char *word , int guess=-1 ) ;

#ifdef DEBUG
	void outputText() ;
#endif
};

#endif
