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

MusNeumeElement::MusNeumeElement(int pitchDiff)
{
	pitchDifference = pitchDiff;
}

MusNeumeElement::MusNeumeElement(int _pitch, int _oct)
{
	pitch = _pitch;
	octave = _oct;
}

MusNeumeElement::MusNeumeElement(MeiElement &element) {
    m_meiref = &element;
    MeiAttribute *p = m_meiref->getAttribute("pname");
    if (p) {
        m_pitch = (p->getValue()).c_str();
		if (m_pitch=='c') {pitch = 1;}
		else if (m_pitch=='d') {pitch = 2;}
		else if (m_pitch=='e') {pitch = 3;}
		else if (m_pitch=='f') {pitch= 4;}
		else if (m_pitch=='g') {pitch = 5;}
		else if (m_pitch=='a') {pitch = 6;}
		else if (m_pitch=='b') {pitch = 7;}
    }
	//Need to initialize the octave.
}

// Duplicate an existing pitch
MusNeumeElement::MusNeumeElement( const MusNeumeElement &other) {
    pitch = other.pitch;
    octave = other.octave;
	pitchDifference = other.pitchDifference;
    m_meiref = other.m_meiref;
    m_pitch = other.m_pitch;
}

int MusNeumeElement::getPitch() {
    return pitch;
}

int MusNeumeElement::getOctave() {
	return octave;
}

//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

MusNeume::MusNeume() : MusElement() {
    TYPE = NEUME;
	m_type = NEUME_TYPE_COMPOUND; //note: for all practical purposes, this can always be punctum.
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
}

// Copy an existing neume
MusNeume::MusNeume( const MusNeume &neume) :
    MusElement(neume) {
	TYPE = neume.TYPE;
	m_type = neume.m_type;
	m_pitches = neume.m_pitches;
}

/**
 * Create a neume from an MeiElement.
 */
MusNeume::MusNeume(MeiElement &element) /*: MusElement() ??? */ {
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
    
    vector<MeiElement> children = m_meiref->getChildren();
    for (vector<MeiElement>::iterator i = children.begin(); i != children.end(); i++) {
        MeiElement e = *i;
        if (e.getName() == "note") {
            MusNeumeElement note = MusNeumeElement(e);
            m_pitches.push_back(note);
        }
    }
	this->pitch = (this->m_pitches.at(0)).getPitch(); //sets the "pitch" member of MusElement class to the first pitch in this neume... or at least it should.
	
	//need to rename the pitches to their values relative to one another.
	for (vector<MusNeumeElement>::iterator i = (this->m_pitches).begin() + 1; i != (this->m_pitches).end(); i++) {
		MusNeumeElement e = *i;
		e.pitchDifference = e.pitch - this->pitch; //this assignment should take octave into account!
	}
}

/**
 * The type of this neume (from MEI) e.g. 'torculus', 'podatus'
 */
