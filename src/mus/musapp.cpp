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
    
	layerRdg->SetLayerApp( this );
    m_rdgs.Add( layerRdg );
}

void MusLayerApp::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];         
    output->WriteLayerApp( this );
    
    // save layers
    MusLayerFunctor layer( &MusLayer::Save );
    this->Process( &layer, params );
}


// functors for MusLayer

void MusLayerApp::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusLayerFunctor *rdgFunctor = dynamic_cast<MusLayerFunctor*>(functor);
    MusLayer *rdg;
	int i;
    for (i = 0; i < (int)m_rdgs.GetCount(); i++) 
	{
        //wxLogDebug("prout rdg");
        rdg = &m_rdgs[i];
        functor->Call( rdg, params );
        if (rdgFunctor) { // is is a MusLayerElementFunctor, call it
            rdgFunctor->Call( rdg, params );
        }
        else { // process it further
            rdg->Process( functor, params );
        }
	}
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

void MusLayerRdg::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WriteLayerRdg( this );
    
    // save elements
    MusLayerElementFunctor element( &MusLayerElement::Save );
    this->Process( &element, params );
    
    output->EndLayerRdg( this );
}
