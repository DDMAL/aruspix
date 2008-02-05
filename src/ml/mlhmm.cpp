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

#include "AruspixHMM.h"
#include "log_add.h"

using namespace Torch;

AruspixHMM::AruspixHMM()
{
	// initialization
	addIOption("number of gaussians", &n_gaussians, 10, "number of gaussians");
	addIOption("number of states", &n_states, 5, "number of states");
	addBOption("train separate", &train_separate, false, "first train separate models");
	
	// data
	addBOption("is symbol", &is_symbol, false, "targets are in symbol format");
	addBOption("add spacing", &add_space_to_targets, false, "add spacing at begining of target");
	addIOption("max load", &max_load, -1, "max number of examples to load for train");
	addBOption("disk", &disk, false, "keep data on disk");

	// training
	addROption("accuracy", &accuracy, 0.00001, "end accuracy");
	addROption("threshold", &threshold, 0.001, "variance threshold");
	addIOption("iterations", &max_iter, 25, "max number of iterations of HMM");
	addROption("prior", &prior, 0.001, "prior on the weights");
	addBOption("viterbi", &viterbi, false, "viterbi training (instead of EM)");
	
	// map
	addROption("map factor", &map_factor, 0.5, "adaptation factor [0-1]");
	addBOption("no means", &no_learn_means, false, "no enroll means");
	addBOption("learn var", &learn_var, false, "enroll var");
	addBOption("learn weights", &learn_weights, false, "enroll weights");
	addBOption("adapt separate", &adapt_separate, false, "adapt models separately");
	addBOption("adapt separate set data", &adapt_separate_set_data, false, "set data to non represented models when adapting  models separately");
	
	lex_input = NULL;
    spacing_model = NULL;
	sent_start_symbol = NULL;
	// data
	input_fnames = NULL;
	n_inputs = 0;
	target_fnames = NULL;
	n_targets = 0;
	// model
	init_shmm = NULL;
	
	// for MAP
	lex_data = NULL;
	lex_output = NULL;
	n_new = 0; // mapping arrays
	new_in_data = NULL;
	new_in_output = NULL;
	
}

void AruspixHMM::init( const char *states_fname )
{	
}

void AruspixHMM::train( const char *input_fname, const char *output_fname, XFile *measurer_file )
{
}

void AruspixHMM::mapInit( const char *states_fname )
{
	if ( n_new == 0 )
	{
		message("No new symbol to initialize\n");
		return;
	}
	
	if ( lex_data == NULL )
		error("The data lexicon has not been initialized\n" );
		
	//------------------ Create the DataSet ... ------------------

	DataSet* data2;
	if (disk)
		data2 = (DataSet*)new(allocator) DiskHTKDataSet( input_fnames, target_fnames, n_inputs, true, max_load, lex_data, is_symbol);
	else
		data2 = (DataSet*)new(allocator) HTKDataSet( input_fnames, target_fnames, n_inputs, true, max_load, lex_data, is_symbol);

	//------------------ Create the HMM of the new symbols ------------------

	real* thresh = (real*)allocator->alloc(data2->n_inputs*sizeof(real));
	initializeThreshold(data2,thresh,threshold);
	
	int n_models = lex_data->phone_info->n_phones;
	int* states = initStates( states_fname, n_models, n_states, lex_data->sil_index);
	
	init_shmm = create( states, n_models, data2->n_inputs, lex_data, thresh );
	init_shmm->setBOption("targets are phonemes",!is_symbol);
	
	//------------------ Initialize the models ------------------	
	


	
    ExampleFrameSelectorDataSet** sub_dataset = (ExampleFrameSelectorDataSet**)allocator->alloc(sizeof(ExampleFrameSelectorDataSet*)*n_models);
    for (int m=0;m<n_models;m++) {
      sub_dataset[m] = new(allocator) ExampleFrameSelectorDataSet(data2);
    }
	
    init_shmm->splitDataSet(data2 , sub_dataset);

    for (int i = 0; i < n_new; i++) 
	{
		int m = new_in_data[i];
		// the transitions and emission parameters will be set in each model
		if (sub_dataset[m]->n_examples==0) {
			message("KMeans initialization of model %d with all data",m);
			init_shmm->models[m]->setDataSet(data2);
		} else if (init_shmm->model_trainer) {
			message("HMM initialization of model %d with own aligned data",m);
			//models[m]->setDataSet(sub_dataset[m]);
			init_shmm->model_trainer[m]->train(sub_dataset[m], init_shmm->initial_models_trainer_measurers);
		} else {
			message("KMeans initialization of model %d with own aligned data",m);
			init_shmm->models[m]->setDataSet(sub_dataset[m]);
		}
    }
	
	/*
	
	init_shmm->setDataSet( data2 ); // initialize all
	
	CmdLine cmd;
	cmd.setWorkingDirectory(".");
	
	writeHTK( "./prout", init_shmm, lex_data->phone_info->phone_names );
	HMM** input = readHTK( "./prout", lex_data, thresh );
	init_shmm = new(allocator) SpeechHMM(n_models,input,lex_data);
	init_shmm->setBOption("initialize", false);
	
	message("Training new models");
	
	// Measurers on the training dataset
	MeasurerList measurers;
	char* n_nll = strConcat(2,"hmm_train_val_",viterbi ? "viterbi" : "em");
	allocator->retain(n_nll);

	NLLMeasurer nll_meas(init_shmm->log_probabilities, data2, cmd.getXFile(n_nll));
	measurers.addNode(&nll_meas);
	// The Gradient Machine Trainer
	EMTrainer trainer(init_shmm);
	trainer.setIOption("max iter", max_iter);
	trainer.setROption("end accuracy", accuracy);
	if (viterbi) 
		trainer.setBOption("viterbi", true);

	//=================== Let's go... ===============================

	trainer.train(data2, &measurers);
	*/
}


