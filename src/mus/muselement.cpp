/////////////////////////////////////////////////////////////////////////////
// Name:        wgelement.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_WG

#ifdef __GNUG__
    #pragma implementation "muselement.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "muselement.h"
#include "muswindow.h"
#include "musstaff.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// WgElement
//----------------------------------------------------------------------------

WgElement::WgElement() :
	WgObject()
{
	liaison = false;
    dliai = false;
    fliai = false;
    lie_up = false;
    rel = false;
    drel = false;
    frel = false;
    oct = 0;
    dimin = 0;
    grp = 0;
    _shport = 0;
    ligat = false;
    ElemInvisible = false;
    pointInvisible = false;
    existDebord = false;
    fligat = false;
    notschowgrp = 0;
    cone = false;
    liaisonPointil = 0;
    reserve1 = 0;
    reserve2 = 0;
    ottava = 0;
    durNum = 1;
    durDen = 1;
    offset = 0;
    pdebord = NULL;
    debordCode = 0;
    debordSize = 0;
	no = 0;
	xrel = 0;
	dec_y = 0;
	
	// Aruspix only, for comparison
	m_im_staff = 0;
	m_im_staff_segment = 0;
	m_im_pos = 0;
	m_cmp_flag = 0;

	code = 0;
}

WgElement::WgElement( const WgElement& element )
{
	TYPE = element.TYPE;
	liaison = element.liaison;
    dliai = element.dliai;
    fliai = element.fliai;
    lie_up = element.lie_up;
    rel = element.rel;
    drel = element.drel;
    frel = element.frel;
    oct = element.oct;
    dimin = element.dimin;
    grp = element.grp;
    _shport = element._shport;
    ligat = element.ligat;
    ElemInvisible = element.ElemInvisible;
    pointInvisible = element.pointInvisible;
    existDebord = element.existDebord;
    fligat = element.fligat;
    notschowgrp = element.notschowgrp;
    cone = element.cone;
    liaisonPointil = element.liaisonPointil;
    reserve1 = element.reserve1;
    reserve2 = element.reserve2;
    ottava = element.ottava;
    durNum = element.durNum;
    durDen = element.durDen;
    offset = element.offset;
	xrel = element.xrel;
	dec_y = element.dec_y;
    debordCode = element.debordCode;
    debordSize = element.debordSize;
	no = element.no;
	code = element.code;
	
	// comparison
	m_im_filename = element.m_im_filename;
	m_im_staff = element.m_im_staff;
	m_im_staff_segment = element.m_im_staff_segment;
	m_im_pos = element.m_im_pos;
	m_cmp_flag = element.m_cmp_flag;

	pdebord = NULL;
	if ( existDebord )
	{
		int size = debordSize - sizeof( debordSize ) - sizeof( debordCode );
		pdebord = malloc( size );
		memcpy( pdebord, element.pdebord, size );
	}
}


WgElement& WgElement::operator=( const WgElement& element)
{
	if ( this != &element ) // not self assignement
	{
		TYPE = element.TYPE;
		liaison = element.liaison;
		dliai = element.dliai;
		fliai = element.fliai;
		lie_up = element.lie_up;
		rel = element.rel;
		drel = element.drel;
		frel = element.frel;
		oct = element.oct;
		dimin = element.dimin;
		grp = element.grp;
		_shport = element._shport;
		ligat = element.ligat;
		ElemInvisible = element.ElemInvisible;
		pointInvisible = element.pointInvisible;
		existDebord = element.existDebord;
		fligat = element.fligat;
		notschowgrp = element.notschowgrp;
		cone = element.cone;
		liaisonPointil = element.liaisonPointil;
		reserve1 = element.reserve1;
		reserve2 = element.reserve2;
		ottava = element.ottava;
		durNum = element.durNum;
		durDen = element.durDen;
		offset = element.offset;
		xrel = element.xrel;
		dec_y = element.dec_y;
		debordCode = element.debordCode;
		debordSize = element.debordSize;
		no = element.no;
		code = element.code;
		
		// comparison
		m_im_filename = element.m_im_filename;
		m_im_staff = element.m_im_staff;
		m_im_staff_segment = element.m_im_staff_segment;
		m_im_pos = element.m_im_pos;
		m_cmp_flag = element.m_cmp_flag;

		pdebord = NULL;
		if ( existDebord )
		{
			int size = debordSize - sizeof( debordSize ) - sizeof( debordCode );
			pdebord = malloc( size );
			memcpy( pdebord, element.pdebord, size );
		}
	}
	return *this;
}

WgElement::~WgElement()
{
	if ( pdebord )
	{
		wxLogDebug( "Delete debord" );
		free( pdebord );
	}
}


void WgElement::ClearElement( wxDC *dc, WgStaff *staff )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	m_w->m_currentColour = wxWHITE;
	m_w->efface = true;

	this->Draw( dc, staff );
	
	m_w->efface = false;
	m_w->m_currentColour = &m_w->m_black;
	
	staff->DrawStaffLines( dc );
}


wxClientDC *WgElement::InitAndClear( WgStaff *staff )
{
	if ( m_w || staff ) // effacement
	{
		wxClientDC *dc = new wxClientDC( m_w );
		m_w->InitDC( dc );
		this->ClearElement( dc, staff );
		return dc;
	}
	else
		return NULL;
}

void WgElement::DrawAndRelease( wxDC *dc, WgStaff *staff )
{
	if ( dc == NULL )
		return;
		
	this->Draw( dc, staff );
	delete dc;
}


int WgElement::filtrcod (int codElement, int *oct)
{	
	*oct = this->oct;

	if (codElement == F1)
	{	
		if ((*oct) != 0) 
			(*oct)--; 
		codElement = F8;
	}
	else if (codElement == F9 || codElement == F10)
	{	
		if ((*oct) < 7) 
			(*oct)++;
		codElement = (codElement == F9) ? F2 : F3;
	}
	return codElement;
}

// WDR: handler implementations for WgElement


#endif // AX_WG
