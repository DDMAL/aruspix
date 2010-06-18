/////////////////////////////////////////////////////////////////////////////
// Name:        mussymbol.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "mussymbol.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "mussymbol.h"
#include "muswindow.h"
#include "musstaff.h"
#include "muspage.h"
#include "musdef.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// MusSymbol
//----------------------------------------------------------------------------

MusSymbol::MusSymbol():
	MusElement()
{
	TYPE = SYMB;
	calte = 0;
	carOrient = 0;
	carStyle = 0;
	flag = 0; 
	fonte = 0;
	l_ptch = 0;
	point = 0;
	s_lie_l = 0;
	m_note_ptr = NULL;
}

MusSymbol::MusSymbol( unsigned char _flag, unsigned char _calte, unsigned short _code ):
	MusElement()
{
	TYPE = SYMB;
	calte = _calte;
	carOrient = 0;
	carStyle = 0;
	code = _code;
	flag = _flag;
	fonte = 0;
	l_ptch = 0;
	point = 0;
	s_lie_l = 0;
	m_note_ptr = NULL;
}

MusSymbol::MusSymbol( const MusSymbol& symbol )
	: MusElement( symbol )
{
	TYPE = symbol.TYPE;
	calte = symbol.calte;
	carOrient = symbol.carOrient;
	carStyle = symbol.carStyle;
	code = symbol.code;
	flag = symbol.flag;
	fonte = symbol.fonte;
	l_ptch = symbol.l_ptch;
	point = symbol.point;
	s_lie_l = symbol.s_lie_l;
	m_note_ptr = symbol.m_note_ptr;
	m_debord_str = symbol.m_debord_str;
}

MusSymbol::~MusSymbol()
{
}


//void MusSymbol::SetPitch( int code, int oct, MusStaff *staff )
void MusSymbol::SetPitch( int code, int oct )
{
	if ( this->TYPE != SYMB )
		return;

	if ((this->flag == PNT) || (this->flag == ALTER))
	{
		if ((this->code == code) && (this->oct == oct ))
			return;

		this->oct = oct;
		this->code = code;
		
		if (m_w)
			m_w->Refresh();
	}
}


void MusSymbol::SetValue( int value, MusStaff *staff, int vflag )
{	
	if ( this->TYPE != SYMB )
		return;

	if ( this->flag == CLE )
	{
		if (m_w)
		{
			m_w->OnBeginEditionClef();
		}

		switch (value)
		{
		case ('1'): this->code = SOL1; break;
		case ('2'): this->code = SOL2; break;
		case ('3'): this->code = UT1; break;
		case ('4'): this->code = UT2; break;
		case ('5'): this->code = UT3; break;
		case ('6'): this->code = UT4; break;
		case ('7'): this->code = UT5; break;
		case ('8'): this->code = FA3; break;
		case ('9'): this->code = FA4; break;
		case ('0'): this->code = FA5; break;
		}

		if (m_w)
		{
			m_w->OnEndEditionClef();
			m_w->Refresh();
		}
	}
	else if ( this->flag == IND_MES )
	{
		switch (value)
		{
		case ('Q'): this->code = 34; break;
		case ('W'): this->code = 38; break;
		case ('E'): this->code = 32; break;
		case ('R'): this->code = 36; break;

		case ('A'): this->code = 18; break;
		case ('S'): this->code = 22; break;
		case ('D'): this->code = 16; break;
		case ('F'): this->code = 20; break;

		case ('Y'): this->code = 10; break;
		case ('X'): this->code = 14; break;
		case ('C'): this->code = 8; break;
		case ('V'): this->code = 12; break;

		case ('1'): this->code = 1; this->durNum = 3; this->durDen = 2; break;
		case ('2'): this->code = 64; this->calte = 2; break;
		case ('3'): this->code = 64; this->calte = 3; break;
		}
	}
	else // ALTER PNT BAR
	{
		if ( vflag & 1 ) // control is down
		switch (value)
		{
			case ('D'): value = 'F'; break;
			case ('B'): value = 'N'; break;
		}

		switch (value)
		{
		case ('D'): this->flag = ALTER; this->calte = DIESE; break;
		case ('B'): this->flag = ALTER; this->calte = BEMOL; break;
		case ('H'): this->flag = ALTER; this->calte = BECAR; break;
		case ('F'): this->flag = ALTER; this->calte = D_DIESE; break;
		case ('N'): this->flag = ALTER; this->calte = D_BEMOL; break;

		case ('P'): this->flag = PNT; break;

		case ('|'): this->flag = BARRE; this->code = '|'; this->oct = 0; break;
		}
	}
	
	if ( m_w )
	{
		m_w->Refresh();
		m_w->OnEndEdition();
	}
}


