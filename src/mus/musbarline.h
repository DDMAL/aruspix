/////////////////////////////////////////////////////////////////////////////
// Name:        musbarline.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_BARLINE_H__
#define __MUS_BARLINE_H__

#include "musdef.h"
#include "muslayerelement.h"

//----------------------------------------------------------------------------
// MusBarline
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <barline> element. 
 */
class MusBarline: public MusLayerElement
{
public:
    // constructors and destructors
    MusBarline();
    virtual ~MusBarline();
    
    /**
     * Comparison operator. 
     * Check if the MusLayerElement if a MusBarline and compare attributes
     */
    virtual bool operator==(MusObject& other);
    
    virtual std::string MusClassName( ) { return "MusBarline"; };
    
private:
    
public:
    /** Indicates the barline type (@rend) */
    BarlineType m_barlineType;
    /** Indicates a partial barline (inbetween the staves) - no MEI equivalent */
    bool m_partialBarline;
    /** Indicates a barline displayed only on the staff - no MEI equivalent */
    bool m_onStaffOnly;

private:
    
};


#endif
