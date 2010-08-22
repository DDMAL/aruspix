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

// more helper functions

// returns true of p1 is lower than p2
bool MusNeume::ascending(int p1, int p2) 
{ return (this->n_pitches.at(p1)->Compare(this->n_pitches.at(p2)) == -1); }

// returns true if p1 is higher than p2
bool MusNeume::descending(int p1, int p2)
{ return !ascending(p1, p2); }


// each "basic" type of ligature will have custom drawing code defined here


void MusNeume::drawLigature( wxDC *dc, MusStaff *staff ) {
	
	//filter pes
	int i;
//	for (i = 1, iter = n_pitches.begin()+1; iter < n_pitches.end(); iter++, i++) {
	if (ascending(0, 1) && n_pitches.size() == 2) {
//		if (i == n_pitches.size() - 1) {
		//	printf("We have a pes!\n");
		this->podatus(dc, staff);
		this->n_type = UNEUME;
		this->name = PODAT;
		return;
	}
//	}
	
	//filter porrectus
//	for (i = 1, iter = n_pitches.begin()+1; iter < n_pitches.end(); iter++, i++) {
	if (n_pitches.size() == 3 && descending(0, 1) && ascending(1,2) )
	{
		if (n_pitches.at(0)->Pitch_Diff(n_pitches.at(1)) >= -4 ) {
			this->porrectus(dc, staff);
			this->n_type = UNEUME;
			this->name = PRECT;
			return;
		}
	}
//	}
	//draw clivis by default
	this->clivis(dc, staff);
	this->n_type = UNEUME;
	this->name = CLVIS;
}

//filter porrectus


//filter climacus


#pragma mark Drawing Code

//need to optimize these drawing methods later

void MusNeume::clivis( wxDC *dc, MusStaff *staff ) {
	int ynn;
	int pTaille = staff->pTaille;
	int bby = staff->yrel - m_w->_portee[pTaille];  // bby= y sommet portee
	
	int x_spacing = 0;
	//placeholder for festa dies strings
	wxString str = "";
	
	MusNeumePitch *temp;
	int xrel_curr = this->xrel; // keep track of where we are on the x axis
	for (unsigned int i = 0; i < n_pitches.size(); i++)
	{
		temp = n_pitches.at(i);
		ynn = staff->y_note((int)temp->code, staff->testcle( this->xrel), temp->oct - 4);
		ynn += staff->yrel;
		//ledger line
		int ledge = m_w->ledgerLine[pTaille][2];
		
		//super hack way to get some strophicus/bivirga stuff happening
		
		if (i) {
			if (temp->Pitch_Diff((n_pitches.at(i-1))->code, 
								 (n_pitches.at(i-1))->oct)) {
				x_spacing = 10;
			} else x_spacing = 15; //same pitch
		}
		
		leg_line( dc, ynn,bby,xrel_curr + x_spacing,ledge, pTaille);		
		
		if (i < n_pitches.size())
		m_w->festa_string(dc, xrel_curr + x_spacing, ynn + 16, 
						  temp->getFestaString() , staff, this->dimin);
		xrel_curr += x_spacing;
	}
	this->xrel_right = xrel_curr + PUNCT_WIDTH;
	//draw debug line to make sure were in the right spot
}

