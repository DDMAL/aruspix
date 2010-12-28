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
#include "musrc.h"

using std::string;

// more helper functions

// returns true of p1 is lower than p2
bool MusNeume::ascending(int p1, int p2) 
{ return (this->n_pitches.at(p1)->Compare(this->n_pitches.at(p2)) == -1); }

// returns true if p1 is higher than p2
bool MusNeume::descending(int p1, int p2)
{ 
	if (!n_pitches.at(p1)->Pitch_Diff(n_pitches.at(p2))) return false;
	else return !ascending(p1, p2); 
}


// each "basic" type of ligature will have custom drawing code defined here


void MusNeume::drawLigature( AxDC *dc, MusStaff *staff ) {
	
	//filter pes
	if (ascending(0, 1) && n_pitches.size() == 2) {
		//	printf("We have a pes!\n");
		this->podatus(dc, staff);
		this->n_type = UNEUME;
//		this->name = PODAT;
		return;
	}
	
	//filter porrectus
	if (n_pitches.size() == 3 && descending(0, 1) && ascending(1,2) )
	{
		if (n_pitches.at(0)->Pitch_Diff(n_pitches.at(1)) >= -2 ) {
			this->porrectus(dc, staff);
			this->n_type = UNEUME;
//			this->name = PRECT;
			return;
		}
	}
	//draw clivis by default
	this->clivis(dc, staff);
	this->n_type = UNEUME;
//	this->name = CLVIS;
}

//filter porrectus


//filter climacus


#pragma mark Drawing Code

//need to optimize these drawing methods 

//stems


// note: the pitch_range refers to the 'direction' that the pitches would
// be sung. 
// e.g. For a clivis, the leading pitch_range would be 2, as if the first punctum
// was approached from below. 
// In reality, stems are drawn from the punctum outward (punctum as origin)

void MusNeume::neume_stem( AxDC *dc, MusStaff *staff, int xrel, int index,
						  int pitch_range, int side ) 
{
	int ynn;
	wxString str = "";
	
	printf("drawing stems! pitch range: %d\n", pitch_range);
	
	// first figure out which line to print
	if (pitch_range > 0) {
		str.append(sizeof(char),(char)(side == LEFT_STEM ? nSTEM_B_LEFT : nSTEM_B_RIGHT));
	} else if (pitch_range < 0) {
		str.append(sizeof(char),(char)(side == LEFT_STEM ? nSTEM_T_LEFT : nSTEM_T_RIGHT));
	} else return; // otherwise, pitch difference is zero. Do nothing.

//	int pitch = abs_pitch((int)temp->code, temp->oct -4);
	printf("str : %s\n", str.c_str());

	printf("need to draw %d stems. ", abs(pitch_range / 2));
	if (pitch_range > 0) printf("upwards\n");
	else printf("downwards\n");
	
	// however, we must 'invert' the 'stem vector' because the punctum is
	// the origin. stems in festa dies will be drawn outwards from the punctum

	// get the pitch we're trying to build a stem for
	MusNeumePitch *temp = new MusNeumePitch(*(n_pitches.at(index)));
	int pitch = abs_pitch(temp->code, temp->oct);
	for (int i = 0; i < abs(pitch_range / 2); i++) {
		ynn = staff->y_note((int)temp->code, staff->testcle( this->xrel), temp->oct - 4);
		ynn += staff->yrel;

		printf("||||||||||||||||||||||||||||||||Drawing line\n");
		m_r->festa_string(dc, xrel, ynn + 16, str, staff, this->dimin);
		//update ynn value
		if (pitch_range > 0) pitch -= 2;
		else pitch += 2;
		temp->SetPitch(abs2code(pitch), abs2oct(pitch));
	}
	delete temp;
}

void MusNeume::clivis( AxDC *dc, MusStaff *staff ) {
	int ynn;
	int pTaille = staff->pTaille;
	int bby = staff->yrel - m_r->_portee[pTaille];  // bby= y sommet portee
	
	int x_spacing = 0;
	//placeholder for festa dies strings
	wxString str = "";

	int xrel_curr = this->xrel; // keep track of where we are on the x axis
	MusNeumePitch *temp;
	for (unsigned int i = 0; i < n_pitches.size(); i++)
	{
		temp = n_pitches.at(i);
		ynn = staff->y_note((int)temp->code, staff->testcle( this->xrel), temp->oct - 4);
		ynn += staff->yrel;
		//ledger line
		int ledge = m_r->ledgerLine[pTaille][2];
		
		//super hack way to get some strophicus/bivirga stuff happening
		//also take care of 'connecting lines'
		
		if (i) {
			if (temp->Pitch_Diff((n_pitches.at(i-1))->code, 
								 (n_pitches.at(i-1))->oct)) {
				x_spacing = CLIVIS_X_DIFF;
			} else x_spacing = CLIVIS_X_SAME; //same pitch
			xrel_curr += x_spacing;
			neume_stem(dc, staff, xrel_curr, i, 
					   n_pitches.at(i-1)->Pitch_Diff(temp), LEFT_STEM);
		} else { // first pitch
			if (descending(0, 1))	
				neume_stem(dc, staff, xrel_curr, i, 2);
		}

		leg_line( dc, ynn,bby,xrel_curr,ledge, pTaille);		
		
		if (i < n_pitches.size())
		m_r->festa_string(dc, xrel_curr, ynn + 16, 
						  temp->getFestaString() , staff, this->dimin);

	}
	this->xrel_right = xrel_curr + PUNCT_WIDTH;
	//draw debug line to make sure were in the right spot
}

