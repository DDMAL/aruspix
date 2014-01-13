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
class MusBarline: public LayerElement
{
public:
    // constructors and destructors
    MusBarline();
    virtual ~MusBarline();
    
    /**
     * Comparison operator. 
     * Check if the LayerElement if a MusBarline and compare attributes
     */
    virtual bool operator==(MusObject& other);
    
    virtual std::string MusClassName( ) { return "MusBarline"; };
    
    /**
     * Use to set the alignment for the MusMeasure MusBarline members.
     * This is as special case where we need to add to the measure aligner.
     */
    void SetAlignment( MusAlignment *alignment ) { m_alignment = alignment; };
    
    /*
     * Return true if the barline type requires repetition dots to be drawn.
     */
    bool HasRepetitionDots( );
    
    /**
     * Return the default horizontal spacing of notes.
     */
    virtual int GetHorizontalSpacing( ) { return 0; }
    
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
