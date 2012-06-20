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
    m_y_abs = 0;
}

MusLaidOutLayerElement::~MusLaidOutLayerElement()
{
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

int MusLaidOutLayerElement::GetElementNo() const
{
    wxASSERT_MSG( m_layer, "LaidOutLayer cannot be NULL");
    
    return m_layer->m_elements.Index( *this );
}

void MusLaidOutLayerElement::SetPitchOrPosition(int pname, int oct) 
{
    if ( this->IsPitchInterface() ){
        MusPitchInterface *pitch = dynamic_cast<MusPitchInterface*>(this->m_layerElement);
        pitch->m_oct = oct;
        pitch->m_pname = pname;
    }
    else if ( this->IsPositionInterface() ) {
        MusPositionInterface *position = dynamic_cast<MusPositionInterface*>(this->m_layerElement);
        position->m_oct = oct;
        position->m_pname = pname;
    }
}

bool MusLaidOutLayerElement::GetPitchOrPosition(int *pname, int *oct) 
{
    if ( this->IsPitchInterface() ){
        MusPitchInterface *pitch = dynamic_cast<MusPitchInterface*>(this->m_layerElement);
        *oct = pitch->m_oct;
        *pname = pitch->m_pname;
        return true;
    }
    else if ( this->IsPositionInterface() ) {
        MusPositionInterface *position = dynamic_cast<MusPositionInterface*>(this->m_layerElement);
        *oct = position->m_oct;
        *pname = position->m_pname;
        return true;
    }
    return false;
}
