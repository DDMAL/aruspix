/////////////////////////////////////////////////////////////////////////////
// Name:        muslayerelement.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "layerelement.h"

//----------------------------------------------------------------------------

#include <assert.h>
#include <typeinfo>

//----------------------------------------------------------------------------

#include "vrv.h"
#include "aligner.h"
#include "app.h"
#include "barline.h"
#include "beam.h"
#include "clef.h"
#include "vrvdef.h"
#include "doc.h"
#include "keysig.h"
#include "io.h"
#include "mensur.h"
#include "multirest.h"
#include "note.h"
#include "rest.h"
#include "symbol.h"
#include "tuplet.h"

namespace vrv {

/**
 * Define the maximum levels between a tuplet and its notes
 */
#define MAX_TUPLET_DEPTH 3


//----------------------------------------------------------------------------
// LayerElement
//----------------------------------------------------------------------------

LayerElement::LayerElement():
    DocObject("le-")
{
    Init();
}


LayerElement::LayerElement(std::string classid):
	DocObject(classid)
{
    Init();
}


LayerElement::~LayerElement()
{
    
}

LayerElement& LayerElement::operator=( const LayerElement& element )
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


LayerElement *LayerElement::GetChildCopy( bool newUuid ) 
{
    
    // Is there another way to do this in C++ ?
    // Yes, change this to the Object::Clone method - however, newUuid will not be possible in this way
    LayerElement *element = NULL;

    if ( this->IsBarline() )
        element = new Barline( *(Barline*)this );
    else if (this->IsClef() )
        element = new Clef( *(Clef*)this );
    else if (this->IsMensur() )
        element = new Mensur( *(Mensur*)this );
    else if (this->IsNote() )
        element = new Note( *(Note*)this );
    else if (this->IsRest() )
        element = new Rest( *(Rest*)this );
    else if (this->IsSymbol() )
        element = new Symbol( *(Symbol*)this );
    else {
        Vrv::LogDebug( "Missing %s", this->MusClassName().c_str() );
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



int LayerElement::GetElementNo() const
{
    assert( m_parent ); // Layer cannot be NULL
    
    return m_parent->GetChildIndex( this );
}


int LayerElement::GetHorizontalSpacing()
{
    return 10; // arbitrary generic value
}

void LayerElement::SetPitchOrPosition(int pname, int oct) 
{
    if ( this->HasPitchInterface() ){
        PitchInterface *pitch = dynamic_cast<PitchInterface*>(this);
        pitch->SetPitch( pname, oct );
    }
    else if ( this->HasPositionInterface() ) {
        PositionInterface *position = dynamic_cast<PositionInterface*>(this);
        position->SetPosition( pname, oct );
    }
}

bool LayerElement::GetPitchOrPosition(int *pname, int *oct) 
{
    if ( this->HasPitchInterface() ){
        PitchInterface *pitch = dynamic_cast<PitchInterface*>(this);
        return pitch->GetPitch( pname, oct );
    }
    else if ( this->HasPositionInterface() ) {
        PositionInterface *position = dynamic_cast<PositionInterface*>(this);
        return position->GetPosition( pname, oct );
    }
    return false;
}

void LayerElement::SetValue( int value, int flag )
{
    if ( this->HasDurationInterface() ){
        DurationInterface *duration = dynamic_cast<DurationInterface*>(this);
        duration->SetDuration( value );
    }
}

void LayerElement::Init()
{
    m_cueSize = false;
    m_hOffset = 0;
    m_staffShift = 0;
    m_visible = true;
    
    m_xAbs = AX_UNSET;
    m_xDrawing = 0;
    m_yRel = 0;
    m_in_layer_app = false;
    
    m_alignment = NULL;
}


bool LayerElement::IsBarline() 
{  
    return (dynamic_cast<Barline*>(this));
}

bool LayerElement::IsBeam() 
{  
    return (dynamic_cast<Beam*>(this));
}

bool LayerElement::IsClef() 
{  
    return (dynamic_cast<Clef*>(this));
}


bool LayerElement::HasDurationInterface() 
{  
    return (dynamic_cast<DurationInterface*>(this));
}

bool LayerElement::IsSymbol( SymbolType type ) 
{  
    Symbol *symbol = dynamic_cast<Symbol*>(this);
    return (symbol && (symbol->m_type == type));
}

bool LayerElement::IsSymbol( ) 
{  
    return (dynamic_cast<Symbol*>(this));
}


bool LayerElement::IsKeySig()
{
    return (dynamic_cast<KeySignature*>(this));
}

bool LayerElement::IsMultiRest() 
{  
    return (dynamic_cast<MultiRest*>(this));
}

bool LayerElement::IsMensur() 
{  
    return (dynamic_cast<Mensur*>(this));
}

bool LayerElement::IsNeume() 
{  
    return false; //return (typeid(*m_layerElement) == typeid(MusNeume)); 
}

bool LayerElement::IsNeumeSymbol() 
{  
    return false; //return (typeid(*m_layerElement) == typeid(MusNeume)); 
}

bool LayerElement::IsNote() 
{  
    return (dynamic_cast<Note*>(this));
}

bool LayerElement::HasPitchInterface() 
{  
    return (dynamic_cast<PitchInterface*>(this));
}

bool LayerElement::HasPositionInterface() 
{  
    return (dynamic_cast<PositionInterface*>(this));
}

bool LayerElement::IsRest() 
{  
    return (dynamic_cast<Rest*>(this));
}


int LayerElement::Save( ArrayPtrVoid params )
{
    // param 0: output stream
    FileOutputStream *output = (FileOutputStream*)params[0];         
    if (!output->WriteLayerElement( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;

}

void LayerElement::AdjustPname( int *pname, int *oct )
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

double LayerElement::GetAlignementDuration()
{
    if ( HasDurationInterface() ) {
        Tuplet *tuplet = dynamic_cast<Tuplet*>( this->GetFirstParent( &typeid(Tuplet), MAX_TUPLET_DEPTH ) );
        int num = 1;
        int numbase = 1;
        if ( tuplet ) {
            num = tuplet->m_num;
            numbase = tuplet->m_numbase;
        }
        DurationInterface *duration = dynamic_cast<DurationInterface*>(this);
        return duration->GetAlignementDuration( num, numbase );
    }
    else {
        return 0.0;
    }
}

int LayerElement::GetXRel()
{
    if (m_alignment) {
        return m_alignment->GetXRel();
    }
    return 0;
}

int LayerElement::Align( ArrayPtrVoid params )
{
    // param 0: the measureAligner
    // param 1: the time
    // param 2: the systemAligner (unused)
    // param 3: the staffNb (unused)
    MeasureAligner **measureAligner = (MeasureAligner**)params[0];
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

    //Vrv::LogDebug("Time %f", (*time) );
    
    return FUNCTOR_CONTINUE;
}

} // namespace vrv
