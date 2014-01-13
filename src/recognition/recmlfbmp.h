/////////////////////////////////////////////////////////////////////////////
// Name:        recmlfbmp.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMLFBMP_H__
#define __MUS_IOMLFBMP_H__

#ifdef AX_RECOGNITION

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musapp/musiomlf.h"

class ImPage;
class ImStaff;


class RecMLFBmpType;
WX_DECLARE_OBJARRAY( RecMLFBmpType, ArrayOfMLFBitmapTypes);



//----------------------------------------------------------------------------
// RecMLFBmpType
//----------------------------------------------------------------------------

class RecMLFBmpType: public wxObject
{
public:
    // constructors and destructors
    RecMLFBmpType( );
    virtual ~RecMLFBmpType() {};
    
        
public:
    //int m_width;
	wxString m_label;
	wxBitmap m_bitmap, m_flip_bitmap;
	int m_bmpoffset;
	bool m_flip;


private:
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
    
        //int GetWidth( MusMLFSymbol *symbol );
	RecMLFBmpType *GetType( MusMLFSymbol *symbol );
	bool Load( wxString file );
	int GetCount( ) { return (int)m_types.GetCount(); }
    
private:
    ArrayOfMLFBitmapTypes m_types;

private:
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
    
    virtual void SetValue( char type, wxString subtype, int position, int value = 0, char pitch = 0, int oct = 0, int flag = 0);
    virtual wxString GetLabel( );
	int GetVOffset() { return m_voffset; }
	bool IsCurrent() { return m_isCurrent; }
	void SetCurrent( bool isCurrent = true ) { m_isCurrent = isCurrent; }
    
protected:
    int m_voffset;
	bool m_isCurrent;

private:
    
	DECLARE_DYNAMIC_CLASS(RecMLFSymbolBmp)
};


//----------------------------------------------------------------------------
// RecMLFBmp
//----------------------------------------------------------------------------

class RecMLFBmp: public MusMLFOutput
{
public:
    // constructors and destructors
    RecMLFBmp( MusDoc *file, wxString filename, wxString model_symbol_name = "RecMLFSymbolBmp" );
    virtual ~RecMLFBmp();
    
        //bool GenerateBitmaps( ImPage *impage );
	wxBitmap GenerateBitmap( ImStaff *imstaff, Layer *musLayer, int currentElementNo = -1 );
	int GetCurrentX() { return m_currentX; }
	int GetCurrentWidth() { return m_currentWidth; }
    //virtual bool WritePage( const Page *page );
    virtual bool WriteLayer( const Layer *layer, int currentElementNo = -1 );
	// specific
	virtual void StartLabel( );
	//virtual void EndLabel( );
    
protected:
    // page
	ImPage *m_impage;
	int m_currentX; // keep x and width of currentElement in staff; used to redraw only this position
	int m_currentWidth;

public:
	RecMLFBmpTypes m_bitmap_types; // tableau des images - par type



private:
    };

#endif //AX_RECOGNITION

#endif
