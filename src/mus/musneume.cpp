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
#include "neumedef.h"

#include <math.h>

// sorting function
int SortElements(MusNeume **first, MusNeume **second)
{
	if ( (*first)->xrel < (*second)->xrel ) {
		return -1;
	} else if ( (*first)->xrel > (*second)->xrel ) {
		return 1;
	} else {
		return 0;
	}
}


//----------------------------------------------------------------------------
// MusNeumePitch
//----------------------------------------------------------------------------

MusNeumePitch::MusNeumePitch(int _code, int _oct, unsigned char _val) 
{
	code = _code;
	oct = _oct;
	this->SetValue(_val);
	m_font_str.assign("d"); // (char)nPUNCTUM
}

// Duplicate an existing pitch
MusNeumePitch::MusNeumePitch( const MusNeumePitch &pitch) {
	code = pitch.code;
	oct = pitch.oct;
	val = pitch.val;
	m_font_str.assign(pitch.m_font_str);
}

void MusNeumePitch::SetPitch( int code, int oct ) {
	this->code = code;
	this->oct = oct;
}

int MusNeumePitch::GetCode() {
	return code;
}

int MusNeumePitch::GetOct() {
	return oct;
}

void MusNeumePitch::SetValue( int value ) 
{ 
	this->val = value;
	m_font_str.assign("");
	
	switch (value) {
		case 0:
		default:
			m_font_str.append(sizeof(char), (char)nPUNCTUM);
			break;
		case 1:
			m_font_str.append(sizeof(char), (char)nDIAMOND);
			break;
		case 2:
			m_font_str.append(sizeof(char), (char)nCEPHALICUS);
			break;
		case 3:
			m_font_str.append(sizeof(char), (char)nPUNCT_UP);
			break;
		case 4:
			m_font_str.append(sizeof(char), (char)nQUILISMA);			
			break;
		case 5: //virga
			m_font_str.append(sizeof(char), (char)nPUNCTUM);
			m_font_str.append(sizeof(char), (char)nSTEM_B_RIGHT);
			break;
	}	
}

int MusNeumePitch::GetValue() {
	return this->val;
}

wxString MusNeumePitch::GetFestaString() {
	return m_font_str;
}

int MusNeumePitch::Compare(const MusNeumePitch &other)
{
	int a = this->code + (this->oct * 7);
	int b = other.code + (other.oct * 7);

	if (a < b) {
		/*printf("ascending\n");*/
		return -1;
	} else if (a == b) {
		/*printf("same pitch\n");*/
		return 0;
	} else {
		/*printf("descending\n");*/
		return 1;
	}
}

// returns an int value of the difference between pitches
int MusNeumePitch::Pitch_Diff(const MusNeumePitch &other) 
{
	return abs_pitch(other.code, other.oct) - abs_pitch(this->code, this->oct);
}

int MusNeumePitch::Pitch_Diff(int code, int oct)
{
	return abs_pitch(code, oct) - abs_pitch(this->code, this->oct);
}

int MusNeumePitch::filtrcod( int codElement, int *oct )
{	
	*oct = this->oct;
	
	if (codElement == F1)
	{	
		if ((*oct) != 0) 
			(*oct)--; 
		codElement = F8;
	}
	else if (codElement == F9 || codElement == F10)
	{	
		if ((*oct) < 7) 
			(*oct)++;
		codElement = (codElement == F9) ? F2 : F3;
	}
	return codElement;
}

//----------------------------------------------------------------------------
// MusNeume
//----------------------------------------------------------------------------

MusNeume::MusNeume() : MusElement() {
    int _code, _val = 0;
    
    TYPE = NEUME;
	closed = true;
	n_selected = 0;
	MusNeumePitch firstPitch(_code, 0, _val);
	n_pitches.push_back(firstPitch);
	code = _code;	
	p_range = p_min = p_max = 0;
	n_type = name = form = NULL; //this gets set when ligature is drawn
}

// Copy an existing neume
MusNeume::MusNeume( const MusNeume &neume) :
    MusElement(neume) {
	TYPE = neume.TYPE;
	closed = true;	//all neumes are closed by default
	
	for (unsigned int i = 0; i < neume.n_pitches.size(); i++) {
        MusNeumePitch pitch = neume.n_pitches.at(i);
		n_pitches.push_back(pitch);
	}
	
	n_selected = 0;
	this->GetPitchRange();
	n_type = neume.n_type;
	name = neume.name;
	form = neume.form;	
	
	code = neume.code;
	oct = neume.oct;
}

MusNeume::~MusNeume()
{	
}

bool MusNeume::IsClosed() { return this->closed; }

void MusNeume::SetClosed(bool value) {
	this->closed = value;
	
	if (this->closed) {
		n_selected = 0;
	}
	
	if (m_r) {
		m_r->DoRefresh();
	}

	this->printNeumeList();
}

