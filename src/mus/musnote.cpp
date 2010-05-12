/////////////////////////////////////////////////////////////////////////////
// Name:        musnote.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musnote.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "musnote.h"
#include "muswindow.h"
#include "musstaff.h"
#include "mussymbol.h"
#include "muspage.h"
#include "musdef.h"

#include <math.h>

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusSymbols );

// sorting function
int SortElements(MusSymbol **first, MusSymbol **second)
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
// MusNote
//----------------------------------------------------------------------------

MusNote::MusNote():
	MusElement()
{
	TYPE = NOTE;
    sil = false;
    val = 0;
    inv_val = false;
    point = 0;
    stop_rel = 0;
    acc = 0;
    accInvis = false;
    q_auto = true;
    queue = false;
    stacc = false;
    oblique = false;
    queue_lig = false;
    chord = false;
    fchord = false;
    lat = false;
    haste = false;
    code = 0;
    tetenot = 0;
    typStac = 0;
	for ( int i = 0; i < (int)this->m_lyrics.GetCount(); i++){
		MusSymbol *lyric = NULL;
		this->m_lyrics.Add( lyric );
	}
}

MusNote::MusNote( char _sil, unsigned char _val, unsigned char _code )
	: MusElement()
{
	TYPE = NOTE;
    sil = _sil;
    val = _val;
    inv_val = false;
    point = 0;
    stop_rel = 0;
    acc = 0;
    accInvis = false;
    q_auto = true;
    queue = false;
    stacc = false;
    oblique = false;
    queue_lig = false;
    chord = false;
    fchord = false;
    lat = false;
    haste = false;
    code = _code;
    tetenot = 0;
    typStac = 0;
	for ( int i = 0; i < (int)this->m_lyrics.GetCount(); i++){
		MusSymbol *lyric = NULL;
		this->m_lyrics.Add( lyric );
	}
		
	oct = 4;
}

MusNote::MusNote( const MusNote& note )
	: MusElement( note )
{
	TYPE = note.TYPE;
	sil = note.sil;
	val = note.val;
	inv_val = note.inv_val;
	point = note.point;
	stop_rel = note.stop_rel;
	acc = note.acc;
	accInvis = note.accInvis;
	q_auto = note.q_auto;
	queue = note.queue;
	stacc = note.stacc;
	oblique = note.oblique;
	queue_lig = note.queue_lig;
	chord = note.chord;
	fchord = note.fchord;
	lat = note.lat;
	haste = note.haste;
	code = note.code;
	tetenot = note.tetenot;
	typStac = note.typStac;
	for (int i = 0; i < (int)note.m_lyrics.GetCount(); i++){
		MusSymbol *lyric = new MusSymbol( note.m_lyrics[i] );
		lyric->m_note_ptr = this;
		this->m_lyrics.Add( lyric );
	}	
} 

MusNote& MusNote::operator=( const MusNote& note )
{
	if ( this != &note ) // not self assignement
	{
		// For base class MusElement copy assignement
		(MusElement&)*this = note;
		TYPE = note.TYPE;
		sil = note.sil;
		val = note.val;
		inv_val = note.inv_val;
		point = note.point;
		stop_rel = note.stop_rel;
		acc = note.acc;
		accInvis = note.accInvis;
		q_auto = note.q_auto;
		queue = note.queue;
		stacc = note.stacc;
		oblique = note.oblique;
		queue_lig = note.queue_lig;
		chord = note.chord;
		fchord = note.fchord;
		lat = note.lat;
		haste = note.haste;
		code = note.code;
		tetenot = note.tetenot;
		typStac = note.typStac;
		for (int i = 0; i < (int)note.m_lyrics.GetCount(); i++){
			MusSymbol *lyric = new MusSymbol( note.m_lyrics[i] );
			lyric->m_note_ptr = this;
			this->m_lyrics.Add( lyric );
			
		}
	}
	return *this;
}

MusNote::~MusNote()
{	
}

bool MusNote::BelongsToTheNote( MusElement *element )
{
	for ( int i = 0; i < (int)m_lyrics.GetCount(); i++ )
	{
		MusSymbol *lyric = &m_lyrics[i];
		if (lyric == element )
			return true;
	}
	return false;
}


void MusNote::SetPitch( int code, int oct, MusStaff *staff )
{
	if ( this->TYPE != NOTE )
		return;

	if ((this->code == code) && (this->oct == oct ))
		return;

	this->oct = oct;
	this->code = code;
	

	if (m_w)
		m_w->Refresh();
}



