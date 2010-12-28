/////////////////////////////////////////////////////////////////////////////
// Name:        muswindow.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musrc.h"
#endif

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "musrc.h"
#include "muspage.h"
#include "musfile.h"


void MusPage::braces ( AxDC *dc, int x, int y1, int y2, int cod, int pTaille)
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
        dc->SetPen( m_r->m_currentColour , 1, wxSOLID );
        dc->SetBrush( m_r->m_currentColour , wxTRANSPARENT );

		ecart = m_r->DELTABAR;
		centre = x - ecart;
		
		xg = centre - ecart*2;
		xd = centre + ecart*2;
		
		yg = y1 + m_r->_interl[ pTaille ] * 2;
		yd = y1;
		m_r->SwapY( &yg, &yd );
		dc->DrawEllipticArc( m_r->ToZoom(xg), m_r->ToZoomY(yg), m_r->ToZoom(xd-xg), m_r->ToZoom(yg-yd),272, 360 );
	
		yg = y2;
		yd = y2 - m_r->_interl[ pTaille ] * 2;
		m_r->SwapY( &yg, &yd );
		dc->DrawEllipticArc( m_r->ToZoom(xg), m_r->ToZoomY(yg), m_r->ToZoom(xd-xg), m_r->ToZoom(yg-yd), 0, 88 );
		
        dc->ResetPen();
        dc->ResetBrush();

		xg = x - (m_r->DELTABLANC[0] + 1);
		// determine le blanc entre barres grosse et mince
		m_r->v_bline2( dc, y1, y2, xg, m_r->DELTANbBAR[0]);
	}
	//if (cod)
		//v_bline(dc, y1, y2, x, m_fh->param.EpBarreMesure);

	return;
}


void MusPage::DrawBarres( AxDC *dc )
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
			decPortType_i = m_r->_interl[staff->pTaille]*5;
		else
			decPortType_i = m_r->_portee[staff->pTaille];
		xx = staff->indent ? staff->indent*10 : 0;

		if (staff->portNbLine == 1 || staff->portNbLine == 4)
			portee = m_r->_portee[ staff->pTaille ]*2;
		else
			portee = m_r->_portee[ staff->pTaille ] + ((staff->portNbLine-1) * m_r->_interl[staff->pTaille]);

		if (staff->vertBarre == DEBUT)
			b_gr = (int)(m_r->kPos[i].yp - decPortType_i);
		// key[i].yp est position du curseur par default _portee (4 interl) au-dessus de ligne superieure
		// decporttyp est la valeur de remplacement de _portee si on veut autre espace
		else if (staff->vertBarre == DEB_FIN)
		{	b_gr = (int)(m_r->kPos[i].yp - decPortType_i);
			bb_gr = (int)(m_r->kPos[i].yp - portee);//_portee[staff->pTaille]*2;
			flLine = 1;
		}
		else if (staff->vertBarre == FIN)
		{	bb_gr = (int)(m_r->kPos[i].yp - portee);//_portee[staff->pTaille]*2;
			flLine = 1;
		}
		if (staff->brace == DEBUT)
			b_acc = (int)(m_r->kPos[i].yp - decPortType_i);
		else if (staff->brace == DEB_FIN)
		{	b_acc = (int)(m_r->kPos[i].yp - decPortType_i);
			bb_acc = (int)(m_r->kPos[i].yp - portee);//_portee[staff->pTaille]*2;
			flBrace = 1;
		}
		else if (staff->brace == FIN)
		{	bb_acc = (int)(m_r->kPos[i].yp - portee);//_portee[staff->pTaille]*2;
			flBrace = 1;
		}

		//if (!modMetafile || ( bb_gr < drawRect.top && b_gr > drawRect.bottom && 
		//			 in (xx, drawRect.left, drawRect.right)))
			if (flLine)
			{ 	
				if (m_p->EpBarreMesure > 2)
					m_r->v_bline2( dc ,b_gr,bb_gr,xx, m_p->EpBarreMesure);
				else
					m_r->v_bline( dc ,b_gr,bb_gr,xx, m_p->EpBarreMesure);
				flLine = 0;
			}

		//if (!modMetafile || ( bb_acc < drawRect.top && b_acc > drawRect.bottom && 
		//			 in (xx, drawRect.left, drawRect.right)))
		if (flBrace)
		{		if (staff->accol)
					braces ( dc, xx- m_r->_pas/2 , b_acc, bb_acc, 2, staff->pTaille);
				else
					braces ( dc, xx,b_acc,bb_acc, 1, staff->pTaille);
			flBrace = 0;
		}

		//if (_encontinu && staff->vertBarre > 1) // > DEBUT
		//	break;
	}

	return;
}



