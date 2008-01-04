
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

#define SP_START "SP_START"
#define SP_END "SP_END"
#define SP_WORD "SP"

using namespace Torch;

void get_line(FILE* f, char* line, int n);

void initializeThreshold(DataSet* data,real* thresh, real threshold);

void read_phonemes(const char* filename, char*** phonemes_, int *n_phonemes, Allocator* allocator);

int* read_states(const char* filename, int n_phonemes, int n_states, int pause_index, Allocator* allocator);

// this function saves phones as a list
void save_phones(char* filename, PhoneInfo *phones );

void write_htk_model(FILE* f, HMM* hmm, char*phoneme );

// this function saves in HTK format a given SpeechHMM
void save_htk_model(char* filename, SpeechHMM* shmm, char**phonemes );

// load a SpeechHMM that was saved in the HTK format
// assumes that it uses DiagonalGMMs
void load_htk_model(char* filename, SpeechHMM* shmm);

void setTransitions(real** transitions, int phoneme_n_states, int current_phoneme, int sp_phoneme, int silence_phoneme, bool full_connect);

SpeechHMM* newSpeechHMM(int* states, int n_gaussians, int n_models, int n_inputs, LexiconInfo* lex, real* thresh, real prior, Allocator* allocator);


//AruspixHMM* newAruspixHMMWithTrainer(int* states, int n_gaussians, int n_models, int n_inputs, LexiconInfo* lex, 
//	DataSet* data, real* thresh, real prior, int max_iter, real accuracy, bool train_separate, Allocator* allocator);

SpeechHMM* newSpeechHMMFromTorch(char* file, LexiconInfo* lex, real* thresh, real prior, Allocator* allocator, CmdLine* cmd);

SpeechHMM* newSpeechHMMFromHTK(char* file, LexiconInfo* lex, real* thresh, real prior, Allocator* allocator);

HMM* extend_HMM_to_min_duration(HMM* hmm,int min_duration, Allocator* allocator);

SpeechHMM* extend_SpeechHMM_to_min_duration(SpeechHMM* shmm,int min_duration, Allocator* allocator, int silence_model);


