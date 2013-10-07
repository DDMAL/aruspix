/////////////////////////////////////////////////////////////////////////////
// Name:        musbeam.cpp
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


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
   
    element->SetParent( this );
    m_children.push_back(element);
    Modify();
}

void MusBeam::FilterList()
{
    /* RZ
     FIXME megahack of the day:
     when erasing, erasing during iteration and modifying
     the iterator will cause the loop to skip the last object
     if the one before was deleted. Ex:
     note note note tuplet beam
     tuplet gets removed, modifies iterator so iter == m_list.rend()
     and loop exits before the beam is removes
     The hack consists in doing this in two steps
     the FIXME is to find out a better way.
     */
    // We want to keep only notes and rest
    // Eventually, we also need to filter out grace notes properly (e.g., with sub-beams)
    ListOfMusObjects::reverse_iterator iter;
    ListOfMusObjects::iterator remove_iter;    
    ListOfMusObjects removes;
    
    for (iter = m_list.rbegin(); iter != m_list.rend(); ++iter)
    {
        MusLayerElement *currentElement = dynamic_cast<MusLayerElement*>(*iter);
        if ( currentElement && !currentElement->HasDurationInterface() )
        {
            //Mus::LogDebug("KILLED!!! %s", currentElement->MusClassName().c_str() );
            //m_list.erase( --(iter.base()) );
            removes.push_back(currentElement);
        }
    }
    
    for (remove_iter = removes.begin(); remove_iter != removes.end(); remove_iter++) {
        m_list.remove(dynamic_cast<MusLayerElement*>(*remove_iter));
    }
    
}


