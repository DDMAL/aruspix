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

#ifndef __ML_DECODER_H__
#define __ML_DECODER_H__

#ifdef __GNUG__
    #pragma interface "mldecoder.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/thread.h"

#include "ml.h"

//using namespace Torch;

// WDR: class declarations


//----------------------------------------------------------------------------
// MlDecoder
//----------------------------------------------------------------------------

class MlDecoder: public wxThread
{

public:
    // constructors and destructors
    MlDecoder( wxString input_fname, wxString models_fname, wxString dic_fname );
    virtual ~MlDecoder();
	
    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    //virtual void OnExit();
    
    // WDR: method declarations for MlDecoder
	//void Run();
    
public:
    // WDR: member variable declarations for MlDecoder
	wxString am_models_fname;
	wxString am_sil_phone;
	wxString am_pause_phone;
	double am_phone_del_pen;
	bool am_apply_pause_del_pen;

	// Lexicon Parameters
	wxString lex_dict_fname;
	wxString lex_sent_start_word;
	wxString lex_sent_end_word;
	wxString lex_sil_word;

	// Music Model Parameters
	int lm_ngram_order;
	wxString lm_fname;
	double lm_scaling_factor;

	// Beam Search Decoder Parameters
	double dec_int_prune_window; // plus petit = pruning -> plus rapide, mais plus d'erreurs (si trop petit)
	double dec_end_prune_window;
	double dec_word_entr_pen;
	bool dec_verbose;
	bool dec_delayed_lm;
	// output options
	bool print_by_page;
	bool print_by_part;
	bool print_by_book;

	// Batch Test Parameters
	wxString input_fname;
	wxString wrdtrns_fname;
	wxString output_fname;

	// Log
	wxString log_fname;

private:
	// WDR: handler declarations for MlDecoder

};

#endif
