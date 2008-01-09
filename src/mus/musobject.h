/////////////////////////////////////////////////////////////////////////////
// Name:        wgobject.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __WG_OBJECT_H__
#define __WG_OBJECT_H__

#ifdef AX_WG

#ifdef __GNUG__
	#pragma interface "wgobject.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class WgWindow;
class WgFile;
class WgFileHeader;
class WgParameters;
class WgParameters2;


// WDR: class declarations

//----------------------------------------------------------------------------
// WgObject
//----------------------------------------------------------------------------

class WgObject: public wxObject
{
public:
    // constructors and destructors
    WgObject();
    virtual ~WgObject();
    
    // WDR: method declarations for WgObject
	bool Init( WgWindow *win );
	bool Check() { return m_ok; }
    
protected:
    // WDR: member variable declarations for WgObject
	WgWindow *m_w;
	WgFile *m_f;
	WgFileHeader *m_fh;
	WgParameters *m_p;
	WgParameters2 *m_p2;

private:
	bool m_ok;


public:
    // WDR: member variable declarations for WgObject


private:
    // WDR: handler declarations for WgObject

};

#endif //AX_WG

#endif
