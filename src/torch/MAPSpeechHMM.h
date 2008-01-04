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

#ifndef MAP_SPEECH_HMM_INC
#define MAP_SPEECH_HMM_INC

#include "SpeechHMM.h"

namespace Torch {

/** This class implements a special case of Hidden Markov Models that
    can be used to do connected word speech recognition for small
    vocabulary, for MAP adaptation

    @author Laurent Pugin (laurent.pugin@lettres.unige.ch)
*/
class MAPSpeechHMM : public SpeechHMM
{
  public:

    /** In order to create a SpeechHMM, we need to give a vector of #n_models_#
        #HMM#s as well as their corresponding name, a lexicon,
        an optional log_word_entrance_penalty and an optional trainer that can be
        used to initialize each model independently.
    */
    MAPSpeechHMM(int n_models_, HMM **models_, LexiconInfo* lex_, EMTrainer** model_trainer_ = NULL);

    virtual void Init(DataSet* data_);

    virtual ~MAPSpeechHMM();
};


}

#endif
