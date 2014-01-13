/////////////////////////////////////////////////////////////////////////////
// Name:        musclef.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_CLEF_H__
#define __MUS_CLEF_H__

#include "musdef.h"
#include "muslayerelement.h"

//----------------------------------------------------------------------------
// MusClef
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <clef> element. 
 */
class MusClef: public LayerElement
{
public:
    // constructors and destructors
    MusClef();
    virtual ~MusClef();
    
    /**
     * Comparison operator. 
     * Check if the LayerElement if a MusClef and compare attributes
     */
    virtual bool operator==(MusObject& other);
    
    virtual std::string MusClassName( ) { return "MusClef"; };
    
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
