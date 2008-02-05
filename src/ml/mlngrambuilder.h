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

#ifndef LMNGRAMBUILDER_INC
#define LMNGRAMBUILDER_INC

#include "general.h"
#include "log_add.h"
#include "Vocabulary.h"
#include "LMNGramEntry.h"

namespace Torch {


/**
    This class is the main class for N-gram language modelling.
    After language model entries have been added, lookups with full
    backoff can be performed.
      
    @author Laurent Pugin (laurent.pugin@lettres.unige.ch)
*/

class LMNGramBuilder : public Object
{
public:
    /// The order of the N-Gram
    int order ;
    Vocabulary *vocab ;

	//LMNGramEntry *entries;
	LMNGramEntry *root;

    /* Constructors / destructor */

    /// Creates an empty N-Gram data structure. 'n_' is the N-gram order.
    LMNGramBuilder( int n_ , Vocabulary *vocab_ ) ;
    virtual ~LMNGramBuilder() ;

    /* Methods */
	
	void loadFile( const char *wrdtrns_fname, const char *output_fname, const char *data_fname ) ;
	
	void reloadFile( const char *fname ) ;

	void printGram( const char *output_fname );
	
	void saveData( const char *data_fname );


    /// Adds a new entry to the N-gram.
    /// 'order' is the order of the entry (ie. 2 for a bigram entry).
    /// The 'words' array needs to be in oldest-word-first order.
    ///   ([w1 w2 w3] for trigram entry).
    /// 'prob' is the log probability for the entry
    /// 'bo_wt' is the log back-off weight for the entry. If order is
    ///   equal to the LMNGramBuilder order (n), then 'bo_wt' is ignored.
    //void addEntry( int order , int *words , real prob , real bo_wt=LOG_ZERO ) ;

    /// Finds the N-gram probability of a given word sequence, with full
    ///   backoff.
    /// 'order' is the number of words in the 'words' array.
    /// The ordering in 'words' is W3,W2,W1,W4 for a 4-gram query.
    ///   (ie. for the query : what is P(W4|W1,W2,W3)?)
    //real getLogProbBackoff( int order , int *words ) ;

#ifdef DEBUG
	void checkGram();
#endif

#ifdef DEBUG
    void outputText() ;
#endif
};


}

#endif
