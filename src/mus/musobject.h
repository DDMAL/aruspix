/////////////////////////////////////////////////////////////////////////////
// Name:        musobject.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_OBJECT_H__
#define __MUS_OBJECT_H__

#ifdef __GNUG__
	#pragma interface "musobject.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class MusWindow;
class MusFile;
class MusFileHeader;
class MusParameters;


// WDR: class declarations

//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

class MusObject: public wxObject
{
public:
    // constructors and destructors
    MusObject();
    virtual ~MusObject();
    
    // WDR: method declarations for MusObject
	bool Init( MusWindow *win );
	bool Check() { return m_ok; }
    
protected:
    // WDR: member variable declarations for MusObject
	MusWindow *m_w;
	MusFile *m_f;
	MusFileHeader *m_fh;
	MusParameters *m_p;

private:
	bool m_ok;


public:
    // WDR: member variable declarations for MusObject


private:
    // WDR: handler declarations for MusObject

};


#endif
