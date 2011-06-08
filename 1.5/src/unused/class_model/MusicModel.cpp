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

#include "MusicModel.h"

// torch
#include "Allocator.h"
#include "string_stuff.h"
#include "DiskXFile.h"

using namespace Torch;


MusicModel::MusicModel( Vocabulary *vocabulary_, real main_scaling_factor_ )
{
    if ( vocabulary_ == NULL )
        error("MusicModel::MusicModel - no vocabulary defined\n") ;
        
    vocabulary = vocabulary_ ;
    n_words = vocabulary->n_words ;
	main_scaling_factor = main_scaling_factor_;
	sum_class_factors = 0.0;

	if ( vocabulary->sent_start_index != -1 )
		lm_has_start_word = true ;
	else
		lm_has_start_word = false ; // ????
	if ( vocabulary->sent_end_index != -1 )
		lm_has_end_word = true ;
	else 
	  lm_has_end_word = false ;  // ????

	//general_model = new ClassModel( order, vocabulary, lm_fname, "", lm_scaling_factor, true );
	general_model = NULL;

	n_class_models = 0;
	class_models = NULL;
 
#ifdef DEBUG
    //outputText() ;
#endif
}


MusicModel::~MusicModel()
{
   if ( general_model )
	   delete general_model;

   if ( class_models != NULL )
    {
        for ( int i=0 ; i<n_class_models ; i++ )
            delete class_models[i] ;
        free( class_models ) ;
    }
}


void MusicModel::addClassModel( char *voc_fname, char *cm_fname, char *map_fname,
							   int order_,  real lm_scaling_factor_ , 
							   bool interval_model_, char *voc_notes_fname_ )
{
    if ( order_ <= 0 )
        error("MusicModel::addClassModel - LM order must be > 0\n") ;
    if ( lm_scaling_factor_ <= 0.0 )
        error("MusicModel::addClassModel - LM scaling factor must be > 0\n") ;

	sum_class_factors += lm_scaling_factor_;
	n_class_models++;
	class_models = (ClassModel **)Allocator::sysRealloc( class_models , n_class_models * sizeof(ClassModel *) ) ;
	class_models[n_class_models-1] = new ClassModel( vocabulary, 
		voc_fname, cm_fname,  map_fname,  
		order_, lm_scaling_factor_,
		interval_model_, voc_notes_fname_ );

	int i;
	for( i = 0; i < n_class_models; i++ )
	{
		class_models[i]->lm_class_factor = class_models[i]->lm_scaling_factor / sum_class_factors;
		//printf("Class %d : class factor = %f\n", i, class_models[i]->lm_class_factor );
	}
}

void MusicModel::addClassModel( char *cm_fname, int order_,  real lm_scaling_factor_  )
{
    if ( order_ <= 0 )
        error("MusicModel::addClassModel - LM order must be > 0\n") ;
    if ( lm_scaling_factor_ <= 0.0 )
        error("MusicModel::addClassModel - LM scaling factor must be > 0\n") ;

	sum_class_factors += lm_scaling_factor_;
	n_class_models++;
	class_models = (ClassModel **)Allocator::sysRealloc( class_models , n_class_models * sizeof(ClassModel *) ) ;
	class_models[n_class_models-1] = new ClassModel( vocabulary, cm_fname,	order_, lm_scaling_factor_ );

	int i;
	for( i = 0; i < n_class_models; i++ )
	{
		class_models[i]->lm_class_factor = class_models[i]->lm_scaling_factor / sum_class_factors;
		//printf("Class %d : class factor = %f\n", i, class_models[i]->lm_class_factor );
	}
}