void MusPage::accolade ( AxDC *dc, int x, int y1, int y2, int pTaille)
{	
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	m_r->SwapY( &y1, &y2 );
	
	int ymed, xdec, fact, nbrInt;
//	static POINT *bcoord;
	AxPoint *ptcoord;

    dc->SetPen( m_r->m_currentColour , 1, wxSOLID );
    dc->SetBrush( m_r->m_currentColour , wxSOLID );

	x -= m_r->DELTABLANC[ pTaille ];  // distance entre barre et debut accolade

	nbrInt = PTCONTROL;

	ymed = (y1 + y2) / 2;
	fact = m_r->DELTANbBAR[ pTaille ]-1 + m_fh->param.EpBarreMesure;
	xdec = m_r->ToZoom(fact);

	m_r->point_[0].x = m_r->ToZoom(x);
	m_r->point_[0].y = m_r->ToZoomY(y1);
	m_r->point_[1].x = m_r->ToZoom(x - m_r->_pas3);
	m_r->point_[1].y = m_r->point_[0].y - m_r->ToZoom( m_r->_interl[ pTaille ]*3);
	m_r->point_[3].x = m_r->ToZoom(x - m_r->_pas*2);
	m_r->point_[3].y = m_r->ToZoomY(ymed);
	m_r->point_[2].x = m_r->ToZoom(x + m_r->_pas);
	m_r->point_[2].y = m_r->point_[3].y + m_r->ToZoom( m_r->_interl[ pTaille ]);

	ptcoord = &m_r->bcoord[0];
	m_r->calcBez ( ptcoord, nbrInt );

	m_r->pntswap (&m_r->point_[0], &m_r->point_[3]);
	m_r->pntswap (&m_r->point_[1], &m_r->point_[2]);
	
	m_r->point_[1].x += xdec;
	m_r->point_[2].x += xdec;
	m_r->point_[1].y = m_r->point_[0].y + m_r->ToZoom( m_r->_interl[ pTaille ]*2);


	ptcoord = &m_r->bcoord[nbrInt+1];	// suite de la matrice: retour du bezier
	m_r->calcBez ( ptcoord, nbrInt );

	//SetPolyFillMode (hdc, WINDING);
	dc->DrawPolygon (nbrInt*2,  m_r->bcoord, 0, 0, wxWINDING_RULE ); //(sizeof (bcoord)*2) / sizeof (POINT)); nbrInt*2+ 1;

	// on produit l'image reflet vers le bas: 0 est identique 
	m_r->point_[1].y = m_r->point_[0].y - m_r->ToZoom( m_r->_interl[ pTaille ]*2);
	m_r->point_[3].y = m_r->ToZoomY(y2);
	m_r->point_[2].y = m_r->point_[3].y + m_r->ToZoom( m_r->_interl[ pTaille ]*3);

	ptcoord = &m_r->bcoord[0];
	m_r->calcBez ( ptcoord, nbrInt );

	m_r->pntswap (&m_r->point_[0], &m_r->point_[3]);
	m_r->pntswap (&m_r->point_[1], &m_r->point_[2]);
	
	m_r->point_[1].x -= xdec;
	m_r->point_[2].x -= xdec;
	m_r->point_[2].y = m_r->point_[3].y - m_r->ToZoom( m_r->_interl[ pTaille ]);

	ptcoord = &m_r->bcoord[nbrInt+1];	// suite de la matrice: retour du bezier 
	m_r->calcBez ( ptcoord, nbrInt );

	dc->DrawPolygon (nbrInt*2,  m_r->bcoord, 0, 0, wxWINDING_RULE  ); //(sizeof (bcoord)*2) / sizeof (POINT)); nbrInt*2+ 1;
	
    dc->ResetPen();
    dc->ResetBrush();

	return;
}

void MusPage::bardroit ( AxDC *dc, int x, int y1, int y2, int pTaille)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	//if (!modMetafile || ( y2 < drawRect.top && y1 > drawRect.bottom && 
	//				 in (x, drawRect.left, drawRect.right)))
	{	 	
		m_r->v_bline( dc, y1, y2, x, m_p->EpBarreMesure);
		braces ( dc, x, y1, y2, 1, pTaille );
	}
	return;
}


