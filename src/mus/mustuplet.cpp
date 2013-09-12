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
}

void MusTuplet::AddNote(MusLayerElement *element) {
    
    if (!element->HasDurationInterface()) {
        return;
    }
    
    m_children.push_back(element);
    Modify();
}