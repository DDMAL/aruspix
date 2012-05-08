/////////////////////////////////////////////////////////////////////////////
// Name:        musrest.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_REST_H__
#define __MUS_REST_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "muslayer.h"
#include "musdurationinterface.h"
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
    
    virtual wxString MusClassName( ) { return "MusRest"; };
    
private:
    
public:

private:
    
};


#endif
