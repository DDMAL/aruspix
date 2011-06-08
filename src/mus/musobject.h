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

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class MusRC;
class MusFile;
class MusFileHeader;
class MusParameters;



//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

class MusObject: public wxObject
{
public:
    // constructors and destructors
    MusObject();
    virtual ~MusObject();
    
    bool Init( MusRC *renderer );
	bool Check() { return m_ok; }
    
protected:
    MusRC *m_r;
	MusFile *m_f;
	MusFileHeader *m_fh;
	MusParameters *m_p;

private:
	bool m_ok;


public:
    

private:
    
};


#endif