real MusicModel::calcLMProb( DecodingHypothesis *prev_word_end_hyp , int next_word )
{
    int words[30] , n_wrds, i , n_prob;
    WordChainElem *temp_elem ;
    real prob;

	//if ( general_model == NULL )
    //        error("MusicModel::calcLMProb(2) - general_model is NULL\n") ;

#ifdef DEBUG
    if ( next_word < 0 )
        error("MusicModel::calcLMProb(2) - next_word < 0\n") ;
#endif

    // We have a word end hypothesis and a next word.
    // We want to calculate the LM probability for this next word.
    // eg. We have a word end for some w2 and a next word w3.
    //     We want to calculate P(w3|w1,w2)
    
    // If the next word is silence or a sentence marker, don't do a LM lookup.
    if ( (next_word == vocabulary->sent_start_index) && (lm_has_start_word == false) )
        return 0.0 ;
    if ( (next_word == vocabulary->sent_end_index) && (lm_has_end_word == false) )
        return 0.0 ;
    if ( next_word == vocabulary->sil_index )
        return 0.0 ;
    
    // Construct a list of the previous words.
    n_wrds = 0 ;
	n_prob = 0 ;
	prob = 0.0;
    if ( prev_word_end_hyp != NULL )
    {    
        temp_elem = prev_word_end_hyp->word_level_info ;
#ifdef DEBUG
        if ( prev_word_end_hyp->word_level_info == NULL )
            error("MusicModel::calcLMProb(2) - word_level_info is NULL\n") ;
#endif

		// Find the n-gram probability for every class model

		int map_wrd;
		int last_pitch = -1, pitch;
		int last_value, value;
		char interval[100];
		for (i = 0; i < n_class_models; i++)
		{

			class_models[i]->prob = 0.0;
			n_wrds = 0 ;
			temp_elem = prev_word_end_hyp->word_level_info ;

			// first check if last word is in mapped vocabulary
			// if not, skip this class model because it won't give any information 
			if ( class_models[i]->mapper->map[ temp_elem->word ] == -1 )
				continue;

			Vocabulary *map_voc = class_models[i]->getVocabulary();
			while ( temp_elem != NULL )
			{
				// first get the mapped version of the word
				map_wrd = class_models[i]->mapper->map[ temp_elem->word ];
				if ( map_wrd == -1 ) // this word (usually SP) is not in vocabulary - skip it
				{
					temp_elem = temp_elem->prev_elem ;
					continue;
				}

				if ( (lm_has_start_word==false) && ( map_wrd==map_voc->sent_start_index) )
				{
					// skip these words for the purpose of LM lookups
					temp_elem = temp_elem->prev_elem ;
					continue ;
				}

				// special staff for interval model : keep last pitch and value and calculate interval
				if ( class_models[i]->interval_model )
				{
					if ((map_wrd!=map_voc->sent_start_index) && (map_wrd!=map_voc->sent_end_index))
					{
						class_models[i]->getPitch( map_wrd, &pitch, &value );
						//pitch = atoi( class_models[i]->notes_vocabulary->getWord( map_wrd ) );
						if ( last_pitch != -1 ) // is not the first pitch, calculate interval
						{
							class_models[i]->getInterval( interval, pitch, value, last_pitch, last_value );
							//sprintf( interval, "%d", pitch - last_pitch );
							map_wrd = class_models[i]->vocabulary->getIndex( interval );
							last_pitch = pitch; 
							last_value = value;
						}
						else // keep it and skip it - interval will be calculate at next step if it's a note
						{
							last_pitch = pitch;
							last_value = value;
							temp_elem = temp_elem->prev_elem ;
							continue ;
						}
					}
					else
					{
						// if START or END, just add them add reset last_pitch
						map_wrd = class_models[i]->getIndexFromNote( map_wrd );
						last_pitch = -1;
					}
				}

				words[n_wrds++] = map_wrd ;
				temp_elem = temp_elem->prev_elem ;
				if ( n_wrds >= class_models[i]->order )
					break ;
			}

			words[n_wrds++] = next_word ;
    
			if ( n_wrds != 0 )
			{
				class_models[i]->prob = class_models[i]->getLogProbBackoff( n_wrds , words );
				n_prob++;
			}
		}
    }
    // Tune the word end hypothesis score using the n-gram probability and return
	if ( n_prob == 0 )
		warning("MusicModel::calcLMProb - warning - no model applied\n" ) ;
	else // compute scaling depending on the differents class factors an the number of
		// models applied
	{
		real prob_factor = (real)n_class_models / (real)n_prob;
		for( i = 0; i < n_class_models; i++ )
			prob += class_models[i]->prob * class_models[i]->lm_class_factor * prob_factor;
	}
	prob *= main_scaling_factor;

    return ( prob ) ;
}