void MusNote::SetValue( int value, MusStaff *staff, int vflag )
{	
	if ( this->TYPE != NOTE )
		return;

	//if (this->val == value) //
	//	return;

	//if (this->val == CUSTOS) // can't change custos value
	//	return;

	if ((value < 0) || (value > TC)) // min and max values
		if (!(vflag & 1) || (value != CUSTOS)) // custos exception
			return;

	//wxClientDC *dc = this->InitAndClear( staff );

	this->val = value;
	if ( vflag & 1 )
		this->sil = _SIL;
	else
		this->sil = _NOT;
		
	// remove ligature flag for silences and inadequate values	
	//if ( ( this->sil == _SIL ) || ( value < BR ) || ( value > RD ) )
		this->ligat = false;
		
	this->inv_val = false;
	this->oblique = false;
		
	// remove qauto flag for silences and inadequate values	
	if ( ( this->sil == _SIL ) || (( value > LG ) && ( value < BL )) )
		this->q_auto = true;
	
	if (m_w)
		m_w->Refresh();
}


void MusNote::ChangeColoration( MusStaff *staff )
{
	if ( ( this->sil == _SIL ) || ( this->val == CUSTOS ) )
		return;
		
	// mimized the number of symbols
	switch ( this->val )
	{
	case (LG) : 
	case (BR) : 
	case (RD) : this->inv_val = !this->inv_val; break;
	case (BL) : this->val = NR; break;
	case (NR) : this->val = BL; break;
	default : this->oblique = !this->oblique;
	}
	
	if (m_w)
		m_w->Refresh();

}

void MusNote::SetLigature( MusStaff *staff )
{
	if ( ( this->sil == _SIL ) || ( this->val == LG ) || ( this->val > RD ) )
		return;
	
	this->q_auto = true;
	this->ligat = true;
	
	if (m_w)
		m_w->Refresh();
}

void MusNote::ChangeStem( MusStaff *staff )
{
	if ( ( this->sil == _SIL ) || (( this->val > LG ) && ( this->val < BL )) )
		return;
		
	this->q_auto = !this->q_auto;
	
	if (m_w)
		m_w->Refresh();
}


