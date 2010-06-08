/////////////////////////////////////////////////////////////////////////////
// Name:        musneume.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
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
#include "muswindow.h"
#include "musstaff.h"
#include "muspage.h"
#include "musdef.h"

#include <math.h>

// sorting function
int SortElements(MusNeume **first, MusNeume **second)
{
	if ( (*first)->xrel < (*second)->xrel )
		return -1;
	else if ( (*first)->xrel > (*second)->xrel )
		return 1;
	else 
		return 0;
}

// WDR: class implementations

//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

//constructors are only called when brand new neumes are created. 
// otherwise, pitches are just added to existing neumes through the append() method.


MusNeume::MusNeume():
	MusElement()
{
	
	
	TYPE = NEUME;
	length = 1;
	closed = false; // automatically build up neume clusters
	n_selected = 0; // instantiation of class always creates a single note
	n_pitches = (NPitch*)malloc(sizeof(NPitch));
	//set initial pitch, for entire neume as well as the first NPitch element
	code = n_pitches[0].code = 0;
	val = 0;	//square punctum by default
	
	//oct = 4; //? again, following laurent's example
}

MusNeume::MusNeume( unsigned char _val, unsigned char _code )
{
	TYPE = NEUME;
	length = 1;
	closed = false;
	n_selected = 0;
	n_pitches = (NPitch*)malloc(sizeof(NPitch));
	
	code = n_pitches[0].code = _code;
	val = _val;
}

MusNeume::MusNeume( const MusNeume& neume )
	: MusElement( neume )
{
	TYPE = neume.TYPE;
	this->length = neume.length;
	this->closed = neume.closed;
	this->n_selected = neume.n_selected;
	this->n_pitches = neume.n_pitches;
	this->code = neume.code;
	//add more...
}

MusNeume& MusNeume::operator=( const MusNeume& neume )
{
	if ( this != &neume ) // not self assignement
	{
		// For base class MusElement copy assignement
		(MusElement&)*this = neume;
		TYPE = neume.TYPE;
        // copy each member
        // ...
		this->length = neume.length;
		this->closed = neume.closed;
		this->n_selected = neume.n_selected;
		this->n_pitches = neume.n_pitches;
		this->code = neume.code;
		//add more...
	}
	return *this;
}

MusNeume::~MusNeume()
{	
	//we need to delete all the NPitch elements
	for (int i = this->length - 1; i >= 0; i--) 
	{
		free(this->n_pitches + i);
	}
}

//might have to expand on this? probably not though

bool MusNeume::IsClosed() { return this->closed; }

void MusNeume::SetClosed(bool value) {
	this->closed = value;
}

// if open, returns next individual pitch
// if closed, return false and let musstaff select the next element
bool MusNeume::GetNext() { 
	
	if (closed) return false;
	else 
	{
		if (n_selected < length - 1) 
		{
			n_selected++;
			//refresh window or call drawing code
		}
		return true;
	}
}

bool MusNeume::GetPrevious() {
	if (closed) return false;
	else 
	{
		if (n_selected > 0) 
		{
			n_selected--;
			//refresh window or call drawing code
		} 
		return true;	//stay in 'individual edit mode' since the neume is open
	}
}

void MusNeume::Append() {
	
}

// I'm sure this could be optimized

void MusNeume::SetPitch( int code, int oct, MusStaff *staff )
{
	if ( this->TYPE != NEUME )
		return;

    // Change the neume pitch
    // ...

	//copypaste from musnote
	
	if ((this->code == code) && (this->oct == oct ))
		return;
	
	
	if (this->closed) {
		this->oct = oct;
		this->code = code;
		n_pitches[0].oct = oct;
		n_pitches[0].code = code;
	} else {
		if (n_pitches + n_selected != NULL) {
			n_pitches[n_selected].oct = oct;
			n_pitches[n_selected].code = code;
		}
	}

	
	if (m_w)
		m_w->Refresh();
}

void MusNeume::SetValue( int value, MusStaff *staff, int vflag )
{	
	if ( this->TYPE != NEUME )
		return;

    // Change the neume value
    // ...
	
	//only a punctum for now, still have to decide the scheme for neume values
	
	if ((value < 0 || (value > TC)))
		 return;
		
	this->val = value;
	
	//refresh drawing automatically
	if (m_w)
		m_w->Refresh();
}


//should have some loop for drawing each element in the neume

