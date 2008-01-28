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

#ifndef __CLASS_MODEL_H__
#define __CLASS_MODEL_H__

#include "ClassMapper.h"

// torch
#include "general.h"
#include "DecodingHypothesis.h"
#include "Vocabulary.h"
#include "LMNGram.h"

using namespace Torch;


/** 
    This object implements an n-gram language model. The n-gram data structures
    are encapsulated in the ngram member variable (see LMNGram class). Methods
    are provided to read a LM file in ARPA format or in Noway binary format.
    A method is provided to calculate a LM prob (with backoff) for a given 
    sequence of words.
      
    @author Darren Moore (moore@idiap.ch)
*/

class ClassModel
{
public:
    int order ;
    int n_words ;
    Vocabulary *vocabulary ;
	Vocabulary *notes_vocabulary; // used only with interval_model : vocabulary and ngram are 
								  // 'intervals' but mapper and notes_vocabulary are 'notes'
    LMNGram *ngram ;
    real lm_scaling_factor ;
	real lm_class_factor ; // this factor based on scaling factor and classes number in the music model
						   // is must be recalculated each time a class is added in the model
	real prob; // keep last calculated probability for general scaling

    bool lm_has_start_word ;
    bool lm_has_end_word ;

	ClassMapper *mapper;

	bool interval_model;


    /* constructors / destructor */

    /// Creates the language model. 
    /// 'order_' is the order of the LM (eg. 3 for trigram).
    ClassModel(  Vocabulary *vocabulary_ , 
		char *voc_fname, char *lm_fname , char *map_fname, 
		int order_ , real lm_scaling_factor_=1.0, 
		bool interval_model_=false, char *voc_notes_fname_=NULL ) ; 
    /// Constructor for a class model without mapping i.e a normal model
    ClassModel(  Vocabulary *vocabulary_ , char *lm_fname ,  
		int order_ , real lm_scaling_factor_=1.0 ) ; 
    virtual ~ClassModel() ;

    /* methods */
    /// Calculates the language model probability (with backoff) of the word
    ///   sequence stored in the hypothesis pointed to by 'word_end_hyp'.
    //real calcLMProb( DecodingHypothesis *word_end_hyp ) ;
    
    /// Calculates the language model probability (with backoff) of 'next_word'
    ///   given the previous word sequence stored in the hypothesis pointed to 
    ///   by 'prev_word_end_hyp'.
    //real calcLMProb( DecodingHypothesis *prev_word_end_hyp , int next_word ) ;

	/// Calculates the class model probability (with backoff) of the word sequence
	real getLogProbBackoff( int order, int *words );

	/// Calculates the class model probability of the word within the class model P(w|c)
	/// This information is stored in the mapper
	real getLogProbClass( int word );

    /// Creates a language model from an ARPA format file. Internal function.
    void readARPA( FILE *arpa_fd ) ;

	// Return the mapping vocabulary which is notes_vocabulary if interval_model
	Vocabulary *getVocabulary();

	// In a interval_model, extract the pitch information (8 bits) and the value
	// information (4 bits) from a word. 
	// 'wrd_map' is the index in the notes_vocabulary
	void getPitch( int wrd_map, int *pitch, int *value );

	// In a interval_model, generate a interval code from 2 notes (pitch and value)
	// code is : value (4bits) | last_value (4bits) | pitch - last_pitch (8bits)
	// The code is written is interval
	void getInterval( char *interval, int pitch, int value, int last_pitch, int last_value );

	// In a interval_model, retrieve the vocabulary index from a notes_vocabulary index
	// Word must exists in both vocabularies (i.e. SP_START, SP_END, ...)
	int getIndexFromNote( int note_index );

#ifdef DEBUG
    void outputText() ;
#endif
};


#endif
