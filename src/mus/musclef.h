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
#include "musdef.h"

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
    
    virtual wxString MusClassName( ) { return "MusClef"; };	
    
    /**
     * Return the offset of the clef
     */
    int GetClefOffset();
    
private:
    
public:
    /** Indicates the shape and the line of the clef using clef ids  */
    unsigned char m_clefId;

private:
    
};


#endif
