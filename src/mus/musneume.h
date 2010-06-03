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

// 'hot corners' defines...

#define UPPER 0
#define LOWER 1
#define LEFT_CORNER 0
#define RIGHT_CORNER 1

class MusStaff;

// WDR: class declarations

//----------------------------------------------------------------------------
// MusNeume
//
// A neume by nature is a composite class, it can consist of a single note, or
// of many notes. 
//----------------------------------------------------------------------------


typedef struct n_pitch {
	int code;
	int oct;
}NPitch;

class MusNeume: public MusElement
{
public:
    // constructors and destructors
    MusNeume();
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
	
	bool MusNeume::IsClosed();
	
	//when appending notes, do we discard the object?
	//may need a helper class for composite neumes.
	void MusNeume::Append();

	//how many notes are in this neume?
	int length;
	
	//which note of the group has been selected?
	int n_selected;
	
	//the list of actual pitches within the neume
	NPitch *n_pitches;
	
	//different single note 'styles' that can be cycled through
	int modes[3];
	
	// possible places for ligatures to 'attach'
	// hotCorners[upper/lower][left/right]
	// you cannot have more than one 'vertical attachment' on a single note
		
	bool closed; 
		
	//helper methods
	
	
public:
	// WDR: member variable declarations for MusNeume
	int size;	//how many notes in group
	wxArrayPtrVoid n_list; //the list of notes in this group
	
private:
    // WDR: handler declarations for MusNeume
	
};


#endif
