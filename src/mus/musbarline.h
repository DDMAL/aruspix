/////////////////////////////////////////////////////////////////////////////
// Name:        musbarline.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_BARLINE_H__
#define __MUS_BARLINE_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "muslayer.h"

enum BarlineType {
    BARLINE_SINGLE = 0,
    BARLINE_END,
    BARLINE_START, // no MEI equivalent
    BARLINE_RPTBOTH,
    BARLINE_RPTSTART,
    BARLINE_RPTEND,
    BARLINE_DBL,
    BARLINE_BRACE, // no MEI equivalent
    BARLINE_BRACKET
};

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
