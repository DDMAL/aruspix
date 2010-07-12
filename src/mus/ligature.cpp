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

#define LEFT_LINE 0
#define RIGHT_LINE 10

// each "basic" type of ligature will have custom drawing code defined here


void MusNeume::drawLigature( wxDC *dc, MusStaff *staff ) {
//	switch (this->n_pitches.size()) {
//		case 0:
//		case 1:
//			//no ligatures drawn
//			return;
//		case 2:
//			int i = n_pitches.at(0)->Compare(n_pitches.at(1));
//			if (i == -1) {
//				//printf("2 notes ascending, drawing podatus\n");
//				this->podatus(dc, staff);
//				return;
//			} else if (i == 1) {
//				//printf("2 notes descending, drawing clivis\n");
//				this->clivis(dc, staff);
//				return;
//			}
//			break;
//		case 3:
//			i = n_pitches.at(0)->Compare(n_pitches.at(1));
//			int j = n_pitches.at(1)->Compare(n_pitches.at(2));
//			if (i && !j)  //one note descending, one note ascending
//			{
//				this->porrectus(dc, staff);
//				return;
//			}
//	}
	
	
	
	//filter pes
	
	
	//filter porrectus
	//filter climacus
	//draw clivis
	this->clivis(dc, staff);
}




//need to optimize these drawing methods later

void MusNeume::clivis( wxDC *dc, MusStaff *staff ) {
	int ynn;
	
	if (n_pitches.size() >= 2 && 
		((MusNeumePitch*)n_pitches.at(0))->Pitch_Diff((MusNeumePitch*)n_pitches.at(1)) < 0)
		this->neume_line(dc, staff, 0, 1, LEFT_LINE);
	
	MusNeumePitch *temp;
	for (unsigned int i = 0; i < n_pitches.size(); i++)
	{
		temp = n_pitches.at(i);
		ynn = staff->y_note((int)temp->code, staff->testcle( this->xrel), temp->oct - 4);
		ynn += staff->yrel;
		//ledger line
		
		if (i < n_pitches.size() - 1)
			this->neume_line(dc, staff, i, i+1, RIGHT_LINE);

		//punctum
		m_w->putfont(dc, this->xrel + (i * 10), ynn + 16, temp->getPunctumType(), staff,
					 this->dimin, NEUME);

		
	}
	
	
	//left line always spans at least two pitches
//	
//	this->neume_line(dc, staff, this->n_pitches.at(0), this->n_pitches.at(1), LEFT_LINE);
//	this->neume_line(dc, staff, this->n_pitches.at(0), this->n_pitches.at(1), RIGHT_LINE);
}

void MusNeume::podatus( wxDC *dc, MusStaff *staff ) {
	int pTaille = staff->pTaille;
	
	int oct = this->oct - 4;
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	int ynn = this->dec_y + staff->yrel; 
	int ynn2;
	
	
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

// start_pitch and end_pitch are indexes of pitches inside the n_pitches array

void MusNeume::neume_line( wxDC *dc, MusStaff *staff, int start_pitch, 
						  int end_pitch, int side)
{
	
	if (start_pitch > n_pitches.size() - 1 || end_pitch > n_pitches.size() - 1) 
		return;
	
	// line needs to be at least 2 pitches in length always
	MusNeumePitch *p1 = (MusNeumePitch*)n_pitches.at(start_pitch);
	MusNeumePitch *p2 = (MusNeumePitch*)n_pitches.at(end_pitch);	
	
	int ynn = staff->y_note((int)p1->code, staff->testcle(this->xrel), p1->oct - 4) 
		+ staff->yrel; 
	int ynn2 = staff->y_note((int)p2->code, staff->testcle(this->xrel), p2->oct - 4) 
		+ staff->yrel; 
	
	
	
	wxPen pen( *(m_w->m_currentColour), m_w->ToZoom(2), wxSOLID);
	dc->SetPen( pen );
	
	
	// MAGIC NUMBERS!!!!
	
	dc->DrawLine(m_w->ToZoom(this->xrel + side + (start_pitch * 9)), 
				 m_w->ToZoomY(ynn), 
				 m_w->ToZoom(this->xrel + side + (start_pitch * 9)), 
				 m_w->ToZoomY(ynn2));
	dc->SetPen(wxNullPen);
	
}

void MusNeume::porrectus( wxDC *dc, MusStaff *staff )
{
	int pTaille = staff->pTaille;
	
	int oct = this->oct - 4;
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	int ynn = this->dec_y + staff->yrel; 
	int ynn2;
	
	
	int ledge = m_w->ledgerLine[pTaille][2];
	
	int punct_y;
	
	MusNeumePitch *temp = this->n_pitches.at(0);
}