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

#include "musmlfdic.h"

#include "mus/muslayer.h"
#include "mus/muspage.h"

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
class MusMLFOutput: public wxFileOutputStream
{
public:
    // constructors and destructors
    MusMLFOutput( MusDoc *doc, wxString filename, MusMLFDictionary *dict, wxString model_symbol_name = "MusMLFSymbol" );
	MusMLFOutput( MusDoc *doc, int fd, wxString filename,  MusMLFDictionary *dict, wxString model_symbol_name = "MusMLFSymbol" );
    virtual ~MusMLFOutput();

    virtual bool WritePage( const MusPage *page, bool write_header = false );
	bool WritePage( const MusPage *page, wxString filename, ImPage *imPage,
		std::vector<int> *staff_numbers = NULL ); // manage staves throuhg staff_numbers
											// write all staves if staff_numbers == NULL
											
    bool WriteLayer( const MusLayer *layer, int offset = -1, int end_point = -1 );
    bool WriteNote( MusLayerElement *element );
    bool WriteSymbol( MusLayerElement *element );
	// specific
	//static MusStaff *SplitSymboles( MusStaff *staff );
	static MusLayer *GetUt1( MusLayer *layer );
	static void GetUt1( MusLayer *layer, MusLayerElement *pelement, int *code, int *oct);
	void StartLabel( );
	void EndLabel( int offset = -1, int end_point = -1 );
	// access
	ArrayOfMLFSymbols *GetSymbols( ) { return &m_symbols; };
    
protected:
    MusDoc *m_doc;
    wxString m_filename;
	// specific
	ArrayOfMLFSymbols m_symbols; // symbol list
	wxString m_mlf_class_name;
	wxString m_shortname;
	MusLayer *m_layer; // utilise pour les segments de portee, doit etre accessible dans WriteSymbol
	// page, staff index
	int m_staff_i;
	bool m_addHeader; // used to know if #MLF# header must be added (first file or not)

public:
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
class MusMLFInput: public wxFileInputStream
{
public:
    // constructors and destructors
    MusMLFInput( MusDoc *file, wxString filename );
    virtual ~MusMLFInput();
    
    bool ReadPage( MusPage *page, bool firstLineMLF, ImPage *imPage = NULL );
    bool ReadLabel( MusLayer *layer, int offset = 0 );
	// specific
    /**
     * Parse a MLF line. Format is "[BEGIN END] LABEL". 
     * The first letter of LABEL is the elementType (e.g., N for note) and set in elementType
     * LABEL is set in elementLine
     * BEGIN (if any) is set in pos
     */
    static bool ParseLine( wxString line, char *elementType, wxString *elementLine, int *pos );
	static MusLayerElement *ConvertSymbol( wxString line );
	static MusLayerElement *ConvertNote( wxString line  );
	static void GetNotUt1( MusLayer *layer );
	static void GetNotUt1( MusLayer *layer, MusLayerElement *pelement, int *code, int *oct);
	static void GetPitchWWG( char code, int *code1);
	bool ReadLine( wxString *line );
	bool ReadLabelStr( wxString label );


    
protected:
    MusDoc *m_doc;
    // page, staff index
	int m_staff_i, m_staff_label;
    MusLayer *m_logLayer; // the layer to which logical elements will be added. Currently only one

private:
    };




#endif
