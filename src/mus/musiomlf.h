/////////////////////////////////////////////////////////////////////////////
// Name:        iomlf.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMLF_H__
#define __MUS_IOMLF_H__

#ifdef AX_WG

#ifdef __GNUG__
    #pragma interface "iomlf.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musfile.h"

class MLFSymbol;
WX_DECLARE_OBJARRAY( MLFSymbol, ArrayOfMLFSymboles);

class ImPage;
class ProgressDlg;

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


class RecSymbol;
WX_DECLARE_OBJARRAY(RecSymbol, ArrayOfRecSymbols);

// WDR: class declarations

//----------------------------------------------------------------------------
// RecSymbol
//----------------------------------------------------------------------------

class RecSymbol: public wxObject
{
public:
    // constructors and destructors
    RecSymbol() {};
    ~RecSymbol() {};

public:
	wxString m_word;
	wxString m_hmm_symbol;
    int m_states;
};


//----------------------------------------------------------------------------
// MLFSymbol
//----------------------------------------------------------------------------

class MLFSymbol: public wxObject
{
public:
    // constructors and destructors
    MLFSymbol();
    virtual ~MLFSymbol() {};
    
    // WDR: method declarations for MLFSymbol
	virtual void SetValue( char type, wxString subtype, int position, int value = 0, char pitch = 0, int oct = 0, int flag = 0);
    virtual wxString GetLabel( );
	wxString GetLabelType( );
	int GetNbOfStates( );
	int GetPosition( ) { return m_position; }
	int GetWidth( );
    
protected:
    // WDR: member variable declarations for MLFSymbol
	char m_type, m_pitch;
	wxString m_subtype;
	
public:
	int m_value, m_oct, m_flag, m_position;

private:
    // WDR: handler declarations for MLFSymbol

	DECLARE_DYNAMIC_CLASS(MLFSymbol)
};


//----------------------------------------------------------------------------
// MLFOutput
//----------------------------------------------------------------------------

class MLFOutput: public WgFileOutputStream
{
public:
    // constructors and destructors
    MLFOutput( WgFile *file, wxString filename, wxString model_symbole_name = "MLFSymbol" );
	MLFOutput( WgFile *file, int fd, wxString filename, wxString model_symbole_name = "MLFSymbol" );
	//MLFOutput( WgFile *file, wxFile *wxfile, wxString filename, wxString model_symbole_name = "MLFSymbol" );
    virtual ~MLFOutput();
    
    // WDR: method declarations for MLFOutput
	bool ExportFile( WgFile *file, wxString filename);	// replace  wgfile set in the constructor
														// and export it
														// allow exportation of several files in one mlf
    virtual bool ExportFile( );
    virtual bool WritePage( const WgPage *page, bool write_header = false );
	bool WritePage( const WgPage *page, wxString filename, ImPage *imPage,
		wxArrayInt *staff_numbers = NULL ); // manage segments through imPage and staves throuhg staff_numbers
											// write all staves if staff_numbers == NULL
    virtual bool WriteStaff( const WgStaff *staff, int offsets[] = NULL, int split_points[] = NULL, int end_points[] = NULL );
    virtual bool WriteNote( WgNote *note );
    virtual bool WriteSymbole( WgSymbole *symbole );
	// specific
	static WgStaff *SplitSymboles( WgStaff *staff );
	static WgStaff *GetUt1( WgStaff *staff, bool inPlace = false );
	static void GetUt1( WgStaff *staff, WgElement *pelement, int *code, int *oct);
	// charge le dictionnaire ( .dic )
	void LoadSymbolDictionary( wxString filename );
	void WriteSymbolDictionary( wxString filename );
	// si writePosition, charge .xml et calcul la largeur
	//void LoadTypes( wxString filename );
	void CreateSubFile(); // open a subfile xxxp to write subsymbols with position
	void LoadSubFile(); // idem but with cache
	// output methods
	void WriteDictionary( wxString filename );
	void WriteStatesPerSymbol( wxString filename );
	void WriteHMMSymbols( wxString filename );
	virtual void StartLabel( );
	virtual void EndLabel( int offsets[] = NULL, int end_points[] = NULL );
	// access
	ArrayOfMLFSymboles *GetSymbols( ) { return &m_symboles; };
    
protected:
    // WDR: member variable declarations for MLFOutput
    wxString m_filename;
	wxFileOutputStream *m_subfile;
	// specific
	ArrayOfMLFSymboles m_symboles; // tableau des symbole
	//MLFTypes m_types; // tableau des largeur - par type, uniquement avec MLFSymbol
	wxString m_model_symbole_name;
	wxString m_shortname;
	WgStaff *m_staff; // utilise pour les segments de portee, doit etre accessible dans WriteSymbole
	// page, staff and segment index
	int m_page_i;
	int m_staff_i;
	int m_segment_i;
	bool m_addHeader; // used to know if #MLF# header must be added (first file or not)

public:
	bool m_addPageNo;
	wxArrayString m_loadedDict; // symboles charge avec LoadDictionnary
	wxArrayString m_dict; // symboles de l'exportation
	ArrayOfRecSymbols m_dictSymbols; // symboles de l'exportation, version complete avec phone et position. m_dict allows fast access
	bool m_writePosition; // ecrit les position dans MLF, uniquement avec MLFSymbol_


private:
    // WDR: handler declarations for MLFOutput
};


//----------------------------------------------------------------------------
// MLFInput
//----------------------------------------------------------------------------

class MLFInput: public WgFileInputStream
{
public:
    // constructors and destructors
    MLFInput( WgFile *file, wxString filename );
    virtual ~MLFInput();
    
    // WDR: method declarations for MLFInput
    virtual bool ImportFile( int staff_per_page = -1 );
    virtual bool ReadPage( WgPage *page , bool firstLineMLF, ImPage *imPage = NULL );
    virtual bool ReadLabel( WgStaff *staff, int offset = 0 );
	// specific
	static bool IsElement( bool *note, wxString *line, int *pos );
	static WgSymbole *ConvertSymbole( wxString line );
	static WgNote *ConvertNote( wxString line  );
	static WgStaff *GetNotUt1( WgStaff *staff, bool inPlace = false );
	static void GetNotUt1( WgStaff *staff, WgElement *pelement, int *code, int *oct);
	static void GetPitchWWG( char code, int *code1);
	bool ReadLine( wxString *line );
	bool ReadLabelStr( wxString label );


    
protected:
    // WDR: member variable declarations for MLFInput
	// page, staff and segment index
	int m_staff_i, m_staff_label;
	int m_segment_i, m_segment_label;

private:
    // WDR: handler declarations for MLFInput
};



#endif //AX_WG

#endif