void AruspixHMM::mapAdapt( const char *input_fname, const char *output_fname, XFile *measurer_file )
{
	if ( (n_new > 0) && !init_shmm )
		error("The new symbols have not been initialized\n" );	
		
	DataSet* data;
	if (disk)
		data = (DataSet*)new(allocator) DiskHTKDataSet( input_fnames, target_fnames, n_inputs, true, max_load, lex_output, is_symbol);
	else
		data = (DataSet*)new(allocator) HTKDataSet( input_fnames, target_fnames, n_inputs, true, max_load, lex_output, is_symbol);
		
	real* thresh = (real*)allocator->alloc(data->n_inputs*sizeof(real));
	initializeThreshold(data,thresh,threshold); 
	
	HMM** input = readHTK( input_fname, lex_input, thresh );
	
	int n_models = lex_output->phone_info->n_phones;
	int i_new = 0;
	int i_input = 0;
	HMM** output = (HMM**)allocator->alloc(sizeof(HMM*)*n_models);
	//
	EMTrainer** hmm_trainer = (EMTrainer **)allocator->alloc(sizeof(EMTrainer*)*n_models);
	//
	
	MAPDiagonalGMM*** gmms = (MAPDiagonalGMM ***)allocator->alloc(sizeof(MAPDiagonalGMM**)*n_models);
	DiagonalGMM** priors = NULL;
	for (int i=0; i < n_models; i++)
	{
		bool new_model = false;
		if ( (i_new < n_new) && (i == new_in_output[i_new]) ) { // this is a new model
			output[i] = init_shmm->models[new_in_data[i_new]];
#ifdef __AXDEBUG__
			printf("New internal symbol %s\n", lex_data->phone_info->getPhone(new_in_data[i_new]) ) ;
#endif
			i_new++;
			new_model = true;
		} else {
			output[i] = input[i_input];
			i_input++;
		}
		int states = output[i]->n_states;
		priors = (DiagonalGMM**)output[i]->states;
		gmms[i] = (MAPDiagonalGMM**)allocator->alloc(sizeof(MAPDiagonalGMM*)*states);
		for (int j=1;j<states-1;j++) 
		{
			MAPDiagonalGMM* gmm = new(allocator)MAPDiagonalGMM(priors[j]);
			// set the training options
			if (thresh)
				gmm->setVarThreshold(thresh);
			if (prior>0)
				gmm->setROption("prior weights",prior);
		
			gmm->setROption("weight on prior",map_factor);
			gmm->setBOption("learn means",!no_learn_means);
			gmm->setBOption("learn variances",learn_var);
			gmm->setBOption("learn weights",learn_weights);
			if ( new_model )
			{
				//gmm->setROption("weight on prior",0);
				//gmm->setBOption("learn variances",true);
				//gmm->setBOption("learn weights",true);			
			}
			gmms[i][j] = gmm;
		}
		gmms[i][0] = NULL;
		gmms[i][states-1] = NULL;

		// MAP on new models only when not separate training !
		if ( !new_model || !adapt_separate )
			output[i]->states = (Distribution**)gmms[i];
			
		if ( /* !new_model && */ adapt_separate )	
		{
			hmm_trainer[i] = new(allocator)EMTrainer(output[i]);
			hmm_trainer[i]->setIOption("max iter", max_iter * 1);
			hmm_trainer[i]->setROption("end accuracy", accuracy * 1.0);
		}
		else
			hmm_trainer[i] = NULL;
	}
	
	SpeechHMM* shmm = new(allocator) SpeechHMM( n_models , output , lex_output, hmm_trainer );
	shmm->setBOption("targets are phonemes",!is_symbol);
	
	if ( adapt_separate )
	{
		ExampleFrameSelectorDataSet** sub_dataset = (ExampleFrameSelectorDataSet**)allocator->alloc(sizeof(ExampleFrameSelectorDataSet*)*n_models);
		for (int m=0;m<n_models;m++) {
		sub_dataset[m] = new(allocator) ExampleFrameSelectorDataSet(data);
		}
	
		shmm->splitDataSet(data , sub_dataset);

		for (int i = 0; i < n_models; i++) 
		{
			int m = i;
			// the transitions and emission parameters will be set in each model
			if (sub_dataset[m]->n_examples==0) {
				if ( adapt_separate_set_data )
				{
					message("No example (just set data)");
					shmm->models[m]->setDataSet(data);
				}
				//else
				//	message("No example (don't set data)");
			} else if (shmm->model_trainer[m]) {
				message("HMM adaptation of model %d with own aligned data",m);
				shmm->model_trainer[m]->train(sub_dataset[m], shmm->initial_models_trainer_measurers);
			} else {
				//message("No trainer (don't set data)");
				//shmm->models[m]->setDataSet(sub_dataset[m]);
			}
		}
	}
	else
	{
		shmm->setBOption("initialize", false);
	
		// Measurers on the training dataset
		MeasurerList measurers;
		NLLMeasurer nll_meas(shmm->log_probabilities, data, measurer_file );
		measurers.addNode(&nll_meas);

		// The Gradient Machine Trainer
		EMTrainer trainer(shmm);
		trainer.setIOption("max iter", max_iter);
		trainer.setROption("end accuracy", accuracy);
		if (viterbi) 
			trainer.setBOption("viterbi", true);

		trainer.train(data, &measurers);
	}
	
	writeHTK( output_fname, shmm, lex_output->phone_info->phone_names );

}


