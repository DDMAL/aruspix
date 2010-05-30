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
//
// A neume by nature is a composite class, it can consist of a single note, or
// of many notes. 
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
	void MusNeume::note( wxDC *dc, MusStaff *staff );

	void MusNeume::append( wxDC *dc, MusStaff *staff ); //for creating multi-note neumes
	
	//should have some sort of 'toggle' or 'redraw' method here for switching between styles
	
    // WDR: method declarations for MusNeume
	virtual void MusNeume::SetPitch( int code, int oct, MusStaff *staff = NULL );
	virtual void MusNeume::SetValue( int value, MusStaff *staff = NULL, int vflag = 0 );

	
	//helper methods
	
	
public:
	// WDR: member variable declarations for MusNeume
	int size;	//how many notes in group
	wxArrayPtrVoid n_list; //the list of notes in this group
	
private:
    // WDR: handler declarations for MusNeume
	
};


#endif
