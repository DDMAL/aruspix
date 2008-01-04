
#include "Common.h"

void get_line(FILE* f, char* line, int n)
{
  char* r = fgets(line,n,f);
  if (r != NULL && (int)strlen(r) == n-1)
    error("the string is too long!");
}

void initializeThreshold(DataSet* data,real* thresh, real threshold)
{
  MeanVarNorm norm(data);
  real* ptr = norm.inputs_stdv;
  real* p_var = thresh;
  for(int i=0;i<data->n_inputs;i++)
    *p_var++ = *ptr * *ptr++ * threshold;
}

void read_phonemes(const char* filename, char*** phonemes_, int *n_phonemes, Allocator* allocator)
{
  DiskXFile f(filename, "r");

  *n_phonemes = 0;
  char bidon[10000];
  do {
    f.gets(bidon,10000);
    *n_phonemes += 1;
  } while (!f.eof());
  *n_phonemes -= 1;
  f.rewind();
  
  char** phonemes = (char**)allocator->alloc(sizeof(char*)* *n_phonemes);
  char word[100];
  for (int i=0;i< *n_phonemes;i++) {
    f.scanf("%s",word);
    phonemes[i] = (char*)allocator->alloc(sizeof(char)*(strlen(word)+1));
    strcpy(phonemes[i],word);
  }
  message("%d phonemes detected", *n_phonemes);
  *phonemes_ = phonemes;
}

int* read_states(const char* filename, int n_phonemes, int n_states, int pause_index, Allocator* allocator)
{
  int* states = (int*)allocator->alloc(sizeof(int)*n_phonemes);
  if (strcmp(filename,"")==0) {
    for (int i=0;i<n_phonemes;i++)
      states[i] = n_states;
    if (pause_index>=0)
      states[pause_index] = 3;
  } else {
    DiskXFile f(filename, "r");

    for (int i=0;i< n_phonemes;i++) {
      f.scanf("%d",&states[i]);
    }
  }
  return states;
}

// this function saves phones as a list
void save_phones(char* filename, PhoneInfo *phones )
{
  FILE* f=fopen(filename,"w");
  if (!f)
    error("file %s cannot be opened",filename);
  for ( int i=0; i < phones->n_phones; i++ )
		fprintf(f,"%s\n", phones->getPhone(i) ) ;
  fprintf(f,"\n");
  fclose(f);
}

// this function saves in HTK format a given SpeechHMM
void write_htk_model(FILE* f, HMM* hmm, char*phoneme )
{
	fprintf(f,"~h \"%s\"\n",phoneme);
    fprintf(f,"<BEGINHMM>\n");
    fprintf(f,"<NUMSTATES> %d\n",hmm->n_states);
    for (int j=1;j<hmm->n_states-1;j++) {
      DiagonalGMM* gmm = (DiagonalGMM*)hmm->states[j];
      fprintf(f,"<STATE> %d\n",j+1);
      fprintf(f,"<NUMMIXES> %d\n",gmm->n_gaussians);
      for (int k=0;k<gmm->n_gaussians;k++) {
        fprintf(f,"<MIXTURE> %d %12.10e\n",k+1,exp(gmm->log_weights[k]));
        fprintf(f,"<MEAN> %d\n",gmm->n_inputs);
        for (int l=0;l<gmm->n_inputs;l++) {
          fprintf(f,"%12.10e ",gmm->means[k][l]);
        }
        fprintf(f,"\n");
        fprintf(f,"<VARIANCE> %d\n",gmm->n_inputs);
        for (int l=0;l<gmm->n_inputs;l++) {
          fprintf(f,"%12.10e ",gmm->var[k][l]);
        }
        fprintf(f,"\n");
      }
    }
    fprintf(f,"<TRANSP> %d\n",hmm->n_states);
    for (int j=0;j<hmm->n_states;j++) {
      for (int k=0;k<hmm->n_states;k++) {
        fprintf(f,"%12.10e ",hmm->log_transitions[k][j] != LOG_ZERO ? exp(hmm->log_transitions[k][j]) : 0);
      }
      fprintf(f,"\n");
    }
    fprintf(f,"<ENDHMM>\n");
}