void AruspixHMM::setLexicon( const char *model_fname_, const char *spacing_model_ , const char *lex_fname_ , const char *sent_start_symbol_ )
{
	// NULL is string is empty
	if ( strlen(spacing_model_) == 0 )
			spacing_model_ = NULL;

	// keep values
	if ( spacing_model_ )
	{
		spacing_model = (char *)allocator->alloc(strlen(spacing_model_)+1);
		strcpy(spacing_model, spacing_model_);
	}
	if ( sent_start_symbol_ )
	{
		sent_start_symbol = (char *)allocator->alloc(strlen(sent_start_symbol_)+1);
		strcpy(sent_start_symbol, sent_start_symbol_);	
	}

	lex_input = new(allocator) LexiconInfo( model_fname_, spacing_model_ , NULL ,
				lex_fname_ , sent_start_symbol_ , NULL , NULL  ) ;
}


void AruspixHMM::mapSetDataLexicon( const char *model_data_fname, const char *lex_data_fname,
		const char *model_output_fname, const char *lex_output_fname )
{
	lex_data = new(allocator) LexiconInfo( model_data_fname, spacing_model , NULL ,
				lex_data_fname , sent_start_symbol , NULL , NULL  ) ;
				
	//-------------------------
	// first merge models lists
	//-------------------------
	
	// allocate the memory for the mapping. Probably won't be use completely, but not as big
	new_in_data = (int *)allocator->alloc( lex_data->phone_info->n_phones * sizeof(int) ) ;
	new_in_output = (int *)allocator->alloc( lex_data->phone_info->n_phones * sizeof(int) ) ;
	n_new = 0;
	
	// temporary array to sort the models
	int n_models = 0;
	char **models = (char **)allocator->alloc( (lex_input->phone_info->n_phones + lex_data->phone_info->n_phones) * sizeof(char *) ) ;
	
	// copy phones1; we assume that there are in alphabetical order
	for (n_models = 0; n_models < lex_input->phone_info->n_phones; n_models++)
	{
		models[n_models] = (char *)allocator->alloc( (strlen(lex_input->phone_info->getPhone(n_models))+1) * sizeof(char) ) ;
        strcpy( models[n_models] , lex_input->phone_info->getPhone(n_models) ) ;
	}
	
	// now add missing models from the second phones2
	for ( int i=0; i < lex_data->phone_info->n_phones; i++ )
	{
		char *phone = lex_data->phone_info->getPhone(i);
		int cmp_result = 0;
		
		// compare with last
		
		if ( n_models > 0 )
			cmp_result = strcmp( models[n_models-1] , phone ) ;
        
		if ( (cmp_result < 0) || (n_models == 0) )
		{
			// The new phone belongs at the end of the list
			// Allocate memory in the list of symbols for the new symbol   
			models[n_models] = (char *)allocator->alloc( (strlen(phone)+1) * sizeof(char) ) ;
			strcpy( models[n_models] , phone ) ;
			n_models++ ;
			new_in_data[n_new]=i;
			n_new++;
#ifdef __AXDEBUG__
			printf("New internal symbol %s\n", lex_data->phone_info->getPhone(i) ) ;
#endif
			continue;
		}
		else if ( cmp_result > 0 )
		{
			// Find the place in the list of symbols where we want to insert the new symbol
			for ( int j=0 ; j<n_models ; j++ )
			{
				cmp_result = strcmp( models[j] , phone ) ;
				if ( cmp_result > 0 )
				{
					// Shuffle down all symbols from i onwards and place the
					//   new symbol in position i.

					// Allocate memory in the list of symbols for the new symbol   
					for ( int k=n_models ; k>j ; k-- )
						models[k] = models[k-1] ;
					models[j] = (char *)allocator->alloc( (strlen(phone)+1) * sizeof(char) ) ;
					strcpy( models[j] , phone ) ;
					n_models++ ;
					new_in_data[n_new]=i;
					n_new++;
#ifdef __AXDEBUG__
					printf("New internal symbol %s\n", lex_data->phone_info->getPhone(i) ) ;
#endif
					break;
				}
				else if ( cmp_result == 0 )
					// the phone is already in the list - don't duplicate
					break;
			}
        }
	}

	// save new models list
	FILE* fp=fopen(model_output_fname,"w");
	if (!fp)
		error("AruspixHMM::mapSetDataLexicon : file %s cannot be opened", model_output_fname);
	for ( int i=0; i < n_models; i++ )
		fprintf(fp,"%s\n", models[i] ) ;
	//fprintf(fp,"\n");
	fclose(fp);
	allocator->free( models );
	
	//--------------------
	// then merge lexicons
	//--------------------
	
	char line[1000];
	FILE* fl=fopen(lex_output_fname,"w");
	if (!fl)
		error("AruspixHMM::mapSetDataLexicon : file %s cannot be opened", lex_output_fname);
		
	// copy all entries in the first lex1
    for ( int i=0 ; i<lex_input->n_entries ; i++ )
    {
		fprintf(fl,"%s ", lex_input->vocabulary->getWord( lex_input->entries[i].vocab_index ) ) ;
			for ( int j=0 ; j<lex_input->entries[i].n_phones ; j++ )
        		fprintf(fl," %s" , lex_input->phone_info->getPhone( lex_input->entries[i].phones[j] )) ;
			fprintf(fl,"\n") ;
    }

	// go through all entries in the second lex2 and check if they exist in the first lex
    for ( int i=0 ; i<lex_data->n_entries ; i++ )
    {
		bool found = false;
		char *symbol = lex_data->vocabulary->getWord( lex_data->entries[i].vocab_index );
		int symbol_index1 = lex_input->vocabulary->getIndex( symbol );
		if  ( symbol_index1 != -1 ) 
		// check the prounciation only if the symbol exists
		{	
			for ( int j=0; (j < lex_input->vocab_to_lex_map[symbol_index1].n_pronuns) && !found ; j++ )
			// go through all pronounciation in the first lex
			{
				int p = lex_input->vocab_to_lex_map[symbol_index1].pronuns[j];
				if ( lex_data->entries[i].n_phones != lex_input->entries[p].n_phones )
					continue; // not the same length, false anyway
				else
				{
					for ( int k=0 ; (k < lex_data->entries[i].n_phones) && (k < lex_input->entries[p].n_phones); k++ )
					{
						if ( strcmp( lex_data->phone_info->getPhone( lex_data->entries[i].phones[k] ) , lex_input->phone_info->getPhone( lex_input->entries[p].phones[k] ) ) )
							break;
						else if ( k == lex_data->entries[i].n_phones - 1 ) // last phone
							found = true;
					}
				}
			}
		}
		if ( !found )
		{
			sprintf(line,"%s", symbol ) ;
			for ( int j=0 ; j<lex_data->entries[i].n_phones ; j++ )
			{
				strcat(line," ");
        		strcat(line, lex_data->phone_info->getPhone( lex_data->entries[i].phones[j] ) ) ;
			}
			fprintf(fl, "%s\n", line );
#ifdef __AXDEBUG__
			printf( "New symbol: %s\n", line );
#endif
		}
    }
	//fprintf(fl,"\n");
	fclose(fl);

	// load the new lexicon
	lex_output = new(allocator) LexiconInfo( model_output_fname, spacing_model , NULL ,
				lex_output_fname , sent_start_symbol , NULL , NULL  ) ;
	
	//------------------
	// mapping in output
	//------------------
	for( int i = 0; i < n_new; i++)
	{
		new_in_output[i] = lex_output->phone_info->getIndex( lex_data->phone_info->getPhone( new_in_data[i] ) );
		if ( new_in_output[i] == -1 )
			error("AruspixHMM::mapSetDataLexicon : the new phone %s not found in the output set\n",lex_data->phone_info->getPhone( new_in_data[i] ));
	}
}