void MusSymbol::ResetToKey( )
{
	MusSymbol reset;
	reset.flag = CLE;
	reset.code = SOL2;
	*this = reset;
}

void MusSymbol::ResetToSymbol( )
{
	MusSymbol reset;
	reset.flag = PNT;
	*this = reset;
}

void MusSymbol::ResetToProportion( )
{
	MusSymbol reset;
	reset.flag = IND_MES;
	reset.code = 34;
	*this = reset;
}

void MusSymbol::Draw ( wxDC *dc, MusStaff *pportee)
// touche;	 code relecture input (1) ou coord. du decalage (0) 
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	wxASSERT_MSG( m_w, "MusWindow cannot be NULL ");
	if ( !Check() )
		return;	

	if ( this->ElemInvisible )
		return;
	
	if (!m_w->efface && (this == m_w->m_currentElement))
		m_w->m_currentColour = wxRED;
	else if (!m_w->efface && (this->m_cmp_flag == CMP_MATCH))
		m_w->m_currentColour = wxLIGHT_GREY;
	else if (!m_w->efface && (this->m_cmp_flag == CMP_DEL))
		m_w->m_currentColour = wxGREEN;
	else if (!m_w->efface && (this->m_cmp_flag == CMP_SUBST))
		m_w->m_currentColour = wxBLUE;
	else if (!m_w->efface && (this->m_cmp_flag == CMP_INS))
		m_w->m_currentColour = wxRED;


	
	int x = this->xrel + this->offset;
	char valeurcod = 0;
	//Debord *pt;

	int oct = this->oct - 4;

	if (this->flag == ALTER || this->flag == PNT || this->flag == LIAI)
		this->dec_y= pportee->y_note((int)this->code, pportee->testcle(x), oct );
	if (this->flag == AX_VARIANT)
		this->dec_y= pportee->y_note((int)this->code, 12, 0 );

	switch (this->flag)
	{
		case BARRE : m_w->discontinu = this->l_ptch;	// pour pointilles 
			switch (this->code)
			{	case '|' : m_w->m_page->bar_mes( dc, x,  m_p->EpBarreMesure, this->carStyle, pportee);
						break;
				case CTRL_L: m_w->m_page->barMesPartielle ( dc, x, pportee); break;
				default:
//				case 'E' :
						valeurcod = (char) this->code;
						if (this->calte == 1)
							valeurcod = -valeurcod;
						m_w->m_page->bigbarre( dc, x, valeurcod, this->carStyle, pportee);
			}
			break;
		case ALTER : dess_symb ( dc,x,this->dec_y,this->flag,this->calte, pportee);
			break;
		case PNT : dess_symb ( dc,x,this->dec_y,this->flag,this->point, pportee);
			break;
		case AX_VARIANT : dess_symb ( dc,x,this->dec_y, this->flag,this->point, pportee);
			break;
		/*case LIAI :	if (this->s_lie_l == 1)
					liais_m (hdc, '(', x, this->dec_y, pportee->yrel);
				else if (this->s_lie_l == 2)
				{	discontinu = this->l_ptch;	// pour pointilles
					liais_m (hdc, ')', x, this->dec_y, pportee->yrel);
//					discontinu=0;
				}
			break;*/
		case IND_MES :	// MESURECOL;
			afficheMesure ( dc, pportee );
			break;
		case CLE :		 
			calcoffs (&x,(int)this->code);
			this->dec_y = x;
			this->dess_cle( dc,m_w->kPos[pportee->no].compte, pportee); 
			break;

		/*case VECT: pt = (Debord *)this->pdebord; pt++; //SYMBOLCOL;
			playGraphRec (hdc, x, (pportee->yrel+this->dec_y*v4_unit[pTaille]), (short *)pt);
				break;
		case METAFILE_W:
			playMetafRec (hdc, x,  (pportee->yrel+this->dec_y*v4_unit[pTaille]), (short *)this->pdebord);
				break;
		case CHAINE: lit_txt(hdc); break;
		case CROCHET:
		case BEZIER:
		case VALBARRES:
		case DYNAMIC:
			lit_debord(hdc, this);
			break;*/

	}
	//discontinu=0;

	if ( !m_w->efface )
		m_w->m_currentColour = &m_w->m_black;
	

	return;
}


