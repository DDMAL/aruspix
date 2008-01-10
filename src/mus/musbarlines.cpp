/////////////////////////////////////////////////////////////////////////////
// Name:        muswindow.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "muswindow.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "muswindow.h"
#include "muspage.h"
#include "musfile.h"

// WDR: class implementations

void MusPage::braces ( wxDC *dc, int x, int y1, int y2, int cod, int pTaille)
//	int x, y1, y2;	x, 1e et 2e y de barre vert
//	int cod; si ON, on fait 2e barre vert. mince en position  x
{
	if ( !Check() )
		return;

	int xg, xd, yg, yd, ecart, centre;
 	//HPEN hPen;
  
	//discontinu = 0;

	if (cod == 2)
	{
		accolade ( dc, x, y1, y2, pTaille);
		return;
	}
	//else if (hdc)
	{
		wxPen pen( *m_w->m_currentColour , 1, wxSOLID );
		dc->SetPen( pen );
		wxBrush brush( *m_w->m_currentColour , wxTRANSPARENT );
		dc->SetBrush( brush );

		ecart = m_w->DELTABAR;
		centre = x - ecart;
		
		xg = centre - ecart*2;
		xd = centre + ecart*2;
		
		yg = y1 + m_w->_interl[ pTaille ] * 2;
		yd = y1;
		m_w->SwapY( &yg, &yd );
		dc->DrawEllipticArc( m_w->ToZoom(xg), m_w->ToZoomY(yg), m_w->ToZoom(xd-xg), m_w->ToZoom(yg-yd),272, 360 );
	
		yg = y2;
		yd = y2 - m_w->_interl[ pTaille ] * 2;
		m_w->SwapY( &yg, &yd );
		dc->DrawEllipticArc( m_w->ToZoom(xg), m_w->ToZoomY(yg), m_w->ToZoom(xd-xg), m_w->ToZoom(yg-yd), 0, 88 );
		
		dc->SetPen( wxNullPen );
		dc->SetBrush( wxNullBrush );

		xg = x - (m_w->DELTABLANC[0] + 1);
		// determine le blanc entre barres grosse et mince
		m_w->v_bline2( dc, y1, y2, xg, m_w->DELTANbBAR[0]);
	}
	//if (cod)
		//v_bline(dc, y1, y2, x, m_fh->param.EpBarreMesure);

	return;
}


void MusPage::DrawBarres( wxDC *dc )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	int i, flLine=0, flBrace=0;
	int xx, portee;
	int b_gr, bb_gr;
	int b_acc, bb_acc;                
	//extern int _encontinu;

    MusStaff *staff;

// 	if ((wxg+margeMorteHor) > (staff->indent ? portIndent : portNoIndent))
 	//if ((wxg+margeMorteHor) > max (portIndent, portNoIndent))
 	//	return;