void AruspixHMM::setData( char **input_fnames_, int n_inputs_, char** target_fnames_, int n_targets_ )
{
	// some basic tests on the files
	if (n_inputs_ != n_targets_) {
		error("the input and target files should have the same number of files (%d != %d)\n",n_inputs_,n_targets_);
	}
	
	input_fnames = (char **)allocator->alloc(sizeof(char *) * n_inputs_);
	target_fnames = (char **)allocator->alloc(sizeof(char *) * n_targets_);
	
	for (int i=0;i<n_targets_;i++) 
	{
		char* si = strRemoveSuffix(input_fnames_[i]);
		char* st = strRemoveSuffix(target_fnames_[i]);
		if (strcmp(strBaseName(si),strBaseName(st)))
		warning("input file (%s) do not correspond to target file (%s)",input_fnames_[i],target_fnames_[i]);
		free(si);
		free(st);

		input_fnames[i] = (char *)allocator->alloc(strlen(input_fnames_[i])+1);
		strcpy(input_fnames[i], input_fnames_[i]);
		target_fnames[i] = (char *)allocator->alloc(strlen(target_fnames_[i])+1);
		strcpy(target_fnames[i], target_fnames_[i]);
	}
	
	n_inputs = n_inputs_;
	n_targets = n_targets_;
}

