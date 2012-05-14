/////////////////////////////////////////////////////////////////////////////
// Name:        muslayer.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musio.h"
#include "muslayer.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusLayers );
WX_DEFINE_OBJARRAY( ArrayOfMusLayerElements );


//----------------------------------------------------------------------------
// MusLayer
//----------------------------------------------------------------------------

MusLayer::MusLayer():
    MusLogicalObject()
{
    m_staff = NULL;
}


MusLayer::~MusLayer()
{
}

void MusLayer::AddLayerElement( MusLayerElement *layerElement )
{
	layerElement->SetLayer( this );
	m_layerElements.Add( layerElement );
}

void MusLayer::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WriteLayer( this );
    
    // save elements
    MusLayerElementFunctor element( &MusLayerElement::Save );
    this->Process( &element, params );
}

void MusLayer::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileInputStream *input = (MusFileInputStream*)params[0];       
    
    // load elements
    MusLayerElement *element;
    while ( (element = input->ReadLayerElement()) ) {
        this->AddLayerElement( element );
    }
}

// functors for MusLayer

void MusLayer::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusLayerElementFunctor *elementFunctor = dynamic_cast<MusLayerElementFunctor*>(functor);
    MusLayerElement *element;
	int i;
    for (i = 0; i < (int)m_layerElements.GetCount(); i++) 
	{
        element = &m_layerElements[i];
        if (elementFunctor) { // is is a MusSystemFunctor, call it
            elementFunctor->Call( element, params );
        }
        else { // process it further
            // nothing we can do...
        }
	}
}

//----------------------------------------------------------------------------
// MusLayerElement
//----------------------------------------------------------------------------

MusLayerElement::MusLayerElement():
	MusLogicalObject()
{
    m_layer = NULL;
    m_cueSize = false;
    m_hOffset = 0;
    m_staffShift = 0;
    m_visible = true;
}


MusLayerElement::~MusLayerElement()
{
}


void MusLayerElement::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];         
    output->WriteLayerElement( this );
}


bool MusLayerElement::FindWithUuid( wxArrayPtrVoid params )
{
    // param 0: the uuid we are looking for
    // parma 1: the pointer to the element
    uuid_t *uuid = (uuid_t*)params[0];  
    MusLayerElement **element = (MusLayerElement**)params[1];  
    
    if ( (*element) ) {
        return true;
    }
    
    if ( uuid_compare( *uuid, *this->GetUuid() ) == 0 ) {
        (*element) = this;
        //wxLogDebug("Found it!");
        return true;
    }
    //wxLogDebug("Still looking for uuid...");
    return false;
}



//----------------------------------------------------------------------------
// MusLayerElementFunctor
//----------------------------------------------------------------------------

MusLayerElementFunctor::MusLayerElementFunctor( void(MusLayerElement::*_fpt)( wxArrayPtrVoid ))
{ 
    fpt=_fpt; 
    fpt_bool=NULL; 
}

MusLayerElementFunctor::MusLayerElementFunctor( bool(MusLayerElement::*_fpt_bool)( wxArrayPtrVoid ))
{ 
    fpt_bool=_fpt_bool; 
    fpt=NULL;
}
    

void MusLayerElementFunctor::Call( MusLayerElement *ptr, wxArrayPtrVoid params )
{ 
    if (fpt) {
        (*ptr.*fpt)( params );
    }
    else 
    {
            m_success = (*ptr.*fpt_bool)( params );
    }
}