//	SYMBOLCOL;
//  MESURECOL;

	b_gr = bb_gr = b_acc = bb_acc = 0;                

    for (i = 0; i < this->nbrePortees; i++) 
	{
		int decPortType_i;
		staff =  &this->m_staves[i];
		if (staff->portNbLine == 4) 
			decPortType_i = m_w->_interl[staff->pTaille]*5;
		else
			decPortType_i = m_w->_portee[staff->pTaille];
		xx = staff->indent ? staff->indent*10 : 0;

		if (staff->portNbLine == 1 || staff->portNbLine == 4)
			portee = m_w->_portee[ staff->pTaille ]*2;
		else
			portee = m_w->_portee[ staff->pTaille ] + ((staff->portNbLine-1) * m_w->_interl[staff->pTaille]);

		if (staff->vertBarre == DEBUT)
			b_gr = (int)(m_w->kPos[i].yp - decPortType_i);
		// key[i].yp est position du curseur par default _portee (4 interl) au-dessus de ligne superieure
		// decporttyp est la valeur de remplacement de _portee si on veut autre espace
		else if (staff->vertBarre == DEB_FIN)
		{	b_gr = (int)(m_w->kPos[i].yp - decPortType_i);
			bb_gr = (int)(m_w->kPos[i].yp - portee);//_portee[staff->pTaille]*2;
			flLine = 1;
		}
		else if (staff->vertBarre == FIN)
		{	bb_gr = (int)(m_w->kPos[i].yp - portee);//_portee[staff->pTaille]*2;
			flLine = 1;
		}
		if (staff->brace == DEBUT)
			b_acc = (int)(m_w->kPos[i].yp - decPortType_i);
		else if (staff->brace == DEB_FIN)
		{	b_acc = (int)(m_w->kPos[i].yp - decPortType_i);
			bb_acc = (int)(m_w->kPos[i].yp - portee);//_portee[staff->pTaille]*2;
			flBrace = 1;
		}
		else if (staff->brace == FIN)
		{	bb_acc = (int)(m_w->kPos[i].yp - portee);//_portee[staff->pTaille]*2;
			flBrace = 1;
		}

		//if (!modMetafile || ( bb_gr < drawRect.top && b_gr > drawRect.bottom && 
		//			 in (xx, drawRect.left, drawRect.right)))
			if (flLine)
			{ 	
				if (m_p->EpBarreMesure > 2)
					m_w->v_bline2( dc ,b_gr,bb_gr,xx, m_p->EpBarreMesure);
				else
					m_w->v_bline( dc ,b_gr,bb_gr,xx, m_p->EpBarreMesure);
				flLine = 0;
			}

		//if (!modMetafile || ( bb_acc < drawRect.top && b_acc > drawRect.bottom && 
		//			 in (xx, drawRect.left, drawRect.right)))
		if (flBrace)
		{		if (staff->accol)
					braces ( dc, xx- m_w->_pas/2 , b_acc, bb_acc, 2, staff->pTaille);
				else
					braces ( dc, xx,b_acc,bb_acc, 1, staff->pTaille);
			flBrace = 0;
		}

		//if (_encontinu && staff->vertBarre > 1) // > DEBUT
		//	break;
	}

	return;
}



void MusPage::accolade ( wxDC *dc, int x, int y1, int y2, int pTaille)
{	
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	m_w->SwapY( &y1, &y2 );
	
	int ymed, xdec, fact, nbrInt;
//	static POINT *bcoord;
	wxPoint *ptcoord;

	wxPen pen( *m_w->m_currentColour, 1, wxSOLID );
	dc->SetPen( pen );
	wxBrush brush( *m_w->m_currentColour, wxSOLID );
	dc->SetBrush( brush );

	x -= m_w->DELTABLANC[ pTaille ];  // distance entre barre et debut accolade

	nbrInt = PTCONTROL;

	ymed = (y1 + y2) / 2;
	fact = m_w->DELTANbBAR[ pTaille ]-1 + m_fh->param.EpBarreMesure;
	xdec = m_w->ToZoom(fact);

	m_w->point_[0].x = m_w->ToZoom(x);
	m_w->point_[0].y = m_w->ToZoomY(y1);
	m_w->point_[1].x = m_w->ToZoom(x - m_w->_pas3);
	m_w->point_[1].y = m_w->point_[0].y - m_w->ToZoom( m_w->_interl[ pTaille ]*3);
	m_w->point_[3].x = m_w->ToZoom(x - m_w->_pas*2);
	m_w->point_[3].y = m_w->ToZoomY(ymed);
	m_w->point_[2].x = m_w->ToZoom(x + m_w->_pas);
	m_w->point_[2].y = m_w->point_[3].y + m_w->ToZoom( m_w->_interl[ pTaille ]);

	ptcoord = &m_w->bcoord[0];
	m_w->calcBez ( ptcoord, nbrInt );

	m_w->pntswap (&m_w->point_[0], &m_w->point_[3]);
	m_w->pntswap (&m_w->point_[1], &m_w->point_[2]);
	
	m_w->point_[1].x += xdec;
	m_w->point_[2].x += xdec;
	m_w->point_[1].y = m_w->point_[0].y + m_w->ToZoom( m_w->_interl[ pTaille ]*2);


	ptcoord = &m_w->bcoord[nbrInt+1];	// suite de la matrice: retour du bezier
	m_w->calcBez ( ptcoord, nbrInt );

	//SetPolyFillMode (hdc, WINDING);
	dc->DrawPolygon (nbrInt*2,  m_w->bcoord, 0, 0, wxWINDING_RULE ); //(sizeof (bcoord)*2) / sizeof (POINT)); nbrInt*2+ 1;

	// on produit l'image reflet vers le bas: 0 est identique 
	m_w->point_[1].y = m_w->point_[0].y - m_w->ToZoom( m_w->_interl[ pTaille ]*2);
	m_w->point_[3].y = m_w->ToZoomY(y2);
	m_w->point_[2].y = m_w->point_[3].y + m_w->ToZoom( m_w->_interl[ pTaille ]*3);

	ptcoord = &m_w->bcoord[0];
	m_w->calcBez ( ptcoord, nbrInt );

	m_w->pntswap (&m_w->point_[0], &m_w->point_[3]);
	m_w->pntswap (&m_w->point_[1], &m_w->point_[2]);
	
	m_w->point_[1].x -= xdec;
	m_w->point_[2].x -= xdec;
	m_w->point_[2].y = m_w->point_[3].y - m_w->ToZoom( m_w->_interl[ pTaille ]);

	ptcoord = &m_w->bcoord[nbrInt+1];	// suite de la matrice: retour du bezier 
	m_w->calcBez ( ptcoord, nbrInt );

	dc->DrawPolygon (nbrInt*2,  m_w->bcoord, 0, 0, wxWINDING_RULE  ); //(sizeof (bcoord)*2) / sizeof (POINT)); nbrInt*2+ 1;
	
	dc->SetBrush( wxNullBrush );
	dc->SetPen( wxNullPen );

	return;
}