SpeechHMM* AruspixHMM::create(int* states, int n_models, int n_inputs, LexiconInfo *lex, real* thresh)
{
	// create the GMM
	DiagonalGMM*** gmms = (DiagonalGMM ***)allocator->alloc(sizeof(DiagonalGMM**)*n_models);
	EMTrainer** hmm_trainer = (EMTrainer **)allocator->alloc(sizeof(EMTrainer*)*n_models);
	HMM** hmm = (HMM**)allocator->alloc(sizeof(HMM*)*n_models);
	KMeans*** kmeans = (KMeans***)allocator->alloc(sizeof(KMeans**)*n_models);
	EMTrainer*** kmeans_trainer = (EMTrainer***)allocator->alloc(sizeof(EMTrainer**)*n_models);
	real*** transitions = (real***)allocator->alloc(n_models*sizeof(real**));
	for (int i=0;i<n_models;i++) 
	{
		//printf( lex.phone_info->phone_names[i] );
		//printf( "\n" );
		gmms[i] = (DiagonalGMM**)allocator->alloc(sizeof(DiagonalGMM*)*states[i]);
		kmeans[i] = (KMeans**)allocator->alloc(sizeof(KMeans*)*states[i]);
		kmeans_trainer[i] = (EMTrainer**)allocator->alloc(sizeof(EMTrainer*)*states[i]);
		for (int j=1;j<states[i]-1;j++) 
		{
			// create a KMeans object to initialize the GMM
			kmeans[i][j] = new(allocator)KMeans(n_inputs, n_gaussians);
			kmeans[i][j]->setVarThreshold(thresh);
			// the kmeans trainer
			kmeans_trainer[i][j] = new(allocator) EMTrainer(kmeans[i][j]);
			kmeans_trainer[i][j]->setROption("end accuracy", accuracy);
			kmeans_trainer[i][j]->setIOption("max iter", max_iter);

			gmms[i][j] = new(allocator)DiagonalGMM(n_inputs,n_gaussians,kmeans_trainer[i][j]);

			// set the training options
			gmms[i][j]->setVarThreshold(thresh);
			gmms[i][j]->setROption("prior weights", prior);
		}

		gmms[i][0] = NULL;
		gmms[i][states[i]-1] = NULL;

		// the transition table probability: left-right topology or full connect
		transitions[i] = (real**)allocator->alloc(states[i]*sizeof(real*));
		for (int j=0;j<states[i];j++) 
		{
			transitions[i][j] = (real*)allocator->alloc(states[i]*sizeof(real));
		}
		setTransitions(transitions[i], states[i], i, lex->phone_info->sil_index );

		hmm[i] = new (allocator)HMM(states[i],(Distribution**)gmms[i],transitions[i]);
		hmm[i]->setROption("prior transitions",prior);
		hmm[i]->setBOption("linear segmentation", true); // can be changed ?
		hmm_trainer[i] = new(allocator)EMTrainer(hmm[i]);
		hmm_trainer[i]->setIOption("max iter", max_iter);
		hmm_trainer[i]->setROption("end accuracy", accuracy);
	}

	EMTrainer** hmm_trainer_ = NULL;
	if (train_separate)
		hmm_trainer_ = hmm_trainer;

	SpeechHMM* shmm = new(allocator) SpeechHMM(n_models,hmm,lex,hmm_trainer_);
	return shmm;
}


