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

#ifndef AX_HMM_INC
#define AX_HMM_INC

#include "Object.h"
#include "LexiconInfo.h"
#include "EMTrainer.h"
#include "HMM.h"
#include "KMeans.h"
#include "MatDataSet.h"
#include "CmdLine.h"
#include "NLLMeasurer.h"
#include "Random.h"
#include "DiskHTKDataSet.h"
#include "HTKDataSet.h"
#include "SpeechHMM.h"
#include "string_utils.h"
#include "FileListCmdOption.h"
#include "PhoneInfo.h"
#include "MAPDiagonalGMM.h"

#include "ml.h"

using namespace Torch;

/** This class implements a special case of Hidden Markov Models that
    can be used to do connected symbol speech recognition for small
    vocabulary, for MAP adaptation

    @author Laurent Pugin (laurent.pugin@lettres.unige.ch)
*/
class MlHMM : public Object
{
public:
	
	// lexicon
	LexiconInfo *lex_input;
    char *spacing_model;
	char *sent_start_symbol;
	// data
	char **input_fnames;
	int n_inputs;
	char** target_fnames;
	int n_targets;
	// model
	SpeechHMM *init_shmm;
	
	// for MAP
	LexiconInfo *lex_data, *lex_output;
	int n_new; // mapping arrays
	int *new_in_data;
	int *new_in_output;
	
	// Options
	// initialization
	int n_gaussians;
	int n_states;
	bool train_separate;
	
	// data
	bool is_symbol;
	bool add_space_to_targets;
	int max_load;
	bool disk;

	// training
	real accuracy;
	real threshold;
	int max_iter;
	real prior;
	bool viterbi;
	
	// map
	real map_factor;
	bool no_learn_means;
	bool learn_var;
	bool learn_weights;
	bool adapt_separate;
	bool adapt_separate_set_data;
	
    MlHMM( );
	
	virtual void setLexicon( const char *model_fname_, const char *spacing_model_ , const char *lex_fname_ , const char *sent_start_symbol_=NULL );
				 
	virtual void setData( char **input_fnames_, int n_inputs_, char** target_fname_, int n_targets_ );
	
	virtual void init( const char *states_fname );
	
	virtual void train( const char *input_fname, const char *output_fname, XFile *measurer_file );
	
	// for MAP
	virtual void mapSetDataLexicon( const char *model_data_fname, const char *lex_data_fname,
		const char *model_output_fname, const char *lex_output_fname );
	
    virtual void mapInit( const char *states_fname );
	
	virtual void mapAdapt( const char *input_fname, const char *output_fname, XFile *measurer_file );
	
	// internal methods
	
	virtual void setTransitions(real** transitions, int model_n_states, int current_model, int space_model );
	
	virtual int* initStates(const char* filename, int n_models, int n_states, int space_model);
	
	virtual void initializeThreshold(DataSet* data,real* thresh, real threshold);
	
	virtual SpeechHMM *create(int* states, int n_models, int n_inputs, LexiconInfo *lex, real* thresh);
	
	virtual HMM** create(int* states, int* gaussians, int n_models, int n_inputs, LexiconInfo* lex, real* thresh);
	
	virtual HMM** readHTK(const char* filename, LexiconInfo* lex, real* thresh );
	
	void virtual writeHTK(const char* filename, SpeechHMM* shmm, char**phonemes );

    virtual ~MlHMM();
};


#endif
