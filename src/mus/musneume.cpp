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
#include "neumedef.h"

#include <math.h>
using namespace std;

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
// MusNeumePitch
//----------------------------------------------------------------------------

// we don't want this constructor called, pretty much ever. Need to 
// get rid of this eventually, (although it is called at app launch by default)

MusNeumePitch::MusNeumePitch() 
{
	oct = 0;
	code = 0;
	val = 0;
}

MusNeumePitch::MusNeumePitch(int code, int oct, unsigned char val) 
{
	this->code = code;
	this->oct = oct;
	this->val = val;
}

MusNeumePitch::MusNeumePitch( const MusNeumePitch& pitch ) 
{
	this->code = pitch.code;
	this->oct = pitch.oct;
	this->val = pitch.val;
}

MusNeumePitch& MusNeumePitch::operator=( const MusNeumePitch& pitch )
{
	if ( this != &pitch ) 
	{
		printf("we're copying properly\n");
		(MusNeumePitch&)*this = pitch;
		this->code = pitch.code;
		this->oct = pitch.oct;
		this->val = pitch.val;
	} printf("not copying properly :(:(:(:(:(\n");
	return *this;
}

void MusNeumePitch::SetPitch( int code, int oct )
{
	this->code = code;
	this->oct = oct;
//	printf("Changing pitch: c: %d, o: %d\n", this->code, this->oct);
}

void MusNeumePitch::SetValue( int value ) { this->val = value; }

int MusNeumePitch::GetValue() { return this->val; }

//helper method

int MusNeumePitch::getPunctumType( ) 
{
	switch (this->val) {
		case 0:
			return nPUNCTUM;
			break;
		case 1:
			return nDIAMOND;
			break;
		case 2:
			return nCEPHALICUS;
			break;
		case 3:
			return nPODATUS;
			break;
			//		case 4:
			//		fontNo = nDIAMOND_SMALL;		//small diamonds don't draw correctly for some reason
			//		break;
		case 4:
			return nQUILISMA;
			break;
		default:
			return 0;
			break;
	}
}
// compare pitches
int MusNeumePitch::Compare(MusNeumePitch *other)
{
	int a = this->code + (this->oct * 7);
	int b = other->code + (other->oct * 7);

	if (a < b) return -1;
	else if (a == b) return 0;
	else return 1;
}

//returns an int value of the difference between pitches
//musnote doesn't need to use this, so no need to make a macro
int MusNeumePitch::Pitch_Diff(MusNeumePitch *other) 
{
	return abs_pitch(other->code, other->oct) - abs_pitch(this->code, this->oct);
}

int MusNeumePitch::Pitch_Diff(int code, int oct)
{
	return abs_pitch(code, oct) - abs_pitch(this->code, this->oct);
}


//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

//constructors are only called when brand new neumes are created. 
// otherwise, pitches are just added to existing neumes through the append() method.


// this is called at application launch, before anything is drawn

MusNeume::MusNeume():
	MusElement()
{
	
	TYPE = NEUME;
	closed = true; 
	n_selected = 0; // instantiation of class always creates a single note
	//set initial pitch, for entire neume as well as the first NPitch element
	MusNeumePitch *firstPitch = new MusNeumePitch();
	n_pitches.push_back(firstPitch);
	code = 0;
	p_range = p_min = p_max = 0;
	
}

MusNeume::MusNeume( unsigned char _val, unsigned char _code )
{
	TYPE = NEUME;
	closed = true;
	n_selected = 0;
	MusNeumePitch *firstPitch = new MusNeumePitch( _code, 0, _val);
	n_pitches.push_back(firstPitch);
	code = _code;	
	p_range = p_min = p_max = 0;
}

// this constructor is called first upon insert

