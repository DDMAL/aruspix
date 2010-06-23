/////////////////////////////////////////////////////////////////////////////
// Name:        ligature.cpp
// Author:      Chris Niven 
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musneume.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include <vector>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "musneume.h"
#include "musstaff.h"
#include "muswindow.h"

// each "basic" type of ligature will have custom drawing code defined here


void MusNeume::drawLigature( wxDC *dc, MusStaff *staff ) {
	switch (this->n_pitches.size()) {
		case 0:
		case 1:
			//no ligatures drawn
			return;
		case 2:
			int i = n_pitches.at(0)->Compare(n_pitches.at(1));
			if (i == -1) {
				printf("2 notes ascending, drawing podatus\n");
				this->podatus(dc, staff);
				return;
			} else if (i == 1) {
				printf("2 notes descending, drawing clivis\n");
				this->clivis(dc, staff);
				return;
			}
			break;
	}
}

//need to optimize these drawing methods later

void MusNeume::clivis( wxDC *dc, MusStaff *staff ) {
	printf("Drawing a clivis\n");
	int pTaille = staff->pTaille;
	
	int oct = this->oct - 4;
	int xn = this->xrel;
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	int ynn = this->dec_y + staff->yrel; 
	int ynn2;
	xn += this->offset;
	
	int ledge = m_w->ledgerLine[pTaille][2];
	
	int punct_y;
	
	MusNeumePitch *temp = this->n_pitches.at(0);
	m_w->putfont(dc, this->xrel - 5, ynn + 16, temp->getPunctumType(),
				 staff, this->dimin, NEUME);
	temp = this->n_pitches.at(1);
	
	punct_y = staff->y_note((int)temp->code, staff->testcle( this->xrel ), temp->oct - 4);
	ynn2 = punct_y + staff->yrel; 
	
	m_w->putfont(dc, this->xrel + 4, ynn2 + 16, temp->getPunctumType(),
				 staff, this->dimin, NEUME);
	
	//left line always spans at least two pitches
	
	
	
	wxPen pen( *(m_w->m_currentColour), m_w->ToZoom(2), wxSOLID);
	dc->SetPen( pen );
	dc->DrawLine(m_w->ToZoom(this->xrel - 4), m_w->ToZoomY(ynn), 
				 m_w->ToZoom(this->xrel - 4), m_w->ToZoomY(ynn2 - 5));
	//right line
	dc->DrawLine(m_w->ToZoom(this->xrel + 4), m_w->ToZoomY(ynn), 
				 m_w->ToZoom(this->xrel + 4), m_w->ToZoomY(ynn2));
	
	dc->SetPen(wxNullPen);
}

void MusNeume::podatus( wxDC *dc, MusStaff *staff ) {
	int pTaille = staff->pTaille;
	
	int oct = this->oct - 4;
	int xn = this->xrel;
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	int ynn = this->dec_y + staff->yrel; 
	int ynn2;
	
	xn += this->offset;
	
	int ledge = m_w->ledgerLine[pTaille][2];
	
	int punct_y;
	
	MusNeumePitch *temp = this->n_pitches.at(0);
	m_w->putfont(dc, this->xrel - 5, ynn + 16, nPES,
				 staff, this->dimin, NEUME);
	temp = this->n_pitches.at(1);
	
	punct_y = staff->y_note((int)temp->code, staff->testcle( this->xrel ), temp->oct - 4);
	ynn2 = punct_y + staff->yrel; 
	
	m_w->putfont(dc, this->xrel - 5, ynn2 + 16, temp->getPunctumType(),
				 staff, this->dimin, NEUME);
	
	//draw a line connecting the two now
	wxPen pen( *(m_w->m_currentColour), m_w->ToZoom(2), wxSOLID);
	dc->SetPen( pen );
	dc->DrawLine(m_w->ToZoom(this->xrel + 4), m_w->ToZoomY(ynn), 
				 m_w->ToZoom(this->xrel + 4), m_w->ToZoomY(ynn2));
	dc->SetPen(wxNullPen);
	
}