void MusSymbol::dess_symb( wxDC *dc, int x, int y, int symc, int symf, MusStaff *pportee)
// symc = cat du symb, symf=code d'identite 
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;	

	//extern LOGFONT lf;

	if (symc == ALTER)
	{	y += pportee->yrel;
		switch (symf)
		{	case BECAR :  symc=sBECARRE; break;
			case D_DIESE : symc=sDIESE; m_w->putfont ( dc, x, y, symc, pportee, this->dimin, SYMB);
						y += 7*m_w->_espace[pportee->pTaille]; // LP
			case DIESE : symc=sDIESE; break;
			case D_BEMOL :  symc=sBEMOL; m_w->putfont ( dc, x, y, symc, pportee, this->dimin, SYMB);
						y += 7*m_w->_espace[pportee->pTaille]; // LP
			case BEMOL :  symc=sBEMOL; break;
			case Q_DIESE : symc=sQDIESE; break;
			case Q_BEMOL :  symc=sQBEMOL; break;
		}
		m_w->putfont ( dc, x, y, symc, pportee, this->dimin, SYMB);
	}
	else if (symc == AX_VARIANT)
	{	
		y += pportee->yrel;
		m_w->putfont ( dc, x, y, '+', pportee, this->dimin, SYMB);
	}
	else
		switch (symf)
		{	case 1 : m_w->pointer( dc,x,y,0,pportee); x += max (6, m_w->_pas);
			case 0 : m_w->pointer ( dc,x,y,0,pportee);
		}


}


void MusSymbol::afficheMesure ( wxDC *dc, MusStaff *staff)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;	
	
	int x, yp;
	float mDen=1.0, mNum=1.0;
	int caractere;

	if (this->code & 64)
	{	
		yp = staff->yrel - (m_w->_portee[ staff->pTaille ]+ m_w->_espace[ staff->pTaille ]*6);
		switch (this->calte)
		{	case 0: caractere = MES_CSimple;
					mNum = 4; mDen = 4; break;
			case 1: caractere = MES_CBar;
					mNum = 2; mDen = 2; break;
			case 2: caractere = MES_2Simple;
					mNum = 2; mDen = 2; break;
			case 3: caractere = MES_3Simple;
					mNum = 3; mDen = 2; break;
			case 4: caractere = MES_2;
					mNum = 4; mDen = 2; break;
			case 5: caractere = MES_3;
					mNum = 6; mDen = 2; break;
		}
		if ( dc )
		{	
			//wxString s( char(caractere), 1);
			wxString s = char(caractere);
			m_w->putstring ( dc, this->xrel, yp, s, 1, staff->pTaille);
		}
	}

	else
	{
		if (this->code & 32)
		{	mNum = 2; mDen = 2;
			mesCercle ( dc, this->xrel, staff->yrel, staff);
		}
		else if (this->code & 16)
		{	mNum = 2; mDen = 2;
			demi_cercle ( dc, this->xrel, staff->yrel, staff);
		}
		else if (this->code & 8)
		{	mNum = 4; mDen = 2;
			inv_d_cercle ( dc, this->xrel, staff->yrel, staff);
		}
		if (this->code & 4)
		{	stroke ( dc, this->xrel, staff->yrel, staff);
			mDen = 1;
		}
		if (this->code & 2)
		{	prol (dc, this->xrel, staff->yrel, staff);
			mNum = 9; mDen = 4;
		}
	}


	if ( this->code & 1)
	{	x = this->xrel;
		if (this->code > 1)
			x += m_w->_pas*5;
		chiffres ( dc, x, staff->yrel, staff);
		mDen = max ( this->durDen, 1);
		mNum = max ( this->durNum, 1);
	}
	m_w->MesVal = mNum / mDen;
	m_w->mesureNum = (int)mNum;
	m_w->mesureDen = (int)mDen;

	return;
}


void MusSymbol::mesCercle ( wxDC *dc, int x, int yy, MusStaff *staff )
{	
	int y =  m_w->ToZoomY (yy - m_w->_interl[ staff->pTaille ] * 6);
	int r = m_w->ToZoom( m_w->_interl[ staff->pTaille ]);

	int w = max( m_w->ToZoom(4), 2 );

	wxPen pen( *m_w->m_currentColour, w, wxSOLID );
	dc->SetPen( pen );
	wxBrush brush( *m_w->m_currentColour, wxTRANSPARENT );
	dc->SetBrush( brush );

	dc->DrawCircle( m_w->ToZoom(x), y, r );
		
	dc->SetPen( wxNullPen );
	dc->SetBrush( wxNullBrush );
}	

void MusSymbol::demi_cercle ( wxDC *dc, int x, int yy, MusStaff *staff )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;	

	int w = max( m_w->ToZoom(4), 2 );
	wxPen pen( *m_w->m_currentColour, w, wxSOLID );
	dc->SetPen( pen );
	wxBrush brush( *m_w->m_currentColour, wxTRANSPARENT );
	dc->SetBrush( brush );

	int y =  m_w->ToZoomY (yy - m_w->_interl[ staff->pTaille ] * 5);
	int r = m_w->ToZoom( m_w->_interl[ staff->pTaille ]);

	x = m_w->ToZoom (x);
	x -= 3*r/3;

	dc->DrawEllipticArc( x, y, 2*r, 2*r, 70, 290 );
		
	dc->SetPen( wxNullPen );
	dc->SetBrush( wxNullBrush );

	return;
}	