MusNeume::MusNeume( const MusNeume& neume )
	: MusElement( neume )
{
	TYPE = neume.TYPE;
	this->closed = true;	//all neumes are closed by default
	this->n_selected = neume.n_selected;
	

//	printf("This is the address of the original pitch list: %d\n", (int)&(this->n_pitches));
//	printf("This is the address of the new pitch list: %d\n", (int)&(neume.n_pitches));
//	//this is supposed to deep copy, but it doesn't for some strange reason	
//	this->n_pitches = neume.n_pitches;
//	printf("*************************************************\n");
//	printf("NOW, this is the address of the original pitch list: %d\n", (int)&(this->n_pitches));
//	printf("This is the address of the new pitch list: %d\n", (int)&(neume.n_pitches));
//	
	
	//pitch list is instantiated — not copied (for now)

	this->n_pitches.empty();
	
	MusNeumePitch *firstPitch = new MusNeumePitch();
	n_pitches.push_back(firstPitch);
	
	this->SetPitch(code, oct);
	
	this->p_range = neume.p_range;
	this->p_max = neume.p_max;
	this->p_min = neume.p_min;
	
	this->printNeumeList();
}


//crash caused by this constructor after inserting second neume element

MusNeume& MusNeume::operator=( const MusNeume& neume )
{
	if ( this != &neume ) // not self assignement
	{
		// For base class MusElement copy assignement
		(MusElement&)*this = neume;                  //find out if this was here for a reason!!! sa

		TYPE = neume.TYPE;
		this->closed = true; //all neumes are closed by default
		this->n_selected = neume.n_selected;
		//this->n_pitches = neume.n_pitches;
		
		//pitch list is instantiated — not copied (for now)
		this->n_pitches.empty();
		
		MusNeumePitch *firstPitch = new MusNeumePitch();
		n_pitches.push_back(firstPitch);
		
		
//		printf("This is the address of the original pitch list: %d\n", (int)&(this->n_pitches));
//		printf("This is the address of the new pitch list: %d\n", (int)&(neume.n_pitches));
//		
//		this->n_pitches = neume.n_pitches;
//		printf("*************************************************\n");
//		printf("NOW, this is the address of the original pitch list: %d\n", (int)&(this->n_pitches));
//		printf("This is the address of the new pitch list: %d\n", (int)&(neume.n_pitches));
//		
		
//		std::vector<MusNeumePitch*> temp (neume.n_pitches.size());
//		
//		printf("^^^^^^^^^^^VECTOR SIZE: %d\n", temp.size());
		
//		std::vector<MusNeumePitch*> temp;
//		//super hack-y
//		for (iter=n_pitches.begin(); iter != n_pitches.end(); ++iter)
//		{
//			temp.push_back(new MusNeumePitch((*iter)->code, (*iter)->oct, (*iter)->val));
//		}
//		this->n_pitches = temp;
		
		this->SetPitch(code, oct);
		this->p_range = neume.p_range;
		this->p_max = neume.p_max;
		this->p_min = neume.p_min;
	}
	
	this->printNeumeList();
	
	return *this;
}

MusNeume::~MusNeume()
{	
	//we need to delete all the NPitch elements
	n_pitches.empty();
	//not sure if this is all we need
}

//might have to expand on this? probably not though

bool MusNeume::IsClosed() { return this->closed; }

void MusNeume::SetClosed(bool value) {
	this->closed = value;
	
	
	if (this->closed) {
		n_selected = 0;
		//break up neumes if there are repeated pitches
		this->CheckForBreaks();
		wxClientDC dc(m_w);
		this->drawLigature(&dc, m_w->m_currentStaff);
	}
	
	if (m_w)
	{
	//	printf("Hderp!\n");
		m_w->Refresh();
	}
	
}

void MusNeume::CheckForBreaks()
{
	iter = n_pitches.begin();
	for (unsigned int i = 0; i < n_pitches.size() - 1; i++, iter++)
	{
		if (!((*iter)->Pitch_Diff(n_pitches.at(i+1))))
		{
			this->Split(i+1);
			return;
		}
	}
}

