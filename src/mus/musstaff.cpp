/////////////////////////////////////////////////////////////////////////////
// Name:        musstaff.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musio.h"
#include "musstaff.h"
#include "muslayer.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusStaves );


//----------------------------------------------------------------------------
// MusStaff
//----------------------------------------------------------------------------

MusStaff::MusStaff():
    MusLogicalObject()
{
    m_measure = NULL;
    m_section = NULL;
}

MusStaff::~MusStaff()
{
}

bool MusStaff::Check()
{
    wxASSERT( ( m_measure || m_section ) );
    return ( ( m_measure || m_section ) );
}



void MusStaff::AddLayer( MusLayer *layer )
{
	layer->SetStaff( this );
	m_layers.Add( layer );	
}

void MusStaff::SetMeasure( MusMeasure *measure )
{
    wxASSERT_MSG( !m_measure, "A staff cannot be child of a section and a measure at the same time" );   
    m_measure = measure;    
}

void MusStaff::SetSection( MusSection *section )
{
    wxASSERT_MSG( !m_measure, "A staff cannot be child of a section and a measure at the same time" );    
    m_section = section;    
}


void MusStaff::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WriteStaff( this );
    
    // save layers
    MusLayerFunctor layer( &MusLayer::Save );
    this->Process( &layer, params );
}

void MusStaff::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileInputStream *input = (MusFileInputStream*)params[0];       
    
    // load layers
    MusLayer *layer;
    while ( (layer = input->ReadLayer()) ) {
        layer->Load( params );
        this->AddLayer( layer );
    }
}

// functors for MusStaff

void MusStaff::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusLayerFunctor *layerFunctor = dynamic_cast<MusLayerFunctor*>(functor);
    MusLayer *layer;
    int i;
    for (i = 0; i < (int)m_layers.GetCount(); i++) 
	{
        layer = &m_layers[i];
        functor->Call( layer, params );
        if (layerFunctor) { // is is a MusLayerFunctor, call it
            layerFunctor->Call( layer, params );
        }
        else { // process it further
            layer->Process( functor, params );
        }
	}
}
