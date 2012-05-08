/////////////////////////////////////////////////////////////////////////////
// Name:        muslaidoutlayerelement.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

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
}

MusLaidOutLayerElement::MusLaidOutLayerElement( MusLayerElement *element ):
	MusLayoutObject()
{
    m_layerElement = element;
}

MusLaidOutLayerElement::~MusLaidOutLayerElement()
{
}

void MusLaidOutLayerElement::CheckIntegrity()
{
	wxASSERT_MSG( m_layer, "MusLaidOutLayer parent cannot be NULL");
}

int MusLaidOutLayerElement::GetElementNo() const
{
    wxASSERT_MSG( m_layer, "LaidOutLayer cannot be NULL");
    
    return m_layer->m_elements.Index( *this );
}

bool MusLaidOutLayerElement::IsClef() 
{  
    //return (typeid(*m_layerElement) == typeid(MusClef));
    if (!m_layerElement) {
        return false;
    }
    return (dynamic_cast<MusClef*>(m_layerElement));
}

bool MusLaidOutLayerElement::IsNeume() 
{  
    return false; //return (typeid(*m_layerElement) == typeid(MusNeume)); 
}

bool MusLaidOutLayerElement::IsNote() 
{  
    //return (typeid(*m_layerElement) == typeid(MusNote)); 
    if (!m_layerElement) {
        return false;
    }
    return (dynamic_cast<MusNote*>(m_layerElement));
}