void MusNeume::Split(int pos) {
	printf("Splitting Neume\n");
	MusNeume *split = new MusNeume();
	split->n_pitches = this->n_pitches;
	this->n_pitches.resize(pos);
	
	split->n_pitches.erase(split->n_pitches.begin(), split->n_pitches.begin()+pos);
	
	MusNeumePitch *temp = this->n_pitches.at(pos - 1);
	split->SetPitch(temp->code, temp->oct);
	split->xrel = this->xrel + PUNCT_PADDING;
	
	
	if (m_w)
		m_w->Refresh();
}

// if open, returns next individual pitch
// if closed, return false and let musstaff select the next element
bool MusNeume::GetNext() { 
	//printf("n_selected: %d size: %d\n", n_selected, n_pitches.size());
	if (closed) return false;
	else 
	{
		if (n_selected < n_pitches.size() - 1) 
		{
			n_selected++;
			//refresh window or call drawing code
			//printf("Moving to pitch %d.\n", n_selected);
		} else if (n_selected == n_pitches.size() - 1)
		{
			//printf("At end (n_selected: %d). Closing neume.\n", n_selected);
			this->SetClosed(true);
			return false;
		}
		return true;
	}
}
//
bool MusNeume::GetPrevious() {
	if (closed) return false;
	else 
	{
		if (n_selected > 0) 
		{
			n_selected--;
			//refresh window or call drawing code
			//printf("Moving to pitch %d.\n", n_selected);
		} else if (!n_selected)
		{
			//printf("At beginning (n_selected: %d). Closing neume.\n", n_selected);
			this->SetClosed(true);
			return false;
		}
		return true;	//stay in 'individual edit mode' since the neume is open
	}
}

void MusNeume::Append() {
	// add a new pitch with the same value as the previous
	
	if (this->IsClosed()) return; //can only append pitches in open mode
	
	////printf("************************************* printing neumelist DEBUG\n");
	//printNeumeList();
	
	//using vector::end() causes this to crash, use vector::back() instead
	
	MusNeumePitch *new_pitch = new MusNeumePitch(*(n_pitches.back())); 
	n_pitches.push_back(new_pitch);
	
	n_selected = n_pitches.size() - 1;
	
	this->GetPitchRange();
	
	if (m_w)
		m_w->Refresh();
	
}

void MusNeume::InsertPitchAfterSelected()
{
	if (this->IsClosed()) return; //can only insert pitches in open mode
	// shouldn't need this since we're using n_selected as the index
	//	if (index <= 0 || index >= n_pitches.size()) return;
	
	MusNeumePitch *new_pitch;
	
	//insert at front
	iter = n_pitches.begin();
//	if (n_selected == n_pitches.size() - 1) { //append pitch at end
//		iter = n_pitches.back(); 
//	} else { //insert in middle
		for (unsigned int i = 0; i < n_selected; i++, iter++); //iterate to selected element
//	}

	
	
	new_pitch = new MusNeumePitch((*iter)->code, (*iter)->oct, (*iter)->val);
	n_pitches.insert(iter, new_pitch);
	n_selected++;
	
	this->GetPitchRange();
	
	if (m_w)
		m_w->Refresh();
	
}

void MusNeume::RemoveSelectedPitch()
{
	if (this->IsClosed()) return; //can only remove pitches in open mode
	
	if (n_pitches.size() == 1) return; //removing last node crashes aruspix
	// cannot remove last node (for now)
	
	iter = n_pitches.begin();
	for (unsigned int i = 0; i < n_selected; i++, iter++);
	n_pitches.erase(iter);
	
	if (n_selected) n_selected--;
	
	this->GetPitchRange();
	if (m_w)
		m_w->Refresh();
}


// superhack helper methods 

int MusNeume::GetCode()
{
	if (this->IsClosed()) 
		return this->code; 
	else return n_pitches.at(n_selected)->code;
}

int MusNeume::GetOct()
{
	if (this->IsClosed())
		return this->oct;
	else return n_pitches.at(n_selected)->oct;
}

// I'm sure this could be optimized