// this function saves in HTK format a given SpeechHMM
void save_htk_model(char* filename, SpeechHMM* shmm, char**phonemes )
{
	FILE *f=fopen(filename,"w");
	if (!f)
	error("file %s cannot be opened",filename);
	fprintf(f,"~o\n");
	fprintf(f,"<STREAMINFO> 1 %d\n",shmm->n_inputs);
	fprintf(f,"<VECSIZE> %d<NULLD><MFCC_D_A_O>\n",shmm->n_inputs);

	for (int i=0;i<shmm->n_models;i++) {
		write_htk_model( f, shmm->models[i], phonemes[i] );
    
  }
  fclose(f);
}

// load a SpeechHMM that was saved in the HTK format
// assumes that it uses DiagonalGMMs
//void load_htk_model(char* filename, SpeechHMM* shmm)
void load_htk_model(char* filename, HMM** hmms, int n_models )
{
  char line[10000];
  char* values[10000];
  int n;
  FILE* f=fopen(filename,"r");
  if (!f)
    error("file %s cannot be opened",filename);
  // initialization
  //for (int i=0;i<shmm->n_models;i++) {
    //HMM* hmm = shmm->models[i];
  for (int i=0;i<n_models;i++) {
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
  HMM* hmm = NULL;
  DiagonalGMM* gmm = NULL;
  get_line(f,line,10000);
  while (!feof(f)) {
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
      get_line(f,line,10000);
	  //printf("mean %s",line);
      values[0] = strtok(line," ");
      for (n=1;(values[n]=strtok(NULL," "));n++);
      for (int l=0;l<gmm->n_inputs;l++) {
        gmm->means[mixture-1][l] = (real)atof(values[l]);;
      }
    } else if (strstr(line,"<VARIANCE>")) {
      get_line(f,line,10000);
	  //printf("variance %s",line);
      values[0] = strtok(line," ");
      for (n=1;(values[n]=strtok(NULL," "));n++);
      for (int l=0;l<gmm->n_inputs;l++) {
        gmm->var[mixture-1][l] = (real)atof(values[l]);;
      }
    } else if (strstr(line,"<TRANSP>")) {
      for (int j=0;j<hmm->n_states;j++) {
        get_line(f,line,10000);
        values[0] = strtok(line," ");
        for (n=1;(values[n]=strtok(NULL," "));n++);
        for (int k=0;k<hmm->n_states;k++) {
          w = atof(values[k]);
          hmm->log_transitions[k][j] = w == 0 ? LOG_ZERO : log(w);
        }
      }
    } 
    get_line(f,line,10000);
  }   
  fclose(f);
}   

