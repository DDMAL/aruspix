/////////////////////////////////////////////////////////////////////////////
// Name:        musapp.cpp
// Author:      Laurent Pugin
// Created:     2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musapp.h"

#include "musio.h"

//----------------------------------------------------------------------------
// MusLayerApp
//----------------------------------------------------------------------------

MusLayerApp::MusLayerApp():
    MusLayerElement()
{
}

MusLayerApp::~MusLayerApp()
{
    
}

void MusLayerApp::AddLayerRdg( MusLayerRdg *layerRdg )
{
    //wxASSERT_MSG( this->m_layer->m_staff, "Layer staff cannot be NULL when adding and <rdg>" ); 
    // The the to the parent staff will be NULL!
    //layerRdg->SetStaff( this->m_layer->m_staff );
    
	layerRdg->SetParent( this );
    m_children.Add( layerRdg );
    Modify();
}

bool MusLayerApp::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];         
    return !output->WriteLayerApp( this );
}

//----------------------------------------------------------------------------
// MusLayerRdg
//----------------------------------------------------------------------------

MusLayerRdg::MusLayerRdg( ):
    MusObject()
{

}


MusLayerRdg::~MusLayerRdg()
{
    
}


void MusLayerRdg::AddElement( MusLayerElement *element, int idx )
{
	element->SetParent( this );
    if ( idx == -1 ) {
        m_children.Add( element );
    }
    else {
        m_children.Insert( element, idx );
    }
}


bool MusLayerRdg::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    return !output->WriteLayerRdg( this );
}