void MusNeume::SelectNextPunctum() { 
	if (n_selected < n_pitches.size() - 1) 
	{
		n_selected++;
	}
	
	if (m_r) {
        m_r->DoRefresh();
    }
}

void MusNeume::SelectPreviousPunctum() {
	if (n_selected > 0) 
	{
		n_selected--;
	}
	
	if (m_r) {
        m_r->DoRefresh();
    }
}

void MusNeume::InsertPitchAfterSelected()
{
	if (this->IsClosed()) return; //can only insert pitches in open mode
	
	MusNeumePitch selected = this->n_pitches.at(this->n_selected);
	
	MusNeumePitch new_pitch = selected;
	
	iter = this->n_pitches.begin();
	
	printf("appending pitch\n");
	
	n_pitches.insert(iter + n_selected, new_pitch);
	n_selected++;
	
	this->GetPitchRange();
	
	if (m_r) {
		m_r->DoRefresh();
    }
}

void MusNeume::RemoveSelectedPitch()
{
	if (this->IsClosed()) return; //can only remove pitches in open mode
	
	if (n_pitches.size() == 1) return; //removing last node crashes aruspix
	// cannot remove last node (for now)
	
	iter = n_pitches.begin();
	n_pitches.erase(iter + n_selected);
	
	if (n_selected) n_selected--;
	
	this->GetPitchRange();
	if (m_r) {
		m_r->DoRefresh();
    }
}

int MusNeume::GetCode()
{
	if (this->IsClosed()) {
		return this->code; 
	} else {
		return n_pitches.at(n_selected).GetCode();
	}
}

int MusNeume::GetOct()
{
	if (this->IsClosed()) {
		return this->oct;
	} else {
		return n_pitches.at(n_selected).GetOct();
	}
}

void MusNeume::SetPitch( int code, int oct )
{
	if ( this->TYPE != NEUME )
		return;

	int diff = n_pitches.at(0).Pitch_Diff(code, oct);
	
	if (this->IsClosed()) {
		//shift all pitches!
		this->code = code;
		this->oct = oct;
		
		n_pitches.at(0).SetPitch(code, oct);
	
		for (unsigned int i = 1; i < n_pitches.size(); i++) {
			//unnecessarily shifted
			
			int newoctave;
			MusNeumePitch p = n_pitches.at(i);
			int newcode = p.filtrcod(p.GetCode() + diff, &newoctave);
			n_pitches.at(i).SetPitch(newcode, newoctave);
		}
		
	} else { // open mode
		int newoctave;
		MusNeumePitch p = n_pitches.at(n_selected);
		int newcode = p.filtrcod(p.GetCode() + diff, &newoctave);
		n_pitches.at(n_selected).SetPitch(newcode, newoctave);

		if (n_selected == 0) { 
			this->SetPitch(newcode, newoctave);
		}
	}

	this->printNeumeList();
	
	this->GetPitchRange(); //necessary for drawing the box properly in open mode	
	if (m_r)
		m_r->DoRefresh();
}

void MusNeume::SetValue( int value, MusStaff *staff, int vflag )
{	
	if ( this->TYPE != NEUME ) {
		return;
	}

	if (this->IsClosed() && Length() == 1) {
        // If there's just 1 element, change it even if we're closed.
        n_pitches.at(n_selected).SetValue(value);
	} else {
		n_pitches.at(n_selected).SetValue(value);
	}
	
	//refresh drawing automatically
	if (m_r) {
		m_r->DoRefresh();
	}
}

int MusNeume::GetValue()
{
	if (this->IsClosed() && Length() == 1) {
        // If there's just 1, return it even if we're closed
		return n_pitches.at(n_selected).GetValue();
	} else {
		return n_pitches.at(n_selected).GetValue();
	}
}


//debug helper method
void MusNeume::printNeumeList() 
{
	//printf("Neume Address:\n---------\n");
	//printf("%d [%X]\n---------\n", (int)&(*this),(unsigned int)&(*this));
	//printf("Vector Address:\n---------\n"); 
	//printf("%d [%d]\n", (int)&(this->n_pitches), (int)&(this->n_pitches));
	//printf("Neume List: (length %d)\n", (int)n_pitches.size());
	//printf("***********************\n");
	//int count = 0;
	//for (iter=n_pitches.begin(); iter != n_pitches.end(); ++iter, count++) {
		//printf("%d: %d [%X] code: %d oct: %d val: %d\n", 
		//	   count, (int)&(*iter), (unsigned int)&(*iter), (*iter)->code, 
		//	   (*iter)->oct, (*iter)->val);
	//}
	//printf("***********************\n");
}

int MusNeume::Length() { return (int)this->n_pitches.size(); };

int MusNeume::GetMaxPitch() { return this->p_max; }

int MusNeume::GetMinPitch() { return this->p_min; }

