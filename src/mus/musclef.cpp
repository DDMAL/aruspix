/////////////////////////////////////////////////////////////////////////////
// Name:        musclef.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musclef.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "mussymbol.h"
#include "musrc.h"
#include "muspage.h"
#include "musfile.h"
#include "neumedef.h"

/* Calcule l'offset de la cle; doit etre separe de cle_id pour pouvoir
  etre appele depuis rd_symbol() en cas de changement de definition.
	La valeur attribuee est un facteur de multiplication d'espaces
  (espace[pTaille]) decrivant l'eloignement de la clef par rapport a
  la ligne du bas.
*/

void MusSymbol::calcoffs (int *offst, int clid)
{	
	*offst = 0;		// case 5: UT 1e ligne par default, valable pour PERC
	switch(clid)
	{	case SOL2 : *offst = -2; break;
		case SOL1 : *offst = -4; break;
		case SOLva : *offst = 5; break;
		case FA5 : *offst = 12; break;
		case FA4 : *offst = 10; break;
		case FA3 : *offst = 8; break;
		case UT2 : *offst = 2; break;
		case UT3 : *offst = 4; break;
		case UT5 : *offst = 8; break;
		case UT4 : *offst =  6;		
		default: break;
	}
	return;
}

// Verifie la clef en fonction de la position a, b du curseur. 
//   Retourne le decalage de la clef contenu dans struct poscle.
int MusStaff::testcle (int a)
{
	if ( !Check() )
		return 0;	

	int s;
	MusPosClef *pKey = &m_r->kPos[this->no];
	s = pKey->compte;

	if (s == 0)
		return (0);
	else if (*(pKey->posx+(--s)) < a)	// NB: s est decremente 
		return(*(pKey->dec+s));
	else
		while(*(pKey->posx+s) > a)
			s--;
	return(in(s,0,MAXCLE) ? *(pKey->dec+s) : 0);
}



void MusSymbol::dess_cle ( AxDC *dc, int i, MusStaff *pportee)
// int i;	indice de position clef in poscle
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;		
	
	int b = (pportee->yrel- m_r->_portee[ pportee->pTaille ]);
	int a = this->xrel;
	unsigned int dim = dimin,
			 sym = sSOL;	//sSOL, position d'ordre des cles sol fa ut in fonts

	dimin = this->dimin;

	if (pportee->portNbLine > 5)
		b -= ((pportee->portNbLine - 5) * 2) *m_r->_espace[ pportee->pTaille ];

/*  poser sym=no de position sSOL dans la fonte
 *	au depart; ne faire operation sur b qu'une fois pour cas semblables,
 *  et au palier commun superieur, incrementer sym, sans break.
 */
	wxString sym2;
	switch(this->code)	// cleid
	{
		case UT1 : sym += 2;
		case SOL1 : b -= m_r->_portee[ pportee->pTaille ]; break;
		case SOLva : sym += 1;
		case UT2 : sym += 2;
		case SOL2 : b -= m_r->_interl[ pportee->pTaille ]*3; break;
		case FA3 : sym--;
		case UT3 : b -= m_r->_interl[ pportee->pTaille ]*2; sym += 2; break;
		case FA5 : sym++; break;
		case FA4 : sym--;
		case UT4 : b -= m_r->_interl[ pportee->pTaille ];
		case UT5 :  sym += 2; break;
		case CLEPERC :  b -= m_r->_interl[ pportee->pTaille ]*2;
					sym = sPERC; break;
		default: break;
	}

	a -= m_r->_pas*2;
	if (dimin)	// && ptRul->defin<3)
		a+= m_r->_pas;

	//if ((!this->ElemInvisible || illumine) && (!modMetafile || in (chk->xrel, drawRect.left, drawRect.right) && in (b, drawRect.top, drawRect.bottom)))
	m_r->putfont ( dc,a,b,(int)sym, pportee, this->dimin, SYMB );
	dimin = dim;

	char dum = 0;
	oct = -pportee->getOctCl (this, &dum);

	// on met a jour struct poscle 
	pportee->updat_pscle (i,this);



	return;
}


