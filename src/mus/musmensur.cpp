/////////////////////////////////////////////////////////////////////////////
// Name:        musmensur.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musmensur.h"
   

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
}


MusMensur::~MusMensur()
{
}