void MusNote::Draw( wxDC *dc, MusStaff *staff)
// unsigned touche;	 code relecture input (1) ou coord. du decalage (0) 
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	wxASSERT_MSG( m_w, "MusWindow cannot be NULL ");
	if ( !Check() )
		return;	

	int oct = this->oct - 4;

	if (!m_w->efface && (this == m_w->m_currentElement))
		m_w->m_currentColour = wxRED;
	else if (!m_w->efface && (this->m_cmp_flag == CMP_MATCH))
		m_w->m_currentColour = wxLIGHT_GREY;
	else if (!m_w->efface && (this->m_cmp_flag == CMP_DEL))
		m_w->m_currentColour = wxGREEN;
	else if (!m_w->efface && (this->m_cmp_flag == CMP_SUBST))
		m_w->m_currentColour = wxBLUE;	
	
	// To Tristan
	// this is call every single time a note is drawn, so it has to be optimized
	// First thing : don't do the loop if efface or !lyricMode
	// Second thing : break as soon as you get the element
	// Third thing : put the access stuff into a method (used below as well)
	if ( !m_w->efface && m_w->m_lyricMode && BelongsToTheNote( m_w->m_currentElement ) )
		m_w->m_currentColour = wxCYAN;
		
	if ( this->sil == _NOT)
	{	
		this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);

		//if (!pelement->ElemInvisible || illumine) 
		{	
			if (!this->chord)	// && (!pelement->ElemInvisible || illumine))
				this->note(dc, staff);
			//else
			//	this->accord(dc, staff);
		}
	}
	else
	{	//if (!transp_sil)
		//	pnote->code = getSilencePitch (pelement);
		this->dec_y = staff->y_note((int)this->code, staff->testcle( this->xrel ), oct);
		silence ( dc, staff );
	}
    
    // draw the beams
    if (staff->beamListPremier && this->frel) {
        staff->beam( dc );
    }
    

		wxPen pen( *m_w->m_currentColour, m_p->EpLignesPortee, wxSOLID );
		dc->SetPen( pen );
		wxBrush brush( *m_w->m_currentColour , wxSOLID );
		dc->SetBrush( brush );

    
    /* liaison, testcode 
	int nbrInt;
	wxPoint *ptcoord;
	nbrInt = PTCONTROL;
    
    m_w->point_[0].x = m_w->ToZoom(xrel);
    m_w->point_[0].y =  m_w->ToZoomY(staff->yrel);
    m_w->point_[1].x =  m_w->ToZoom(xrel + 100);
    m_w->point_[1].y = m_w->ToZoomY(staff->yrel + 50);
    m_w->point_[2].x =  m_w->ToZoom(xrel + 300);
    m_w->point_[2].y = m_w->ToZoomY(staff->yrel + 50);
    m_w->point_[3].x =  m_w->ToZoom(xrel + 400);
    m_w->point_[3].y = m_w->ToZoomY(staff->yrel);
    //dc->DrawSpline( 4, point );
    
	ptcoord = &m_w->bcoord[0];
	m_w->calcBez ( ptcoord, nbrInt );

	m_w->pntswap (&m_w->point_[0], &m_w->point_[3]);
	m_w->pntswap (&m_w->point_[1], &m_w->point_[2]);
	
	m_w->point_[1].y +=  m_w->ToZoom(5);
	m_w->point_[2].y +=  m_w->ToZoom(5);

	ptcoord = &m_w->bcoord[nbrInt+1];	// suite de la matrice: retour du bezier
	m_w->calcBez ( ptcoord, nbrInt );

	//SetPolyFillMode (hdc, WINDING);
	dc->DrawPolygon (nbrInt*2,  m_w->bcoord, 0, 0, wxWINDING_RULE ); //(sizeof (bcoord)*2) / sizeof (POINT)); nbrInt*2+ 1;
    */
    
		dc->SetPen( wxNullPen );
		dc->SetBrush( wxNullBrush );
	
	for ( int i = 0; i < (int)m_lyrics.GetCount(); i++ ){
		MusSymbol *lyric = &m_lyrics[i];
		
		if ( lyric != NULL )
		{
			if (!m_w->efface) 
			{
				if ( lyric == m_w->m_currentElement )
					m_w->m_currentColour = wxRED;
				else if ( (this == m_w->m_currentElement) || BelongsToTheNote( m_w->m_currentElement ) )
					m_w->m_currentColour = wxCYAN;
			}
			m_w->putlyric(dc, lyric->xrel + staff->xrel, staff->yrel + lyric->dec_y , 
						  lyric->m_debord_str, staff->pTaille, ( lyric == m_w->m_currentElement && m_w->m_inputLyric ) );
		}		
	}
	
	if ( !m_w->efface )
		m_w->m_currentColour = &m_w->m_black;
	
	return;
}


// dessine la note en a,b+by. Calcule et dessine lignes addit. avec by=yrel
// b = decalage en fonction oct., clef, a partir du curseur; by = pos. curs.
// Accords: note doit connaitre le x non modifie par accord(), la fin de 
// l'accord (ptr_n->fchord), la valeur y extreme opposee au sommet de la
// queue: le ptr *testchord extern peut garder le x et l'y.

