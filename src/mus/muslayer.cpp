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
#include "musdef.h"

#include "musbarline.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"

#include "musdoc.h"
#include "muslaidoutlayerelement.h"

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
    Deactivate();
}

bool MusLayer::Check()
{
    wxASSERT( m_staff );
    return ( m_staff && MusLogicalObject::Check() );
}



void MusLayer::AddLayerElement( MusLayerElement *layerElement )
{
	layerElement->SetLayer( this );
	m_elements.Add( layerElement );
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
    for (i = 0; i < (int)m_elements.GetCount(); i++) 
	{
        element = &m_elements[i];
        functor->Call( element, params );
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
    // If there is a div, access the document are delete all LaidOutLayerElements pointing to it
    if ( m_div ) {
        wxArrayPtrVoid params;
        params.Add( this );
        MusLaidOutLayerElementFunctor del( &MusLaidOutLayerElement::Delete );
        m_div->m_doc->ProcessLayout( &del, params );
    }
    
    // If the is a parent layer and it is still active (the layer is not being deleted)
    // we remove the element from its list
    if ( m_layer && m_layer->IsActive() ) {
        wxLogDebug("Removing the LayerElement from its parent" );
        m_layer->m_elements.Detach( m_layer->m_elements.Index( *this ) );
    }
}

bool MusLayerElement::Check()
{
    wxASSERT( m_layer );
    return ( m_layer && MusLogicalObject::Check() );
}



MusLayerElement *MusLayerElement::GetChildCopy() 
{
    
    // Is there another way to do this in C++ ?

    if ( this->IsBarline() )
        return new MusBarline( *(MusBarline*)this );
    else if (this->IsClef() )
        return new MusClef( *(MusClef*)this );
    else if (this->IsMensur() )
        return new MusMensur( *(MusMensur*)this );
    else if (this->IsNote() )
        return new MusNote( *(MusNote*)this );
    else if (this->IsRest() )
        return new MusRest( *(MusRest*)this );
    else if (this->IsSymbol() )
        return new MusSymbol( *(MusSymbol*)this );
    else {
        wxASSERT_MSG( false , "Copy of this type unimplemented" );
        return NULL;
    }
}


bool MusLayerElement::IsBarline() 
{  
    return (dynamic_cast<MusBarline*>(this));
}

bool MusLayerElement::IsClef() 
{  
    return (dynamic_cast<MusClef*>(this));
}

bool MusLayerElement::IsSymbol( SymbolType type ) 
{  
    MusSymbol *symbol = dynamic_cast<MusSymbol*>(this);
    return (symbol && (symbol->m_type == type));
}

bool MusLayerElement::IsSymbol( ) 
{  
    return (dynamic_cast<MusSymbol*>(this));
}

bool MusLayerElement::IsMensur() 
{  
    return (dynamic_cast<MusMensur*>(this));
}

bool MusLayerElement::IsNeume() 
{  
    return false; //return (typeid(*m_layerElement) == typeid(MusNeume)); 
}

bool MusLayerElement::IsNeumeSymbol() 
{  
    return false; //return (typeid(*m_layerElement) == typeid(MusNeume)); 
}

bool MusLayerElement::IsNote() 
{  
    return (dynamic_cast<MusNote*>(this));
}

bool MusLayerElement::IsPitchInterface() 
{  
    return (dynamic_cast<MusPitchInterface*>(this));
}

bool MusLayerElement::IsPositionInterface() 
{  
    return (dynamic_cast<MusPositionInterface*>(this));
}

bool MusLayerElement::IsRest() 
{  
    return (dynamic_cast<MusRest*>(this));
}


void MusLayerElement::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];         
    output->WriteLayerElement( this );
}

void MusLayerElement::AdjustPname( int *pname, int *oct )
{
	if ((*pname) < PITCH_C)
	{
		if ((*oct) > 0)
			(*oct)-- ;
        (*pname) = PITCH_B;
        
	}
	else if ((*pname) > PITCH_B)
	{
		if ((*oct) < 7)
			(*oct)++;
        (*pname) = PITCH_C;
	}
}

