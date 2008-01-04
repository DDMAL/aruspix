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

#include "MAPSpeechHMM.h"
#include "log_add.h"

namespace Torch {

MAPSpeechHMM::MAPSpeechHMM(int n_models_, HMM **models_, LexiconInfo* lex_, EMTrainer** model_trainer_) : 
	SpeechHMM::SpeechHMM(n_models_, models_, lex_, model_trainer_)
{
}

void MAPSpeechHMM::Init(DataSet* data_)
{
  // if alignment information is given in the dataset, use it.
  // otherwise, do a linear alignment along the states

  data = data_;

  add_to_targets = 0;
  if (!phoneme_targets) {
    if (lexicon->vocabulary->sil_index >= 0)
      add_to_targets++;
    if (lexicon->vocabulary->sent_end_index >= 0)
      add_to_targets++;
  }

  if (initialize) {
    // initialize model
    ExampleFrameSelectorDataSet** sub_dataset = (ExampleFrameSelectorDataSet**)allocator->alloc(sizeof(ExampleFrameSelectorDataSet*)*n_models);
    for (int m=0;m<n_models;m++) {
      sub_dataset[m] = new(allocator) ExampleFrameSelectorDataSet(data_);
    }
    splitDataSet(data_, sub_dataset);

    for (int m=0;m<n_models;m++) {
      // the transitions and emission parameters will be set in each model
      if (sub_dataset[m]->n_examples==0) {
          message("KMeans initialization of model %d with all data",m);
          models[m]->setDataSet(data_);
      } else if (model_trainer) {
          message("HMM initialization of model %d with own aligned data",m);
          model_trainer[m]->train(sub_dataset[m], initial_models_trainer_measurers);
      } else {
          message("KMeans initialization of model %d with own aligned data",m);
          models[m]->setDataSet(sub_dataset[m]);
      }
    }
  } else {
    // we still need to set the dataset of each model
    for (int m=0;m<n_models;m++) {
      models[m]->setDataSet(data_);
    }
  }
}

MAPSpeechHMM::~MAPSpeechHMM()
{
}

}