void MusNeume::SetPitch( int code, int oct )
{
	//printf("Setting the pitch woooohoooo\n");
	if ( this->TYPE != NEUME )
		return;
	
//	if (this->code == code && this->oct == oct) {
	if (abs_pitch(this->code, this->oct) == abs_pitch(code, oct)) {
		iter = n_pitches.begin();
		(*iter)->SetPitch(code, oct);	//hack fix for first pitch being set to 0
		return;
	}
	
	//if the neume is closed, we pitch shift the entire group
	//if open, we change a single pitch (punctum) in the group

	if (this->IsClosed()) {
		int diff, newpitch;
		iter = n_pitches.begin();
		diff = (*iter)->Pitch_Diff(code, oct);
		
		for (iter; iter != n_pitches.end(); ++iter)
		{
			newpitch = abs_pitch((*iter)->code, (*iter)->oct) + diff;
			(*iter)->SetPitch(abs2pitch(newpitch));
			
			if (iter==n_pitches.begin())
			{
				this->code = code;
				this->oct = oct;
			}
		}
		
	} else {
		//printf("Changing pitch for element %d [open mode]\n", n_selected);
		MusNeumePitch *temp = n_pitches.at(n_selected);
		temp->SetPitch( code, oct );
		
		
		//this->printNeumeList();
		//shift pitch for entire neume if first punctum is selected
	    //this may cause problems with multiple punctum neumes!!
		
		//NOTE: for drawing, make sure that all subsequent pitches are
		// drawn relative to the 'fundamental' first pitch
		if (n_selected == 0) {
			this->code = code;
			this->oct = oct;
		}
	}
	
	this->GetPitchRange(); //necessary for drawing the box properly in open mode
	
	if (m_w)
		m_w->Refresh();
}

void MusNeume::SetValue( int value, MusStaff *staff, int vflag )
{	
	if ( this->TYPE != NEUME )
		return;
//
//    // Change the neume value
//    // ...
//	
//	//only a punctum for now, still have to decide the scheme for neume values
//	
	if (this->IsClosed()) {
		// change to different neume type (lookup table)
	} else {
		//printf("oh man!\n");
		MusNeumePitch *temp = n_pitches.at(n_selected);
		temp->SetValue(value);
	}
	
	//refresh drawing automatically
	if (m_w)
		m_w->Refresh();
}

int MusNeume::GetValue()
{
	if (this->IsClosed())
	{
		//do nothing yet
		return 0;
	} else {
		return n_pitches.at(n_selected)->GetValue();
	}
}


//debug helper method
void MusNeume::printNeumeList() 
{
	printf("Neume List: (length %d)\n", (int)n_pitches.size());
	int count = 0;
	for (iter=n_pitches.begin(); iter != n_pitches.end(); ++iter, count++)
	{
		printf("*** Address: %d Pitch %d: code: %d oct: %d val: %d\n", 
			   (int)&(*iter), count, (*iter)->code, (*iter)->oct, (*iter)->val);
	}
}

int MusNeume::Length() { return (int)this->n_pitches.size(); };

int MusNeume::GetMaxPitch() { return this->p_max; }

int MusNeume::GetMinPitch() { return this->p_min; }

