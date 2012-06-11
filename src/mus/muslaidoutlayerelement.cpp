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

MusLaidOutLayerElement::MusLaidOutLayerElement():
	MusLayoutObject()
{
    m_layerElement = NULL;
    m_layer = NULL;
    m_x_abs = 0;
    m_y_abs = 0;
}

MusLaidOutLayerElement::MusLaidOutLayerElement( MusLayerElement *element ):
	MusLayoutObject()
{
    m_layerElement = element;
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

bool MusLaidOutLayerElement::IsBarline() 
{  
    return (dynamic_cast<MusBarline*>(m_layerElement));
}

bool MusLaidOutLayerElement::IsClef() 
{  
    return (dynamic_cast<MusClef*>(m_layerElement));
}

bool MusLaidOutLayerElement::IsSymbol( SymbolType type ) 
{  
    MusSymbol *symbol = dynamic_cast<MusSymbol*>(m_layerElement);
    return (symbol && (symbol->m_type == type));
}

bool MusLaidOutLayerElement::IsSymbol( ) 
{  
    return (dynamic_cast<MusSymbol*>(m_layerElement));
}

bool MusLaidOutLayerElement::IsMensur() 
{  
    return (dynamic_cast<MusMensur*>(m_layerElement));
}

bool MusLaidOutLayerElement::IsNeume() 
{  
    return false; //return (typeid(*m_layerElement) == typeid(MusNeume)); 
}

bool MusLaidOutLayerElement::IsNeumeSymbol() 
{  
    return false; //return (typeid(*m_layerElement) == typeid(MusNeume)); 
}

bool MusLaidOutLayerElement::IsNote() 
{  
    return (dynamic_cast<MusNote*>(m_layerElement));
}

bool MusLaidOutLayerElement::IsPitchInterface() 
{  
    return (dynamic_cast<MusPitchInterface*>(m_layerElement));
}

bool MusLaidOutLayerElement::IsPositionInterface() 
{  
    return (dynamic_cast<MusPositionInterface*>(m_layerElement));
}

bool MusLaidOutLayerElement::IsRest() 
{  
    return (dynamic_cast<MusRest*>(m_layerElement));
}
