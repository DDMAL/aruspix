 /////////////////////////////////////////////////////////////////////////////
// Name:        muselement.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "muselement.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "muselement.h"
#include "musrc.h"
#include "musstaff.h"

#include <uuid/uuid.h>
#include <algorithm>

//----------------------------------------------------------------------------
// MusElement
//----------------------------------------------------------------------------

MusElement::MusElement() :
	MusObject()
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
	m_im_pos = 0;
	m_cmp_flag = 0;
	m_debord_str = "";

	code = 0;
	pitch = 0;
}

MusElement::MusElement( const MusElement& element )
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
	pitch = element.pitch;
	
	// comparison
	m_im_filename = element.m_im_filename;
	m_im_staff = element.m_im_staff;
	m_im_pos = element.m_im_pos;
	m_cmp_flag = element.m_cmp_flag;
	m_debord_str = element.m_debord_str;

	pdebord = NULL;
	if ( existDebord )
	{
		int size = debordSize - sizeof( debordSize ) - sizeof( debordCode );
		pdebord = malloc( size );
		memcpy( pdebord, element.pdebord, size );
	}
} 

MusElement& MusElement::operator=( const MusElement& element )
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
		pitch = element.pitch;
		
		// comparison
		m_im_filename = element.m_im_filename;
		m_im_staff = element.m_im_staff;
		m_im_pos = element.m_im_pos;
		m_cmp_flag = element.m_cmp_flag;
		m_debord_str = element.m_debord_str;

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

MusElement::~MusElement()
{
	if ( pdebord )
	{
		wxLogDebug( "Delete debord" );
		free( pdebord );
	}
}

int MusElement::filtrcod( int codElement, int *oct )
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

int MusElement::filtrpitch( int pitchElement, int *oct)
{
	*oct = this->oct;
	
	if (pitchElement < 1)
	{
		if ((*oct) != 0)
			(*oct) -= floor((1 - pitchElement)/7) + 1;
	}
	else if (pitchElement > 7)
	{
		if ((*oct) < 7)
			(*oct) += floor((pitchElement - 7)/7) + 1;
	}
	pitchElement = pitchElement%7;
	if (pitchElement==0)
		pitchElement = 7;
	return pitchElement;
}

std::string MusElement::PitchToStr(int pitch)
{
    string value;
    switch (pitch) {
        case 0: value = "b"; break;
        case 1: value = "c"; break;
        case 2: value = "d"; break;
        case 3: value = "e"; break;
        case 4: value = "f"; break;
        case 5: value = "g"; break;
        case 6: value = "a"; break;
        default: break;
    }
    return value;
}

int MusElement::StrToPitch(std::string pitch)
{
    int value;
    if (pitch == "c") {
        value = 1;
    } else if (pitch == "d") {
        value = 2;
    } else if (pitch == "e") {
        value = 3;
    } else if (pitch == "f") {
        value = 4;
    } else if (pitch == "g") {
        value = 5;
    } else if (pitch == "a") {
        value = 6;
    } else if (pitch == "b") {
        value = 7;
    } else {
        throw "bad pitch: " + pitch;
    }
    return value;
}

void MusElement::newMeiRef(MusElement* previous)
{
	newmeielement = true;
	MeiElement* element = previous->getMeiRef();
	MeiElement* zone = new MeiElement("zone", element->getNs());
	std::string facs;
	char uuidbuff[36];
	uuid_t uuidGenerated;
	uuid_generate(uuidGenerated);
	uuid_unparse(uuidGenerated, uuidbuff);
	facs = string(uuidbuff);
	std::transform(facs.begin(), facs.end(), facs.begin(), ::tolower);
	facs = "m-" + facs;
	zone->setId(facs);
	vector<MeiElement*>::iterator i = element->getZone()->getParent().getChildren().begin();
	while (i != element->getZone()->getParent().getChildren().end()) {
		if (*i == element->getZone()) {
			element->getZone()->getParent().getChildren().insert(i, zone);
			zone->setParent(element->getZone()->getParent());
			break;
		} else {
			i++;
		}
	}
	MeiElement *meiref = new MeiElement("");
	meiref->setFacs(facs);
	meiref->setZone(zone);
	uuid_generate(uuidGenerated);
	uuid_unparse(uuidGenerated, uuidbuff);
	facs = string(uuidbuff);
	std::transform(facs.begin(), facs.end(), facs.begin(), ::tolower);
	facs = "m-" + facs;
	meiref->setId(facs);
	setMeiRef(meiref);
	this->updateMeiRef();
	vector<MeiElement*>::iterator it = element->getParent().getChildren().begin();
	while (it != element->getParent().getChildren().end()) {
		if ((*it)->getId() == element->getId()) {
			element->getParent().getChildren().insert(it, meiref);
			meiref->setParent(element->getParent());
			break;
		} else {
			it++;
		}
	}
}