// getter and simultaneous setter
int MusNeume::GetPitchRange()
{
	int ymin, ymax, abs_pitch, count, range, max_rel, min_rel;
	count = 0;
	//printf("***********************************************\n");
	for (iter=n_pitches.begin(); iter != n_pitches.end(); ++iter, count++)
	{
		abs_pitch = (*iter)->code + ((*iter)->oct * 7);
		
	//	printf("Pitch %d == %d\n", count, abs_pitch);
		
		if (!count) ymin = ymax = abs_pitch;
		
		//printf("Absolute pitch for note %d: %d\n", count, abs_pitch);
		
		if (abs_pitch > ymax)
			ymax = abs_pitch;
		else if (abs_pitch < ymin)
			ymin = abs_pitch;
	}
	//printf("***********************************************\n");			   
	
	range = ymax - ymin;
	max_rel = ymax - (this->code + (this->oct * 7));
	min_rel = ymin - (this->code + (this->oct * 7));
	
	//printf("The pitch range is %d semitones\n", range);
	//printf("Max: %d, Min: %d\n", max_rel, min_rel);
	
	
	
	//do some field setting for convenience, if necessary
	if (range != this->p_range) this->p_range = range;
	if (max_rel != this->p_max) this->p_max = max_rel;
	if (min_rel != this->p_min) this->p_min = min_rel;
	
	return range;
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
	//printf("In %s mode, dec_y is this: %d\n", this->IsClosed() ? "closed" : "open",
	//	   this->dec_y);
	
	//printf("closed code value: %d\nclosed oct: %d\n", 
	//	   (int)this->code, oct );
	
	if (this->IsClosed()) {
		//neume drawing routine
		this->DrawNeume( dc, staff );
	} else {
		//individual punctum edit mode drawing routine
		this->DrawPunctums( dc, staff );
	}
	
	//just trying a hack here
	m_w->m_currentColour = wxBLACK;
	
	//seems to fix the problem somewhat...
	
	
	return;
}

void MusNeume::DrawBox( wxDC *dc, MusStaff *staff ) //revise
{
	// now it would be nice to see a red box around the group just to indicate 
	// that we're in 'open editing' mode
	
	m_w->m_currentColour = wxRED;
	
	int x1, x2, y1, y2;
	int ynn = this->dec_y + staff->yrel; 
	// get x coords for bounding box
	x1 = this->xrel - PUNCT_PADDING;
	
	// determine how long the box should be	
	x2 = (n_pitches.size() + 1) * PUNCT_PADDING;
	// now get x2 coordinate relative to x1
	x2 += x1;
	
	// now for y coords
	// for y1, we need to know the pitch range, specifically the highest pitch
	y1 = (ynn + PUNCT_PADDING) + (this->p_max * m_w->_espace[staff->pTaille]);
	// y2 requires lowest pitch
	y2 = (ynn - PUNCT_PADDING) - abs(this->p_min * m_w->_espace[staff->pTaille]);
	
	// now get the range
	//y2 = this->p_range * m_w->_espace[staff->pTaille] + PUNCT_PADDING;
	
	//	//printf("Drawing box: x1: %d, y1: %d, x2: %d, y2: %d\n", x1, y1, x2, y2);

	m_w->m_currentColour = new wxColour(0, 255, 255, 100);
	m_w->rect_plein2( dc, x1, y1, x2, y2);
	m_w->m_currentColour = wxRED;
	m_w->box( dc, x1, y1, x2, y2 );

	
	m_w->m_currentColour = wxBLACK;
	
}


// this method is the same thing as DrawPunctums right now, but once 
// multi-punctum neumes are supported this method will be very different

void MusNeume::DrawNeume( wxDC *dc, MusStaff *staff ) 
{
	// magic happens here
	int pTaille = staff->pTaille;
	
	int xn = this->xrel, xl = this->xrel;
	int bby = staff->yrel - m_w->_portee[pTaille];
	int ynn = this->dec_y + staff->yrel;
	//printf("closed ynn value: %d\nclosed dec_y: %d\nclosed yrel: %d\n", 
	//	   ynn, this->dec_y, staff->yrel );
	
	xn += this->offset;
	
	int ledge = m_w->ledgerLine[pTaille][2];
	
	MusNeumePitch *temp;
	//for (unsigned int i = 0; i < n_pitches.size(); i++)
	// for now, only draw the first punctum
//	for (unsigned int i = 0; i < 1; i++)
//	{
//		temp = n_pitches.at(i);
//		leg_line( dc, ynn,bby,xn,ledge, pTaille);
//		//printf("Putting the (closed) neume here: ynn + 65: %d\n", ynn + 65);
//		
//		// draw some "debug" graphics
//		m_w->m_currentColour = wxCYAN;
//		m_w->rect_plein2(dc, this->xrel - 3, ynn - 3, this->xrel + 3, ynn + 3);
//		//printf("\nxrel: %d, ynn: %d\n\n", this->xrel, ynn);
//		m_w->m_currentColour = wxBLACK;
//		
//		m_w->putfont( dc, this->xrel, ynn + 16, 
//					  temp->getPunctumType(), staff, this->dimin, NEUME); 
//	}
	
	if (this->n_pitches.size() == 1) {
		temp = n_pitches.at(0);
		leg_line( dc, ynn,bby,xn,ledge, pTaille);
		m_w->putfont( dc, this->xrel - 5, ynn + 16, 
					 temp->getPunctumType(), staff, this->dimin, NEUME); 
	} else if (this->n_pitches.size() >= 1) {
		// we need to draw a ligature
		this->drawLigature(dc, staff);
	}
	
}