HMM** AruspixHMM::create(int* states, int* gaussians, int n_models, int n_inputs, LexiconInfo* lex, real* thresh)
{
	DiagonalGMM*** gmms = (DiagonalGMM ***)allocator->alloc(sizeof(DiagonalGMM**)*n_models);
	HMM** hmm = (HMM**)allocator->alloc(sizeof(HMM*)*n_models);
	real*** transitions = (real***)allocator->alloc(n_models*sizeof(real**));
	for (int i=0;i<n_models;i++) {
		gmms[i] = (DiagonalGMM**)allocator->alloc(sizeof(DiagonalGMM*)*states[i]);
		for (int j=1;j<states[i]-1;j++) {
			DiagonalGMM* gmm = new(allocator)DiagonalGMM(n_inputs,gaussians[i]);

			// set the training options
			if (thresh)
				gmm->setVarThreshold(thresh);
			if (prior>0)
				gmm->setROption("prior weights",prior);
			gmms[i][j] = gmm;
		}

		gmms[i][0] = NULL;
		gmms[i][states[i]-1] = NULL;

		// the transition table probability: left-right topology
		transitions[i] = (real**)allocator->alloc(states[i]*sizeof(real*));
		for (int j=0;j<states[i];j++) {
			transitions[i][j] = (real*)allocator->alloc(states[i]*sizeof(real));
		}
		setTransitions(transitions[i], states[i], i, lex->phone_info->sil_index );

		hmm[i] = new (allocator)HMM(states[i],(Distribution**)gmms[i],transitions[i]);
		hmm[i]->setROption("prior transitions",prior);
		hmm[i]->setBOption("initialize", false);
  }

  return hmm;
}