void MusPage::bardroit ( wxDC *dc, int x, int y1, int y2, int pTaille)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	//if (!modMetafile || ( y2 < drawRect.top && y1 > drawRect.bottom && 
	//				 in (x, drawRect.left, drawRect.right)))
	{	 	
		m_w->v_bline( dc, y1, y2, x, m_p->EpBarreMesure);
		braces ( dc, x, y1, y2, 1, pTaille );
	}
	return;
}


void MusPage::bar_mes ( wxDC *dc, int x, int cod, int porteeAutonome, MusStaff *pportee)
// cod: 0 = barre d'epaisseur 1 point; 1 = barre d'ep. "epLignesVer"
// porteeAutonome: indique s'il faut des barres privees sur chaque portee plutôt que traversantes
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	int i, j, accDeb=0, flLine=0, portee;
	float a;
	float b, bb;
/*
	a = ((x >= wxmax) ? (wxmax - 2*h_pnt) : x);
	if (!in (a, wxg,wxd)) return;
*/
	/***if (modMetafile && !in (x, drawRect.left, drawRect.right) )
		return;***/

	a = x;

//	MESURECOL;
	//TUE_RECT;

	/***if (_shport)
		shportee (0);***/

	i=j= pportee->no;
	if (pportee->noGrp)
	{	
		while (i && (&this->m_staves[i])->vertBarre != DEBUT && (&this->m_staves[i])->vertBarre != DEB_FIN)
			--i;	/* position 1e portee du groupe */
		while (j<this->nbrePortees && (&this->m_staves[j])->vertBarre != FIN && (&this->m_staves[j])->vertBarre != DEB_FIN)
			++j;	/* position derniere portee du groupe */
	}
	b = bb = 0.0;

/*	if (discontinu)
		PenDash (discontinu);
*/
	MusStaff *st_i = NULL;

	for (; i <= j; i++)	// parcours du groupe de portees concernees 
	{
		st_i = &this->m_staves[i];

		int decPortType_i;
		if (st_i->portNbLine == 4) 
			decPortType_i = m_w->_interl[st_i->pTaille]*5;
		else
			decPortType_i = m_w->_portee[st_i->pTaille];
		
		// on calcule l'epaisseur de la portee courante
		if (st_i->portNbLine == 1 || st_i->portNbLine == 4)
			portee = m_w->_portee[ st_i->pTaille ]*2;
		else
			portee = m_w->_portee[ st_i->pTaille ] + ((st_i->portNbLine-1) * m_w->_interl[st_i->pTaille]);
		// on place les marqueurs DEB et FIN des barres
		if (porteeAutonome || st_i->brace == DEB_FIN || st_i->vertBarre == DEB_FIN
			|| !st_i->brace || !pportee->noGrp)
		{	if (!accDeb)
			{	b = m_w->kPos[i].yp - decPortType_i;
				bb = m_w->kPos[i].yp - portee;//_portee[st_ipTaille]*2;
				flLine = 1;
			}
		}
		else if (st_i->brace == DEBUT || st_i->vertBarre == DEBUT)
		{	b = m_w->kPos[i].yp - decPortType_i;
			bb = m_w->kPos[i].yp - portee;//_portee[st_ipTaille]*2;
			if (st_i->brace)
				accDeb = ON;
		}
		else if (st_i->brace == FIN || st_i->vertBarre == FIN)
		{	bb = m_w->kPos[i].yp - portee;//_portee[st_ipTaille]*2;
			flLine = 1;
			accDeb=0;
		}

		if (flLine)
		{	//if (b > drawRect.top && bb < drawRect.bottom)
			//	b = drawRect.top;

			//if (!modMetafile || ( bb < drawRect.top && b > drawRect.bottom))
			{	
				if (cod > 2)	// barres plus epaisses qu'un 1/2 mm
					m_w->v_bline2( dc, (int)b, (int)bb, (int)a, cod); 
				else
					m_w->v_bline( dc, (int)b, (int)bb, (int)a, cod);
			}
			flLine = 0;
		}


	}
	return;
}