//helper function

//int MusNeume::getYPos(int index, MusStaff *staff) {
//	int ynn = this->dec_y + staff->yrel;
//	if (!index) return ynn + 65;
//	else 
//	{
//		MusNeumePitch *temp = n_pitches.at(index);
//		int diff = temp->code + (temp->oct * 12);
//		diff -= this->code + (temp->oct * 12);
//		
//		return diff + ynn + 65;
//	}
//}



void MusNeume::DrawPunctums( wxDC *dc, MusStaff *staff )
{
	// draw each individual punctum in the group, side by side for easy editing
	// draw a rectangular box around this group to indicate open editing mode
	
	int pTaille = staff->pTaille;
	
//	int b = this->dec_y;
//	int up=0, i, valdec, fontNo, ledge, queueCentre;
//	int x1, x2, y2, espac7, decval, vertical;
//	int formval = 0;
//	int rayon, milieu = 0;
	int oct = this->oct - 4; //? 
	int xn = this->xrel, xl = this->xrel;
	int bby = staff->yrel - m_w->_portee[pTaille];  // bby= y sommet portee
	this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
	int ynn = this->dec_y + staff->yrel; 
	//printf("open code value: %d\nopen oct: %d\n", 
	//	   (int)this->code, oct );
	
	xn += this->offset;
	
	int ledge = m_w->ledgerLine[pTaille][2];
	
	int punct_y;
	
	this->DrawBox( dc, staff );
	
	MusNeumePitch *temp;
	for (unsigned int i = 0; i < n_pitches.size(); i++) 
	{
		temp = n_pitches.at(i);
		
		punct_y = staff->y_note((int)temp->code, staff->testcle( this->xrel ), temp->oct - 4);
		ynn = punct_y + staff->yrel; 
		
		leg_line( dc, ynn,bby,xl + (i * PUNCT_PADDING),ledge, pTaille);
		// colour the selected item red
		if (i == n_selected) m_w->m_currentColour = wxRED;
		else m_w->m_currentColour = wxBLACK;
		printf("Putting the (open) neume here: ynn + 65: %d\n", ynn + 16);
		m_w->putfont( dc, this->xrel + (i * PUNCT_PADDING), ynn + 16, 
					  temp->getPunctumType(), staff, this->dimin, NEUME);	
	}

}

/*
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
	
	
//	switch (val) {
//		case 0:
//		fontNo = nPUNCTUM;
//		break;
//		case 1:
//		fontNo = nDIAMOND;
//		break;
//		case 2:
//		fontNo = nCEPHALICUS;
//		break;
//		case 3:
//		fontNo = nPODATUS;
//		break;
////		case 4:
////		fontNo = nDIAMOND_SMALL;		//small diamonds don't draw correctly for some reason
////		break;
//		case 4:
//		fontNo = nQUILISMA;
//		break;
//	}
		
	
	// issue with y position — empirically setting y position (for now)
	// as of 06/04/2010 punctums draw a 7th below the mouse click
	
	//more hacks... since neumes dont' have time values (yet)
	
	ledge = m_w->ledgerLine[pTaille][2];
	
	leg_line( dc, ynn,bby,xl,ledge, pTaille);	// dessin lignes additionnelles	
	m_w->putneume( dc, this->xrel, ynn + 65, fontNo, staff, this->dimin); //worry about the font later
	
	
}
*/
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



