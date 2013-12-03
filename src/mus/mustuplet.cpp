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
MusLayerElement("tuplet-")
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