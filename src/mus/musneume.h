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
    NEUME_TYPE_ANCUS, // Like a climacus with a liquescent at the end
    NEUME_TYPE_CEPHALICUS,
    NEUME_TYPE_CLIVIS,
    NEUME_TYPE_EPIPHONUS,
    NEUME_TYPE_PODATUS,
    NEUME_TYPE_PORRECTUS,
    NEUME_TYPE_PORRECTUS_FLEXUS,
    NEUME_TYPE_PUNCTUM,
    NEUME_TYPE_SALICUS,
    NEUME_TYPE_SCANDICUS,
    NEUME_TYPE_SCANDICUS_FLEXUS,
    NEUME_TYPE_TORCULUS,
    NEUME_TYPE_TORCULUS_LIQUESCENT,
    NEUME_TYPE_TORCULUS_RESUPINUS,
    NEUME_TYPE_VIRGA,
    NEUME_TYPE_VIRGA_LIQUESCENT,
    NEUME_TYPE_COMPOUND
};

enum NeumeElementType {
    NEUME_ELEMENT_PUNCTUM,
    NEUME_ELEMENT_PUNCTUM_WHITE,
    NEUME_ELEMENT_INCLINATUM,
    NEUME_ELEMENT_QUISLIMA,
    NEUME_ELEMENT_CUSTOS
};


class MusNeumeElement
{
public:
	MusNeumeElement(const MusNeumeElement &element);
    MusNeumeElement(MeiElement &meielement, int pitch, int oct);
	virtual ~MusNeumeElement() {};
    
    int getPitchDifference();

private:
	int m_pitch_difference;
	
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
	void NeumeLine( AxDC *dc, MusStaff *staff, int x1, int x2, int y1, int y2);
	void DrawPunctum( AxDC *dc, MusStaff *staff);
	void DrawPunctumInclinatum( AxDC *dc, MusStaff *staff);
	void DrawVirga( AxDC *dc, MusStaff *staff);
	void DrawPodatus( AxDC *dc, MusStaff *staff);
	void DrawClivis( AxDC *dc, MusStaff *staff);
	void DrawPorrectus( AxDC *dc, MusStaff *staff);
	void DrawScandicus( AxDC *dc, MusStaff *staff);
	void DrawTorculus( AxDC *dc, MusStaff *staff);
	void DrawCompound( AxDC *dc, MusStaff *staff);
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
