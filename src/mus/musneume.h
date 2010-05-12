/////////////////////////////////////////////////////////////////////////////
// Name:        musneume.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_NEUME_H__
#define __MUS_NEUME_H__

#ifdef __GNUG__
    #pragma interface "musneume.cpp"    
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/dynarray.h"

#include "muselement.h"

class MusStaff;

// WDR: class declarations

//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

class MusNeume: public MusElement
{
public:
    // constructors and destructors
    MusNeume();
	MusNeume( char _sil, unsigned char _val, unsigned char _code );
	MusNeume( const MusNeume& neume ); // copy contructor
	MusNeume& operator=( const MusNeume& neume ); // copy assignement;
    virtual ~MusNeume();
    
	virtual void MusNeume::Draw( wxDC *dc, MusStaff *staff);

    // WDR: method declarations for MusNeume
	virtual void MusNeume::SetPitch( int code, int oct, MusStaff *staff = NULL );
	virtual void MusNeume::SetValue( int value, MusStaff *staff = NULL, int vflag = 0 );

public:
	// WDR: member variable declarations for MusNeume
	
private:
    // WDR: handler declarations for MusNeume
};


#endif
