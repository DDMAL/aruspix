//
//  musbeam.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 26/06/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musslur.h"


//----------------------------------------------------------------------------
// MusSlur
//----------------------------------------------------------------------------

MusSlur::MusSlur():
MusLayerElement()
{
}


MusSlur::~MusSlur()
{
    // we need to detach all notes because it is not to the beam object to delete them
    int i;
    for (i = (int)m_notes.GetCount(); i > 0; i--) {
        dynamic_cast<MusDurationInterface*>(m_notes.Detach(i - 1));
    }
}

void MusSlur::AddNote(MusLayerElement *element) {
    
    if (!element->HasDurationInterface()) {
        return;
    }

    m_notes.Add(element);
    
}