void MusNote::note ( wxDC *dc, MusStaff *staff )
{
	int pTaille = staff->pTaille;

	//	int horphyspoint=h_pnt;
	int b = this->dec_y;
	int up=0, i, valdec, fontNo, ledge, queueCentre;
	int x1, x2, y2, espac7, decval, vertical;
	int formval = 0;	// pour permettre dessiner colorations avec d‚calage de val
	int rayon, milieu = 0;

	int xn = this->xrel, xl = this->xrel;
	int bby = staff->yrel - m_w->_portee[pTaille];  // bby= y sommet portee
	int ynn = this->dec_y + staff->yrel; 
	static int ynn_chrd;

	xn += this->offset;
	val=this->val;
	formval = (this->inv_val && val > RD) ? (val+1) : val;
	queueCentre = 0;

	rayon = m_w->rayonNote[pTaille][this->dimin];

	if (val > RD || (val == RD && staff->notAnc))	// annuler provisoirement la modif. des lignes addit.
		ledge = m_w->ledgerLine[pTaille][this->dimin];
	else
	{	ledge= m_w->ledgerLine[pTaille][2];
		rayon += rayon/3;
	}

	/*NEW: diminuer le rayon d'une quantit‚ param‚trable*/
	if (val>RD)
		rayon -= ((rayon * m_fh->param.hampesCorr) / 20);
	x1 = xn-rayon;	// position d'appel du caractŠre et de la queue gauche
	xl = xn;

	// permettre d'inverser le cot‚ de la tete de note avec flag lat
	if (this->lat && !this->chord)
	{	if (this->queue)
		{	x1 = xn + rayon;
			xl = xn + rayon * 2;
		}
		else
		{	x1 = xn - rayon * 3;
			xl = xn - rayon * 2;
		}
	}

	leg_line( dc, ynn,bby,xl,ledge, pTaille);	// dessin lignes additionnelles

	if (val==LG || val == BR || (ligat && val == RD))	// dessin carrees
	{
		ligature ( dc, ynn, staff);
 	}
	else if (val==RD)
	{	if (in (this->tetenot, LOSANGEVIDE, OPTIONLIBRE))
			fontNo = OffsetTETE_NOTE+this->tetenot;
		else if (this->inv_val && !this->oblique)
			fontNo = sRONDE_N;
		else
			fontNo = sRONDE_B;

		m_w->putfont( dc,x1, ynn, fontNo, staff, this->dimin);
		decval = ynn;
	}
	else
	{	if (in (this->tetenot, LOSANGEVIDE, OPTIONLIBRE))
			fontNo = OffsetTETE_NOTE+this->tetenot;

		else if (this->oblique || formval == BL)
			fontNo = sBLANCHE;
		else
			fontNo = sNOIRE;

		m_w->putfont( dc,x1, ynn, fontNo, staff, this->dimin);

		milieu = bby - m_w->_interl[pTaille]*2;

// test ligne mediane pour direction queues: notation mesuree, milieu queue haut
		if (staff->notAnc)
			milieu +=  m_w->_espace[pTaille];

		if (this->chord) /*** && this == testchord)***/
			ynn_chrd = ynn;
		if (this->rel && formval > NR)
		{	if (this->drel)
				staff->beamListPremier = this;
		}
		else if (this->tetenot != SANSQUEUE && (!this->chord || this->fchord))
		{	if (this->chord)
			{	
				/***up = testchord->obj.not.haste;
				xn = testchord->xrel;***/
			}
			else
				//***up = this->q_auto ? ((ynn < milieu)? ON :OFF):this->queue;
				// ENZ
				up = (ynn < milieu) ? ON : OFF;
			
			// ENZ
			if ( !this->q_auto )
				up = (up == ON) ? OFF : ON;
				

			espac7 = this->dimin ? ( m_w->_espace[pTaille]*5) : ( m_w->_espace[pTaille]*7);
			vertical = this->dimin ?  m_w->_espace[pTaille] :  m_w->_interl[pTaille];
			decval = vertical * (valdec = formval-CR);
			
			/***if (this->existDebord)	// queue longueur manuelle
				traiteQueue (&espac7, this);***/

			// diminuer le rayon de la moitie de l'epaisseur du trait de queue
			rayon -= (m_p->EpQueueNote-1) / 2;

			if (!up)	// si queue vers le bas (a gauche)
			{	espac7 = -espac7;
				decval = -decval;
				rayon = -rayon;
			}

			y2 = ((formval>CR) ? (ynn + espac7 + decval) : (ynn + espac7));

			if (this->chord)
				ynn = ynn_chrd;

			/***if (this->q_auto)
			{	this->queue = (up > 0);
				if ( (y2 >= milieu && ynn > milieu) || (y2 <= milieu && ynn < milieu) )
				// note et queue du meme cote par rapport au centre de la portee
				{	y2 = milieu;
					queueCentre = 1;
				}
			}***/

			if (staff->notAnc)
				rayon = 0;
			x2 = xn + rayon;

			if (up)
			{

				if (formval > CR && !queueCentre)
				// Le 24 Septembre 1993. Correction esthetique pour rapprocher tailles 
				//   des CR et DC (longeur de queues trop inegales).
					y2 -= m_w->_espace[pTaille];
				decval = y2;
				if (staff->notAnc)
					m_w->v_bline ( dc,y2,(int)(ynn+ m_w->_espace[pTaille]),x2, m_w->ToZoom(m_p->EpQueueNote) );//queue en descendant
				else
					m_w->v_bline ( dc,y2,(int)(ynn+ m_w->v4_unit[pTaille]),x2, m_w->ToZoom(m_p->EpQueueNote) );//queue en descendant
				if (formval > NR)
				{
					m_w->putfont( dc,x2,y2,sCROCHET_H, staff, this->dimin);
					for (i=0; i < valdec; i++)
						m_w->putfont( dc,x2,y2-=vertical,sCROCHET_H, staff, this->dimin);
				}
			}
			else
			{

				if (formval > CR && !queueCentre)
				// Le 24 Septembre 1993. Correction esthetique pour rapprocher tailles 
				//   des CR et DC (longeur de queues trop inegales).
					y2 += m_w->_espace[pTaille];
				decval = y2;

				if (staff->notAnc)
					m_w->v_bline ( dc,y2,ynn- m_w->_espace[pTaille],x2,m_w->ToZoom(m_p->EpQueueNote) );//queue en descendant
				else
					m_w->v_bline ( dc,y2,(int)(ynn- m_w->v4_unit[pTaille]),x2, m_w->ToZoom(m_p->EpQueueNote) );	// queue en montant

				// ENZ
				// decalage du crochet vers la gauche
				// changement dans la fonte Leipzig 4.3 à cause de problemes d'affichage
				// en deçà de 0 avec la notation ancienne
				// dans la fonte les crochets ont ete decales de 164 vers la droite
				int cr_offset = m_w->rayonNote[pTaille][this->dimin];
				if (formval > NR)
				{
					m_w->putfont( dc,x2 - cr_offset,y2,sCROCHET_B , staff, this->dimin);
					for (i=0; i < valdec; i++)
						m_w->putfont( dc,x2  - cr_offset,y2+=vertical,sCROCHET_B, staff, this->dimin);
				}
			}
		}	// fin de dessin queues et crochets

	}

	leg_line( dc, ynn,bby,xl,ledge, pTaille);

	if (this->liaison)
	{	if (this->fliai)
		{	/***liaison = OFF;	// pour tests de beam...
			liais_note(hdc, this);***/
		}
		/***if (this->dliai && !attente)	premierLie = this;***/
	}

	if (this->acc && !this->accInvis)
	{
		if (this->chord)
		{}/***x1 = x_acc_chrd (this,0);***/
		else
			x1 -= m_w->largAlter[pTaille][this->dimin];
		MusSymbol symb;
		symb.Init( m_w );
		symb.flag = ALTER;
		symb.calte = this->acc;
		symb.dess_symb ( dc, x1, b, ALTER, this->acc, staff);
	}
	if (this->chord)
	{	/***x2 = testchord->xrel + _pas3;
		if (this->haste)
		{	if (this->lat || (this->ptr_fe && this->ptr_fe->type==NOTE && this->ptr_fe->obj.not.lat)
				|| (this->ptr_pe && this->xrel==this->ptr_pe->xrel && this->ptr_pe->type==NOTE && this->ptr_pe->obj.not.lat
					&& this->dec_y - this->ptr_pe->dec_y < _interl[pTaille]
					&& 0 != ((int)b % (int)_interl[pTaille]))
				)
				x2 += rayonNote[pTaille][dimin] * 2;
		}*///
	}
	else
	{	if (this->val < BL || (this->val > CR && !this->rel && up))
			x2 = xn + m_w->_pas*7/2;
		else
			x2 = xn + m_w->_pas*5/2;

		if (this->lat)
				x2 += rayon*2;
	}

	if (this->point && (!this->pointInvisible))
	{
		pointage( dc,x2,b, 0, this->point, staff );
	}
/*
	if (this->stacc && (!this->rel || !this->queue_lig))
	{
		if (!this->chord || ((!this->queue_lig && this == testchord) || (this->queue_lig && this->fchord )))
		{
			if (val > BR)
			{	if  (!this->queue_lig)
				{	if ((this->queue && !this->chord) || (this->chord && this->haste))
					{	b -= _interl[pTaille];
						decval = -_interl[pTaille];
					}
					else
					{	b += _interl[pTaille];
						decval = 0;
					}

				}
				else	// tous les cas invers‚s par queue_lig
				{	b = decval-staff->yrel;
	
					if ((!this->queue && !this->chord) || (this->chord && !this->haste))
					{	b -= _interl[pTaille];
						decval = -1;
						if (val <= RD)
							decval = -_interl[pTaille];

					}
					else
					{	b += _espace[pTaille];
						decval = 0;
						if (val <= RD)
							b += _espace[pTaille];
					}
				}

			}
			else
				return;
			putStacc (hdc,xn,b,decval,this->typStac);
		}
	}
*/

	return;
}


