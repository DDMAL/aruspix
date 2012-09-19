/////////////////////////////////////////////////////////////////////////////
// Name:        musapp.cpp
// Author:      Laurent Pugin
// Created:     2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musapp.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusLayerRdgs );


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
    
	layerRdg->SetLayerApp( this );
    m_rdgs.Add( layerRdg );
}


//----------------------------------------------------------------------------
// MusLayerRdg
//----------------------------------------------------------------------------

MusLayerRdg::MusLayerRdg():
	MusLayer()
{
    m_app = NULL;
}


MusLayerRdg::~MusLayerRdg()
{
    
}
