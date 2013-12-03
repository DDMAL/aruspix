/////////////////////////////////////////////////////////////////////////////
// Name:        musslur.cpp
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "mustuplet.h"

//----------------------------------------------------------------------------
// MusTuplet
//----------------------------------------------------------------------------

MusTuplet::MusTuplet():
MusLayerElement("tuplet-"), MusObjectListInterface()
{
}


MusTuplet::~MusTuplet()
{
}

void MusTuplet::AddElement(MusLayerElement *element) {
    
    //if (!element->HasDurationInterface()) {
    //    return;
    //}
    
    element->SetParent( this );
    m_children.push_back(element);
    Modify();
}

void MusTuplet::FilterList()
{
    // We want to keep only notes and rest
    // Eventually, we also need to filter out grace notes properly (e.g., with sub-beams)
    ListOfMusObjects::iterator iter = m_list.begin();
    ListOfMusObjects beamedList;
    
    while ( iter != m_list.end()) {
        MusLayerElement *currentElement = dynamic_cast<MusLayerElement*>(*iter);
        if ( currentElement && !currentElement->HasDurationInterface() )
        {
            MusBeam *aBeam = dynamic_cast<MusBeam*>(currentElement);
            if (aBeam) {
                std::copy( aBeam->m_list.begin(), aBeam->m_list.end(), std::back_inserter( beamedList ) );
            }
            iter = m_list.erase( iter );
        } else {
            iter++;
        }
    }
    
    if (beamedList.size() > 0)
        std::copy( beamedList.begin(), beamedList.end(), std::back_inserter( m_list ) );
}