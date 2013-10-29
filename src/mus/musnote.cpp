/////////////////////////////////////////////////////////////////////////////
// Name:        musnote.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musnote.h"

//----------------------------------------------------------------------------
// MusNote
//----------------------------------------------------------------------------

MusNote::MusNote():
	MusLayerElement(), MusDurationInterface(), MusPitchInterface()
{
    m_artic = 0;
    m_chord = 0;
    m_colored = false;
    m_headshape = 0;
    m_lig = 0;
    m_ligObliqua = false;
    memset(m_slur, 0, sizeof(unsigned char) * NOTE_MAX_SLURS);
    m_stemDir = 0;
    m_stemLen = 0;
    m_acciaccatura = false;
    m_embellishment = EMB_NONE;
}


MusNote::~MusNote()
{
}

bool MusNote::operator==( MusObject& other )
{
    MusNote *otherNote = dynamic_cast<MusNote*>( &other );
    if ( !otherNote ) {
        return false;
    }
    if ( this->m_artic != otherNote->m_artic ) {
        return false;
    }
    if ( this->m_chord != otherNote->m_chord ) {
        return false;
    }
    if ( this->m_colored != otherNote->m_colored ) {
        return false;
    }
    if ( this->m_headshape != otherNote->m_headshape ) {
        return false;
    }
    if ( this->m_lig != otherNote->m_lig ) {
        return false;
    }
    if ( this->m_ligObliqua != otherNote->m_ligObliqua ) {
        return false;
    }
    // slur need value by value comparison
    //if ( this->m_slur != otherNote->m_slur ) {
    //    return false;
    //}
    if ( this->m_stemDir != otherNote->m_stemDir ) {
        return false;
    }
    if ( this->m_stemLen != otherNote->m_stemLen ) {
        return false;
    }
    if ( this->m_acciaccatura != otherNote->m_acciaccatura ) {
        return false;
    }
    if ( this->m_embellishment != otherNote->m_embellishment ) {
        return false;
    }
    if ( !this->HasIdenticalPitchInterface( otherNote ) ) {
        return false;
    }
    if ( !this->HasIdenticalDurationInterface( otherNote ) ) {
        return false;
    }
    
    return true;
}

void MusNote::SetValue( int value, int flag )
{
    MusDurationInterface::SetDuration( value ); 
    
	// remove ligature flag for  inadequate values	
	if ( ( value < DUR_BR ) || ( value > DUR_1 ) ) {
        this->m_lig = 0;
    }

	this->m_colored = false;
	this->m_ligObliqua = false;
    
	// remove qauto flag for silences and inadequate values	
	if ( ( value > DUR_LG ) && ( value < DUR_2 ) ) {
		this->m_stemDir = 0;
        this->m_stemLen = 0;
    }    
}

void MusNote::ChangeColoration( )
{
    this->m_colored = !this->m_colored;
}


void MusNote::ChangeStem( )
{
	if ( ( this->m_dur > DUR_LG ) && ( this->m_dur < DUR_2 ) )
		return;
    
	this->m_stemDir = !this->m_stemDir;
}


void MusNote::SetLigature( )
{
	if ( ( this->m_dur == DUR_LG ) || ( this->m_dur > DUR_1 ) ) {
		return;
    }
	
	this->m_lig = true;
}

int MusNote::GetHorizontalSpacing()
{
    if (this->m_cueSize) {
        return 3;
    }
    return 5; // arbitrary generic value
}
