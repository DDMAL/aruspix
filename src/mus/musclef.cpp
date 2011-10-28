/////////////////////////////////////////////////////////////////////////////
// Name:        musclef.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musclef.h"


//----------------------------------------------------------------------------
// MusClef
//----------------------------------------------------------------------------

MusClef::MusClef():
	MusLayerElement()
{
    m_clefId = UT1;
}


MusClef::~MusClef()
{
}

int MusClef::GetClefOffset ()
{	
	int offset = 0;		// case 5: UT 1e ligne par default, valable pour PERC
	switch(m_clefId)
	{	case SOL2 : offset = -2; break;
		case SOL1 : offset = -4; break;
		case SOLva : offset = 5; break;
		case FA5 : offset = 12; break;
		case FA4 : offset = 10; break;
		case FA3 : offset = 8; break;
		case UT2 : offset = 2; break;
		case UT3 : offset = 4; break;
		case UT5 : offset = 8; break;
		case UT4 : offset =  6;		
		default: break;
	}
	return offset;
}
