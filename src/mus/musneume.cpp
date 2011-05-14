/////////////////////////////////////////////////////////////////////////////
// Name:        musneume.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin, Chris Niven
// All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musneume.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "musneume.h"
#include "musrc.h"
#include "musstaff.h"
#include "muspage.h"
#include "musdef.h"
#include "neumedef.h"

#include <math.h>

#include <mei/mei.h>
#include <exception>

//----------------------------------------------------------------------------
// MusNeumeElement
//----------------------------------------------------------------------------

MusNeumeElement::MusNeumeElement(MeiElement &element, int pitch, int oct) {
    m_meiref = &element;
    MeiAttribute *p = m_meiref->getAttribute("pname");
    MeiAttribute *o = m_meiref->getAttribute("oct");
    if (p && o) {
        string pitch = p->getValue();
		if (pitch == "c") {
            m_pitch_difference = 1;
        } else if (pitch == "d") {
            m_pitch_difference = 2;
        } else if (pitch == "e") {
            m_pitch_difference = 3;
        } else if (pitch == "f") {
            m_pitch_difference = 4;
        } else if (pitch == "g") {
            m_pitch_difference = 5;
        } else if (pitch == "a") {
            m_pitch_difference = 6;
        } else if (pitch == "b") {
            m_pitch_difference = 7;
        }
    } else {
        throw "missing pitch or octave";
    }
	//Need to initialize the octave.
}

// Duplicate an existing pitch
MusNeumeElement::MusNeumeElement( const MusNeumeElement &other) {
	m_pitch_difference = other.m_pitch_difference;
    m_meiref = other.m_meiref;
}

int MusNeumeElement::getPitchDifference() {
    return m_pitch_difference;
}

//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

MusNeume::MusNeume() : MusElement() {
    TYPE = NEUME;
	m_type = NEUME_TYPE_COMPOUND; //note: for all practical purposes, this can always be punctum.
    /*
    // For testing only
	MusNeumeElement first = MusNeumeElement(this->pitch,this->oct);
	MusNeumeElement next = MusNeumeElement(-1);
	MusNeumeElement third = MusNeumeElement(2);
	MusNeumeElement fourth = MusNeumeElement(-4);
	MusNeumeElement fifth = MusNeumeElement(3);
	m_pitches.push_back(first);
	m_pitches.push_back(next);
	m_pitches.push_back(third);
	m_pitches.push_back(fourth);
	m_pitches.push_back(fifth);
    */
	m_meiref = 0; //this is necessary to avoid garbage when things aren't called from MEIs.
}

// Copy an existing neume
MusNeume::MusNeume( const MusNeume &neume) :
    MusElement(neume) {
	TYPE = neume.TYPE;
	m_type = neume.m_type;
	m_pitches = neume.m_pitches;
	m_meiref = neume.m_meiref;
}

/**
 * Create a neume from an MeiElement.
 */
MusNeume::MusNeume(MeiElement &element) {
    m_meiref = &element;
    MeiAttribute *nameattr = m_meiref->getAttribute("name");
    MeiAttribute *variantattr = m_meiref->getAttribute("variant");
    wxString type = "";
    wxString variant = "";
    if (nameattr) {
        type = (nameattr->getValue()).c_str();
    }
    // e.g. punctum inclinatum has a variant of 'inclinatum'
    if (variantattr) {
        variant = (variantattr->getValue()).c_str();
    }
    setType(type, variant);
    
	//this->pitch = (this->m_pitches.at(0)).getPitch(); //sets the "pitch" member of MusElement class to the first pitch in this neume... or at least it should.
    vector<MeiElement> children = m_meiref->getChildren();
    for (vector<MeiElement>::iterator i = children.begin(); i != children.end(); i++) {
        MeiElement e = *i;
        if (e.getName() == "note") {
            // XXX: base pitch / octave
            MusNeumeElement note = MusNeumeElement(e, 0, 0);
            m_pitches.push_back(note);
        }
    }
    // XXX: Move pitch difference into element constructor
	
	//need to rename the pitches to their values relative to one another.
	//for (vector<MusNeumeElement>::iterator i = (this->m_pitches).begin() + 1; i != (this->m_pitches).end(); i++) {
	//	MusNeumeElement e = *i;
	//	e.getPitchDifference() = e.pitch - this->pitch; //this assignment should take octave into account!
	//}
}

