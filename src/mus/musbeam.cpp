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
    for (i = GetNoteCount(); i > 0; i--) {
        MusDurationInterface *note = dynamic_cast<MusDurationInterface*>(m_children.Detach(i - 1));
        note->m_beam[0] = 0;
    }
}

void MusBeam::AddNote(MusLayerElement *element) {
   
    if (!element->HasDurationInterface()) {
        return;
    }
    element->SetParent( this );
    
    MusDurationInterface *note = dynamic_cast<MusDurationInterface*>(element);
    // Set the first as initial
    if (GetNoteCount() == 0)
        note->m_beam[0] = BEAM_INITIAL;
    else
        note->m_beam[0] = BEAM_TERMINAL;
    m_children.Add(element);
    
    // Set the last note to median if we have more than one note in the array
    if (GetNoteCount() > 2) {
        MusDurationInterface *last_note = dynamic_cast<MusDurationInterface*>(&m_children[GetNoteCount() - 2]);
        last_note->m_beam[0] = BEAM_MEDIAL;
    }
}

/*
bool MusBeam::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    return !output->WriteLayerRdg( this );
}
*/