// getter and simultaneous setter
int MusNeume::GetPitchRange()
{
	int ymin, ymax, abs_pitch, count, range, max_rel, min_rel;
	count = 0;
	for (iter=n_pitches.begin(); iter != n_pitches.end(); ++iter, count++)
	{
		abs_pitch = (*iter).GetCode() + ((*iter).GetOct() * 7);
		
		if (!count) ymin = ymax = abs_pitch;
		
		if (abs_pitch > ymax)
			ymax = abs_pitch;
		else if (abs_pitch < ymin)
			ymin = abs_pitch;
	}
	
	range = ymax - ymin;
	max_rel = ymax - (this->code + (this->oct * 7));
	min_rel = ymin - (this->code + (this->oct * 7));
	
	//do some field setting for convenience, if necessary
	if (range != this->p_range) this->p_range = range;
	if (max_rel != this->p_max) this->p_max = max_rel;
	if (min_rel != this->p_min) this->p_min = min_rel;
	
	return range;
}

//should have some loop for drawing each element in the neume
void MusNeume::Draw( AxDC *dc, MusStaff *staff)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	wxASSERT_MSG( m_r, "MusRC cannot be NULL ");
	if ( !Check() )
		return;	

    // Draw the neume
    // ...
	
	// following the example set by musnote...
	
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
	//printf("In %s mode, dec_y is this: %d\n", this->IsClosed() ? "closed" : "open",
	//	   this->dec_y);
	
	//printf("closed code value: %d\nclosed oct: %d\n", 
	//	   (int)this->code, oct );
	
	if (this->IsClosed() == true) {
		this->DrawNeume( dc, staff );
	} else {
		this->DrawPunctums( dc, staff );
	}
	
	m_r->m_currentColour = AxBLACK;
	return;
}

void MusNeume::DrawBox( AxDC *dc, MusStaff *staff ) //revise
{
	// now it would be nice to see a red box around the group just to indicate 
	// that we're in 'open editing' mode
	
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
	
	// now get the range
	//y2 = this->p_range * m_r->_espace[staff->pTaille] + PUNCT_PADDING;
	//	//printf("Drawing box: x1: %d, y1: %d, x2: %d, y2: %d\n", x1, y1, x2, y2);
	
	//m_r->m_currentColour = wxWHITE; //??? to fix memory leak
	//m_r->rect_plein2( dc, x1, y1, x2, y2);
	m_r->m_currentColour = AxRED;
	m_r->box( dc, x1, y1, x2, y2 );

	
	m_r->m_currentColour = AxBLACK;
	
}


// this method is the same thing as DrawPunctums right now, but once 
// multi-punctum neumes are supported this method will be very different

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

void MusNeume::DrawPunctums( AxDC *dc, MusStaff *staff )
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
	
	
	yh = y_p + m_r->_espace[pTaille]; yb = y_p- m_r->_portee[pTaille]- m_r->_espace[pTaille];
	
	if (!in(y_n,yh,yb))                           // note hors-portee?
	{
		xng = xn + 8 - smaller; //xn gauche MAGIC NUMBERS YEAHHH
		xnd = xn + smaller; //xn droite
//		printf("xn = %d, xn gauche: %d, xn droigt: %d\n", xn, xng, xnd);
//		printf("xrel = %d\n", this->xrel);
		
		dist = ((y_n > yh) ? (y_n - y_p) : y_p - m_r->_portee[pTaille] - y_n);
  		ynt = ((dist % m_r->_interl[pTaille] > 0) ? (dist - m_r->_espace[pTaille]) : dist);
		test = ynt/ m_r->_interl[pTaille];
		if (y_n > yh)
		{	yn = ynt + y_p;
			v_decal = - m_r->_interl[pTaille];
		}
		else
			yn = y_p - m_r->_portee[pTaille] - ynt;
		
		//hPen = (HPEN)SelectObject (hdc, CreatePen (PS_SOLID, _param.EpLignesPORTEE+1, workColor2));
		//xng = toZoom(xng);
		//xnd = toZoom(xnd);
		
        dc->SetPen( m_r->m_currentColour, m_p->EpLignesPortee, wxSOLID );
        dc->SetBrush( m_r->m_currentColour , wxTRANSPARENT );
		
		for (i = 0; i < test; i++)
		{
			dc->DrawLine( m_r->ToZoom(xng) , m_r->ToZoomY ( yn ) , m_r->ToZoom(xnd) , m_r->ToZoomY ( yn ) );
			//m_r->h_line ( dc, xng, xnd, yn, m_r->_param.EpLignesPORTEE);
			//yh =  toZoom(yn);
			//MoveToEx (hdc, xng, yh, NULL);
			//LineTo (hdc, xnd, yh);
			
			yn += v_decal;
		}
		
        dc->ResetPen();
        dc->ResetBrush();
	}
	return;
}




