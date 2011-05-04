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

#include <math.h>

#include <mei/mei.h>
#include <exception>

//----------------------------------------------------------------------------
// MusNeumeElement
//----------------------------------------------------------------------------

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
    }
}

// Duplicate an existing pitch
MusNeumeElement::MusNeumeElement( const MusNeumeElement &other) {
    pitch = other.pitch;
    octave = other.octave;
    m_meiref = other.m_meiref;
    m_pitch = other.m_pitch;
}

wxString MusNeumeElement::getPitch() {
    return m_pitch;
}

//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

MusNeume::MusNeume() : MusElement() {
    TYPE = NEUME;
}

// Copy an existing neume
MusNeume::MusNeume( const MusNeume &neume) :
    MusElement(neume) {
	TYPE = neume.TYPE;
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
    
    vector<MeiElement> children = m_meiref->getChildren();
    for (vector<MeiElement>::iterator i = children.begin(); i != children.end(); i++) {
        MeiElement e = *i;
        if (e.getName() == "note") {
            MusNeumeElement note = MusNeumeElement(e);
            m_pitches.push_back(note);
        }
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