void MusNeume::setType(wxString type, wxString variant) {
    if (type == "punctum" && variant == "") {
        m_type = NEUME_TYPE_PUNCTUM;
    } else if (type == "punctum" && variant == "inclinatum") {
        m_type = NEUME_TYPE_PUNCTUM_INCLINATUM;
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


//same as MusNote. Refactor?
void MusNeume::SetPitch( int pitch, int oct )
{
	if ( this->TYPE != NEUME )
		return;
	
	if ((this->pitch == pitch) && (this->oct == oct ))
		return;
	
	this->oct = oct;
	this->pitch = pitch;
	
	if (m_r)
		m_r->DoRefresh();
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
	this->DrawNeume( dc, staff );
	
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

void MusNeume::DrawNeume( AxDC *dc, MusStaff *staff ) 
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
	
	if (this->m_type==NEUME_TYPE_PODATUS) {
		leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn + 19, nPES, staff, this->dimin);
		int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).pitchDifference);
		leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
		this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn, ynn2);		
	}
	else if (this->m_type==NEUME_TYPE_CLIVIS) {
		int ynn2, dx;
		leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
		m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
		ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).pitchDifference);
		if (ynn2 == ynn)
			dx = CLIVIS_X_SAME;
		else dx = CLIVIS_X_DIFF;
		leg_line( dc, ynn2,bby,this->xrel + dx,ledge, pTaille);
		m_r->festa_string( dc, xn + dx, ynn2 + 19, nPUNCTUM, staff, this->dimin);
	}
	else if (this->m_type==NEUME_TYPE_VIRGA) {
		leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
		m_r->festa_string( dc, xn + PUNCT_WIDTH, ynn + 19, '#', staff, this->dimin); //bottom right stem
	}
	else if (this->m_type==NEUME_TYPE_TORCULUS) {
		leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
		int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).pitchDifference);
		this->NeumeLine( dc, staff, xn + 9, xn + 9, ynn, ynn2);
		xn += PUNCT_WIDTH - 2;
		leg_line( dc, ynn2,bby,xn,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
		int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).pitchDifference);
		xn += PUNCT_WIDTH - 2;
		this->NeumeLine( dc, staff, xn + 1, xn + 1, ynn2, ynn3);
		leg_line( dc, ynn3,bby,xn,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, this->dimin);
		
	}
	else if (this->m_type==NEUME_TYPE_PORRECTUS) {
		leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn + 19, '3', staff, this->dimin);
		wxString slope; int dx;
		if ((this->m_pitches.at(1)).pitchDifference==-1) {
			slope = nPORRECT_1;
			dx = -4;
		}
		else if ((this->m_pitches.at(1)).pitchDifference==-2) {
			slope = nPORRECT_2;
			dx = -2;
		}
		else if ((this->m_pitches.at(1)).pitchDifference==-3) {
			slope = nPORRECT_3;
			dx = 3;
		}
		else if ((this->m_pitches.at(1)).pitchDifference==-4) {
			slope = nPORRECT_4;
			dx = 3;
		}
		m_r->festa_string( dc, xn, ynn + 19, slope, staff, this->dimin);
		xn += 3*PUNCT_WIDTH + dx;
		int ynn2 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).pitchDifference);
		leg_line( dc, ynn,bby,xn,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, this->dimin);
		xn += PUNCT_WIDTH - 1;
		int ynn3 = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).pitchDifference);
		this->NeumeLine( dc, staff, xn, xn, ynn2, ynn3);
	}
	else if (this->m_type==NEUME_TYPE_SCANDICUS) {
		leg_line( dc, ynn,bby,xn,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
		xn += PUNCT_WIDTH;
		ynn = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(1)).pitchDifference);
		leg_line( dc, ynn,bby,xn,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
		xn += PUNCT_WIDTH;
		ynn = ynn + (m_r->_espace[pTaille])*((this->m_pitches.at(2)).pitchDifference);
		leg_line( dc, ynn,bby,xn,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
	}
	else if (this->m_type==NEUME_TYPE_COMPOUND) {
		leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin);
		int dx = (PUNCT_WIDTH + 1)*(this->m_pitches.size());
		int y1 = 0, y2 = 0;
		for (vector<MusNeumeElement>::iterator i = this->m_pitches.begin() + 1; i != this->m_pitches.end(); i++) {
			MusNeumeElement e = *i;
			if (e.pitchDifference > y1)
				y1 = e.pitchDifference;
			if (e.pitchDifference < y2)
				y2 = e.pitchDifference;
		}
		y1 = ynn + (m_r->_espace[pTaille])*y1;
		y2 = ynn + (m_r->_espace[pTaille])*y2;
		m_r->box( dc, xn, y1, xn + dx, y2 );
	}
	else if (this->m_type==NEUME_TYPE_PUNCTUM) {
		leg_line( dc, ynn,bby,this->xrel,ledge, pTaille); //draw ledger lines
		m_r->festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, this->dimin); //draw punctum
	}
	else if (this->m_type==NEUME_TYPE_PUNCTUM_INCLINATUM) {
		leg_line( dc, ynn,bby,this->xrel,ledge, pTaille); //draw ledger lines
		m_r->festa_string( dc, xn, ynn + 19, nDIAMOND, staff, this->dimin); //draw punctum inclinatum
	}

}


//Also same as MusNote.
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