/**
 * The type of this neume (from MEI) e.g. 'torculus', 'podatus'
 */
void MusNeume::setType(wxString type, wxString variant) {
    if (type == "punctum" && variant == "") {
        m_type = NEUME_TYPE_PUNCTUM;
    } else if (type == "virga") {
        m_type = NEUME_TYPE_VIRGA;
    } else if (type == "podatus") {
        m_type = NEUME_TYPE_PODATUS;
    } else if (type == "clivis") {
        m_type = NEUME_TYPE_CLIVIS;
    } else if (type == "porrectus") {
        m_type = NEUME_TYPE_PORRECTUS;
    } else if (type == "scandicus") {
        m_type = NEUME_TYPE_SCANDICUS;
    } else if (type == "torculus") {
        m_type = NEUME_TYPE_TORCULUS;
    } else if (type == "compound") {
        m_type = NEUME_TYPE_COMPOUND;
    } else {
        string t = type.mb_str();
        throw "unknown neume type " + t;
    }
}

void MusNeume::setType(NeumeType type) {
    m_type = type;
}

NeumeType MusNeume::getType() {
    return m_type;
}

MeiElement &MusNeume::getMeiElement() {
    return *m_meiref;
}

vector<MusNeumeElement> MusNeume::getPitches() {
    return m_pitches;
}


//same as MusNote, except for the MEI stuff.
void MusNeume::SetPitch( int pitch, int oct )
{
	if ( this->TYPE != NEUME )
		return;
	
	if ((this->pitch == pitch) && (this->oct == oct ))
		return;
	
	this->oct = oct;
	this->pitch = pitch;
	
	if (m_meiref) {
		vector<MeiElement> children = m_meiref->getChildren();
		for (vector<MeiElement>::iterator i = children.begin(); i != children.end(); i++) {
			MeiElement e = *i;
			if (e.getName() == "note") {
				int thispitch = ((this->m_pitches.at(distance(children.begin(), i))).getPitchDifference() + pitch)%7;
				string value;
				switch (thispitch) {
					case 0: value = "b"; break;
					case 1: value = "c"; break;
					case 2: value = "d"; break;
					case 3: value = "e"; break;
					case 4: value = "f"; break;
					case 5: value = "g"; break;
					case 6: value = "a"; break;
					default: break;
				}
			(e.getAttribute("pname"))->setValue(value);
			//still need code for updating the octave attribute.
			}
		}
	}
	if (m_r) {
		m_r->DoRefresh();
    }
}

void MusNeume::Draw( AxDC *dc, MusStaff *staff)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	wxASSERT_MSG( m_r, "MusRC cannot be NULL ");
	if ( !Check() )
		return;
	
	int oct = this->oct - 4; //for some reason, notes are automatically drawn 4 octaves too high?
	
	if (!m_r->m_eraseElement && (this == m_r->m_currentElement))
		m_r->m_currentColour = AxRED;
	else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_MATCH))
		m_r->m_currentColour = AxLIGHT_GREY;
	else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_DEL))
		m_r->m_currentColour = AxGREEN;
	else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_SUBST))
		m_r->m_currentColour = AxBLUE;
	
	this->dec_y = staff->y_neume(this->pitch, staff->testcle( this->xrel ), oct);
	switch (m_type) {
		case (NEUME_TYPE_PUNCTUM): this->DrawPunctum(dc, staff); break;
		//case (NEUME_TYPE_PUNCTUM_INCLINATUM): this->DrawPunctumInclinatum(dc, staff); break;
		case (NEUME_TYPE_VIRGA): this->DrawVirga(dc, staff); break;
		case (NEUME_TYPE_PODATUS): this->DrawPodatus(dc, staff); break;
		case (NEUME_TYPE_CLIVIS): this->DrawClivis(dc, staff); break;
		case (NEUME_TYPE_PORRECTUS): this->DrawPorrectus(dc, staff); break;
		case (NEUME_TYPE_SCANDICUS): this->DrawScandicus(dc, staff); break;
		case (NEUME_TYPE_TORCULUS): this->DrawTorculus(dc, staff); break;
		case (NEUME_TYPE_COMPOUND): this->DrawCompound(dc, staff); break;
		default: break;
	}
	
	m_r->m_currentColour = AxBLACK;
	return;
}

