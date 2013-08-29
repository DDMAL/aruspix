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

#include "muslayerelement.h"
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
    
    /**
     * Comparison operator. 
     * Check if the MusLayerElement if a MusClef and compare attributes
     */
    virtual bool operator==(MusLayerElement& other);
    
    virtual wxString MusClassName( ) { return "MusClef"; };	
    
    /**
     * Return the offset of the clef
     */
    int GetClefOffset();
    
    /**
     * Set the value for the clef.
     */
	virtual void SetValue( int value, int flag = 0 );
    
private:
    
public:
    /** Indicates the shape and the line of the clef using clef ids  */
    ClefId m_clefId;

private:
    
};


#endif