void MusNote::leg_line( wxDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int pTaille)
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


void MusNote::silence ( wxDC *dc, MusStaff *staff)
{	
	int formval = this->val;
	int a = this->xrel + this->offset, b = this->dec_y;

	//unsigned char dot = this->point;
	if (inv_val && (!this->oblique && formval > RD || this->oblique && formval > BL))
		formval += 1;

	if (this->val == 15)
		formval = RD;
	else if (formval > BL)
		a -= m_w->rayonNote[staff->pTaille][dimin];

	if (formval == BR || formval == BL || this->val == 15)
		b -= 0; //m_w->_interl[staff->pTaille]; // LP position des silences

	if (formval == RD)
	{	if (staff->portNbLine == 1)
		// silences sur portee a une seule ligne
			b += m_w->_interl[staff->pTaille];
		else
			//b += m_w->_interl[staff->pTaille]*2; 
			b -= 0; //m_w->_interl[staff->pTaille]*2;// LP positions des silences
	}

	if (this->val == 15)
		s_special ( dc, a, staff);
	else
	{	switch (formval)
		{	case LG: s_lg ( dc, a, b, staff); break;
			case BR: s_br ( dc, a, b, staff); break;
			case RD:
			case BL: s_rd ( dc, a, b, formval, staff); break;
			case CUSTOS: s_nr ( dc, a, b - m_w->_espace[staff->pTaille] + 1, '#' - sSilNOIRE + NR , staff); break;
			default: s_nr ( dc, a, b, formval, staff);
		}
	}
	return;
}


