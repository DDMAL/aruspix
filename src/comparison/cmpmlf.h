/////////////////////////////////////////////////////////////////////////////
// Name:        cmpmlf.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_CMPMLF_H__
#define __MUS_CMPMLF_H__

#ifdef AX_RECOGNITION
		#ifdef AX_COMPARISON

#ifdef __GNUG__
    #pragma interface "cmpmlf.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mus/musiomlf.h"


// WDR: class declarations


//----------------------------------------------------------------------------
// CmpMLFSymb
//----------------------------------------------------------------------------

class CmpMLFSymb: public MusMLFSymbol
{
public:
    // constructors and destructors
    CmpMLFSymb();
    virtual ~CmpMLFSymb() {};
    
    // WDR: method declarations for CmpMLFSymb
	virtual void SetValue( char type, wxString subtype, int position, int value = 0, char pitch = 0, int oct = 0, int flag = 0);
    virtual wxString GetLabel( );

protected:
    // WDR: member variable declarations for CmpMLFSymb
	
public:
	wxString m_im_filename;
	int m_im_staff;
	int m_im_staff_segment;
	int m_im_pos;
	int m_index;
	//
	wxString m_cache;

private:
    // WDR: handler declarations for CmpMLFSymb

	DECLARE_DYNAMIC_CLASS(CmpMLFSymb)
};


//----------------------------------------------------------------------------
// CmpMLFOutput
//----------------------------------------------------------------------------

class CmpMLFOutput: public MusMLFOutput
{
public:
    // constructors and destructors
    CmpMLFOutput( MusFile *file, wxString filename, wxString model_symbole_name = "CmpSymbol" );
	CmpMLFOutput( MusFile *file, int fd, wxString filename, wxString model_symbole_name = "CmpSymbol" );
	//CmpMLFOutput( MusFile *file, wxFile *wxfile, wxString filename, wxString model_symbole_name = "CmpSymbol" );
    virtual ~CmpMLFOutput();
    
    // WDR: method declarations for CmpMLFOutput
	//bool ExportFile( MusFile *file, wxString filename);	// replace  musfile set in the constructor
														// and export it
														// allow exportation of several files in one mlf
														
    //virtual bool WritePage( const MusPage *page, bool write_header = false );
    virtual bool WriteStaff( const MusStaff *staff );
	// output methods
	virtual void StartLabel( );
	virtual void EndLabel( );
    
protected:
    // WDR: member variable declarations for CmpMLFOutput

public:
	bool m_ignore_clefs;
	bool m_ignore_custos;
	bool m_ignore_keys;

private:
    // WDR: handler declarations for CmpMLFOutput
};


//----------------------------------------------------------------------------
// CmpMLFInput
//----------------------------------------------------------------------------

class CmpMLFInput: public MusMLFInput
{
public:
    // constructors and destructors
    CmpMLFInput( MusFile *file, wxString filename );
    virtual ~CmpMLFInput();
    
    // WDR: method declarations for CmpMLFInput
    virtual MusStaff *ImportFileInStaff(  );
    virtual bool ReadLabel( MusStaff *staff );
	// specific
	bool ReadLabelStr( wxString label );
    
protected:
    // WDR: member variable declarations for CmpMLFInput
	// page, staff and segment index

private:
    // WDR: handler declarations for CmpMLFInput
	int m_cmp_pos;
	wxString m_cmp_page_label;
};

		#endif // AX_COMPARISON
#endif // AX_RECOGNITION

#endif
