/////////////////////////////////////////////////////////////////////////////
// Name:        musmeasure.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musmeasure.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusMeasureElements );


//----------------------------------------------------------------------------
// MusMeasure
//----------------------------------------------------------------------------

MusMeasure::MusMeasure():
    MusLogicalObject(), MusSectionInterface()
{
}

MusMeasure::~MusMeasure()
{
}


void MusMeasure::AddMeasureElement( MusMeasureInterface *measureElement )
{
	measureElement->SetMeasure( this );
	m_measureElements.Add( measureElement );
}


//----------------------------------------------------------------------------
// MusMeasureInterface
//----------------------------------------------------------------------------

MusMeasureInterface::MusMeasureInterface()
{
    m_measure = NULL;
}


MusMeasureInterface::~MusMeasureInterface()
{
}
