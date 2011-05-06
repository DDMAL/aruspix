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

#include <vector>
using std::vector;

#include "muselement.h"

#include <mei/mei.h>

class MusStaff;

enum NeumeOrnament {
	HE,
	VE,
	DOT
};

enum NeumeType {
    NEUME_TYPE_PUNCTUM,
    NEUME_TYPE_PUNCTUM_INCLINATUM,
    NEUME_TYPE_VIRGA,
    NEUME_TYPE_PODATUS,
    NEUME_TYPE_CLIVIS,
    NEUME_TYPE_PORRECTUS,
    NEUME_TYPE_SCANDICUS,
    NEUME_TYPE_TORCULUS,
    NEUME_TYPE_COMPOUND
};



class MusNeumeElement //what if these were lists? 1 element for pitch, 1 for octave, then maybe 1 for notehead type (punctum, quilisma, etc) and 1 for ornament?
{
public:
	MusNeumeElement( int _pitch, int _oct );
	MusNeumeElement( const MusNeumeElement &element );
    MusNeumeElement(MeiElement &meielement);
	virtual ~MusNeumeElement() {};
    
    int getPitch();
	int getOctave();

private:
	int pitch;
	int octave;
	
    wxString m_pitch;
	NeumeOrnament ornament;
    MeiElement *m_meiref;
}; 

class MusNeume: public MusElement
{
public:
    // constructors and destructors
	MusNeume();
	MusNeume( const MusNeume &neume);
    MusNeume(MeiElement &meielement);
    virtual ~MusNeume() {};
    
    void setType(wxString type, wxString variant);
    void setType(NeumeType type);
    NeumeType getType();
    MeiElement &getMeiElement();
    vector<MusNeumeElement> getPitches();
    
	//Drawing code
	virtual void Draw( AxDC *dc, MusStaff *staff);
	void DrawNeume( AxDC *dc, MusStaff *staff);
	void leg_line( AxDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int pTaille);

	//void append( AxDC *dc, MusStaff *staff ); //for creating multi-note neumes
	
    virtual void SetPitch( int code, int oct );
	//virtual void SetValue( int value, MusStaff *staff = NULL, int vflag = 0 );
	//int GetValue();
	
	//helper debug method
	//void printNeumeList();
	

private:
	NeumeOrnament ornament;
    NeumeType m_type;
    MeiElement *m_meiref;

	vector<MusNeumeElement> m_pitches;
};

#endif // __MUS_NEUME_H__
