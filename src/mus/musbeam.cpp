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
}

void MusBeam::AddElement(MusLayerElement *element) {
   
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
    m_children.push_back(element);
    Modify();
    
    // Set the last note to median if we have more than one note in the array
    if (GetNoteCount() > 2) {
        MusDurationInterface *last_note = dynamic_cast<MusDurationInterface*>(m_children[GetNoteCount() - 2]);
        last_note->m_beam[0] = BEAM_MEDIAL;
    }
}

void MusBeam::FilterList()
{
    // We want to keep only notes and rest
    // Eventually, we also need to filter out grace notes properly (e.g., with sub-beams)
    ListOfMusObjects::iterator iter;
    for (iter = m_list.begin(); iter != m_list.end(); ++iter)
    {
        MusLayerElement *currentElement = dynamic_cast<MusLayerElement*>(*iter);
        if ( currentElement && !currentElement->HasDurationInterface() )
        {
            wxLogDebug("%s", currentElement->MusClassName().c_str() );
            m_list.erase( iter );
        }
    }
}