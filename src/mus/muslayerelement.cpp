/////////////////////////////////////////////////////////////////////////////
// Name:        muslayerelement.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "muslayerelement.h"

//----------------------------------------------------------------------------

#include <assert.h>
#include <typeinfo>

//----------------------------------------------------------------------------

#include "mus.h"
#include "musaligner.h"
#include "musapp.h"
#include "musbarline.h"
#include "musbeam.h"
#include "musclef.h"
#include "musdef.h"
#include "musdoc.h"
#include "muskeysig.h"
#include "musio.h"
#include "musmensur.h"
#include "musmultirest.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"
#include "mustuplet.h"

/**
 * Define the maximum levels between a tuplet and its notes
 */
#define MAX_TUPLET_DEPTH 3


//----------------------------------------------------------------------------
// MusLayerElement
//----------------------------------------------------------------------------

MusLayerElement::MusLayerElement():
    MusDocObject("le-")
{
    Init();
}

MusLayerElement::MusLayerElement(std::string classid):
	MusDocObject(classid)
{
    Init();
}


MusLayerElement::~MusLayerElement()
{
    
}

MusLayerElement& MusLayerElement::operator=( const MusLayerElement& element )
{
	if ( this != &element ) // not self assignement
	{
        // is this necessary?
        m_cueSize = element.m_cueSize;
        m_hOffset = element.m_hOffset;
        m_staffShift = element.m_staffShift;
        m_visible = element.m_visible;
        // pointers have to be NULL
        m_parent = NULL;
        m_alignment = NULL;
	}
	return *this;
}


MusLayerElement *MusLayerElement::GetChildCopy( bool newUuid ) 
{
    
    // Is there another way to do this in C++ ?
    // Yes, change this to the MusObject::Clone method - however, newUuid will not be possible in this way
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
        Mus::LogDebug( "Missing %s", this->MusClassName().c_str() );
        assert( false ); // Copy of this type unimplemented
        return NULL;
    }
        
    element->m_parent = NULL;
    
    if ( !newUuid ) {
        element->SetUuid( this->GetUuid() );
    }
    else {
        element->ResetUuid( );
    }
    
    return element;
}



int MusLayerElement::GetElementNo() const
{
    assert( m_parent ); // Layer cannot be NULL
    
    return m_parent->GetChildIndex( this );
}


int MusLayerElement::GetHorizontalSpacing()
{
    return 10; // arbitrary generic value
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

void MusLayerElement::Init()
{
    m_cueSize = false;
    m_hOffset = 0;
    m_staffShift = 0;
    m_visible = true;
    
    m_x_abs = AX_UNSET;
    m_x_drawing = 0;
    m_y_drawing = 0;
    m_in_layer_app = false;
    
    m_alignment = NULL;
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


bool MusLayerElement::IsKeySig()
{
    return (dynamic_cast<MusKeySig*>(this));
}

bool MusLayerElement::IsMultiRest() 
{  
    return (dynamic_cast<MusMultiRest*>(this));
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


int MusLayerElement::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];         
    if (!output->WriteLayerElement( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;

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

double MusLayerElement::GetAlignementDuration()
{
    if ( HasDurationInterface() ) {
        MusTuplet *tuplet = dynamic_cast<MusTuplet*>( this->GetFirstParent( &typeid(MusTuplet), MAX_TUPLET_DEPTH ) );
        int num = 1;
        int numbase = 1;
        if ( tuplet ) {
            num = tuplet->m_num;
            numbase = tuplet->m_numbase;
        }
        MusDurationInterface *duration = dynamic_cast<MusDurationInterface*>(this);
        return duration->GetAlignementDuration( num, numbase );
    }
    else {
        return 0.0;
    }
}

int MusLayerElement::GetXRel()
{
    if (m_alignment) {
        return m_alignment->GetXRel();
    }
    return 0;
}

int MusLayerElement::Align( ArrayPtrVoid params )
{
    // param 0: the measureAligner
    // param 1: the time
    // param 2: the systemAligner (unused)
    // param 3: the staffNb (unused)
    MusMeasureAligner **measureAligner = (MusMeasureAligner**)params[0];
    double *time = (double*)params[1];
    
    MusAlignmentType type = ALIGNMENT_DEFAULT;
    if ( this->IsBarline() ) {
        type = ALIGNMENT_BARLINE;
    }
    else if ( this->IsClef() ) {
        type = ALIGNMENT_CLEF;
    }
    else if ( this->IsKeySig() ) {
        type = ALIGNMENT_KEYSIG;
    }
    else if ( this->IsMensur() ) {
        type = ALIGNMENT_MENSUR;
    }
    else if ( this->IsMultiRest() ) {
        type = ALIGNMENT_MULTIREST;
    }
    
    // get the duration of the event
    double duration = this->GetAlignementDuration();
    
    (*measureAligner)->SetMaxTime( (*time) + duration );
    
    m_alignment = (*measureAligner)->GetAlignmentAtTime( *time, type );
    
    // increase the time position
    (*time) += duration;

    //Mus::LogDebug("Time %f", (*time) );
    
    return FUNCTOR_CONTINUE;
}