void setTransitions(real** transitions, int phoneme_n_states, int current_phoneme, int sp_phoneme, int silence_phoneme, bool full_connect)
{
  for (int j=0;j<phoneme_n_states;j++) {
    for (int k=0;k<phoneme_n_states;k++)
      transitions[j][k] = 0;
  }
  if (full_connect) {
    for (int i=1;i<phoneme_n_states-1;i++) {
       transitions[i][0] = 1./(phoneme_n_states-2);
       for (int j=1;j<phoneme_n_states;j++) {
          transitions[j][i] = 1./(phoneme_n_states-1);
       }
    }
  } else {
    transitions[1][0] = 1;
    for (int j=1;j<phoneme_n_states-1;j++) {
      transitions[j][j] = 0.5;
      transitions[j+1][j] = 0.5;
    }
  }

  // the short pause is special
  if (current_phoneme == sp_phoneme) {
    for (int j=0;j<phoneme_n_states;j++) {
      for (int k=0;k<phoneme_n_states;k++)
        transitions[j][k] = 0;
    }
    transitions[1][0] = 0.5;
    transitions[2][0] = 0.5;
  }
  
  if (current_phoneme == silence_phoneme && !full_connect) {
	printf("init silence phoneme %d\n", current_phoneme );
    for (int j=0;j<phoneme_n_states;j++) {
      for (int k=0;k<phoneme_n_states;k++)
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

  // the silence model is special
  /*if (current_phoneme == silence_phoneme && !full_connect) {
    for (int j=1;j<phoneme_n_states-2;j++) {
      transitions[j+2][j] = 1./3.;
      transitions[j+1][j] = 1./3.;
      transitions[j][j] = 1./3.;
    }
  }*/
}

HMM** newHMM(int* states, int n_gaussians, int n_models, int n_inputs, LexiconInfo* lex, real* thresh, real prior, Allocator* allocator)
{
  DiagonalGMM*** gmms = (DiagonalGMM ***)allocator->alloc(sizeof(DiagonalGMM**)*n_models);
  HMM** hmm = (HMM**)allocator->alloc(sizeof(HMM*)*n_models);
  real*** transitions = (real***)allocator->alloc(n_models*sizeof(real**));
  for (int i=0;i<n_models;i++) {
    gmms[i] = (DiagonalGMM**)allocator->alloc(sizeof(DiagonalGMM*)*states[i]);
    for (int j=1;j<states[i]-1;j++) {
      DiagonalGMM* gmm = new(allocator)DiagonalGMM(n_inputs,n_gaussians);

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
    setTransitions(transitions[i], states[i], i, lex->phone_info->pause_index, 
      lex->phone_info->sil_index,false);

    hmm[i] = new (allocator)HMM(states[i],(Distribution**)gmms[i],transitions[i]);
    hmm[i]->setROption("prior transitions",prior);
    hmm[i]->setBOption("initialize", false);
  }

  return hmm;
  //SpeechHMM* shmm = new(allocator) SpeechHMM(n_models,hmm,lex);
  //shmm->setBOption("initialize", false);
  //return shmm;
}

SpeechHMM* newSpeechHMM(int* states, int n_gaussians, int n_models, int n_inputs, LexiconInfo* lex, real* thresh, real prior, Allocator* allocator)
{
  DiagonalGMM*** gmms = (DiagonalGMM ***)allocator->alloc(sizeof(DiagonalGMM**)*n_models);
  HMM** hmm = (HMM**)allocator->alloc(sizeof(HMM*)*n_models);
  real*** transitions = (real***)allocator->alloc(n_models*sizeof(real**));
  for (int i=0;i<n_models;i++) {
    gmms[i] = (DiagonalGMM**)allocator->alloc(sizeof(DiagonalGMM*)*states[i]);
    for (int j=1;j<states[i]-1;j++) {
      DiagonalGMM* gmm = new(allocator)DiagonalGMM(n_inputs,n_gaussians);

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
    setTransitions(transitions[i], states[i], i, lex->phone_info->pause_index, 
      lex->phone_info->sil_index,false);

    hmm[i] = new (allocator)HMM(states[i],(Distribution**)gmms[i],transitions[i]);
    hmm[i]->setROption("prior transitions",prior);
    hmm[i]->setBOption("initialize", false);
  }

  SpeechHMM* shmm = new(allocator) SpeechHMM(n_models,hmm,lex);
  shmm->setBOption("initialize", false);
  return shmm;
}

/*
AruspixHMM* newAruspixHMMWithTrainer(int* states, int n_gaussians, int n_models, int n_inputs, LexiconInfo* lex, 
	DataSet* data, real* thresh, real prior, int max_iter, real accuracy, bool train_separate, Allocator* allocator)
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
			kmeans[i][j] = new(allocator)KMeans(data->n_inputs, n_gaussians);
			kmeans[i][j]->setVarThreshold(thresh);
			// the kmeans trainer
			kmeans_trainer[i][j] = new(allocator) EMTrainer(kmeans[i][j]);
			kmeans_trainer[i][j]->setROption("end accuracy", accuracy);
			kmeans_trainer[i][j]->setIOption("max iter", max_iter);


			gmms[i][j] = new(allocator)DiagonalGMM(data->n_inputs,n_gaussians,kmeans_trainer[i][j]);

			// set the training options
			gmms[i][j]->setVarThreshold(thresh);
			gmms[i][j]->setROption("prior weights",prior);
		}

		gmms[i][0] = NULL;
		gmms[i][states[i]-1] = NULL;

		// the transition table probability: left-right topology or full connect
		transitions[i] = (real**)allocator->alloc(states[i]*sizeof(real*));
		for (int j=0;j<states[i];j++) 
		{
			transitions[i][j] = (real*)allocator->alloc(states[i]*sizeof(real));
		}
		setTransitions(transitions[i], states[i], i, lex->phone_info->pause_index, lex->phone_info->sil_index, false );

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

	AruspixHMM* ahmm = new(allocator) AruspixHMM(n_models,hmm,lex,hmm_trainer_);
	return ahmm;
}
*/

SpeechHMM* newSpeechHMMFromTorch(char* file, LexiconInfo* lex, real* thresh, real prior, Allocator* allocator, CmdLine* cmd)
{
  int n_gaussians;
  int n_phonemes;
  int n_inputs;
  DiskXFile f(file,"r");
  cmd->loadXFile(&f);
  f.taggedRead(&n_gaussians, sizeof(int), 1, "n_gaussians");
  f.taggedRead(&n_phonemes, sizeof(int), 1, "n_phonemes");
  int* states = (int*)allocator->alloc(sizeof(int)*n_phonemes);
  f.taggedRead(states, sizeof(int), n_phonemes, "n_states");
  f.taggedRead(&n_inputs, sizeof(int), 1, "n_inputs");
  SpeechHMM* shmm = newSpeechHMM(states, n_gaussians, n_phonemes, n_inputs, 
    lex, thresh, prior, allocator);
  shmm->loadXFile(&f);
  return shmm;
}

SpeechHMM* newSpeechHMMFromHTK(char* file, LexiconInfo* lex, real* thresh, real prior, Allocator* allocator)
{
  DiskXFile* f = new(allocator) DiskXFile(file,"r");
  int n_gaussians = 0;
  int n_phonemes = 0;
  int n_inputs = 0;
  int* states = NULL;
  char line[10000];
  char* values[10000];
  f->gets(line,10000);
  while (!f->eof()) {
    if (strstr(line,"~h")) {
	  //printf("model %s",line);
      n_phonemes++;
      states = (int*)allocator->realloc(states,sizeof(int)*n_phonemes);
    } else if (strstr(line,"<NUMSTATES>")) {
      values[0] = strtok(line," ");
      values[1] = strtok(NULL," ");
	  //printf("states %s",values[1]);
      states[n_phonemes-1] = atoi(values[1]);
    } else if (strstr(line,"<NUMMIXES>")) {
      values[0] = strtok(line," ");
      values[1] = strtok(NULL," ");
	  //printf("nummixes %s",values[1]);
      n_gaussians = atoi(values[1]);
    } else if (strstr(line,"<MEAN>")) {
      values[0] = strtok(line," ");
      values[1] = strtok(NULL," ");
      n_inputs = atoi(values[1]);
    }
    f->gets(line,10000);
  }
  allocator->free(f);
  HMM** hmms = newHMM(states, n_gaussians, n_phonemes, n_inputs, 
    lex, thresh, prior, allocator);
  load_htk_model(file, hmms, n_phonemes );
  SpeechHMM* shmm = new(allocator) SpeechHMM(n_phonemes,hmms,lex);
  shmm->setBOption("initialize", false);
  
  printf("Loaded before\n");
	
  //SpeechHMM* shmm = newSpeechHMM(states, n_gaussians, n_phonemes, n_inputs, 
  //  lex, thresh, prior, allocator);
  //load_htk_model(file, shmm);
  return shmm;
}

HMM* extend_HMM_to_min_duration(HMM* hmm,int min_duration, Allocator* allocator)
{
  // how many states should I add
  int n_states_to_add = min_duration - (hmm->n_states - 2);

  if (n_states_to_add <= 0) {
    warning("no added state");
    return hmm;
  }
  
  // the solution can not work if there are already shared states
  if (hmm->n_shared_states > 0)
    error("cannot add states since n_shared_states = %d",hmm->n_shared_states);

  // let's duplicate the last real state
  // we first need to create shared states
  int n_shared_states = hmm->n_states - 2;
  Distribution** shared_states = (Distribution**)allocator->alloc(sizeof(Distribution*)*n_shared_states);
  for (int i=0;i<n_shared_states;i++)
    shared_states[i] = hmm->states[i+1];

  // then redo the states themselves
  int new_n_states = hmm->n_states + n_states_to_add;
  Distribution** states = (Distribution**)allocator->alloc(sizeof(Distribution*)*new_n_states);
  states[0] = NULL;
  states[new_n_states-1] = NULL;
  for (int i=1;i<hmm->n_states-1;i++)
    states[i] = shared_states[i-1];
  for (int i=0;i<n_states_to_add;i++)
    states[i+hmm->n_states-1] = shared_states[n_shared_states-1];

  // then the transitions
  real** transitions = (real**)allocator->alloc(sizeof(real*)*new_n_states);
  for (int i=0;i<new_n_states;i++) {
    transitions[i] = (real*)allocator->alloc(sizeof(real)*new_n_states);
    for (int j=0;j<new_n_states;j++) {
      transitions[i][j] = 0;
    }
  }
  // copy old transitions
  for (int i=0;i<hmm->n_states;i++) {
    for (int j=0;j<hmm->n_states;j++) {
      if (hmm->log_transitions[i][j] != LOG_ZERO) {
        transitions[i][j] = exp(hmm->log_transitions[i][j]);
      }
    }
  }
  // add new transitions based on the old last state transitions
  // (assume left-right transitions!)
  int last_s = hmm->n_states-2;
  for (int i=0;i<n_states_to_add;i++) {
    int s = i+hmm->n_states-1;
    transitions[s][s] = exp(hmm->log_transitions[last_s][last_s]);
    transitions[s+1][s] = exp(hmm->log_transitions[last_s+1][last_s]);
  }

  HMM* n_hmm = new(allocator) HMM(new_n_states,states,transitions,n_shared_states,shared_states);
  n_hmm->initialize = hmm->initialize;
  n_hmm->linear_segmentation = hmm->linear_segmentation;
  n_hmm->prior_transitions = hmm->prior_transitions;
  for (int i=0;i<n_hmm->n_states;i++) {
    real *p = n_hmm->transitions[i];
    real *lp = n_hmm->log_transitions[i];
    for (int j=0;j<n_hmm->n_states;j++,lp++,p++) {
      if (*p > 0)
        *lp = log(*p);
      else
        *lp = LOG_ZERO;
    }
  }
  return n_hmm;
}

SpeechHMM* extend_SpeechHMM_to_min_duration(SpeechHMM* shmm,int min_duration, Allocator* allocator, int silence_model)
{
  LexiconInfo* lex = shmm->lexicon;
  EMTrainer** mtrainer = shmm->model_trainer;
  int n_models = shmm->n_models;
  HMM** models = (HMM**)allocator->alloc(sizeof(HMM*)*n_models);
  for (int i=0;i<n_models;i++) {
    if (i == silence_model)
      models[i] = shmm->models[i];
    else
      models[i] = extend_HMM_to_min_duration(shmm->models[i],min_duration,allocator);
  }
  SpeechHMM* new_shmm = new(allocator) SpeechHMM(n_models,models,lex,mtrainer);
  new_shmm->prior_transitions = shmm->prior_transitions;
  new_shmm->phoneme_targets = shmm->phoneme_targets;
  return new_shmm;
}

