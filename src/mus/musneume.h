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

enum NeumeOrnament {
	HE,
	VE,
	DOT
};

class MusNeumeElement
{
public:
	MusNeumeElement( int _pitch, int _oct );
	MusNeumeElement( const MusNeumeElement &element );
	virtual ~MusNeumeElement() {};

private:	
	int pitch;
	int octave;
	NeumeOrnament ornament;
}; 

class MusNeume: public MusElement
{
public:
    // constructors and destructors
	MusNeume();
	MusNeume( const MusNeume &neume);
    virtual ~MusNeume() {};
    
	//Drawing code
	virtual void Draw( AxDC *dc, MusStaff *staff) {};

	//void append( AxDC *dc, MusStaff *staff ); //for creating multi-note neumes
	
    //virtual void SetPitch( int code, int oct );
	//virtual void SetValue( int value, MusStaff *staff = NULL, int vflag = 0 );
	//int GetValue();
	
	//helper debug method
	//void printNeumeList();
	

private:
	NeumeOrnament ornament;

	//std::vector<MusNeumeElement> n_pitches;
};

#endif // __MUS_NEUME_H__
