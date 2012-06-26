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

void MusBeam::AddNote(MusNote *note) {
   
    note->in_beam = true;
    m_notes.Add(note);
    
}