/////////////////////////////////////////////////////////////////////////////
// Name:        musiomlf.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMLF_H__
#define __MUS_IOMLF_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musdoc.h"
#include "musmlfdic.h"

class ImPage;
class AxProgressDlg;

#define TYPE_NOTE 'N'
#define TYPE_REST 'R'
#define TYPE_KEY 'K'
#define TYPE_ALTERATION 'A'
#define TYPE_CUSTOS 'C'
#define TYPE_POINT 'P'
#define TYPE_MESURE 'M'
#define TYPE_SYMBOLE 'S'

// MLFsymbol flags
#define NOTE_STEM 1
#define NOTE_COLORATION 2
#define NOTE_LIGATURE 4

//----------------------------------------------------------------------------
// MusMLFSymbol
//----------------------------------------------------------------------------

/**
 * This class represents an HTK Master Label File symbol.
*/
class MusMLFSymbol: public wxObject
{
public:
    // constructors and destructors
    MusMLFSymbol();
    virtual ~MusMLFSymbol() {};
    
    virtual void SetValue( char type, wxString subtype, int position, int value = 0, char pitch = 0, int oct = 0, int flag = 0);
    virtual wxString GetLabel( );
	wxString GetLabelType( );
	int GetNbOfStates( );
	int GetPosition( ) { return m_position; }
	int GetWidth( );
    
protected:
    char m_type, m_pitch;
	wxString m_subtype;
	
public:
	int m_value, m_oct, m_flag, m_position;

private:
    
	DECLARE_DYNAMIC_CLASS(MusMLFSymbol)
};


//----------------------------------------------------------------------------
// MusMLFOutput
//----------------------------------------------------------------------------

/**
 * This class is a file output stream for writing HTK Master Label Files (MLF).
 * Broken in Aruspix 2.0
*/
class MusMLFOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusMLFOutput( MusDoc *doc, wxString filename, MusMLFDictionary *dict, wxString model_symbol_name = "MusMLFSymbol" );
	MusMLFOutput( MusDoc *doc, int fd, wxString filename,  MusMLFDictionary *dict, wxString model_symbol_name = "MusMLFSymbol" );
    virtual ~MusMLFOutput();
    
    //bool ExportFile( MusDoc *file, wxString filename);	// replace  musfile set in the constructor
														// and export by calling ExportFile
														// allow exportation of several files in one mlf
    //virtual bool ExportFile( );
    virtual bool WritePage( const MusPage *page, bool write_header = false );
	bool WritePage( const MusPage *page, wxString filename, ImPage *imPage,
		wxArrayInt *staff_numbers = NULL ); // manage staves throuhg staff_numbers
											// write all staves if staff_numbers == NULL
											
    bool WriteStaff( const MusLaidOutStaff *staff, int offset = -1, int end_point = -1 );
    //bool WriteNote( MusNote1 *note ); // ax2
    //bool WriteSymbol( MusSymbol1 *symbol ); // ax2
	// specific
	static MusLaidOutStaff *SplitSymboles( MusLaidOutStaff *staff );
	static MusLaidOutStaff *GetUt1( MusLaidOutStaff *staff, bool inPlace = false );
	static void GetUt1( MusLaidOutStaff *staff, MusLaidOutLayerElement *pelement, int *code, int *oct);
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
	void StartLabel( );
	void EndLabel( int offset = -1, int end_point = -1 );
	// access
	ArrayOfMLFSymbols *GetSymbols( ) { return &m_symbols; };
    
protected:
    wxString m_filename;
	//wxFileOutputStream *m_subfile;
	// specific
	ArrayOfMLFSymbols m_symbols; // symbol list
	wxString m_mlf_class_name;
	wxString m_shortname;
	MusLaidOutStaff *m_staff; // utilise pour les segments de portee, doit etre accessible dans WriteSymbol
	// page, staff index
	int m_page_i;
	int m_staff_i;
	bool m_addHeader; // used to know if #MLF# header must be added (first file or not)

public:
	bool m_addPageNo;
	//wxArrayString m_loadedDict; // symbols charge avec LoadDictionnary
	//wxArrayString m_dict; // symbols de l'exportation
	MusMLFDictionary *m_dict; // symbols de l'exportation, version complete avec phone et position. m_dict allows fast access
	bool m_pagePosition; // ecrit les position dans MLF, avec MusMLFSymbol
	bool m_hmmLevel; // write symbols rather the words in EndLabel; basic for now, do not handle several symbols per word


private:
    };


//----------------------------------------------------------------------------
// MusMLFInput
//----------------------------------------------------------------------------

/**
 * This class is a file input stream for reading HTK Master Label Files (MLF).
 * Broken in Aruspix 2.0
*/
class MusMLFInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusMLFInput( MusDoc *file, wxString filename );
    virtual ~MusMLFInput();
    
        virtual bool ImportFile( int staff_per_page = -1 );
    bool ReadPage( MusPage *page , bool firstLineMLF, ImPage *imPage = NULL );
    bool ReadLabel( MusLaidOutStaff *staff, int offset = 0 );
	// specific
	static bool IsElement( bool *note, wxString *line, int *pos );
	//static MusSymbol1 *ConvertSymbole( wxString line ); // ax2
	//static MusNote1 *ConvertNote( wxString line  ); // ax2
	static MusLaidOutStaff *GetNotUt1( MusLaidOutStaff *staff, bool inPlace = false );
	static void GetNotUt1( MusLaidOutStaff *staff, MusLaidOutLayerElement *pelement, int *code, int *oct);
	static void GetPitchWWG( char code, int *code1);
	bool ReadLine( wxString *line );
	bool ReadLabelStr( wxString label );


    
protected:
    // page, staff index
	int m_staff_i, m_staff_label;

private:
    };




#endif
