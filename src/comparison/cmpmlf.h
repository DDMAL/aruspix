/////////////////////////////////////////////////////////////////////////////
// Name:        cmpmlf.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.   
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




//----------------------------------------------------------------------------
// CmpMLFSymb
//----------------------------------------------------------------------------

class CmpMLFSymb: public MusMLFSymbol
{
public:
    // constructors and destructors
    CmpMLFSymb();
    virtual ~CmpMLFSymb() {};
    
    virtual void SetValue( char type, wxString subtype, int position, int value = 0, char pitch = 0, int oct = 0, int flag = 0);
    virtual wxString GetLabel( );

protected:
    
public:
	wxString m_im_filename;
	int m_im_staff;
	int m_im_pos;
	int m_index;
	//
	wxString m_cache;

private:
    
	DECLARE_DYNAMIC_CLASS(CmpMLFSymb)
};


//----------------------------------------------------------------------------
// CmpMLFOutput
//----------------------------------------------------------------------------

class CmpMLFOutput: public MusMLFOutput
{
public:
    // constructors and destructors
    CmpMLFOutput( MusDoc *file, wxString filename, wxString model_symbol_name = "CmpSymbol" );
	CmpMLFOutput( MusDoc *file, int fd, wxString filename, wxString model_symbol_name = "CmpSymbol" );
	//CmpMLFOutput( MusDoc *file, wxFile *wxfile, wxString filename, wxString model_symbol_name = "CmpSymbol" );
    virtual ~CmpMLFOutput();
    
    //bool ExportFile( MusDoc *file, wxString filename);	// replace  musfile set in the constructor
														// and export it
														// allow exportation of several files in one mlf
														
    //virtual bool WritePage( const MusPage *page, bool write_header = false );
    virtual bool WriteStaff( const MusLaidOutStaff *staff );
	// output methods
	virtual void StartLabel( );
	virtual void EndLabel( );
    
protected:
    
public:
	bool m_ignore_clefs;
	bool m_ignore_custos;
	bool m_ignore_keys;

private:
    };


//----------------------------------------------------------------------------
// CmpMLFInput
//----------------------------------------------------------------------------

class CmpMLFInput: public MusMLFInput
{
public:
    // constructors and destructors
    CmpMLFInput( MusDoc *file, wxString filename );
    virtual ~CmpMLFInput();
    
        virtual MusLaidOutStaff *ImportFileInStaff(  );
    virtual bool ReadLabel( MusLaidOutStaff *staff );
	// specific
	bool ReadLabelStr( wxString label );
    
protected:
    
private:
    int m_cmp_pos;
	wxString m_cmp_page_label;
};

		#endif // AX_COMPARISON
#endif // AX_RECOGNITION

#endif
