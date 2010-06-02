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

MusNeume::MusNeume():
	MusElement()
{
	
	
	TYPE = NEUME;
	this->length = 1;
	this->closed = false; // automatically build up neume clusters
	this->n_selected = 0; // instantiation of class always creates a single note

}

MusNeume::MusNeume( const MusNeume& neume )
	: MusElement( neume )
{
	TYPE = neume.TYPE;
    // copy each member
    // ...
	this->length = neume.length;
	this->closed = neume.closed;
	this->n_selected = neume.n_selected;
	
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
		
	}
	return *this;
}

MusNeume::~MusNeume()
{	
}

//might have to expand on this? probably not though

bool MusNeume::IsClosed() { return this->closed; }

void MusNeume::SetPitch( int code, int oct, MusStaff *staff )
{
	if ( this->TYPE != NEUME )
		return;

    // Change the neume pitch
    // ...

	//copypaste from musnote
	
	if ((this->code == code) && (this->oct == oct ))
		return;
	
	this->oct = oct;
	this->code = code;
	
	
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
	
	
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	
	 
		
	//we will have to deal with 'chords' (ligatures) eventually!
	
		//if (!this->chord)	// && (!pelement->ElemInvisible || illumine)) 
		this->note(dc, staff);
		//else
		//	this->accord(dc, staff);
	//{{{{printf("wtf you can do this?\n");}}}}
	
	
	return;
}

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
	
	//not sure what this rayon business does
	
//	//val=this->val;
//	formval = (this->inv_val && val > RD) ? (val+1) : val;
//		queueCentre = 0;
	
	//rayon = m_w->rayonNote[pTaille][this->dimin];
	
	
	//we may need ledger lines for neumes!...eventually
	//leg_line( dc, ynn,bby,xl,ledge, pTaille);	// dessin lignes additionnelles
	
	//then drawing of the actual notehead itself
	
	
	
		//kay weird stuff happening here
	
	
	m_w->putneume( dc, this->xrel, ynn + 5, nPUNCTUM, staff, this->dimin); //worry about the font later
	
}

// WDR: handler implementations for MusNeume



