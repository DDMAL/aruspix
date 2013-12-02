/////////////////////////////////////////////////////////////////////////////
// Name:        musmensur.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musmensur.h"

int MusMensur::s_num = 3;
int MusMensur::s_numBase = 2;

//----------------------------------------------------------------------------
// MusMensur
//----------------------------------------------------------------------------

MusMensur::MusMensur():
	MusLayerElement()
{
    m_dot = 0;
    m_meterSymb = METER_SYMB_NONE;
    m_num = 0;
    m_numBase = 0;
    m_reversed = false;
    m_sign = MENSUR_SIGN_NONE;
    m_slash = 0;
    this->ResetUuid();
}


MusMensur::~MusMensur()
{
}

bool MusMensur::operator==( MusObject& other )
{
    MusMensur *otherMensur = dynamic_cast<MusMensur*>( &other );
    if ( !otherMensur ) {
        return false;
    }
    if ( this->m_dot != otherMensur->m_dot ) {
        return false;
    }
    if ( this->m_meterSymb != otherMensur->m_meterSymb ) {
        return false;
    }
    if ( this->m_num != otherMensur->m_num ) {
        return false;
    }
    if ( this->m_numBase != otherMensur->m_numBase ) {
        return false;
    }
    if ( this->m_reversed != otherMensur->m_reversed ) {
        return false;
    }
    if ( this->m_sign != otherMensur->m_sign ) {
        return false;
    }
    if ( this->m_slash != otherMensur->m_slash ) {
        return false;
    }
    return true;
}

void MusMensur::SetValue( int value, int flag ) 
{
    this->m_num = 0;
    this->m_numBase = 0;
    switch ( value ) {
        // tempus perfectum
        case ('Q'): 
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_O;
            this->m_slash = 0;
            this->m_dot = 1;
            this->m_reversed = false;
            break;
        case ('W'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_O;
            this->m_slash = 1;
            this->m_dot = 1;
            this->m_reversed = false;
            break;
        case ('E'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_O;
            this->m_slash = 0;
            this->m_dot = 0;
            this->m_reversed = false;
            break;
        case ('R'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_O;
            this->m_slash = 1;
            this->m_dot = 0;
            this->m_reversed = false;
            break;
        // tempus imperfectum
        case ('A'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_C;
            this->m_slash = 0;
            this->m_dot = 1;
            this->m_reversed = false;
            break;
        case ('S'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_C;
            this->m_slash = 1;
            this->m_dot = 1;
            this->m_reversed = false;
            break;
        case ('D'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_C;
            this->m_slash = 0;
            this->m_dot = 0;
            this->m_reversed = false;
            break;
        case ('F'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_C;
            this->m_slash = 1;
            this->m_dot = 0;
            this->m_reversed = false;
            break;
        // tempus imperfectum diminutum
        case ('Y'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_C;
            this->m_slash = 0;
            this->m_dot = 1;
            this->m_reversed = true;
            break;
        case ('X'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_C;
            this->m_slash = 1;
            this->m_dot = 1;
            this->m_reversed = true;
            break;
        case ('C'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_C;
            this->m_slash = 0;
            this->m_dot = 0;
            this->m_reversed = true;
            break;
        case ('V'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_C;
            this->m_slash = 1;
            this->m_dot = 0;
            this->m_reversed = true;
            break;;
        case ('1'):
            this->m_meterSymb = METER_SYMB_NONE;
            this->m_sign = MENSUR_SIGN_NONE;
            this->m_slash = 0;
            this->m_dot = 0;
            this->m_reversed = false;
            this->m_num = MusMensur::s_num; 
            this->m_numBase = MusMensur::s_numBase; 
            break;
            
        //case ('2'): this->code = 64; this->calte = 2; break;
        //case ('3'): this->code = 64; this->calte = 3; break;
    }
}