void MusPage::bar_mes ( AxDC *dc, int x, int cod, int porteeAutonome, MusStaff *pportee)
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
			decPortType_i = m_r->_interl[st_i->pTaille]*5;
		else
			decPortType_i = m_r->_portee[st_i->pTaille];
		
		// on calcule l'epaisseur de la portee courante
		if (st_i->portNbLine == 1 || st_i->portNbLine == 4)
			portee = m_r->_portee[ st_i->pTaille ]*2;
		else
			portee = m_r->_portee[ st_i->pTaille ] + ((st_i->portNbLine-1) * m_r->_interl[st_i->pTaille]);
		// on place les marqueurs DEB et FIN des barres
		if (porteeAutonome || st_i->brace == DEB_FIN || st_i->vertBarre == DEB_FIN
			|| !st_i->brace || !pportee->noGrp)
		{	if (!accDeb)
			{	b = m_r->kPos[i].yp - decPortType_i;
				bb = m_r->kPos[i].yp - portee;//_portee[st_ipTaille]*2;
				flLine = 1;
			}
		}
		else if (st_i->brace == DEBUT || st_i->vertBarre == DEBUT)
		{	b = m_r->kPos[i].yp - decPortType_i;
			bb = m_r->kPos[i].yp - portee;//_portee[st_ipTaille]*2;
			if (st_i->brace)
				accDeb = ON;
		}
		else if (st_i->brace == FIN || st_i->vertBarre == FIN)
		{	bb = m_r->kPos[i].yp - portee;//_portee[st_ipTaille]*2;
			flLine = 1;
			accDeb=0;
		}

		if (flLine)
		{	//if (b > drawRect.top && bb < drawRect.bottom)
			//	b = drawRect.top;

			//if (!modMetafile || ( bb < drawRect.top && b > drawRect.bottom))
			{	
				if (cod > 2)	// barres plus epaisses qu'un 1/2 mm
					m_r->v_bline2( dc, (int)b, (int)bb, (int)a, cod); 
				else
					m_r->v_bline( dc, (int)b, (int)bb, (int)a, cod);
			}
			flLine = 0;
		}


	}
	return;
}


void MusPage::bigbarre( AxDC *dc, int x, char code, int porteeAutonome, MusStaff *pportee)
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
			x2 = st->yrel- m_r->_portee[ pportee->pTaille]*2;
			if (bardroite)
				bardroit ( dc, x, pportee->yrel- m_r->_portee[ pportee->pTaille ], x2, pportee->pTaille);
			else 
				accolade ( dc, x, pportee->yrel - m_r->_portee[pportee->pTaille ], x2, pportee->pTaille);
		}
		return;
	}
	x1 = x2 = 0;


	x1 = x - m_r->DELTABLANC[0] - m_r->DELTANbBAR[0] - m_p->EpBarreMesure;
	x2 = x + m_r->DELTABLANC[0] + m_r->DELTANbBAR[0] - m_p->EpBarreMesure;

 /* I, barre d'entree..., equivalente a barre de reprise ouvrante */

	if (code == 'R' || code == 'O' || code == 'I')
	/* une grosse barre entre deux minces ou une grosse+mince qui "Ouvre" */
		bar_mes ( dc, x2, m_p->EpBarreMesure, porteeAutonome, pportee);

	/* une grosse barre */
	if (code == 'E' || code == 'R' || code == 'F' || code == 'O' || code == 'I')
	{	bar_mes ( dc, x, m_r->DELTANbBAR[0], porteeAutonome, pportee);

	}
	if (code == 'D')	/* deux barres minces */
	{	bar_mes ( dc, x,  m_p->EpBarreMesure, porteeAutonome, pportee);
		bar_mes ( dc, x1 + m_r->DELTANbBAR[0],  m_p->EpBarreMesure, porteeAutonome, pportee);
	}
	if (code == 'R' || code == 'E' || code == 'F')
		bar_mes ( dc, x1,  m_p->EpBarreMesure, porteeAutonome, pportee);

	if (code == 'R' || code == 'F' || code == 'O')
	{	if (code == 'F')
			x2 = 0;
		if (code == 'O')
			x1 = 0;
		if (x1)
			x1 -= (m_r->_pas + (m_p->EpBarreMesure/2));
		if (x2)
			x2 += m_r->_pas;
		//putDeuxpoints ( dc, x1 , x2);
	}

}


void MusPage::barMesPartielle ( AxDC *dc, int x, MusStaff *pportee)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	int b, bb;

	MusStaff *next = this->GetNext( false );	
	if ( next )
	{	
		b = pportee->yrel - m_r->_portee[ pportee->pTaille ]*2;
		bb = next->yrel - m_r->_portee[ next->pTaille];

		if (m_p->EpBarreMesure > 2)	// barres plus epaisses qu'un 1/2 mm
			m_r->v_bline2 ( dc, b, bb, x,  m_p->EpBarreMesure);
		else
			m_r->v_bline ( dc, b, bb, x,  m_p->EpBarreMesure);
		
	}

}