HMM** AruspixHMM::readHTK( const char* filename, LexiconInfo* lex, real* thresh )
{
	DiskXFile* f = new(allocator) DiskXFile(filename,"r");
	int n_models = 0;
	int n_inputs = 0;
	int* states = NULL;
	int* gaussians = NULL;
	char line[10000];
	char* values[10000];
	char* str;
	f->gets(line,10000);
	
	//--------------------------------------------
	// first parse the file and create the HMM set
	//--------------------------------------------
	
	while (!f->eof()) {
		if (strstr(line,"~h")) {
			//printf("model %s",line);
			strtok( line , "\"" ) ; // get past the ~h
            if ( (str = strtok( NULL , "\"" )) == NULL )
                error("AruspixHMM::readHTK - could not locate model name\n") ;
			if ( lex->phone_info->getIndex( str ) != n_models )
                error("AruspixHMM::readHTK - order in model list and HMM file do not match\n");
			n_models++;
			states = (int*)allocator->realloc(states,sizeof(int)*n_models);
			gaussians = (int*)allocator->realloc(gaussians,sizeof(int)*n_models);
		} else if (strstr(line,"<NUMSTATES>")) {
			values[0] = strtok(line," ");
			values[1] = strtok(NULL," ");
			//printf("states %s",values[1]);
			states[n_models-1] = atoi(values[1]);
		} else if (strstr(line,"<NUMMIXES>")) {
			// we assume that every state in each model has the same number of mixes
			// the last in the state will be recorded
			values[0] = strtok(line," ");
			values[1] = strtok(NULL," ");
			//printf("nummixes %s",values[1]);
			gaussians[n_models-1] = atoi(values[1]);
		} else if (strstr(line,"<MEAN>")) {
			values[0] = strtok(line," ");
			values[1] = strtok(NULL," ");
			n_inputs = atoi(values[1]);
		}
		f->gets(line,10000);
	}
	HMM** hmms = create(states, gaussians, n_models, n_inputs, lex, thresh);
	
	//-------------------------------
	// laod the file into the HMM set
	//-------------------------------
	
	f->rewind();
	for (int i=0;i<n_models;i++){
		HMM* hmm = hmms[i];
		for (int j=1;j<hmm->n_states-1;j++) {
			DiagonalGMM* gmm = (DiagonalGMM*)hmm->states[j];
			for (int k=0;k<gmm->n_gaussians;k++) {
				gmm->log_weights[k] = LOG_ZERO;
				for (int l=0;l<gmm->n_inputs;l++) {
					gmm->means[k][l] = 0;
					gmm->var[k][l] = 1;
				}
			}
		}
		for (int j=0;j<hmm->n_states;j++) {
			for (int k=0;k<hmm->n_states;k++) {
				hmm->log_transitions[k][j] = LOG_ZERO;
			}
		}
	}
	// reading the model
	int model = -1;
	int state = 0;
	int mixture = 0;
	real w;
	int n;
	HMM* hmm = NULL;
	DiagonalGMM* gmm = NULL;
	f->gets(line,10000);
	while (!f->eof()) {
		if (strstr(line,"~h")) {
			model++;
			hmm = hmms[model];
			//hmm = shmm->models[model];
		} else if (strstr(line,"<STATE>")) {
			sscanf(line,"%*s %d",&state);
			//printf("state %d\n",state);
			gmm = (DiagonalGMM*)hmm->states[state-1];
		} else if (strstr(line,"<MIXTURE>")) {
#ifdef USE_DOUBLE
			sscanf(line,"%*s %d %lf",&mixture,&w);
#else
			sscanf(line,"%*s %d %f",&mixture,&w);
#endif
			//printf("mixture %d\n",mixture);
			gmm->log_weights[mixture-1] = log(w);
		} else if (strstr(line,"<MEAN>")) {
			f->gets(line,10000);
			//printf("mean %s",line);
			values[0] = strtok(line," ");
			for (n=1;(values[n]=strtok(NULL," "));n++);
			for (int l=0;l<gmm->n_inputs;l++) {
				gmm->means[mixture-1][l] = (real)atof(values[l]);;
			}
		} else if (strstr(line,"<VARIANCE>")) {
			f->gets(line,10000);
			//printf("variance %s",line);
			values[0] = strtok(line," ");
			for (n=1;(values[n]=strtok(NULL," "));n++);
				for (int l=0;l<gmm->n_inputs;l++) {
					gmm->var[mixture-1][l] = (real)atof(values[l]);;
				}
		} else if (strstr(line,"<TRANSP>")) {
			for (int j=0;j<hmm->n_states;j++) {
				f->gets(line,10000);
				values[0] = strtok(line," ");
				for (n=1;(values[n]=strtok(NULL," "));n++);
					for (int k=0;k<hmm->n_states;k++) {
						w = atof(values[k]);
						hmm->log_transitions[k][j] = w == 0 ? LOG_ZERO : log(w);
					}
			}
		} 
		f->gets(line,10000);
	}   
	allocator->free(f);
	
	//load_htk_model(file, hmms, n_phonemes );
	//SpeechHMM* shmm = new(allocator) SpeechHMM(n_phonemes,hmms,lex);
	//shmm->setBOption("initialize", false);
	//SpeechHMM* shmm = newSpeechHMM(states, n_gaussians, n_phonemes, n_inputs, 
	//  lex, thresh, prior, allocator);
	//load_htk_model(file, shmm);
	return hmms;
}


