/////////////////////////////////////////////////////////////////////////////
// Name:        musslur.cpp
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


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
}

void MusSlur::AddNote(MusLayerElement *element) {
    
    if (!element->HasDurationInterface()) {
        return;
    }

    m_children.push_back(element);
    Modify();
    
}