/////////////////////////////////////////////////////////////////////////////
// Name:        musobject.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musobject.h"
#include "musrc.h"
#include "muslayout.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusLayoutObjects );

//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

MusObject::MusObject() :
	wxObject()
{
    m_id = 0;
    uuid_generate( m_uuid );
}

MusObject::~MusObject()
{
}

void MusObject::SetUuid( uuid_t uuid )
{ 
    uuid_copy( m_uuid, uuid );
};


//----------------------------------------------------------------------------
// MusLogicalObject
//----------------------------------------------------------------------------

MusLogicalObject::MusLogicalObject() :
	MusObject()
{
}

MusLogicalObject::~MusLogicalObject()
{
}


//----------------------------------------------------------------------------
// MusLayoutObject
//----------------------------------------------------------------------------

MusLayoutObject::MusLayoutObject() :
	MusObject()
{
	m_layout = NULL;
}

MusLayoutObject::~MusLayoutObject()
{
}

void MusLayoutObject::SetLayout( wxArrayPtrVoid params )
{
    // param 0: MusLayout
    wxASSERT( dynamic_cast<MusLayout*>((MusLayout*)params[0]) ); 
    
    m_layout = (MusLayout*)params[0];  
}


//----------------------------------------------------------------------------
// MusEnv
//----------------------------------------------------------------------------

MusEnv::MusEnv()
{
    m_landscape = true;
    m_staffLineWidth = 2;
    m_stemWidth = 3;
    m_barlineWidth = 3;
    m_beamWidth = 10;
    m_beamWhiteWidth = 5;
    m_beamMaxSlope = 30;
    m_beamMinSlope = 10;
    m_paperWidth = 210;
    m_paperHeight = 297;
    m_topMargin = 0;
    m_leftMarginOddPage = 10;
    m_leftMarginEvenPage = 10;
    
    // originally in MusParameters2
    m_smallStaffNum = 16;
    m_smallStaffDen = 20;
    m_graceNum = 3;
    m_graceDen = 4;
	m_stemCorrection = 0;
    m_headerType = 0;
    
    // additional parameters
    //m_notationMode = MUS_MENSURAL_MODE;
    m_notationMode = MUS_CMN_MODE;
}

MusEnv::~MusEnv()
{
}





