/////////////////////////////////////////////////////////////////////////////
// Name:        mlfbmp.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __WG_IOMLFBMP_H__
#define __WG_IOMLFBMP_H__

#ifdef AX_WG

#ifdef __GNUG__
    #pragma interface "mlfbmp.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musiomlf.h"

class ImPage;
class ImStaff;


class MLFBitmapType;
WX_DECLARE_OBJARRAY( MLFBitmapType, ArrayOfMLFBitmapTypes);

// WDR: class declarations


//----------------------------------------------------------------------------
// MLFBitmapType
//----------------------------------------------------------------------------

class MLFBitmapType: public wxObject
{
public:
    // constructors and destructors
    MLFBitmapType( );
    virtual ~MLFBitmapType() {};
    
    // WDR: method declarations for MLFBitmapType
    
public:
    // WDR: member variable declarations for MLFBitmapType
	//int m_width;
	wxString m_label;
	wxBitmap m_bitmap, m_flip_bitmap;
	int m_bmpoffset;
	bool m_flip;


private:
    // WDR: handler declarations for MLFBitmapType
};


//----------------------------------------------------------------------------
// MLFBitmapTypes
//----------------------------------------------------------------------------

class MLFBitmapTypes: public wxObject
{
public:
    // constructors and destructors
    MLFBitmapTypes();
    virtual ~MLFBitmapTypes();
    
    // WDR: method declarations for MLFBitmapTypes
    //int GetWidth( MLFSymbol *symbole );
	MLFBitmapType *GetType( MLFSymbol *symbole );
	bool Load( wxString file );
	int GetCount( ) { return (int)m_types.GetCount(); }
    
private:
    // WDR: member variable declarations for MLFBitmapTypes
	ArrayOfMLFBitmapTypes m_types;

private:
    // WDR: handler declarations for MLFBitmapTypes
};


//----------------------------------------------------------------------------
// MLFSymbolBmp
//----------------------------------------------------------------------------

class MLFSymbolBmp: public MLFSymbol
{
public:
    // constructors and destructors
    MLFSymbolBmp();
    virtual ~MLFSymbolBmp() {};
    
    // WDR: method declarations for MLFSymbolBmp
	virtual void SetValue( char type, wxString subtype, int position, int value = 0, char pitch = 0, int oct = 0, int flag = 0);
    virtual wxString GetLabel( );
	int GetVOffset() { return m_voffset; }
	bool IsCurrent() { return m_isCurrent; }
	void SetCurrent( bool isCurrent = true ) { m_isCurrent = isCurrent; }
    
protected:
    // WDR: member variable declarations for MLFSymbolBmp
	int m_voffset;
	bool m_isCurrent;

private:
    // WDR: handler declarations for MLFSymbolBmp

	DECLARE_DYNAMIC_CLASS(MLFSymbolBmp)
};


//----------------------------------------------------------------------------
// MLFBitmaps
//----------------------------------------------------------------------------

class MLFBitmaps: public MLFOutput
{
public:
    // constructors and destructors
    MLFBitmaps( WgFile *file, wxString filename, wxString model_symbole_name = "MLFSymbolBmp" );
    virtual ~MLFBitmaps();
    
    // WDR: method declarations for MLFBitmaps
    //bool GenerateBitmaps( ImPage *impage );
	wxBitmap GenerateBitmap( ImStaff *imstaff, WgStaff *wgstaff, int currentElementNo = -1 );
	int GetCurrentX() { return m_currentX; }
	int GetCurrentWidth() { return m_currentWidth; }
    //virtual bool WritePage( const WgPage *page );
    virtual bool WriteStaff( const WgStaff *staff, int currentElementNo = -1 );
	// specific
	virtual void StartLabel( );
	//virtual void EndLabel( );
    
protected:
    // WDR: member variable declarations for MLFBitmaps
	// page
	ImPage *m_impage;
	int m_currentX; // keep x and width of currentElement in staff; used to redraw only this position
	int m_currentWidth;

public:
	//bool m_addPageNo;
	MLFBitmapTypes m_bitmap_types; // tableau des images - par type



private:
    // WDR: handler declarations for MLFBitmaps
};


#endif //AX_WG

#endif