void MusNote::s_special ( wxDC *dc, int a, MusStaff *staff)

{	int x, x2, y, y2, off;

	off = (m_w->_pas*2)/3;
	y = staff->yrel - m_w->_interl[staff->pTaille]*6;
	y2 = y + m_w->_interl[staff->pTaille];
	x = a-off; x2 = a+off;
	m_w->rect_plein2( dc,x,y2,x2,y);
	return;
}

void MusNote::s_lg ( wxDC *dc, int a, int b, MusStaff *staff)

{	int x, x2, y = b + staff->yrel, y2;

	x = a; //- m_w->_pas/3; 
	x2 = a+ (m_w->_pas *2 / 3); // LP
	if (b % m_w->_interl[staff->pTaille])
		y -= m_w->_espace[staff->pTaille];
	y2 = y + m_w->_interl[staff->pTaille]*2;
	m_w->rect_plein2( dc,x,y2,x2,y);
	return;
}


void MusNote::s_br ( wxDC *dc, int a, int b, MusStaff *staff)

{	int x, x2, y = b + staff->yrel, y2;

	x = a; //- m_w->_pas/3; 
	x2 = a+ (m_w->_pas *2 / 3); // LP

	if (b % m_w->_interl[staff->pTaille])
		y -= m_w->_espace[staff->pTaille];
	y2 = y + m_w->_interl[staff->pTaille];
	m_w->rect_plein2 ( dc,x,y2,x2,y);
	x = a - m_w->_pas; x2 = a + m_w->_pas;

	m_w->h_bline ( dc, x,x2,y2,1);
	m_w->h_bline ( dc, x,x2,y, 1);
	return;
}

void MusNote::s_rd ( wxDC *dc, int a, int b, int valeur, MusStaff *staff)

{	int x, x2, y = b + staff->yrel, y2, vertic = m_w->_espace[staff->pTaille];
	int off;
	float foff;

	if (staff->notAnc)
		foff = (m_w->_pas *1 / 3);
	else
		foff = (m_w->ledgerLine[staff->pTaille][2] * 2) / 3; // i.e., la moitie de la ronde

	if (dimin)
		foff *= (int)( (float)m_w->RapportDimin[0] / (float)m_w->RapportDimin[1] );
	off = (int)foff;

	x = a - off;
	x2 = a + off;

	if (valeur == RD)
		vertic = -vertic;

	if (b % m_w->_interl[staff->pTaille])
	{
		if (valeur == BL)
			y -= vertic;
		else
			y += vertic;
	}

	y2 = y + vertic;
	m_w->rect_plein2 ( dc, x,y,x2,y2);

	off /= 2;
	x -= off;
	x2 += off;

	if (y > (int)staff->yrel- m_w->_portee[staff->pTaille] || y < (int)staff->yrel-2* m_w->_portee[staff->pTaille])
		m_w->h_bline ( dc, x, x2, y, m_p->EpLignesPortee);

	if (this->point)
		pointage ( dc,(x2 + m_w->_pas), y2, -(int)staff->yrel, this->point, staff);
}


