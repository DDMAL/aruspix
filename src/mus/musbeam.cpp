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
        MusDurationInterface *note = dynamic_cast<MusDurationInterface*>(m_notes.Detach(i - 1));
        note->m_beam[0] = 0;
    }
}

void MusBeam::AddNote(MusLayerElement *element) {
   
    if (!element->HasDurationInterface()) {
        return;
    }
    MusDurationInterface *note = dynamic_cast<MusDurationInterface*>(element);
    // Set the first as initial
    if (m_notes.Count() == 0)
        note->m_beam[0] = BEAM_INITIAL;
    else
        note->m_beam[0] = BEAM_TERMINAL;
    m_notes.Add(element);
    
    // Set the last note to median if we have more than one note in the array
    if (m_notes.Count() > 2) {
        MusDurationInterface *last_note = dynamic_cast<MusDurationInterface*>(&m_notes[m_notes.Count() - 2]);
        last_note->m_beam[0] = BEAM_MEDIAL;
    }
}