// this function saves in HTK format a given SpeechHMM
void AruspixHMM::writeHTK( const char* filename, SpeechHMM* shmm, char**phonemes )
{
	DiskXFile* f = new(allocator) DiskXFile(filename,"w");
	f->printf("~o\n");
	f->printf("<STREAMINFO> 1 %d\n", shmm->n_inputs);
	f->printf("<VECSIZE> %d<NULLD><USER>\n", shmm->n_inputs);

	for (int i=0;i<shmm->n_models;i++) 
	{
		HMM *hmm = shmm->models[i];
		f->printf("~h \"%s\"\n",phonemes[i]);
		f->printf("<BEGINHMM>\n");
		f->printf("<NUMSTATES> %d\n",hmm->n_states);
		for (int j=1;j<hmm->n_states-1;j++) {
			DiagonalGMM* gmm = (DiagonalGMM*)hmm->states[j];
			f->printf("<STATE> %d\n",j+1);
			f->printf("<NUMMIXES> %d\n",gmm->n_gaussians);
			for (int k=0;k<gmm->n_gaussians;k++) {
				f->printf("<MIXTURE> %d %12.10e\n",k+1,exp(gmm->log_weights[k]));
				f->printf("<MEAN> %d\n",gmm->n_inputs);
				for (int l=0;l<gmm->n_inputs;l++) {
					f->printf("%12.10e ",gmm->means[k][l]);
				}
				f->printf("\n");
				f->printf("<VARIANCE> %d\n",gmm->n_inputs);
				for (int l=0;l<gmm->n_inputs;l++) {
					f->printf("%12.10e ",gmm->var[k][l]);
				}
				f->printf("\n");
			}
		}
		f->printf("<TRANSP> %d\n",hmm->n_states);
		for (int j=0;j<hmm->n_states;j++) {
			for (int k=0;k<hmm->n_states;k++) {
				f->printf("%12.10e ",hmm->log_transitions[k][j] != LOG_ZERO ? exp(hmm->log_transitions[k][j]) : 0);
			}
			f->printf("\n");
		}
		f->printf("<ENDHMM>\n");
  }
  allocator->free(f);
}

void AruspixHMM::initializeThreshold(DataSet* data,real* thresh, real threshold)
{
	MeanVarNorm norm(data);
	real* ptr = norm.inputs_stdv;
	real* p_var = thresh;
	for(int i=0;i<data->n_inputs;i++)
		*p_var++ = *ptr * *ptr++ * threshold;
}

int* AruspixHMM::initStates(const char* filename, int n_models, int n_states, int space_model)
{
	int* states = (int*)allocator->alloc(sizeof(int)*n_models);
	if (strcmp(filename,"")==0) {
		for (int i=0;i<n_models;i++)
			states[i] = n_states;
		if (space_model>=0)
			states[space_model] = 4;
	} else {
		DiskXFile f(filename, "r");
		for (int i=0;i< n_models;i++) {
			f.scanf("%d",&states[i]);
		}
	}
	return states;
}

void AruspixHMM::setTransitions(real** transitions, int model_n_states, int current_model, int space_model )
{
	for (int j=0;j<model_n_states;j++) 
	{
		for (int k=0;k<model_n_states;k++)
			transitions[j][k] = 0;
	}
  
	transitions[1][0] = 1;
	for (int j=1;j<model_n_states-1;j++) 
	{
		transitions[j][j] = 0.5;
		transitions[j+1][j] = 0.5;
	}
  
	if (current_model == space_model) 
	{
		if ( model_n_states < 4 )
			error("the spacing model must have at least 4 states\n");
		for (int j=0;j<model_n_states;j++) 
		{
			for (int k=0;k<model_n_states;k++)
				transitions[j][k] = 0;
		}
		transitions[1][0] = 1.0;
		transitions[1][1] = 1./3.;
		transitions[2][1] = 1./3.;
		transitions[3][1] = 1./3.;
		transitions[1][2] = 1./3.;
		transitions[2][2] = 1./3.;
		transitions[3][2] = 1./3.;
	}
}

AruspixHMM::~AruspixHMM()
{
}