void MusNote::s_nr ( wxDC *dc, int a, int b, int valeur, MusStaff *staff)
{
	int _intrl = m_w->_interl[staff->pTaille];

	m_w->putfont( dc, a, (b + staff->yrel), sSilNOIRE + (valeur-NR), staff, this->dimin);

	//m_w->putfont( dc, a, (b + staff->yrel - m_w->_espace[staff->pTaille]), '#', staff, this->dimin);

	if (this->point)
	{	if (valeur >= DC)
			_intrl = 0;
		pointage ( dc, (a+ m_w->_pas3), b, _intrl, this->point, staff);
	}
	return;
}


void MusNote::pointage ( wxDC *dc, int x1, int y1, int offy, unsigned int d_p, MusStaff *staff )

{
	y1 += offy;
	m_w->pointer ( dc, x1,y1, 0, staff);
	if (d_p == D_POINTAGE)
	{	x1 += max (6, m_w->_pas);
		m_w->pointer ( dc, x1,y1,0, staff);
	}
	return;
}


// should probably become a static method of MusNote
// in that case, MusWindow wouldn't be necessary anymore
void lig_x ( MusNote *note, MusStaff *staff, MusWindow *m_w )
{
	if (note == NULL)	return;
    MusElement *next = staff->GetPrevious(note);
	if (next == NULL || !next->IsNote() || next->fligat) return;
	if (next->ligat && next->IsNote() && ((MusNote*)next)->val <= RD)
	{	if (next->fligat && ((MusNote*)next)->lat)
			note->xrel = next->xrel;
		else
			note->xrel = next->xrel + m_w->largeurBreve[staff->pTaille] * 2;
	}
    return;
}


static int ligat_x[2], ligat_y[2];	// pour garder coord. des ligatures
unsigned int MusNote::marq_obl = OFF;	// marque le 1e passage pour une oblique

