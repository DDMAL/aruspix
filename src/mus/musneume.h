/////////////////////////////////////////////////////////////////////////////
// Name:        musneume.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_NEUME_H__
#define __MUS_NEUME_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/dynarray.h"

#include <vector>
using std::vector;
using std::string;

#include "muslayer.h"
#include "muspitchinterface.h"

#include <mei/meielement.h>
#include <mei/exceptions.h>

using mei::MeiElement;
using mei::MeiAttribute;

#include "musdef.h"

class MusLaidOutStaff;

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
};

//----------------------------------------------------------------------------
// MusNeumeElement
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <?> element. 
 */
class MusNeumeElement: public MusLayerElement, public MusPitchInterface
{
public:
    MusNeumeElement(const MusNeumeElement &element);
    MusNeumeElement(MeiElement &meielement, int pitch, int oct);
    //MusNeumeElement(int _pitchDifference);
    virtual ~MusNeumeElement() {}
    
    int getPitchDifference();
    NeumeElementType getElementType();
    MeiElement &getMeiElement();
    void updateMeiRef(string pitch, int oct);
	void deleteMeiRef();
	NeumeOrnament getOrnament();
    
    // ax2 - member previously in MusElement
    // LP: I don't think when need to keep them as member - it would be better to have the a local variables in the MusRC methods
    int m_x_abs; // used for storing the x position while drawing
    int m_y_drawing; // idem for y position
    
	
private:
    int m_pitch_difference;
    NeumeElementType m_element_type;
    NeumeOrnament ornament;
    MeiElement *m_meiref;
};


//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <neume> element. 
 */
class MusNeume: public MusLayerElement, public MusPitchInterface
{
public:
    // constructors and destructors
    MusNeume();
    MusNeume( const MusNeume &neume);
    MusNeume(MeiElement &meielement);
    virtual ~MusNeume() {}
    
    virtual wxString MusClassName( ) { return "MusNeume"; };
    
    void setType(wxString type);
    void setType(NeumeType type);
    NeumeType getType();
    MeiElement &getMeiElement();
    vector<MusNeumeElement> getPitches();
	void deleteMeiRef();

    //void append( MusDC *dc, MusLaidOutStaff *staff ); //for creating multi-note neumes
    
    virtual void SetPitch( int pitch, int oct );
    //virtual void SetValue( int value, MusLaidOutStaff *staff = NULL, int vflag = 0 );
    //int GetValue();
    
    //helper debug method
    //void printNeumeList();
    
    // ax2 - member previously in MusElement
    std::string PitchToStr(int pitch);
    int StrToPitch(std::string pitch);

private:
    void readNoteContainer(MeiElement &nc, int pitch, int oct);

    NeumeOrnament ornament;
    NeumeType m_type;
    /**
     * A member reamaining from before 2.0
     */
    MeiElement *m_meiref;
    

public:
    vector<MusNeumeElement> m_pitches; // LP: made it public for drawing from MusRC
};

#endif // __MUS_NEUME_H__