void  MusSymbol::inv_d_cercle ( wxDC *dc, int x, int yy, MusStaff *staff )
{	
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;	

	int w = max( m_w->ToZoom(4), 2 );
	wxPen pen( *m_w->m_currentColour, w, wxSOLID );
	dc->SetPen( pen );
	wxBrush brush( *m_w->m_currentColour, wxTRANSPARENT );
	dc->SetBrush( brush );

	int y =  m_w->ToZoomY (yy - m_w->_interl[ staff->pTaille ] * 5);
	int r = m_w->ToZoom( m_w->_interl[ staff->pTaille ] );

	x = m_w->ToZoom (x);
	x -= 4*r/3;

	dc->DrawEllipticArc( x, y, 2*r, 2*r, 250, 110 );
		
	dc->SetPen( wxNullPen );
	dc->SetBrush( wxNullBrush );

	return;
}	

void MusSymbol::prol ( wxDC *dc, int x, int yy, MusStaff *staff )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;	

	int y =  m_w->ToZoomY (yy - m_w->_interl[ staff->pTaille ] * 6);
	int r = max( m_w->ToZoom(4), 2 );
	
	wxPen pen( *m_w->m_currentColour, 1, wxSOLID );
	dc->SetPen( pen );
	wxBrush brush( *m_w->m_currentColour, wxSOLID );
	dc->SetBrush( brush );

	dc->DrawCircle( m_w->ToZoom(x) -r/2 , y, r );
		
	dc->SetPen( wxNullPen );
	dc->SetBrush( wxNullBrush );

	return;
}	


void MusSymbol::stroke ( wxDC *dc, int a, int yy, MusStaff *staff )
{	
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;	
	
	int y1 = yy - m_w->_portee[ staff->pTaille ];
	int y2 = y1 - m_w->_portee[ staff->pTaille ];
	
	m_w->v_bline2 ( dc, y1, y2, a, 3);
	return;
}	


void MusSymbol::chiffres ( wxDC *dc, int x, int y, MusStaff *staff)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;		
	
	int ynum, yden;
	wxString s;

	if (this->durDen)
	{	
		ynum = y - (m_w->_portee[staff->pTaille]+ m_w->_espace[staff->pTaille]*4);
		yden = ynum - (m_w->_interl[staff->pTaille]*2);
	}
	else
		ynum = y - (m_w->_portee[staff->pTaille]+ m_w->_espace[staff->pTaille]*6);

	if (this->durDen > 9 || this->durNum > 9)	// avancer
		x += m_w->_pas*2;

	s = wxString::Format("%u",this->durNum);
	m_w->putstring ( dc, x, ynum, s, 1, staff->pTaille);	// '1' = centrer

	if (this->durDen)
	{
	s = wxString::Format("%u",this->durDen);
		m_w->putstring ( dc, x, yden, s, 1, staff->pTaille);	// '1' = centrer

	}
	return;
}

void MusSymbol::InsertCharInLyricAt( int x, char letter )
{	
	if ( x < 0 || !letter )
		return;
	
	wxString lyric = this->m_debord_str;
	wxString part1 = lyric.SubString( 0, x - 1 );
	wxString part2 = lyric.SubString( x, lyric.Length() );
	
	lyric = part1 << letter << part2;
	this->m_debord_str = lyric;
}

bool MusSymbol::DeleteCharInLyricAt( int x )
{
	if ( x < 0 )
		return false;
	
	wxString lyric = this->m_debord_str;
	wxString part1 = lyric.SubString( 0, x - 1 );
	wxString part2 = lyric.SubString( x + 1, lyric.Length() );
	
	lyric = part1 << part2;
	this->m_debord_str = lyric;
	return true;
}

bool MusSymbol::IsLastLyricElementInNote( )
{
	MusNote *note = this->m_note_ptr;	
	int no = (int)note->m_lyrics.GetCount();
	MusSymbol *tmp = &note->m_lyrics[ no - 1 ];

	if ( this == tmp )
		return true;
	else
		return false;
}

// Laurent: I am not sure what this method is doing...
// Why can't we just check the length of the string?
bool MusSymbol::IsLyricEmpty( )
{
	wxString str = this->m_debord_str;
	int length = str.Length();
	int i = 0;
	while ( i < length ){
		if ( str[i] == 20 )
			i++;
		else 
			break;
	}
	if ( length == i ) return true;
	else return false;
}

bool MusSymbol::IsLyric( )
{
    return ((flag == CHAINE) && (fonte == LYRIC)); 
}

// WDR: handler implementations for MusSymbol