real MusicModel::calcLMProbDelayed( DecodingHypothesis *word_end_hyp )
{
    int words[30] , n_wrds , temp , i, j, n_prob ;
    WordChainElem *temp_elem ;
    real prob;

	if ( class_models == NULL )
            error("MusicModel::calcLMProb - class_models is NULL\n") ;
    
    if ( word_end_hyp == NULL )
        return LOG_ZERO ;
        
    // We have a word end hypothesis.  We want to tune this using our language model.
    // eg. we have a word end for some w3 and we want to tune this using P(w3|w1,w2)
        
    // Construct a list of the current word and previous words.
    n_wrds = 0 ;
    temp_elem = word_end_hyp->word_level_info ;
#ifdef DEBUG
    if ( word_end_hyp->word_level_info == NULL )
        error("MusicModel::calcLMProbDelayed - word_level_info is NULL\n") ;
#endif
    // If the most recent word is a sentence marker and the LM does not
    //   have entries for the sentence markers, don't do a LM lookup.
    if ( (temp_elem->word == vocabulary->sent_start_index) && (lm_has_start_word == false) )
        return 0.0 ;
    if ( (temp_elem->word == vocabulary->sent_end_index) && (lm_has_end_word == false) )
        return 0.0 ;
    if ( temp_elem->word == vocabulary->sil_index )
        return 0.0 ;

    // Find the n-gram probability for every class model
	prob = 0.0;
	n_prob = 0;

	int map_wrd;
	int last_pitch = -1, pitch;
	int last_value, value;
	char interval[100];

	for (i = 0; i < n_class_models; i++)
	{
		class_models[i]->prob = 0.0;
		n_wrds = 0 ;
		temp_elem = word_end_hyp->word_level_info ;

		// first check if last word is in mapped vocabulary
		// if not, skip this class model because it won't give any information 
		if ( class_models[i]->mapper->map[ temp_elem->word ] == -1 )
			continue;

		Vocabulary *map_voc = class_models[i]->getVocabulary( );

		while ( temp_elem != NULL )
		{
			// first get the mapped version of the word
			map_wrd = class_models[i]->mapper->map[ temp_elem->word ];

			if ( map_wrd == map_voc->sil_index ) // this word (usually SP) is not in vocabulary - skip it
			{
				temp_elem = temp_elem->prev_elem ;
				continue;
			}

			if ( (lm_has_start_word==false) && ( map_wrd==map_voc->sent_start_index) )
			{
				// skip these words for the purpose of LM lookups
				temp_elem = temp_elem->prev_elem ;
				continue ;
			}

			// special staff for pitch model : keep last pitch and value and calculate interval
			if ( class_models[i]->interval_model )
			{
				if ((map_wrd!=map_voc->sent_start_index) && (map_wrd!=map_voc->sent_end_index))
				{
					class_models[i]->getPitch( map_wrd, &pitch, &value );
					//pitch = atoi( class_models[i]->notes_vocabulary->getWord( map_wrd ) );
					if ( last_pitch != -1 ) // is not the first pitch, calculate interval
					{
						class_models[i]->getInterval( interval, pitch, value, last_pitch, last_value );
						//sprintf( interval, "%d", pitch - last_pitch );
						if ( n_wrds == 0 )
							class_models[i]->prob += class_models[i]->mapper->word_in_class_prob[ temp_elem->word ];
						map_wrd = class_models[i]->vocabulary->getIndex( interval );
						last_pitch = pitch; 
						last_value = value;
					}
					else // keep it and skip it - interval will be calculate at next step if it's a note
					{
						last_pitch = pitch;
						last_value = value;
						temp_elem = temp_elem->prev_elem ;
						continue ;
					}
				}
				else
				{
					// if START or END, just add them add reset last_pitch
					map_wrd = class_models[i]->getIndexFromNote( map_wrd );
					last_pitch = -1;
				}
			}
			else if ( n_wrds == 0 )
				class_models[i]->prob += class_models[i]->mapper->word_in_class_prob[ temp_elem->word ];

			words[n_wrds++] = map_wrd ;
			temp_elem = temp_elem->prev_elem ;
			if ( n_wrds >= class_models[i]->order )
				break ;
		}

		// The method in LMNGram requires a different ordering of words
		temp = words[0] ;
		for ( j=1 ; j<n_wrds ; j++ )
			words[j-1] = words[j] ;
		words[n_wrds-1] = temp ;

		if ( n_wrds != 0 )
		{
			class_models[i]->prob += class_models[i]->getLogProbBackoff( n_wrds , words );
			n_prob++;
		}
	}
        
    // Tune the word end hypothesis score using the n-gram probability and return
	if ( n_prob == 0 )
		{} //warning("MusicModel::calcLMProbDelayed - warning - no model applied\n" ) ;
	else // compute scaling depending on the differents class factors an the number of
		// models applied
	{
		real prob_factor = (real)n_class_models / (real)n_prob;
		for( i = 0; i < n_class_models; i++ )
			//prob += class_models[i]->prob * log(class_models[i]->lm_class_factor) * log(prob_factor);
			/* exponentiel */ 
			prob += (class_models[i]->prob * (class_models[i]->lm_scaling_factor - 0.0) / (real)n_prob); 
			/* lineaire */
			//prob += ((class_models[i]->prob - class_models[i]->lm_scaling_factor + 1.0) / (real)n_prob); 

		
		//prob /= prob_factor;
	}
	//prob * log(main_scaling_factor);
	/* exponentiel */
	prob *= (main_scaling_factor - 0.0);
	/* lineaire */
	//prob -= (main_scaling_factor - 1.0);	

    return ( prob ) ;
}


#ifdef DEBUG
void MusicModel::outputText()
{
}
#endif

