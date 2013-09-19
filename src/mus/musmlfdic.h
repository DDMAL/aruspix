/////////////////////////////////////////////////////////////////////////////
// Name:        musmlfdic.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_MLFDIC_H__
#define __MUS_MLFDIC_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include <vector>

class MusMLFSymbol;
WX_DECLARE_OBJARRAY( MusMLFSymbol, ArrayOfMLFSymbols);

//class ImPage;
//class AxProgressDlg;

#define SP_START "SP_START"
#define SP_END "SP_END"
#define SP_WORD "SP"

class MusMLFWord;
WX_DECLARE_OBJARRAY(MusMLFWord, ArrayOfMLFWords);

int SortMLFWords( MusMLFWord **first, MusMLFWord **second );

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif


//----------------------------------------------------------------------------
// MusMLFWord
//----------------------------------------------------------------------------

/**
 * This class represent a word in an HTK Master Label File (MLF).
*/
class MusMLFWord: public wxObject
{
public:
    // constructors and destructors
    MusMLFWord() {};
    ~MusMLFWord() {};

public:
	wxString m_word;
	wxArrayString m_hmms; // sub_words and states have to be feed in parallel
	std::vector<int> m_states;
};


//----------------------------------------------------------------------------
// MusMLFDictionary
//----------------------------------------------------------------------------

/**
 * This class represent a dictionary for an HTK Master Label File (MLF).
*/
class MusMLFDictionary: public wxObject
{
public:
    // constructors and destructors
    MusMLFDictionary() {};
    ~MusMLFDictionary() {};
	
	void Reset();
	
	int Index( wxString label );
	
    void Load( TiXmlElement *file_root );
    void Save( TiXmlElement *file_root );
	
	void WriteDic( wxString filename );
	void WriteHMMs( wxString filename );
	void WriteStates( wxString filename );

public:
	ArrayOfMLFWords m_dict;
};


#endif