void MusNeume::podatus( wxDC *dc, MusStaff *staff ) {
	int pTaille = staff->pTaille;
	
	int oct = this->oct - 4;
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	int ynn = this->dec_y + staff->yrel; 
	int bby = staff->yrel - m_w->_portee[pTaille];  // bby= y sommet portee
	int ledge = m_w->ledgerLine[pTaille][2];
	
	int punct_y;
	MusNeumePitch *temp = this->n_pitches.at(0);
	m_w->festa_string(dc, this->xrel, ynn + 16, (char)nPES,
				 staff, this->dimin );
	temp = this->n_pitches.at(1);
	
	punct_y = staff->y_note((int)temp->code, staff->testcle( this->xrel ), temp->oct - 4);
	int ynn2 = punct_y + staff->yrel; 
	
	//ledger lines
	leg_line(dc, ynn, bby, this->xrel, ledge, pTaille);
	leg_line(dc, ynn2, bby, this->xrel, ledge, pTaille);
	
	if (n_pitches.at(0)->Pitch_Diff(n_pitches.at(1)) > 2 )
		m_w->festa_string(dc, this->xrel, ynn2 + 16, 
						  nVIRGA, staff, this->dimin );
	else 
		m_w->festa_string(dc, this->xrel, ynn2 + 16, 
						  (char)nPUNCTUM, staff, this->dimin );
	this->xrel_right = this->xrel + PUNCT_WIDTH;
}

// start_pitch and end_pitch are indexes of pitches inside the n_pitches array


// alternative line method that uses festa dies

//void MusNeume::neume_line( wxDC *dc, MusStaff *staff, int side)
//{
//	
////	if (start_pitch > n_pitches.size() - 1 || end_pitch > n_pitches.size() - 1) 
////		return;
////	
////	//start_pitch and end_pitch determintes the vertical length of the line
////	
////	// line needs to be at least 2 pitches in length always
////	MusNeumePitch *p1 = (MusNeumePitch*)n_pitches.at(start_pitch);
////	MusNeumePitch *p2 = (MusNeumePitch*)n_pitches.at(end_pitch);	
////	
//	int oct = this->oct - 4;
//	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
//	int ynn = this->dec_y + staff->yrel; 
//	
//	if (side == LEFT_LINE)
//		m_w->putfont(dc, this->xrel, ynn, nLEFTLINE, staff, this->dimin, NEUME);
//	else m_w->putfont(dc, this->xrel, ynn, nRIGHTLINE, staff, this->dimin, NEUME);
//}

void MusNeume::porrectus( wxDC *dc, MusStaff *staff )
{

	int oct = this->oct - 4;
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	int ynn = this->dec_y + staff->yrel; 

	wxString str = "";
	
	int porrect_type;
	switch(abs(n_pitches.at(0)->Pitch_Diff(n_pitches.at(1)))) {
		case 1:
			porrect_type = nPORRECT_1; //works
			break;
		case 2:
			porrect_type = nPORRECT_2; //doesn't work
			break;
		case 3:
			porrect_type = nPORRECT_3; //doesn't work
			break;
//		case 4:
//			porrect_type = nPORRECT_4; //works
//			break;
		default:
			printf("Invalid pitch range: %d\n", 
				   abs(n_pitches.at(0)->Pitch_Diff(n_pitches.at(1))));
			return; 
	}
	str.Append("3");
	str.Append((char)porrect_type);

	// ledger lines
	int pTaille = staff->pTaille;
	int bby = staff->yrel - m_w->_portee[pTaille];  // bby= y sommet portee
	int ledge = m_w->ledgerLine[pTaille][2];
	
	leg_line( dc, ynn,bby, this->xrel,ledge, pTaille);		
	m_w->festa_string(dc, this->xrel, ynn + 16, str, staff, this->dimin);
	
	//the right edge of the porrect character is tricky.. use GetTextExtent
	wxSize size = dc->GetTextExtent(str);
	this->xrel_right = this->xrel + size.GetX();
	
	//figure out where to draw the last punctum
//	MusNeumePitch *temp = n_pitches.at(2);
//	str = temp->m_font_str;
//	
//	ynn = staff->y_note((int)temp->code, staff->testcle( this->xrel ), temp->oct - 4);
//	ynn += staff->yrel; 
//						
//	if (str.IsSameAs(nPUNCTUM, true)) {
//		m_w->festa_string(dc, m_w->ToZoom(this->xrel_right - 15), 
//						  ynn + 16, str, staff, this->dimin);
//	}
}