void MusNeume::NeumeLine( AxDC *dc, MusStaff *staff, int x1, int x2, int y1, int y2)
{
	dc->SetPen( m_r->m_currentColour, m_r->ToRendererX( m_p->EpLignesPortee ), wxSOLID );
	dc->SetBrush(m_r->m_currentColour , wxTRANSPARENT );
	dc->DrawLine( m_r->ToRendererX(x1) , m_r->ToRendererY (y1) , m_r->ToRendererX(x2) , m_r->ToRendererY (y2) );
	dc->ResetPen();
	dc->ResetBrush();
}

void MusNeume::DrawPunctum( AxDC *dc, MusStaff *staff ) 
{
	// magic happens here
	int pTaille = staff->pTaille;
	
	int xn = this->xrel;
    //int xl = this->xrel;
	int bby = staff->yrel - m_r->_portee[pTaille];
	int ynn = this->dec_y + staff->yrel;
	//printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
	//	   ynn, this->dec_y, staff->yrel );
	
	xn += this->offset;
	
	int ledge = m_r->ledgerLine[pTaille][2];
	leg_line( dc, ynn,bby,this->xrel,ledge, pTaille); //draw ledger lines
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin); //draw punctum
}

void MusNeume::DrawPunctumInclinatum( AxDC *dc, MusStaff *staff ) 
{
	// magic happens here
	int pTaille = staff->pTaille;
	
	int xn = this->xrel;
    //int xl = this->xrel;
	int bby = staff->yrel - m_r->_portee[pTaille];
	int ynn = this->dec_y + staff->yrel;
	//printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
	//	   ynn, this->dec_y, staff->yrel );
	
	xn += this->offset;
	
	int ledge = m_r->ledgerLine[pTaille][2];
	leg_line( dc, ynn,bby,this->xrel,ledge, pTaille); //draw ledger lines
	m_r->festa_string( dc, xn, ynn + 19, nDIAMOND, staff, this->dimin); //draw punctum inclinatum
}

void MusNeume::DrawVirga( AxDC *dc, MusStaff *staff ) 
{
	// magic happens here
	int pTaille = staff->pTaille;
	
	int xn = this->xrel;
    //int xl = this->xrel;
	int bby = staff->yrel - m_r->_portee[pTaille];
	int ynn = this->dec_y + staff->yrel;
	//printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
	//	   ynn, this->dec_y, staff->yrel );
	
	xn += this->offset;
	
	int ledge = m_r->ledgerLine[pTaille][2];
	leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	m_r->festa_string( dc, xn + PUNCT_WIDTH, ynn + 19, '#', staff, this->dimin); //bottom right stem
}

void MusNeume::DrawPodatus( AxDC *dc, MusStaff *staff ) 
{
	// magic happens here
	int pTaille = staff->pTaille;
	
	int xn = this->xrel;
    //int xl = this->xrel;
	int bby = staff->yrel - m_r->_portee[pTaille];
	int ynn = this->dec_y + staff->yrel;
	//printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
	//	   ynn, this->dec_y, staff->yrel );
	
	xn += this->offset;
	
	int ledge = m_r->ledgerLine[pTaille][2];
	leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPES, staff, this->dimin);
	int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
	leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn, ynn2);
}

