/////////////////////////////////////////////////////////////////////////////
// Name:        musnote.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

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
}


MusNote::~MusNote()
{
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