void MusNeume::podatus( AxDC *dc, MusStaff *staff ) {
	int pTaille = staff->pTaille;
	
	int oct = this->oct - 4;
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	int ynn = this->dec_y + staff->yrel; 
	int bby = staff->yrel - m_r->_portee[pTaille];  // bby= y sommet portee
	int ledge = m_r->ledgerLine[pTaille][2];
	
	int punct_y;
	MusNeumePitch *temp = this->n_pitches.at(0);
	m_r->festa_string(dc, this->xrel, ynn + 16, wxString((char)nPES),
				 staff, this->dimin );
	temp = this->n_pitches.at(1);
	
	punct_y = staff->y_note((int)temp->code, staff->testcle( this->xrel ), temp->oct - 4);
	int ynn2 = punct_y + staff->yrel; 
	
	//ledger lines
	leg_line(dc, ynn, bby, this->xrel, ledge, pTaille);
	leg_line(dc, ynn2, bby, this->xrel, ledge, pTaille);

	// for the different type of 'upper punctums'...
	// stem is drawn upwards from the base pitch (pes or punctum)
	
	neume_stem(dc, staff, this->xrel + PUNCT_WIDTH, 1, n_pitches.at(0)->Pitch_Diff(n_pitches.at(1)),
			   RIGHT_STEM);
		
	int xrel_curr = this->xrel; // keep track of where we are on the x axis
	switch (temp->GetValue()) {
			//draw to the right
		case 2:					//nCEPHALICUS
		case 3:					//nPUNCT_UP (upwards auctae)
			xrel_curr += PUNCT_WIDTH;
			m_r->festa_string(dc, xrel_curr, ynn2 + 16, temp->m_font_str, 
							  staff, this->dimin);
			break;
			// not possible; break the neume?
		default:				//draw above, all cases treated as square punctum
			m_r->festa_string(dc, xrel_curr, ynn2 + 16, wxString((char)nPUNCTUM),
							  staff, this->dimin);
			break;
	}
	
	// for use later with compound neumes
	this->xrel_right = this->xrel + xrel_curr;
}

void MusNeume::porrectus( AxDC *dc, MusStaff *staff )
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
			porrect_type = nPORRECT_2; //works
			break;
		case 3:
//			porrect_type = nPORRECT_3; //doesn't work
			break;
		case 4:
//			porrect_type = nPORRECT_4; //doesn't work 
			break;
		default:
			printf("Invalid pitch range: %d\n", 
				   abs(n_pitches.at(0)->Pitch_Diff(n_pitches.at(1))));
			return; 
	}
	str.append("3");
	str.append(sizeof(char),(char)porrect_type);

	// ledger lines
	int pTaille = staff->pTaille;
	int bby = staff->yrel - m_r->_portee[pTaille];  // bby= y sommet portee
	int ledge = m_r->ledgerLine[pTaille][2];
	
	leg_line( dc, ynn,bby, this->xrel,ledge, pTaille);		
	m_r->festa_string(dc, this->xrel, ynn + 16, str, staff, this->dimin);
	
	//the right edge of the porrect character is tricky.. use GetTextExtent
//	wxSize size = dc->GetTextExtent(str);

// TODO: watch this!!!!!	
	
//	this->xrel_right = this->xrel + size.GetX();
	//debug stem
//	neume_stem(dc, staff, this->xrel_right, 0, 2, LEFT_STEM);
	
	// PITCH 3
	//figure out where to draw the last punctum
	MusNeumePitch *temp = n_pitches.at(2);
	str = temp->m_font_str;
	
	ynn = staff->y_note((int)temp->code, staff->testcle( this->xrel ), temp->oct - 4);
	ynn += staff->yrel;

	

	
	//eventually check for open-closed mode change
	int xrel_curr = this->xrel + PUNCT_WIDTH;
	if (this->next == NULL) {
		switch (temp->GetValue()) {
				//draw to the right
			case 2:					//nCEPHALICUS
			case 3:					//nPUNCT_UP (upwards auctae)
	// draw stem if necessary
				xrel_curr += PUNCT_PADDING;
				neume_stem(dc, staff, xrel_curr, 2, n_pitches.at(1)->Pitch_Diff(temp)
						   , LEFT_STEM);
				m_r->festa_string(dc, xrel_curr, 
								  ynn + 16, temp->m_font_str, staff, this->dimin);
				break;
			default:
				xrel_curr += PUNCT_PADDING;
				neume_stem(dc, staff, xrel_curr, 2, n_pitches.at(1)->Pitch_Diff(temp)
						   , RIGHT_STEM);
				m_r->festa_string(dc, xrel_curr - PUNCT_WIDTH, ynn + 16, 
								  wxString((char)nPUNCTUM),staff, this->dimin);
		}
	}
	
}
