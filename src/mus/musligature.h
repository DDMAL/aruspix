/*
 *  MusLigature.h
 *  aruspix
 *
 *  Created by Chris Niven on 10-07-15.
 *  Copyright 2010 com.aruspix.www. All rights reserved.
 *
 */


// void vs. bool? 

#ifndef __MUS_NEUME_H__
#define __MUS_NEUME_H__

#ifdef __GNUG__
#pragma interface "MusLigature.cpp"    
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/dynarray.h"

#include "muselement.h"
#include "musneume.h"
#include <vector>
#include "neumedef.h"

//TODO: EXTREME REFACTORING IN PROGRESS 07/15/2010

// MusLigature interface

class MusLigature {
public:
	MusLigature();
	MusLigature( const MusLigature& MusLigature );
	MusLigature( MusNeume* _neume );
	MusLigature& operator=( const MusLigature& MusLigature );
	virtual ~MusLigature() {}
	virtual void Attach();
	virtual void Draw();	// already defined in MusNeume.h, override
	virtual bool IsPossible(); // filter
	
	// Break() defined in MusNeume.h
	
	std::vector<MusLigature*> *musLigatures = super->musLigatures;
	//next and previous pitches are implicit
	std::vector<MusNeumePitch*> n_pitches; 
	
	//pointers for convenience (for drawing code)
	MusStaff *staff = super->staff;
	AxDC *dc = super->dc;
}

class Clivis: public MusLigature
{
public:
	Clivis() {}
	//override drawing code
	void Draw() {
		if (!staff || !dc) return;
		
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
			m_r->putfont(dc, this->xrel + (i * 10), ynn + 16, temp->getPunctumType(), staff,
						 this->dimin, NEUME);
		}
	}
	//override attach	
	void Attach() {}	
	bool IsPossible() { return true; } // clivis is always possible
};

class Climacus: public MusLigature
{
public:
	Climacus() {}
	//override drawing code	
	void Draw() {
		if (!staff || !dc) return;
		
		
	}
	//override attach	
	void Attach() {}	
	bool IsPossible() {} 	
};

class Porrectus: public MusLigature
{
public:
	Porrectus() {}
	//override drawing code
	void Draw() {
		if (!staff || !dc) return;	
		int pTaille = staff->pTaille;
		
		int oct = this->oct - 4;
		this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
		int ynn = this->dec_y + staff->yrel; 
		int ynn2;
		
		
		int ledge = m_r->ledgerLine[pTaille][2];
		
		int punct_y;
		
		MusNeumePitch *temp = this->n_pitches.at(0);
	}
	//override attach
	void Attach() {}	
	bool IsPossible() {} 	
};

class Podatus: public MusLigature
{
public:
	Podatus() {}
	//override drawing code	
	void Draw() {
		if (!staff || !dc) return;		
		int pTaille = staff->pTaille;
		
		int oct = this->oct - 4;
		this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
		int ynn = this->dec_y + staff->yrel; 
		int ynn2;
		
		int ledge = m_r->ledgerLine[pTaille][2];
		
		int punct_y;
		MusNeumePitch *temp = this->n_pitches.at(0);
		m_r->putfont(dc, this->xrel - 5, ynn + 16, nPES,
					 staff, this->dimin, NEUME);
		temp = this->n_pitches.at(1);
		
		punct_y = staff->y_note((int)temp->code, staff->testcle( this->xrel ), temp->oct - 4);
		ynn2 = punct_y + staff->yrel; 
		
		m_r->putfont(dc, this->xrel - 5, ynn2 + 16, temp->getPunctumType(),
					 staff, this->dimin, NEUME);
		
		//draw a line connecting the two now
		wxPen pen( *(m_r->m_currentColour), m_r->ToZoom(2), wxSOLID);
		dc->SetPen( pen );
		dc->DrawLine(m_r->ToZoom(this->xrel + 4), m_r->ToZoomY(ynn), 
					 m_r->ToZoom(this->xrel + 4), m_r->ToZoomY(ynn2));
		dc->SetPen(wxNullPen);
	}
	//override attach
	void Attach() {}
	bool IsPossible() {} 	
};

