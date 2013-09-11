//
//  musbeam.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 26/06/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mustuplet.h"


//----------------------------------------------------------------------------
// MusTuplet
//----------------------------------------------------------------------------

MusTuplet::MusTuplet():
MusLayerElement()
{
}


MusTuplet::~MusTuplet()
{
    // we need to detach all notes because it is not to the beam object to delete them
    int i;
    for (i = GetNoteCount(); i > 0; i--) {
        m_children.Detach(i - 1);
    }
}

void MusTuplet::AddNote(MusLayerElement *element) {
    
    if (!element->HasDurationInterface()) {
        return;
    }
    
    m_children.Add(element);
    Modify();
}