void MusNeume::DrawClivis( AxDC *dc, MusStaff *staff ) 
{
	// magic happens here
	int pTaille = staff->pTaille;
	
	int xn = this->xrel;
    //int xl = this->xrel;
	int bby = staff->yrel - m_r->_portee[pTaille];
	int ynn = this->dec_y + staff->yrel;
	//printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
	//	   ynn, this->dec_y, staff->yrel );
	
	xn += this->offset;
	
	int ledge = m_r->ledgerLine[pTaille][2];
	int ynn2, dx;
	leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
	ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
	if (ynn2 == ynn)
		dx = CLIVIS_X_SAME;
	else dx = CLIVIS_X_DIFF;
	leg_line( dc, ynn2,bby,this->xrel + dx,ledge, pTaille);
	m_r->festa_string( dc, xn + dx, ynn2 + 19, nPUNCTUM, staff, this->dimin);
}

void MusNeume::DrawPorrectus( AxDC *dc, MusStaff *staff ) 
{
	// magic happens here
	int pTaille = staff->pTaille;
	
	int xn = this->xrel;
    //int xl = this->xrel;
	int bby = staff->yrel - m_r->_portee[pTaille];
	int ynn = this->dec_y + staff->yrel;
	//printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
	//	   ynn, this->dec_y, staff->yrel );
	
	xn += this->offset;
	
	int ledge = m_r->ledgerLine[pTaille][2];
	leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
	wxString slope; int dx;
	if ((this->m_pitches.at(1)).getPitchDifference()==-1) {
		slope = nPORRECT_1;
		dx = -4;
	}
	else if ((this->m_pitches.at(1)).getPitchDifference()==-2) {
		slope = nPORRECT_2;
		dx = -2;
	}
	else if ((this->m_pitches.at(1)).getPitchDifference()==-3) {
		slope = nPORRECT_3;
		dx = 3;
	}
	else if ((this->m_pitches.at(1)).getPitchDifference()==-4) {
		slope = nPORRECT_4;
		dx = 3;
	}
	m_r->festa_string( dc, xn, ynn + 19, slope, staff, this->dimin);
	xn += 3*PUNCT_WIDTH + dx;
	int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
	leg_line( dc, ynn,bby,xn,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
	xn += PUNCT_WIDTH - 1;
	int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
	this->NeumeLine( dc, staff, xn, xn, ynn2, ynn3);
}

void MusNeume::DrawScandicus( AxDC *dc, MusStaff *staff ) 
{
	// magic happens here
	int pTaille = staff->pTaille;
	
	int xn = this->xrel;
    //int xl = this->xrel;
	int bby = staff->yrel - m_r->_portee[pTaille];
	int ynn = this->dec_y + staff->yrel;
	//printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
	//	   ynn, this->dec_y, staff->yrel );
	
	xn += this->offset;
	
	int ledge = m_r->ledgerLine[pTaille][2];
	leg_line( dc, ynn,bby,xn,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	xn += PUNCT_WIDTH;
	ynn = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
	leg_line( dc, ynn,bby,xn,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	xn += PUNCT_WIDTH;
	ynn = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
	leg_line( dc, ynn,bby,xn,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
}

void MusNeume::DrawTorculus( AxDC *dc, MusStaff *staff ) 
{
	// magic happens here
	int pTaille = staff->pTaille;
	
	int xn = this->xrel;
    //int xl = this->xrel;
	int bby = staff->yrel - m_r->_portee[pTaille];
	int ynn = this->dec_y + staff->yrel;
	//printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
	//	   ynn, this->dec_y, staff->yrel );
	
	xn += this->offset;
	
	int ledge = m_r->ledgerLine[pTaille][2];
	leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).getPitchDifference());
	this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn, ynn2);
	xn += PUNCT_WIDTH - 2;
	leg_line( dc, ynn2,bby,xn,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
	int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).getPitchDifference());
	xn += PUNCT_WIDTH - 2;
	this->NeumeLine( dc, staff, xn + 1, xn + 1, ynn2, ynn3);
	leg_line( dc, ynn3,bby,xn,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
}

void MusNeume::DrawCompound( AxDC *dc, MusStaff *staff ) 
{
	// magic happens here
	int pTaille = staff->pTaille;
	
	int xn = this->xrel;
    //int xl = this->xrel;
	int bby = staff->yrel - m_r->_portee[pTaille];
	int ynn = this->dec_y + staff->yrel;
	//printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
	//	   ynn, this->dec_y, staff->yrel );
	
	xn += this->offset;
	
	int ledge = m_r->ledgerLine[pTaille][2];
	leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
	m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	int dx = (PUNCT_WIDTH + 1)*(this->m_pitches.size());
	int y1 = 0, y2 = 0;
	for (vector<MusNeumeElement>::iterator i = this->m_pitches.begin() + 1; i != this->m_pitches.end(); i++) {
		MusNeumeElement e = *i;
		if (e.getPitchDifference() > y1)
			y1 = e.getPitchDifference();
		if (e.getPitchDifference() < y2)
			y2 = e.getPitchDifference();
	}
	y1 = ynn + (m_r->_espace[pTaille])*y1;
	y2 = ynn + (m_r->_espace[pTaille])*y2;
	m_r->box( dc, xn, y1, xn + dx, y2 );
}

//Also same as MusNote. Could use an update, since it fails to draw ledger lines immediately below the staff.
void MusNeume::leg_line( AxDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int pTaille)
{
	int yn, ynt, yh, yb, test, v_decal = m_r->_interl[pTaille];
	int dist, xng, xnd;
	register int i;
	
	
	yh = y_p + m_r->_espace[pTaille]; yb = y_p- m_r->_portee[pTaille]- m_r->_espace[pTaille];
	
	if (!in(y_n,yh,yb))                           // note hors-portee?
	{
		xng = xn - smaller;
		xnd = xn + smaller;
		
		dist = ((y_n > yh) ? (y_n - y_p) : y_p - m_r->_portee[pTaille] - y_n);
  		ynt = ((dist % m_r->_interl[pTaille] > 0) ? (dist - m_r->_espace[pTaille]) : dist);
		test = ynt/ m_r->_interl[pTaille];
		if (y_n > yh)
		{	yn = ynt + y_p;
			v_decal = - m_r->_interl[pTaille];
		}
		else
			yn = y_p - m_r->_portee[pTaille] - ynt;
		
		//hPen = (HPEN)SelectObject (hdc, CreatePen (PS_SOLID, _param.EpLignesPORTEE+1, workColor2));
		//xng = toZoom(xng);
		//xnd = toZoom(xnd);
		
        dc->SetPen( m_r->m_currentColour, m_r->ToRendererX( m_p->EpLignesPortee ), wxSOLID );
        dc->SetBrush(m_r->m_currentColour , wxTRANSPARENT );
		
		for (i = 0; i < test; i++)
		{
			dc->DrawLine( m_r->ToRendererX(xng) , m_r->ToRendererY ( yn ) , m_r->ToRendererX(xnd) , m_r->ToRendererY ( yn ) );
			//m_r->h_line ( dc, xng, xnd, yn, m_r->_param.EpLignesPORTEE);
			//yh =  toZoom(yn);
			//MoveToEx (hdc, xng, yh, NULL);
			//LineTo (hdc, xnd, yh);
			
			yn += v_decal;
		}
		
        dc->ResetPen();
        dc->ResetBrush();
	}
	return;
}
