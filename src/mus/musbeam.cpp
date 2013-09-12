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
   
    element->SetParent( this );
    m_children.push_back(element);
    Modify();
}

void MusBeam::FilterList()
{
    // We want to keep only notes and rest
    // Eventually, we also need to filter out grace notes properly (e.g., with sub-beams)
    ListOfMusObjects::reverse_iterator iter;
    for (iter = m_list.rbegin(); iter != m_list.rend(); ++iter)
    {
        MusLayerElement *currentElement = dynamic_cast<MusLayerElement*>(*iter);
        if ( currentElement && !currentElement->HasDurationInterface() )
        {
            //wxLogDebug("%s", currentElement->MusClassName().c_str() );
            m_list.erase( --(iter.base()) );
        }
    }
}