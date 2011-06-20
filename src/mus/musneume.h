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

#include "neumedef.h"

class MusStaff;

enum NeumeOrnament {
    HE,
    VE,
    DOT,
	NONE
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
    NEUME_TYPE_PUNCTUM_INCLINATUM,
	NEUME_TYPE_PUNCTUM_WHITE,
    NEUME_TYPE_SALICUS,
    NEUME_TYPE_SCANDICUS,
    NEUME_TYPE_SCANDICUS_FLEXUS,
    NEUME_TYPE_TORCULUS,
    NEUME_TYPE_TORCULUS_LIQUESCENT,
    NEUME_TYPE_TORCULUS_RESUPINUS,
    NEUME_TYPE_VIRGA,
    NEUME_TYPE_VIRGA_LIQUESCENT,
    NEUME_TYPE_COMPOUND,
    NEUME_TYPE_CUSTOS
};

enum NeumeElementType {
    NEUME_ELEMENT_PUNCTUM,
    NEUME_ELEMENT_PUNCTUM_WHITE,
    NEUME_ELEMENT_INCLINATUM,
    NEUME_ELEMENT_QUILISMA,
	NEUME_ELEMENT_PES,
	NEUME_ELEMENT_CEPHALICUS,
	NEUME_ELEMENT_CUSTOS,
	NEUME_ELEMENT_LIQUESCENT_UP,
	NEUME_ELEMENT_LIQUESCENT_DOWN,
	NEUME_ELEMENT_EPIPHONUS
};


class MusNeumeElement;

class MusNeume: public MusElement
{
public:
    // constructors and destructors
    MusNeume();
    MusNeume( const MusNeume &neume);
    MusNeume(MeiElement &meielement);
    virtual ~MusNeume() {}
    
    void setType(wxString type, wxString variant);
    void setType(NeumeType type);
    NeumeType getType();
	vector<MusNeumeElement*>& getPitches();
	void CalcPitches(MusStaff *staff);
	void AppendNote(MusNeumeElement* note);
	
	//MEI stuff
    MeiElement* getMeiRef();
	void deleteMeiRef();
	void updateMeiRef();
	void updateMeiZone();
	void setMeiStaffZone(MeiElement *element);
	void setMeiRef(MeiElement *element);
	void setMeiFirstPitch(MeiElement *element);
    
    //Drawing code
    virtual void Draw( AxDC *dc, MusStaff *staff);
    void NeumeLine( AxDC *dc, MusStaff *staff, int x1, int x2, int y1, int y2);
    void DrawAncus( AxDC *dc, MusStaff *staff);
    void DrawCephalicus( AxDC *dc, MusStaff *staff);
    void DrawPunctum( AxDC *dc, MusStaff *staff);
    void DrawPunctumInclinatum( AxDC *dc, MusStaff *staff);
	void DrawPunctumWhite( AxDC *dc, MusStaff *staff);
    void DrawVirga( AxDC *dc, MusStaff *staff);
    void DrawVirgaLiquescent( AxDC *dc, MusStaff *staff);
    void DrawPodatus( AxDC *dc, MusStaff *staff);
    void DrawClivis( AxDC *dc, MusStaff *staff);
    void DrawEpiphonus( AxDC *dc, MusStaff *staff);
    void DrawPorrectus( AxDC *dc, MusStaff *staff);
    void DrawPorrectusFlexus( AxDC *dc, MusStaff *staff);
    void DrawSalicus( AxDC *dc, MusStaff *staff);
    void DrawScandicus( AxDC *dc, MusStaff *staff);
    void DrawScandicusFlexus( AxDC *dc, MusStaff *staff);
    void DrawTorculus( AxDC *dc, MusStaff *staff);
    void DrawTorculusLiquescent( AxDC *dc, MusStaff *staff);
    void DrawTorculusResupinus( AxDC *dc, MusStaff *staff);
    void DrawCompound( AxDC *dc, MusStaff *staff);
    void DrawCustos( AxDC *dc, MusStaff *staff);
	void DrawDots(AxDC *dc, MusStaff *staff);
    void leg_line( AxDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int pTaille);
	void SetColour();

    //void append( AxDC *dc, MusStaff *staff ); //for creating multi-note neumes
    
    virtual void SetPitch( int pitch, int oct );
    //virtual void SetValue( int value, MusStaff *staff = NULL, int vflag = 0 );
    //int GetValue(); 
	
	
	//this is mostly for MEI
	bool inclinatum;
	bool quilisma;
	vector<MusNeumeElement*> m_pitches;

private:
    void readNoteContainer(MeiElement &nc, int pitch, int oct);

    NeumeOrnament ornament;
    NeumeType m_type;
    MeiElement *m_meiref;
	MeiElement *m_meifirstpitch;
	MeiElement *m_meistaffzone;
};

class MusNeumeElement: public MusElement
{
public:
	friend class MusNeume;
	MusNeumeElement();
    MusNeumeElement(const MusNeumeElement &other);
	MusNeume& operator=(const MusNeume &neume);
    MusNeumeElement(MeiElement &meielement, int pitch, int oct);
    MusNeumeElement(int _pitchDifference);
    virtual ~MusNeumeElement() {}
    
	void Draw( AxDC *dc, MusStaff *staff );
	virtual void SetPitch( int pitch, int oct );
	void setParent(MusNeume *_parent);
	void leg_line( AxDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int pTaille);
	
	MusNeume* getParent();
    int getPitchDifference();
    NeumeElementType getElementType();
	void setElementType(NeumeElementType type);
    MeiElement* getMeiElement();
    void updateMeiElement(string pitch, int oct);
	void updateMeiZone();
	void setMeiElement(MeiElement *element);
	void deleteMeiRef();
	NeumeOrnament getOrnament();
	
private:
    int m_pitch_difference;
    NeumeElementType m_element_type;
    NeumeOrnament ornament;
    MeiElement *m_meiref;
	MusNeume *parent;
};

#endif // __MUS_NEUME_H__
