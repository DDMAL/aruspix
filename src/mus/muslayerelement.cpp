/////////////////////////////////////////////////////////////////////////////
// Name:        muslayerelement.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musio.h"
#include "muslayerelement.h"
#include "musdef.h"

#include "musapp.h"
#include "musbarline.h"
#include "musbeam.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"


#include "musdoc.h"


//----------------------------------------------------------------------------
// MusLayerElement
//----------------------------------------------------------------------------

MusLayerElement::MusLayerElement():
	MusDocObject()
{
    m_cueSize = false;
    m_hOffset = 0;
    m_staffShift = 0;
    m_visible = true;
    
    m_x_abs = 0;
    m_y_drawing = 0;
    m_in_layer_app = false;
}


MusLayerElement::~MusLayerElement()
{
    
}

MusLayerElement& MusLayerElement::operator=( const MusLayerElement& element )
{
	if ( this != &element ) // not self assignement
	{
        m_cueSize = element.m_cueSize;
        m_hOffset = element.m_hOffset;
        m_staffShift = element.m_staffShift;
        m_visible = element.m_visible;
        // pointers have to be NULL
        m_parent = NULL;
	}
	return *this;
}


MusLayerElement *MusLayerElement::GetChildCopy( bool newUuid ) 
{
    
    // Is there another way to do this in C++ ?
    MusLayerElement *element = NULL;

    if ( this->IsBarline() )
        element = new MusBarline( *(MusBarline*)this );
    else if (this->IsClef() )
        element = new MusClef( *(MusClef*)this );
    else if (this->IsMensur() )
        element = new MusMensur( *(MusMensur*)this );
    else if (this->IsNote() )
        element = new MusNote( *(MusNote*)this );
    else if (this->IsRest() )
        element = new MusRest( *(MusRest*)this );
    else if (this->IsSymbol() )
        element = new MusSymbol( *(MusSymbol*)this );
    else {
        wxLogDebug( "Missing %s", this->MusClassName().c_str() );
        //wxYield();
        wxASSERT_MSG( false , "Copy of this type unimplemented" );
        return NULL;
    }
        
    element->m_parent = NULL;
    
    if ( !newUuid ) {
        element->SetUuid( *this->GetUuid() );
    }
    else {
        element->ResetUuid( );
    }
    
    return element;
}



int MusLayerElement::GetElementNo() const
{
    wxASSERT_MSG( m_parent, "Layer cannot be NULL");
    
    return m_parent->GetChildIndex( this );
}


int MusLayerElement::GetHorizontalSpacing()
{
    // set less space for grace notes
    if (IsNote()) {
        MusNote *n = dynamic_cast<MusNote*>(this);
        if (n->m_cueSize)
            return 10;
    }
    
    return 20; // arbitrary generic value
}

void MusLayerElement::SetPitchOrPosition(int pname, int oct) 
{
    if ( this->HasPitchInterface() ){
        MusPitchInterface *pitch = dynamic_cast<MusPitchInterface*>(this);
        pitch->SetPitch( pname, oct );
    }
    else if ( this->HasPositionInterface() ) {
        MusPositionInterface *position = dynamic_cast<MusPositionInterface*>(this);
        position->SetPosition( pname, oct );
    }
}

bool MusLayerElement::GetPitchOrPosition(int *pname, int *oct) 
{
    if ( this->HasPitchInterface() ){
        MusPitchInterface *pitch = dynamic_cast<MusPitchInterface*>(this);
        return pitch->GetPitch( pname, oct );
    }
    else if ( this->HasPositionInterface() ) {
        MusPositionInterface *position = dynamic_cast<MusPositionInterface*>(this);
        return position->GetPosition( pname, oct );
    }
    return false;
}

void MusLayerElement::SetValue( int value, int flag )
{
    if ( this->HasDurationInterface() ){
        MusDurationInterface *duration = dynamic_cast<MusDurationInterface*>(this);
        duration->SetDuration( value );
    }
}


bool MusLayerElement::IsBarline() 
{  
    return (dynamic_cast<MusBarline*>(this));
}

bool MusLayerElement::IsBeam() 
{  
    return (dynamic_cast<MusBeam*>(this));
}

bool MusLayerElement::IsClef() 
{  
    return (dynamic_cast<MusClef*>(this));
}


bool MusLayerElement::HasDurationInterface() 
{  
    return (dynamic_cast<MusDurationInterface*>(this));
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

bool MusLayerElement::HasPitchInterface() 
{  
    return (dynamic_cast<MusPitchInterface*>(this));
}

bool MusLayerElement::HasPositionInterface() 
{  
    return (dynamic_cast<MusPositionInterface*>(this));
}

bool MusLayerElement::IsRest() 
{  
    return (dynamic_cast<MusRest*>(this));
}


bool MusLayerElement::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];         
    return !output->WriteLayerElement( this );
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

