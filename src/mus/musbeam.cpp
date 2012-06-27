//
//  musbeam.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 26/06/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musbeam.h"


//----------------------------------------------------------------------------
// MusBeam
//----------------------------------------------------------------------------

MusBeam::MusBeam():
    MusLayerElement()
{
}


MusBeam::~MusBeam()
{
    // we need to detach all notes because it is not to the beam object to delete them
    int i;
    for (i = (int)m_notes.GetCount(); i > 0; i--) {
        m_notes.Detach(i - 1);
    }
}

void MusBeam::AddNote(MusLayerElement *element) {
   
    if (!element->HasDurationInterface()) {
        return;
    }
    MusDurationInterface *note = dynamic_cast<MusDurationInterface*>(note);
    note->m_beam[0] = true;
    m_notes.Add(element);
}