void MusPage::bigbarre( wxDC *dc, int x, char code, int porteeAutonome, MusStaff *pportee)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;	
	
	int x1, x2, bardroite = 0;
	MusStaff *st = NULL;

	//if (modMetafile && !in (x, drawRect.left, drawRect.right) )
	//	return;

	if (code < 0)
	{	
		bardroite = 1;
		code = abs (code);
	}
	if ( in (code, 1, 64) )	/* accolade simple */
	{	
		st = pportee;
		while (this->GetNext( st ) && code > 0)
		{	
			st = this->GetNext( st );
			code--;
		}
		if (code == 0)
		{	
			x2 = st->yrel- m_w->_portee[ pportee->pTaille]*2;
			if (bardroite)
				bardroit ( dc, x, pportee->yrel- m_w->_portee[ pportee->pTaille ], x2, pportee->pTaille);
			else 
				accolade ( dc, x, pportee->yrel - m_w->_portee[pportee->pTaille ], x2, pportee->pTaille);
		}
		return;
	}
	x1 = x2 = 0;


	x1 = x - m_w->DELTABLANC[0] - m_w->DELTANbBAR[0] - m_p->EpBarreMesure;
	x2 = x + m_w->DELTABLANC[0] + m_w->DELTANbBAR[0] - m_p->EpBarreMesure;

 /* I, barre d'entree..., equivalente a barre de reprise ouvrante */

	if (code == 'R' || code == 'O' || code == 'I')
	/* une grosse barre entre deux minces ou une grosse+mince qui "Ouvre" */
		bar_mes ( dc, x2, m_p->EpBarreMesure, porteeAutonome, pportee);

	/* une grosse barre */
	if (code == 'E' || code == 'R' || code == 'F' || code == 'O' || code == 'I')
	{	bar_mes ( dc, x, m_w->DELTANbBAR[0], porteeAutonome, pportee);

	}
	if (code == 'D')	/* deux barres minces */
	{	bar_mes ( dc, x,  m_p->EpBarreMesure, porteeAutonome, pportee);
		bar_mes ( dc, x1 + m_w->DELTANbBAR[0],  m_p->EpBarreMesure, porteeAutonome, pportee);
	}
	if (code == 'R' || code == 'E' || code == 'F')
		bar_mes ( dc, x1,  m_p->EpBarreMesure, porteeAutonome, pportee);

	if (code == 'R' || code == 'F' || code == 'O')
	{	if (code == 'F')
			x2 = 0;
		if (code == 'O')
			x1 = 0;
		if (x1)
			x1 -= (m_w->_pas + (m_p->EpBarreMesure/2));
		if (x2)
			x2 += m_w->_pas;
		//putDeuxpoints ( dc, x1 , x2);
	}

}


void MusPage::barMesPartielle ( wxDC *dc, int x, MusStaff *pportee)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	int b, bb;

	MusStaff *next = this->GetNext( false );	
	if ( next )
	{	
		b = pportee->yrel - m_w->_portee[ pportee->pTaille ]*2;
		bb = next->yrel - m_w->_portee[ next->pTaille];

		if (m_p->EpBarreMesure > 2)	// barres plus epaisses qu'un 1/2 mm
			m_w->v_bline2 ( dc, b, bb, x,  m_p->EpBarreMesure);
		else
			m_w->v_bline ( dc, b, bb, x,  m_p->EpBarreMesure);
		
	}

}