/*
//should have some loop for drawing each element in the neume
void MusNeume::Draw( AxDC *dc, MusStaff *staff)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	wxASSERT_MSG( m_r, "MusRC cannot be NULL ");
	if ( !Check() )
		return;	

	int oct = this->oct - 4; //? 
	if (!m_r->m_eraseElement && (this == m_r->m_currentElement))
		m_r->m_currentColour = AxRED;
	else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_MATCH))
		m_r->m_currentColour = AxLIGHT_GREY;
	else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_DEL))
		m_r->m_currentColour = AxGREEN;
	else if (!m_r->m_eraseElement && (this->m_cmp_flag == CMP_SUBST))
		m_r->m_currentColour = AxBLUE;	
	
	//this stays the same for open and closed mode
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	
	if (this->IsClosed()) {
		this->DrawNeume( dc, staff );
	} else {
		this->DrawPunctums( dc, staff );
	}
	
	m_r->m_currentColour = AxBLACK;
	return;
}

// Draw a box around the current 'open' neume
void MusNeume::DrawBox( AxDC *dc, MusStaff *staff ) //revise
{
	m_r->m_currentColour = AxRED;
	
	int x1, x2, y1, y2;
	int ynn = this->dec_y + staff->yrel; 
	// get x coords for bounding box
	x1 = this->xrel - PUNCT_PADDING + PUNCT_WIDTH;
	
	// determine how long the box should be	
	x2 = ((n_pitches.size() + 1) * PUNCT_PADDING) - PUNCT_WIDTH;
	// now get x2 coordinate relative to x1
	x2 += x1;
	
	// now for y coords
	// for y1, we need to know the pitch range, specifically the highest pitch
	y1 = (ynn + PUNCT_PADDING) + (this->p_max * m_r->_espace[staff->pTaille]);
	// y2 requires lowest pitch
	y2 = (ynn - PUNCT_PADDING) - abs(this->p_min * m_r->_espace[staff->pTaille]);
	
	m_r->m_currentColour = AxRED;
	m_r->box( dc, x1, y1, x2, y2 );
	
	m_r->m_currentColour = AxBLACK;
}

// Draw an uninterrupted neume
void MusNeume::DrawNeume( AxDC *dc, MusStaff *staff ) 
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
	
	if (this->n_pitches.size() == 1) {
		MusNeumePitch temp = n_pitches.at(0);
		leg_line( dc, ynn,bby,this->xrel,ledge, pTaille);
		m_r->festa_string( dc, xn, ynn + 16, 
					 temp.GetFestaString(), staff, this->dimin); 
	} else if (this->n_pitches.size() >= 1) {
		// we need to draw a ligature
		this->drawLigature(dc, staff);
	}
}

// Draw the interrupted neumes that a uneume is made up of
void MusNeume::DrawPunctums( AxDC *dc, MusStaff *staff )
{
	// draw each individual punctum in the group, side by side for easy editing
	// draw a rectangular box around this group to indicate open editing mode
	
	int pTaille = staff->pTaille;
	
	int oct = this->oct - 4; //? 
	int xn = this->xrel, xl = this->xrel;
	int bby = staff->yrel - m_r->_portee[pTaille];  // bby= y sommet portee
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	int ynn = this->dec_y + staff->yrel; 
	//printf("open code value: %d\nopen oct: %d\n", 
	//	   (int)this->code, oct );
	
	xn += this->offset;
	
	int ledge = m_r->ledgerLine[pTaille][2];
	
	//int punct_y;
	
	this->DrawBox( dc, staff );

    unsigned int i = 0;
	for (iter=n_pitches.begin(); iter != n_pitches.end(); iter++) 
	{
		ynn = staff->y_note((*iter).GetCode(), staff->testcle( this->xrel ), (*iter).GetOct() - 4);
		ynn += staff->yrel; 
		
		leg_line( dc, ynn,bby,xl + (i * PUNCT_PADDING),ledge, pTaille);
		// colour the selected item red
		if (i == n_selected) m_r->m_currentColour = AxRED;
		else m_r->m_currentColour = AxBLACK;
		m_r->festa_string( dc, this->xrel + (i * PUNCT_PADDING), ynn + 16, 
                          (*iter).GetFestaString(), staff, this->dimin );
        i++;
	}
}

void MusNeume::leg_line( AxDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int pTaille)
{
	int yn, ynt, yh, yb, test, v_decal = m_r->_interl[pTaille];
	int dist, xng, xnd;
	register int i;
	
	yh = y_p + m_r->_espace[pTaille];
	yb = y_p- m_r->_portee[pTaille]- m_r->_espace[pTaille];
	// printf("yh: %d, yb: %d, y_n: %d, y_p: %d\n", yh, yb, y_n, y_p);
	
	if (!in(y_n,yh,yb)) // note hors-portee?
	{
		xng = xn + 8 - smaller; //xn gauche 
		xnd = xn + smaller; //xn droite
		
		dist = ((y_n > yh) ? (y_n - y_p) : y_p - m_r->_portee[pTaille] - y_n);
  		ynt = ((dist % m_r->_interl[pTaille] > 0) ? (dist - m_r->_espace[pTaille]) : dist);
		test = ynt/ m_r->_interl[pTaille];
		if (y_n > yh) {
			yn = ynt + y_p;
			v_decal = - m_r->_interl[pTaille];
		} else {
			yn = y_p - m_r->_portee[pTaille] - ynt;
		}
		
        dc->SetPen( m_r->m_currentColour, m_p->EpLignesPortee, wxSOLID );
        dc->SetBrush( m_r->m_currentColour , wxTRANSPARENT );
		
		for (i = 0; i < test; i++) {
			dc->DrawLine( m_r->ToRendererX(xng) , m_r->ToRendererY ( yn ) , m_r->ToRendererX(xnd) , m_r->ToRendererY ( yn ) );
			yn += v_decal;
		}
		
        dc->ResetPen();
        dc->ResetBrush();
	}
	return;
}
*/
