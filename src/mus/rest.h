/////////////////////////////////////////////////////////////////////////////
// Name:        musrest.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_REST_H__
#define __MUS_REST_H__

#include "durationinterface.h"
#include "layerelement.h"
#include "positioninterface.h"

namespace vrv {

//----------------------------------------------------------------------------
// Rest
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <rest> element. 
 */
class Rest: public LayerElement, public DurationInterface, public PositionInterface
{
public:
    // constructors and destructors
    Rest();
    virtual ~Rest();
    
    /**
     * Comparison operator. 
     * Check if the LayerElement if a Rest and compare attributes
     */
    virtual bool operator==(Object& other);
    
    virtual std::string MusClassName( ) { return "Rest"; };
    
private:
    
public:
    /* Indicates the value to print above multi-measure rests */
    int m_multimeasure_dur;

private:
    
};

} // namespace vrv    

#endif
