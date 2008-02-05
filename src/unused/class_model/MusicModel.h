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

#ifndef __MUSIC_MODEL_H__
#define __MUSIC_MODEL_H__

#include "ClassModel.h"

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

class MusicModel
{
public:
    int n_words ;
    Vocabulary *vocabulary ;
	ClassModel *general_model ; // general model - used only if none of the class model returned a probability

    bool lm_has_start_word ;
    bool lm_has_end_word ;
	real main_scaling_factor;
	real sum_class_factors; // keep sum of class scaling_factors to compute class_factor when a class is added

	ClassModel **class_models;
	int n_class_models;

    /* constructors / destructor */

    /// Creates the language model. 
    /// 'order_' is the order of the LM (eg. 3 for trigram).
    MusicModel( Vocabulary *vocabulary_ , real main_scaling_factor_ = 1.0 ) ; 
    virtual ~MusicModel() ;
 
    /* methods */
    /// Calculates the language model probability (with backoff) of the word
    ///   sequence stored in the hypothesis pointed to by 'word_end_hyp'.
    real calcLMProbDelayed( DecodingHypothesis *word_end_hyp ) ;
    
    /// Calculates the language model probability (with backoff) of 'next_word'
    ///   given the previous word sequence stored in the hypothesis pointed to 
    ///   by 'prev_word_end_hyp'.
    real calcLMProb( DecodingHypothesis *prev_word_end_hyp , int next_word ) ;

	/// Add a class model with vocabulary voc_fname, class model in cm_fname (ARPA)
	/// if interval, compute note interval probabilty
	void addClassModel( char *voc_fname, char *cm_fname, char *map_fname, 
		 int order_,  real lm_scaling_factor_=1.0, 
		 bool interval_model_=false, char *voc_notes_fname_=NULL );
	// Add a non-mapped class model
	void addClassModel( char *cm_fname, int order_,  real lm_scaling_factor_=1.0 );

#ifdef DEBUG
    void outputText() ;
#endif
};


#endif
