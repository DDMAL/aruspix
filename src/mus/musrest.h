/////////////////////////////////////////////////////////////////////////////
// Name:        musrest.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_REST_H__
#define __MUS_REST_H__

#include "musdurationinterface.h"
#include "muslayerelement.h"
#include "muspositioninterface.h"

//----------------------------------------------------------------------------
// MusRest
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <rest> element. 
 */
class MusRest: public MusLayerElement, public MusDurationInterface, public MusPositionInterface
{
public:
    // constructors and destructors
    MusRest();
    virtual ~MusRest();
    
    /**
     * Comparison operator. 
     * Check if the MusLayerElement if a MusRest and compare attributes
     */
    virtual bool operator==(MusObject& other);
    
    virtual std::string MusClassName( ) { return "MusRest"; };
    
private:
    
public:
    /* Indicates the value to print above multi-measure rests */
    int m_multimeasure_dur;

private:
    
};


#endif
