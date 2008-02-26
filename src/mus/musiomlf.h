/////////////////////////////////////////////////////////////////////////////
// Name:        musiomlf.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMLF_H__
#define __MUS_IOMLF_H__



#ifdef __GNUG__
    #pragma interface "musiomlf.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musfile.h"

class MusMLFSymbol;
WX_DECLARE_OBJARRAY( MusMLFSymbol, ArrayOfMLFSymboles);

class ImPage;
class AxProgressDlg;

#define SP_START "SP_START"
#define SP_END "SP_END"
#define SP_WORD "SP"

#define TYPE_NOTE 'N'
#define TYPE_REST 'R'
#define TYPE_KEY 'K'
#define TYPE_ALTERATION 'A'
#define TYPE_CUSTOS 'C'
#define TYPE_POINT 'P'
#define TYPE_MESURE 'M'
#define TYPE_SYMBOLE 'S'

// MLFsymbole flags
#define NOTE_STEM 1
#define NOTE_COLORATION 2
#define NOTE_LIGATURE 4


class MusMLFWord;
WX_DECLARE_OBJARRAY(MusMLFWord, ArrayOfMLFWords);

// WDR: class declarations

//----------------------------------------------------------------------------
// MusMLFWord
//----------------------------------------------------------------------------

class MusMLFWord: public wxObject
{
public:
    // constructors and destructors
    MusMLFWord() {};
    ~MusMLFWord() {};

public:
	wxString m_word;
	wxString m_hmm_symbol;
    int m_states;
};


//----------------------------------------------------------------------------
// MusMLFDictionary
//----------------------------------------------------------------------------

class MusMLFDictionary: public wxObject
{
public:
    // constructors and destructors
    MusMLFDictionary() {};
    ~MusMLFDictionary() {};

public:
	ArrayOfMLFWords m_dict;
};


//----------------------------------------------------------------------------
// MusMLFSymbol
//----------------------------------------------------------------------------

class MusMLFSymbol: public wxObject
{
public:
    // constructors and destructors
    MusMLFSymbol();
    virtual ~MusMLFSymbol() {};
    
    // WDR: method declarations for MusMLFSymbol
	virtual void SetValue( char type, wxString subtype, int position, int value = 0, char pitch = 0, int oct = 0, int flag = 0);
    virtual wxString GetLabel( );
	wxString GetLabelType( );
	int GetNbOfStates( );
	int GetPosition( ) { return m_position; }
	int GetWidth( );
    
protected:
    // WDR: member variable declarations for MusMLFSymbol
	char m_type, m_pitch;
	wxString m_subtype;
	
public:
	int m_value, m_oct, m_flag, m_position;

private:
    // WDR: handler declarations for MusMLFSymbol

	DECLARE_DYNAMIC_CLASS(MusMLFSymbol)
};


//----------------------------------------------------------------------------
// MusMLFSymbolWP
//----------------------------------------------------------------------------

// Idem MusMLFSymbol but without pitch

class MusMLFSymbolWP: public MusMLFSymbol
{
public:
    // constructors and destructors
    MusMLFSymbolWP();
    virtual ~MusMLFSymbolWP() {};
    
    // WDR: method declarations for MusMLFSymbolWP
	//virtual void SetValue( char type, wxString subtype, int position, int value = 0, char pitch = 0, int oct = 0, int flag = 0);
    virtual wxString GetLabel( );
    
protected:
    // WDR: member variable declarations for MusMLFSymbolWP
	
public:

private:
    // WDR: handler declarations for MusMLFSymbolWP

	DECLARE_DYNAMIC_CLASS(MusMLFSymbolWP)
};


//----------------------------------------------------------------------------
// MusMLFOutput
//----------------------------------------------------------------------------

class MusMLFOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusMLFOutput( MusFile *file, wxString filename, MusMLFDictionary *dict, wxString model_symbole_name = "MusMLFSymbol" );
	MusMLFOutput( MusFile *file, int fd, wxString filename,  MusMLFDictionary *dict, wxString model_symbole_name = "MusMLFSymbol" );
    virtual ~MusMLFOutput();
    
    // WDR: method declarations for MusMLFOutput
	//bool ExportFile( MusFile *file, wxString filename);	// replace  musfile set in the constructor
														// and export by calling ExportFile
														// allow exportation of several files in one mlf
    //virtual bool ExportFile( );
    //virtual bool WritePage( const MusPage *page, bool write_header = false );
	bool WritePage( const MusPage *page, wxString filename, ImPage *imPage,
		wxArrayInt *staff_numbers = NULL ); // manage segments through imPage and staves throuhg staff_numbers
											// write all staves if staff_numbers == NULL
											
    virtual bool WriteStaff( const MusStaff *staff, int offsets[] = NULL, int split_points[] = NULL, int end_points[] = NULL );
    virtual bool WriteNote( MusNote *note );
    virtual bool WriteSymbole( MusSymbol *symbole );
	// specific
	static MusStaff *SplitSymboles( MusStaff *staff );
	static MusStaff *GetUt1( MusStaff *staff, bool inPlace = false );
	static void GetUt1( MusStaff *staff, MusElement *pelement, int *code, int *oct);
	// charge le dictionnaire ( .dic )
	//void LoadSymbolDictionary( wxString filename );
	//void WriteSymbolDictionary( wxString filename );
	// si writePosition, charge .xml et calcul la largeur
	//void CreateSubFile(); // open a subfile xxxp to write subsymbols with position
	//void LoadSubFile(); // idem but with cache
	// output methods
	//void WriteDictionary( wxString filename );
	//void WriteStatesPerSymbol( wxString filename );
	//void WriteHMMSymbols( wxString filename );
	virtual void StartLabel( );
	virtual void EndLabel( int offsets[] = NULL, int end_points[] = NULL );
	// access
	//ArrayOfMLFSymboles *GetSymbols( ) { return &m_symboles; };
    
protected:
    // WDR: member variable declarations for MusMLFOutput
    wxString m_filename;
	//wxFileOutputStream *m_subfile;
	// specific
	ArrayOfMLFSymboles m_symboles; // symbol list
	//MLFTypes m_types; // tableau des largeur - par type, uniquement avec MusMLFSymbol
	wxString m_model_symbole_name;
	wxString m_shortname;
	MusStaff *m_staff; // utilise pour les segments de portee, doit etre accessible dans WriteSymbole
	// page, staff and segment index
	int m_page_i;
	int m_staff_i;
	int m_segment_i;
	bool m_addHeader; // used to know if #MLF# header must be added (first file or not)

public:
	bool m_addPageNo;
	//wxArrayString m_loadedDict; // symboles charge avec LoadDictionnary
	//wxArrayString m_dict; // symboles de l'exportation
	MusMLFDictionary *m_dict; // symboles de l'exportation, version complete avec phone et position. m_dict allows fast access
	bool m_writePosition; // ecrit les position dans MLF, uniquement avec MLFSymbol_


private:
    // WDR: handler declarations for MusMLFOutput
};


//----------------------------------------------------------------------------
// MusMLFOutputWP
//----------------------------------------------------------------------------

// Idem MusMLFOutput but without pitch

class MusMLFOutputWP: public MusMLFOutput
{
public:
    // constructors and destructors
    MusMLFOutputWP( MusFile *file, wxString filename, MusMLFDictionary *dict, wxString model_symbole_name = "MusMLFSymbolWP" );
	MusMLFOutputWP( MusFile *file, int fd, wxString filename, MusMLFDictionary *dict, wxString model_symbole_name = "MusMLFSymbolWP" );
    virtual ~MusMLFOutputWP();
    
    // WDR: method declarations for MusMLFOutputWP
    virtual bool WriteNote( MusNote *note );
    virtual bool WriteSymbole( MusSymbol *symbole );
	// specific
    
protected:
    // WDR: member variable declarations for MusMLFOutputWP

public:

private:
    // WDR: handler declarations for MusMLFOutputWP
};


//----------------------------------------------------------------------------
// MusMLFInput
//----------------------------------------------------------------------------

class MusMLFInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusMLFInput( MusFile *file, wxString filename );
    virtual ~MusMLFInput();
    
    // WDR: method declarations for MusMLFInput
    virtual bool ImportFile( int staff_per_page = -1 );
    virtual bool ReadPage( MusPage *page , bool firstLineMLF, ImPage *imPage = NULL );
    virtual bool ReadLabel( MusStaff *staff, int offset = 0 );
	// specific
	static bool IsElement( bool *note, wxString *line, int *pos );
	static MusSymbol *ConvertSymbole( wxString line );
	static MusNote *ConvertNote( wxString line  );
	static MusStaff *GetNotUt1( MusStaff *staff, bool inPlace = false );
	static void GetNotUt1( MusStaff *staff, MusElement *pelement, int *code, int *oct);
	static void GetPitchWWG( char code, int *code1);
	bool ReadLine( wxString *line );
	bool ReadLabelStr( wxString label );


    
protected:
    // WDR: member variable declarations for MusMLFInput
	// page, staff and segment index
	int m_staff_i, m_staff_label;
	int m_segment_i, m_segment_label;

private:
    // WDR: handler declarations for MusMLFInput
};




#endif
