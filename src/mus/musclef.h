/////////////////////////////////////////////////////////////////////////////
// Name:        musclef.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_CLEF_H__
#define __MUS_CLEF_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "muslayer.h"


enum ClefId {
    SOL2 = 1,
    SOL1,
    FA4,
    FA3,
    UT1,
    UT2,
    UT3,
    UT4,
    SOLva,
    FA5,
    UT5,
    CLEPERC,
    //neumatic clefs
    nC1,
    nC2,
    nC3,
    nC4,
    nF1,
    nF2,
    nF3,
    nF4
};


//----------------------------------------------------------------------------
// MusClef
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <clef> element. 
 */
class MusClef: public MusLayerElement
{
public:
    // constructors and destructors
    MusClef();
    virtual ~MusClef();
    
    /**
     * Return the offset of the clef
     */
    int GetClefOffset();
    
private:
    
public:
    /** Indicates the shape and the line of the clef using clef ids  */
    ClefId m_clefId;

private:
    
};


#endif
