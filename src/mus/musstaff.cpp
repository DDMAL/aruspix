/////////////////////////////////////////////////////////////////////////////
// Name:        musstaff.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musstaff.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusStaffElements );


//----------------------------------------------------------------------------
// MusStaff
//----------------------------------------------------------------------------

MusStaff::MusStaff():
    MusLogicalObject(), MusMeasureInterface(), MusSectionInterface()
{
}


MusStaff::~MusStaff()
{
}

void MusStaff::AddStaffElement( MusStaffInterface *staffElement )
{
	staffElement->SetStaff( this );
	m_staffElements.Add( staffElement );	
}


//----------------------------------------------------------------------------
// MusStaffInterface
//----------------------------------------------------------------------------

MusStaffInterface::MusStaffInterface()
{
    m_staff = NULL;
}


MusStaffInterface::~MusStaffInterface()
{
}