void MusNeume::Draw( wxDC *dc, MusStaff *staff)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	wxASSERT_MSG( m_w, "MusWindow cannot be NULL ");
	if ( !Check() )
		return;	

    // Draw the neume
    // ...
	
	// following the example set by musnote...
	
	int oct = this->oct - 4; //? 
	if (!m_w->efface && (this == m_w->m_currentElement))
		m_w->m_currentColour = wxRED;
	else if (!m_w->efface && (this->m_cmp_flag == CMP_MATCH))
		m_w->m_currentColour = wxLIGHT_GREY;
	else if (!m_w->efface && (this->m_cmp_flag == CMP_DEL))
		m_w->m_currentColour = wxGREEN;
	else if (!m_w->efface && (this->m_cmp_flag == CMP_SUBST))
		m_w->m_currentColour = wxBLUE;	
	
	//this stays the same for open and closed mode
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	
	
	
	
		//if (!this->chord)	// && (!pelement->ElemInvisible || illumine)) 
		this->note(dc, staff);
		//else
		//	this->accord(dc, staff);
	
	if (this->IsClosed()) {
		//neume drawing routine
	} else {
		//individual punctum edit mode drawing routine
	}
	
	//just trying a hack here
	m_w->m_currentColour = wxBLACK;
	
	//seems to fix the problem somewhat...
	
	
	return;
}

//modify this to draw multiple notes in a loop
void MusNeume::note( wxDC *dc, MusStaff *staff ) 
{
	int pTaille = staff->pTaille;
	
	int b = this->dec_y;
	int up=0, i, valdec, fontNo, ledge, queueCentre;
	int x1, x2, y2, espac7, decval, vertical;
	int formval = 0;
	int rayon, milieu = 0;
	
	int xn = this->xrel, xl = this->xrel;
	int bby = staff->yrel - m_w->_portee[pTaille];  // bby= y sommet portee
	int ynn = this->dec_y + staff->yrel; 
	static int ynn_chrd;
	
	xn += this->offset;
	
	
		
	//rayon = m_w->rayonNote[pTaille][this->dimin];
	
	
	//then drawing of the actual notehead itself
	
	
	switch (val) {
		case 0:
		fontNo = nPUNCTUM;
		break;
		case 1:
		fontNo = nDIAMOND;
		break;
		case 2:
		fontNo = nCEPHALICUS;
		break;
		case 3:
		fontNo = nPODATUS;
		break;
//		case 4:
//		fontNo = nDIAMOND_SMALL;		//small diamonds don't draw correctly for some reason
//		break;
		case 4:
		fontNo = nQUILISMA;
		break;
	}
		
	
	// issue with y position — empirically setting y position (for now)
	// as of 06/04/2010 punctums draw a 7th below the mouse click
	
	//more hacks... since neumes dont' have time values (yet)
	
	ledge = m_w->ledgerLine[pTaille][2];
	
	leg_line( dc, ynn,bby,xl,ledge, pTaille);	// dessin lignes additionnelles	
	m_w->putneume( dc, this->xrel, ynn + 65, fontNo, staff, this->dimin); //worry about the font later
	
	
}

void MusNeume::leg_line( wxDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int pTaille)
{
	int yn, ynt, yh, yb, test, v_decal = m_w->_interl[pTaille];
	int dist, xng, xnd;
	register int i;
	
	
	yh = y_p + m_w->_espace[pTaille]; yb = y_p- m_w->_portee[pTaille]- m_w->_espace[pTaille];
	
	if (!in(y_n,yh,yb))                           // note hors-portee?
	{
		xng = xn - smaller;
		xnd = xn + smaller;
		
		dist = ((y_n > yh) ? (y_n - y_p) : y_p - m_w->_portee[pTaille] - y_n);
  		ynt = ((dist % m_w->_interl[pTaille] > 0) ? (dist - m_w->_espace[pTaille]) : dist);
		test = ynt/ m_w->_interl[pTaille];
		if (y_n > yh)
		{	yn = ynt + y_p;
			v_decal = - m_w->_interl[pTaille];
		}
		else
			yn = y_p - m_w->_portee[pTaille] - ynt;
		
		//hPen = (HPEN)SelectObject (hdc, CreatePen (PS_SOLID, _param.EpLignesPORTEE+1, workColor2));
		//xng = toZoom(xng);
		//xnd = toZoom(xnd);
		
		wxPen pen( *m_w->m_currentColour, m_p->EpLignesPortee, wxSOLID );
		dc->SetPen( pen );
		wxBrush brush( *m_w->m_currentColour , wxTRANSPARENT );
		dc->SetBrush( brush );
		
		for (i = 0; i < test; i++)
		{
			dc->DrawLine( m_w->ToZoom(xng) , m_w->ToZoomY ( yn ) , m_w->ToZoom(xnd) , m_w->ToZoomY ( yn ) );
			//m_w->h_line ( dc, xng, xnd, yn, m_w->_param.EpLignesPORTEE);
			//yh =  toZoom(yn);
			//MoveToEx (hdc, xng, yh, NULL);
			//LineTo (hdc, xnd, yh);
			
			yn += v_decal;
		}
		
		dc->SetPen( wxNullPen );
		dc->SetBrush( wxNullBrush );
	}
	return;
}

// WDR: handler implementations for MusNeume



