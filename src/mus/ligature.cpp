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

bool MusNeume::ascending(int p1, int p2) 
{ 
//	printf("Pitch 1: %d, %d\nPitch 2: %d, %d\n", n_pitches.at(p1)->code, 
//		   n_pitches.at(p1)->oct, n_pitches.at(p2)->code, n_pitches.at(p2)->oct);
	return (this->n_pitches.at(p1)->Compare(this->n_pitches.at(p2)) == -1); 
}

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
	for (i = 1, iter = n_pitches.begin()+1; iter < n_pitches.end(); iter++, i++) {
		if (n_pitches.size() == 3 && descending(0, 1) && ascending(1,2) )
		{
			this->porrectus(dc, staff);
			this->n_type = UNEUME;
			this->name = PRECT;
			return;
		}
	}

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
	
	if (n_pitches.size() >= 2 && 
		((MusNeumePitch*)n_pitches.at(0))->Pitch_Diff((MusNeumePitch*)n_pitches.at(1)) < 0)
//		this->neume_line(dc, staff, 0, 1, LEFT_LINE);
		this->neume_line(dc, staff, LEFT_LINE);	
	MusNeumePitch *temp;
	for (unsigned int i = 0; i < n_pitches.size(); i++)
	{
		temp = n_pitches.at(i);
		ynn = staff->y_note((int)temp->code, staff->testcle( this->xrel), temp->oct - 4);
		ynn += staff->yrel;
		//ledger line
		
		if (i < n_pitches.size())
		m_w->festa_string(dc, this->xrel + (i * 10), ynn + 16, 
						  "s*", staff, this->dimin);
	}
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
	m_w->festa_string(dc, this->xrel, ynn + 16, nPES,
				 staff, this->dimin );
	temp = this->n_pitches.at(1);
	
	punct_y = staff->y_note((int)temp->code, staff->testcle( this->xrel ), temp->oct - 4);
	ynn2 = punct_y + staff->yrel; 
	
	if (n_pitches.at(0)->Pitch_Diff(n_pitches.at(1)) > 2 )
		m_w->festa_string(dc, this->xrel, ynn2 + 16, 
						  nVIRGA, staff, this->dimin );
	else 
		m_w->festa_string(dc, this->xrel, ynn2 + 16, 
						  nPUNCTUM, staff, this->dimin );
}

// start_pitch and end_pitch are indexes of pitches inside the n_pitches array


// alternative line method that uses festa dies

void MusNeume::neume_line( wxDC *dc, MusStaff *staff, int side)
{
	
//	if (start_pitch > n_pitches.size() - 1 || end_pitch > n_pitches.size() - 1) 
//		return;
//	
//	//start_pitch and end_pitch determintes the vertical length of the line
//	
//	// line needs to be at least 2 pitches in length always
//	MusNeumePitch *p1 = (MusNeumePitch*)n_pitches.at(start_pitch);
//	MusNeumePitch *p2 = (MusNeumePitch*)n_pitches.at(end_pitch);	
//	
	int oct = this->oct - 4;
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	int ynn = this->dec_y + staff->yrel; 
	
	if (side == LEFT_LINE)
		m_w->putfont(dc, this->xrel, ynn, nLEFTLINE, staff, this->dimin, NEUME);
	else m_w->putfont(dc, this->xrel, ynn, nRIGHTLINE, staff, this->dimin, NEUME);
}


void MusNeume::porrectus( wxDC *dc, MusStaff *staff )
{
	int oct = this->oct - 4;
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	int ynn = this->dec_y + staff->yrel; 

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
		case 4:
			porrect_type = nPORRECT_4; //works
			break;
		default:
			printf("Invalid pitch range: %d\n", 
				   abs(n_pitches.at(0)->Pitch_Diff(n_pitches.at(1))));
			return; 
	}
	
	//front line, replace this with festa dies
//	this->neume_line(dc, staff, 0, 2, LEFT_LINE);	
	this->neume_line(dc, staff, LEFT_LINE);		
	m_w->festa_string(dc, this->xrel, ynn + 16, porrect_type, staff,
				 this->dimin);
//	m_w->festa_string(dc, this->xrel, ynn + 
//	n_pitches.at(2)->SetValue(5); //final pitch is a virga
}