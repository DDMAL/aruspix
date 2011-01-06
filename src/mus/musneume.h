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
#include <vector>
#include "neumedef.h"

class MusStaff;

//----------------------------------------------------------------------------
// MusNeume
//
// A MusNeume contains one or more MusNeumePitches
//----------------------------------------------------------------------------

class MusNeumePitch
{
public:
	MusNeumePitch( int _code, int _oct, unsigned char _val );
	MusNeumePitch( MusNeumePitch *pitch);
	virtual ~MusNeumePitch() {}
	void SetPitch( int code, int oct );
	void SetValue( int value );
	int GetValue( );
	wxString GetFestaString( );
	int GetCode();
	int GetOct();
	
	int Compare(MusNeumePitch *other);
	int Pitch_Diff(MusNeumePitch *other);
	int Pitch_Diff(int code, int oct);

private:	
	wxString m_font_str;	//used for font drawing
	
	int code;
	int oct;
	unsigned char val;
}; 

class MusNeume: public MusElement
{
public:
    // constructors and destructors
	MusNeume();
	MusNeume( unsigned char _val, unsigned char _code );
	MusNeume( MusNeume *neume);
    virtual ~MusNeume();
    
	//Drawing code
	virtual void Draw( AxDC *dc, MusStaff *staff);
//	void note( AxDC *dc, MusStaff *staff );
	void DrawBox( AxDC *dc, MusStaff *staff );
	void DrawNeume( AxDC *dc, MusStaff *staff );
	void DrawPunctums( AxDC *dc, MusStaff *staff );
	void leg_line( AxDC *dc, int y_n, int y_p, int xn, 
							unsigned int smaller, int pTaille);
	void append( AxDC *dc, MusStaff *staff ); //for creating multi-note neumes
	
	//should have some sort of 'toggle' or 'redraw' method here for switching between styles
	
    virtual void SetPitch( int code, int oct );
	virtual void SetValue( int value, MusStaff *staff = NULL, int vflag = 0 );
	int GetValue();
	
	//helper debug method
	void printNeumeList();
	
	int getYPos(int index, MusStaff *staff);
	
	int GetMaxPitch();
	int GetMinPitch();
	int GetPitchRange();
	int Length();
	
	bool ascending(int p1, int p2);
	bool descending(int p1, int p2);
	
	void SetClosed(bool value);
	void GetNextPunctum();
	void GetPreviousPunctum();
	
	bool IsClosed();
	
	//when appending notes, do we discard the object?
	//may need a helper class for composite neumes.
	void InsertPitchAfterSelected();
	void RemoveSelectedPitch();
//	void Append();

	
	// to create compound neumes (ineumes)
	
	MusNeume *next;
	MusNeume *prev;
	
	// x coord of the right-most edge of the neume
	// used for compound neume drawing
	unsigned int xrel_right; // set during ligature drawing
	
	
	//how many pitches are in this neume?
	//int length;
	
	//which note of the group has been selected?
	unsigned int n_selected;
	
	/**the list of actual pitches within the neume
	
	 Pitches for neumes are implemented as follows:
	 The first pitch of the neume group is the same as the 'code' of the entire neume.
	 The rest of the pitches (if any) in the neume group are then drawn relative to the
	 first pitch.	 
	 
	 */
	
	
//		std::vector<Ligature*> ligatures;
	
	//TODO: move to ligature
	std::vector<MusNeumePitch*> n_pitches;
	std::vector<MusNeumePitch*>::iterator iter;
	
	int p_max; //the highest pitch in the neume group, relative to base pitch
	int p_min; //lowest pitch, relative to base pitch
	int p_range; // the pitch range, getter acts as setter!
			
	bool closed; 
		
	//ligature drawing methods
	//TODO: move to ligature class
	void drawLigature( AxDC *dc, MusStaff *staff );
	void clivis( AxDC *dc, MusStaff *staff );
	void podatus( AxDC *dc, MusStaff *staff );
	void porrectus( AxDC *dc, MusStaff *staff );
	void neume_stem( AxDC *dc, MusStaff *staff, int xrel, 
						  int index, int pitch_range = 0, int side = LEFT_STEM);
//							  int end_pitch, int side);
//	void neume_line( AxDC *dc, MusStaff *staff, int side );
//	void climacus( AxDC *dc, MusStaff *staff );
	
	//pitch and octave of first pitch of the neume
	int GetOct(); 
	int GetCode();
	
//	void CheckForBreaks();
	void Copy();
	
	// mei related fields
	
	char n_type; // uneume or ineume, uneume by default
	char name; // name of the neume (climacus, pes, etc)
	char form; // neume form, could be "liquescent1" for example
	
	
	
public:
	//	int size;	//how many notes in group
	
private:
    
};


#endif
