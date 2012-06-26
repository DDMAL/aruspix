/////////////////////////////////////////////////////////////////////////////
// Name:        muslaidoutlayerelement.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musio.h"
#include "muslaidoutlayerelement.h"

#include "musbarline.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusLaidOutLayerElements );

//----------------------------------------------------------------------------
// MusLaidOutLayerElement
//----------------------------------------------------------------------------

MusLaidOutLayerElement::MusLaidOutLayerElement( MusLayerElement *element ):
	MusLayoutObject()
{
    m_layerElement = element;
    m_layer = NULL;
    m_x_abs = 0;
    m_y_drawing = 0;
}

MusLaidOutLayerElement::~MusLaidOutLayerElement()
{
    // If the is a parent layer and it is still active (that is not being deleted)
    // we remove the element from its list
    if ( m_layer && m_layer->IsActive() ) {
        wxLogDebug("Removing the LaidOutLayerElement from its parent" );
        m_layer->m_elements.Detach( m_layer->m_elements.Index( *this ) );
    }
}

bool MusLaidOutLayerElement::Check()
{
    wxASSERT( m_layer && m_layerElement );
    return ( m_layer && m_layerElement && MusLayoutObject::Check());
}

void MusLaidOutLayerElement::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];         
    output->WriteLaidOutLayerElement( this );
}

void MusLaidOutLayerElement::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    // MusFileInputStream *input = (MusFileInputStream*)params[0]; 
    // For now nothing to do here
}

void MusLaidOutLayerElement::Delete( wxArrayPtrVoid params )
{
    // param 0: the MusLayerElement we point to
    MusLayerElement *element = (MusLayerElement*)params[0];   

    if ( m_layerElement == element ) {
        //wxLogMessage( "YES" );
        delete this;
    }
}

void MusLaidOutLayerElement::UpdateXPosition( wxArrayPtrVoid params )
{
    // param 0: the MusLayerElement we point to
	int *current_x_shift = (int*)params[0];  
    
    int negative_offset = this->m_x_abs - this->m_contentBB_x1;
    this->m_x_abs = (*current_x_shift) + negative_offset;
    (*current_x_shift) += (this->m_contentBB_x2 - this->m_contentBB_x1);
}

int MusLaidOutLayerElement::GetElementNo() const
{
    wxASSERT_MSG( m_layer, "LaidOutLayer cannot be NULL");
    
    return m_layer->m_elements.Index( *this );
}