void MusNote::ligature ( wxDC *dc, int y, MusStaff *staff )
{	
	int xn, x1, x2, yy2, y1, y2, y3, y4, y5;
	int milieu, up, epaisseur;

	epaisseur = max (2, m_p->EpBarreValeur/2);
	xn = this->xrel;
	
	if (ligat)
		lig_x ( this, staff, this->m_w );
	else
		xn = this->xrel + this->offset;


	// calcul des dimensions du rectangle
	x1 = xn -  m_w->largeurBreve[staff->pTaille]; x2 = xn +  m_w->largeurBreve[staff->pTaille];
	y1 = y + m_w->_espace[staff->pTaille]; 
	y2 = y - m_w->_espace[staff->pTaille]; 
	y3 = (int)(y1 + m_w->v_unit[staff->pTaille]);	// partie d'encadrement qui depasse
	y4 = (int)(y2 - m_w->v_unit[staff->pTaille]);	

	if (!this->oblique && (!marq_obl))	// notes rectangulaires, y c. en ligature
	{
		if ( !this->inv_val)
		{				//	double base des carrees
			m_w->hGrosseligne ( dc, x1,  y1,  x2,  y1, -epaisseur );
			m_w->hGrosseligne ( dc, x1,  y2,  x2,  y2, epaisseur );
		}
		else
			m_w->rect_plein2( dc,x1,y1,x2,y2);	// dessine val carree pleine // ENZ correction de x2

		m_w->v_bline ( dc, y3, y4, x1, m_w->ToZoom(m_p->EpQueueNote) );	// corset lateral
		m_w->v_bline ( dc, y3, y4, x2, m_w->ToZoom(m_p->EpQueueNote) );
	}
	else			// traitement des obliques
	{
		if (!MusNote::marq_obl)	// 1e passage: ligne verticale initiale
		{
			m_w->v_bline (dc,y3,y4,x1, m_w->ToZoom(m_p->EpQueueNote) );
			MusNote::marq_obl = ON;
			//oblique = OFF;
//			if (val == RD)	// queue gauche haut si RD
//				queue_lig = ON;
		}
		else	// 2e passage: lignes obl. et verticale finale
		{
			x1 -=  m_w->largeurBreve[staff->pTaille]*2;	// avance auto

			y1 = *ligat_y - m_w->_espace[staff->pTaille];	// ligat_y contient y original
			yy2 = y2;
			y5 = y1+ m_w->_interl[staff->pTaille]; y2 += m_w->_interl[staff->pTaille];	// on monte d'un INTERL

			if (inv_val)
				m_w->hGrosseligne ( dc,  x1,  y1,  x2,  yy2, m_w->_interl[staff->pTaille]);
			else
			{	m_w->hGrosseligne ( dc,  x1,  y1,  x2,  yy2, 5);
				m_w->hGrosseligne ( dc,  x1,  y5,  x2,  y2, -5);
			}
			m_w->v_bline ( dc,y3,y4,x2,m_w->ToZoom(m_p->EpQueueNote));	//cloture verticale

			MusNote::marq_obl = OFF;
//			queue_lig = OFF;	//desamorce alg.queue BR

		}
	}

	if (ligat)	// memoriser positions d'une note a l'autre; relier notes par barres
	{	*(ligat_x+1) = x2; *(ligat_y+1) = y;	// relie notes ligaturees par barres verticales
		if (in(x1,(*ligat_x)-2,(*ligat_x)+2) || (this->fligat && this->lat && !MusNote::marq_obl))
			// les dernieres conditions pour permettre ligature verticale ancienne
			m_w->v_bline (dc, *ligat_y, y1, (this->fligat && this->lat) ? x2: x1, m_w->ToZoom(m_p->EpQueueNote));
		*ligat_x = *(ligat_x + 1);
		*ligat_y = *(ligat_y + 1);
	}

	
	y3 = y2 - m_w->_espace[staff->pTaille]*6;

	if (ligat)
	{	if (val == BR  && this->queue_lig)	// queue gauche bas: BR initiale descendante
			m_w->v_bline ( dc, y2, y3, x1, m_w->ToZoom(m_p->EpQueueNote) );

		else if (val == LG && !this->queue_lig) // LG en ligature, queue droite bas
			m_w->v_bline (dc, y2, y3, x2, m_w->ToZoom(m_p->EpQueueNote) );

		else if (val == RD && this->queue_lig )	// queue gauche haut
		{	y2 = y1 + m_w->_espace[staff->pTaille]*6;
			m_w->v_bline ( dc, y1, y2, x1, m_w->ToZoom(m_p->EpQueueNote) );
		} 
	}
	else if (val == LG)		// LG isolee: queue comme notes normales
	{	
		milieu = staff->yrel - m_w->_interl[staff->pTaille]*6;
		//***up = this->q_auto ? ((y < milieu)? ON :OFF):this->queue;
		// ENZ
		up = (y < milieu) ? ON : OFF;
		// ENZ
		if ( !this->q_auto )
			up = (up == ON) ? OFF : ON;
			
		if (up)
		{	y3 = y1 + m_w->_espace[staff->pTaille]*6;
			y2 = y1;
		}
		m_w->v_bline ( dc, y2,y3,x2, m_w->ToZoom(m_p->EpQueueNote) );
	}

	return;
}

void MusNote::DeleteLyricFromNote( MusSymbol *lyric )
{
	for ( int i = 0; i < (int)this->m_lyrics.GetCount(); i++ ){
		MusSymbol *tmp = &this->m_lyrics[i];
		if ( lyric == tmp ){
			this->m_lyrics.RemoveAt( i );
			break;
		}
	}
}

void MusNote::CheckLyricIntegrity( )
{
	this->m_lyrics.Sort( SortElements );
	int num = this->m_lyrics.GetCount();
	int i = 0;
	
	// Check for empty lyric elements
	while ( i < num ){
		MusSymbol *symbol = &this->m_lyrics[i];		
		if ( symbol->IsLyricEmpty() ){
			this->m_lyrics.RemoveAt( i );
			delete symbol;
			num = this->m_lyrics.GetCount();
		}
		else i++;
	}
	
	// Ensure elements have correct numbering within array
	num = this->m_lyrics.GetCount();
	for ( i = 0; i < num; i++ ){
		MusElement *element = &this->m_lyrics[i];
		element->no = i;
	}
}

MusSymbol *MusNote::GetFirstLyric( )
{
	if ( this->m_lyrics.GetCount() == 0 ) 
		return NULL;
	else
		return &this->m_lyrics[0];
}

MusSymbol *MusNote::GetLastLyric( )
{
	int num = this->m_lyrics.GetCount(); 
	if ( num == 0 ) 
		return NULL;
	else 
		return &this->m_lyrics[num-1];
}

MusSymbol *MusNote::GetLyricNo( int no )
{
	int num = this->m_lyrics.GetCount(); 
	if ( (no < 0) || (num <= no) ) 
		return NULL;
	else 
		return &this->m_lyrics[no];
}


// WDR: handler implementations for MusNote



