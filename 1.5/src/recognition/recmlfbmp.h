/////////////////////////////////////////////////////////////////////////////
// Name:        recmlfbmp.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMLFBMP_H__
#define __MUS_IOMLFBMP_H__

#ifdef AX_RECOGNITION

#ifdef __GNUG__
    #pragma interface "recmlfbmp.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "mus/musiomlf.h"

class ImPage;
class ImStaff;


class RecMLFBmpType;
WX_DECLARE_OBJARRAY( RecMLFBmpType, ArrayOfMLFBitmapTypes);

// WDR: class declarations


//----------------------------------------------------------------------------
// RecMLFBmpType
//----------------------------------------------------------------------------

class RecMLFBmpType: public wxObject
{
public:
    // constructors and destructors
    RecMLFBmpType( );
    virtual ~RecMLFBmpType() {};
    
    // WDR: method declarations for RecMLFBmpType
    
public:
    // WDR: member variable declarations for RecMLFBmpType
	//int m_width;
	wxString m_label;
	wxBitmap m_bitmap, m_flip_bitmap;
	int m_bmpoffset;
	bool m_flip;


private:
    // WDR: handler declarations for RecMLFBmpType
};


//----------------------------------------------------------------------------
// RecMLFBmpTypes
//----------------------------------------------------------------------------

class RecMLFBmpTypes: public wxObject
{
public:
    // constructors and destructors
    RecMLFBmpTypes();
    virtual ~RecMLFBmpTypes();
    
    // WDR: method declarations for RecMLFBmpTypes
    //int GetWidth( MusMLFSymbol *symbol );
	RecMLFBmpType *GetType( MusMLFSymbol *symbol );
	bool Load( wxString file );
	int GetCount( ) { return (int)m_types.GetCount(); }
    
private:
    // WDR: member variable declarations for RecMLFBmpTypes
	ArrayOfMLFBitmapTypes m_types;

private:
    // WDR: handler declarations for RecMLFBmpTypes
};


//----------------------------------------------------------------------------
// RecMLFSymbolBmp
//----------------------------------------------------------------------------

class RecMLFSymbolBmp: public MusMLFSymbol
{
public:
    // constructors and destructors
    RecMLFSymbolBmp();
    virtual ~RecMLFSymbolBmp() {};
    
    // WDR: method declarations for RecMLFSymbolBmp
	virtual void SetValue( char type, wxString subtype, int position, int value = 0, char pitch = 0, int oct = 0, int flag = 0);
    virtual wxString GetLabel( );
	int GetVOffset() { return m_voffset; }
	bool IsCurrent() { return m_isCurrent; }
	void SetCurrent( bool isCurrent = true ) { m_isCurrent = isCurrent; }
    
protected:
    // WDR: member variable declarations for RecMLFSymbolBmp
	int m_voffset;
	bool m_isCurrent;

private:
    // WDR: handler declarations for RecMLFSymbolBmp

	DECLARE_DYNAMIC_CLASS(RecMLFSymbolBmp)
};


//----------------------------------------------------------------------------
// RecMLFBmp
//----------------------------------------------------------------------------

class RecMLFBmp: public MusMLFOutput
{
public:
    // constructors and destructors
    RecMLFBmp( MusFile *file, wxString filename, wxString model_symbol_name = "RecMLFSymbolBmp" );
    virtual ~RecMLFBmp();
    
    // WDR: method declarations for RecMLFBmp
    //bool GenerateBitmaps( ImPage *impage );
	wxBitmap GenerateBitmap( ImStaff *imstaff, MusStaff *musStaff, int currentElementNo = -1 );
	int GetCurrentX() { return m_currentX; }
	int GetCurrentWidth() { return m_currentWidth; }
    //virtual bool WritePage( const MusPage *page );
    virtual bool WriteStaff( const MusStaff *staff, int currentElementNo = -1 );
	// specific
	virtual void StartLabel( );
	//virtual void EndLabel( );
    
protected:
    // WDR: member variable declarations for RecMLFBmp
	// page
	ImPage *m_impage;
	int m_currentX; // keep x and width of currentElement in staff; used to redraw only this position
	int m_currentWidth;

public:
	RecMLFBmpTypes m_bitmap_types; // tableau des images - par type



private:
    // WDR: handler declarations for RecMLFBmp
};

#endif //AX_RECOGNITION

#endif
