/////////////////////////////////////////////////////////////////////////////
// Name:        musrc_neumes.cpp
// Author:      Laurent Pugin and Chris Niven
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musrc.h"
#include "muslayout.h"
#include "muspage.h"
#include "mussystem.h"
#include "muslaidoutstaff.h"
#include "muslaidoutlayer.h"

#include "musclef.h"

//----------------------------------------------------------------------------
// MusRC - MusPage
//----------------------------------------------------------------------------

void MusRC::DrawPage( MusDC *dc, MusPage *page, bool background ) 
{
	wxASSERT_MSG( dc , "DC cannot be NULL");

	if ( !page->Check() )
		return;
    
    page->UpdateSystemPositions();

	int i;
	MusSystem *system = NULL;

    if ( background )
        dc->DrawRectangle( 0, 0, m_layout->m_pageWidth, m_layout->m_pageHeight );
    
    MusPoint origin = dc->GetLogicalOrigin();
    dc->SetLogicalOrigin( origin.x - m_layout->m_leftMargin, origin.y );

    dc->StartPage();

    for (i = 0; i < page->GetSystemCount(); i++) 
	{
		system = &page->m_systems[i];
        DrawSystem( dc, system );
        
        // TODO here: also update xrel and yrel positions for system. How to calculate them?
    }
    
    dc->EndPage();
}

//----------------------------------------------------------------------------
// MusRC - MusSystem
//----------------------------------------------------------------------------


// drawing


void MusRC::DrawSystem( MusDC *dc, MusSystem *system ) 
{
	wxASSERT_MSG( dc , "DC cannot be NULL");

	if ( !system->Check() )
		return;

	int i;
    MusLaidOutStaff *staff;
    
    /*
    dc->SetPen( AxRED );
    dc->DrawLine( system->m_xrel, ToRendererY(system->m_yrel), system->m_xrel + 10, ToRendererY(system->m_yrel) );
    dc->ResetPen();
    */
    
    dc->StartGraphic( system, "system", wxString::Format("system_%d", system->GetId() ) );
    
    DrawGroups( dc, system );

    for (i = 0; i < (int)system->m_staves.GetCount(); i++) 
	{
		staff = &system->m_staves[i];
        DrawStaff( dc , staff, system );		
	}
    
    dc->EndGraphic(system); //RZ

}

void MusRC::DrawBracket ( MusDC *dc, MusSystem *system, int x, int y1, int y2, int cod, int staffSize)
//	int x, y1, y2;	x, 1e et 2e y de barre vert
//	int cod; si ON, on fait 2e barre vert. mince en position  x
{
	if ( !system->Check() )
		return;

	int xg, xd, yg, yd, ecart, centre;
 	//HPEN hPen;
  
	//discontinu = 0;

	if (cod == 2)
	{
		DrawBrace ( dc, system, x, y1, y2, staffSize);
		return;
	}
	//else if (hdc)
	{
        dc->SetPen( m_currentColour , 1, wxSOLID );
        dc->SetBrush( m_currentColour , wxTRANSPARENT );

		ecart = m_layout->m_barlineSpacing;
		centre = x - ecart;
		
		xg = centre - ecart*2;
		xd = centre + ecart*2;
		
		yg = y1 + m_layout->m_interl[ staffSize ] * 2;
		yd = y1;
		SwapY( &yg, &yd );
		dc->DrawEllipticArc( ToRendererX(xg), ToRendererY(yg), ToRendererX(xd-xg), ToRendererX(yg-yd), 88, 0 );
	
		yg = y2;
		yd = y2 - m_layout->m_interl[ staffSize ] * 2;
		SwapY( &yg, &yd );
		dc->DrawEllipticArc( ToRendererX(xg), ToRendererY(yg), ToRendererX(xd-xg), ToRendererX(yg-yd), 360, 272 );
		
        dc->ResetPen();
        dc->ResetBrush();

		xg = x - (m_layout->m_beamWhiteWidth[0] + 1);
		// determine le blanc entre barres grosse et mince
		v_bline2( dc, y1, y2, xg, m_layout->m_beamWidth[0]);
	}
	//if (cod)
		//v_bline(dc, y1, y2, x, m_layout->m_parameters.m_barlineWidth);

	return;
}


void MusRC::DrawGroups( MusDC *dc, MusSystem *system )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !system->Check() )
		return;

	int i, flLine=0, flBrace=0;
	int xx, portee;
	int b_gr, bb_gr;
	int b_acc, bb_acc;               
	//extern int _encontinu;

    MusLaidOutStaff *staff;

// 	if ((wxg+margeMorteHor) > (staff->indent ? portIndent : portNoIndent))
 	//if ((wxg+margeMorteHor) > max (portIndent, portNoIndent))
 	//	return;
//	SYMBOLCOL;
//  MESURECOL;

	b_gr = bb_gr = b_acc = bb_acc = 0;                

    for (i = 0; i < system->GetStaffCount(); i++) 
	{
		int decPortType_i;
		staff =  &system->m_staves[i];
		//if (staff->portNbLine == 4) 
		//	decPortType_i = m_interl[staff->staffSize]*5;
		//else
			decPortType_i = m_layout->m_staffSize[staff->staffSize];
		xx = system->indent ? system->indent*10 : 0;

		//if (staff->portNbLine == 1 || staff->portNbLine == 4)
		//	portee = m_staffSize[ staff->staffSize ]*2;
		//else
        portee = m_layout->m_staffSize[ staff->staffSize ] + ((staff->portNbLine-1) * m_layout->m_interl[staff->staffSize]);

		if (staff->vertBarre == START)
			b_gr = (int)(staff->yrel - decPortType_i);
		// key[i].yp est position du curseur par default m_staffSize (4 interl) au-dessus de ligne superieure
		// decporttyp est la valeur de remplacement de m_staffSize si on veut autre espace
		else if (staff->vertBarre == START_END)
		{	b_gr = (int)(staff->yrel - decPortType_i);
			bb_gr = (int)(staff->yrel - portee);//m_staffSize[staff->staffSize]*2;
			flLine = 1;
		}
		else if (staff->vertBarre == END)
		{	bb_gr = (int)(staff->yrel - portee);//m_staffSize[staff->staffSize]*2;
			flLine = 1;
		}
		if (staff->brace == START)
			b_acc = (int)(staff->yrel - decPortType_i);
		else if (staff->brace == START_END)
		{	b_acc = (int)(staff->yrel - decPortType_i);
			bb_acc = (int)(staff->yrel - portee);//m_staffSize[staff->staffSize]*2;
			flBrace = 1;
		}
		else if (staff->brace == END)
		{	bb_acc = (int)(staff->yrel - portee);//m_staffSize[staff->staffSize]*2;
			flBrace = 1;
		}

		//if (!modMetafile || ( bb_gr < drawRect.top && b_gr > drawRect.bottom && 
		//			 in (xx, drawRect.left, drawRect.right)))
			if (flLine)
			{ 	      
                if (m_layout->m_env.m_barlineWidth > 2)
					v_bline2( dc ,b_gr,bb_gr,xx, m_layout->m_env.m_barlineWidth);
				else
					v_bline( dc ,b_gr,bb_gr,xx, m_layout->m_env.m_barlineWidth);
				flLine = 0;
			}

		//if (!modMetafile || ( bb_acc < drawRect.top && b_acc > drawRect.bottom && 
		//			 in (xx, drawRect.left, drawRect.right)))
		if (flBrace)
		{		if (staff->accol)
					DrawBracket ( dc, system, xx- m_layout->m_step1/2 , b_acc, bb_acc, 2, staff->staffSize);
				else
					DrawBracket ( dc, system, xx,b_acc,bb_acc, 1, staff->staffSize);
			flBrace = 0;
		}

		//if (_encontinu && staff->vertBarre > 1) // > START
		//	break;
	}
    
	return;
}



void MusRC::DrawBrace ( MusDC *dc, MusSystem *system, int x, int y1, int y2, int staffSize)
{	
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !system->Check() )
		return;

	SwapY( &y1, &y2 );
	
	int ymed, xdec, fact, nbrInt;
//	static POINT *bcoord;
	MusPoint *ptcoord;

    dc->SetPen( m_currentColour , 1, wxSOLID );
    dc->SetBrush( m_currentColour , wxSOLID );

	x -= m_layout->m_beamWhiteWidth[ staffSize ];  // distance entre barre et debut accolade

	nbrInt = BEZIER_NB_POINTS;

	ymed = (y1 + y2) / 2;
	fact = m_layout->m_beamWidth[ staffSize ]-1 + m_layout->m_env.m_barlineWidth;
	xdec = ToRendererX(fact);

	point_[0].x = ToRendererX(x);
	point_[0].y = ToRendererY(y1);
	point_[1].x = ToRendererX(x - m_layout->m_step2);
	point_[1].y = point_[0].y - ToRendererX( m_layout->m_interl[ staffSize ]*3);
	point_[3].x = ToRendererX(x - m_layout->m_step1*2);
	point_[3].y = ToRendererY(ymed);
	point_[2].x = ToRendererX(x + m_layout->m_step1);
	point_[2].y = point_[3].y + ToRendererX( m_layout->m_interl[ staffSize ]);

	ptcoord = &bcoord[0];
	calcBez ( ptcoord, nbrInt );

	pntswap (&point_[0], &point_[3]);
	pntswap (&point_[1], &point_[2]);
	
	point_[1].x += xdec;
	point_[2].x += xdec;
	point_[1].y = point_[0].y + ToRendererX( m_layout->m_interl[ staffSize ]*2);


	ptcoord = &bcoord[nbrInt+1];	// suite de la matrice: retour du bezier
	calcBez ( ptcoord, nbrInt );

	//SetPolyFillMode (hdc, WINDING);
	dc->DrawPolygon (nbrInt*2,  bcoord, 0, 0, wxWINDING_RULE ); //(sizeof (bcoord)*2) / sizeof (POINT)); nbrInt*2+ 1;

	// on produit l'image reflet vers le bas: 0 est identique 
	point_[1].y = point_[0].y - ToRendererX( m_layout->m_interl[ staffSize ]*2);
	point_[3].y = ToRendererY(y2);
	point_[2].y = point_[3].y + ToRendererX( m_layout->m_interl[ staffSize ]*3);

	ptcoord = &bcoord[0];
	calcBez ( ptcoord, nbrInt );

	pntswap (&point_[0], &point_[3]);
	pntswap (&point_[1], &point_[2]);
	
	point_[1].x -= xdec;
	point_[2].x -= xdec;
	point_[2].y = point_[3].y - ToRendererX( m_layout->m_interl[ staffSize ]);

	ptcoord = &bcoord[nbrInt+1];	// suite de la matrice: retour du bezier 
	calcBez ( ptcoord, nbrInt );

	dc->DrawPolygon (nbrInt*2,  bcoord, 0, 0, wxWINDING_RULE  ); //(sizeof (bcoord)*2) / sizeof (POINT)); nbrInt*2+ 1;
	
    dc->ResetPen();
    dc->ResetBrush();

	return;
}

void MusRC::DrawBarline ( MusDC *dc, MusSystem *system, int x, int cod, bool porteeAutonome, MusLaidOutStaff *pportee)
// cod: 0 = barre d'epaisseur 1 point; 1 = barre d'ep. "epLignesVer"
// porteeAutonome: indique s'il faut des barres privees sur chaque portee plutÙt que traversantes
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !system->Check() )
		return;

	int i, j, accDeb=0, flLine=0, portee;
	float a;
	float b, bb;
/*
	a = ((x >= m_pageMaxX) ? (m_pageMaxX - 2*h_pnt) : x);
	if (!in (a, wxg,wxd)) return;
*/
	/***if (modMetafile && !in (x, drawRect.left, drawRect.right) )
		return;***/

	a = x;

//	MESURECOL;
	//TUE_RECT;

	/***if (_shport)
		shportee (0);***/

	i=j= pportee->GetStaffNo();
	if (pportee->noGrp)
	{	
		while (i && (&system->m_staves[i])->vertBarre != START && (&system->m_staves[i])->vertBarre != START_END)
			--i;	// position 1e portee du groupe 
		while (j<system->GetStaffCount() && (&system->m_staves[j])->vertBarre != END && (&system->m_staves[j])->vertBarre != START_END)
			++j;	// position derniere portee du groupe 
	}
	b = bb = 0.0;

	MusLaidOutStaff *st_i = NULL;

	for (; i <= j; i++)	// parcours du groupe de portees concernees 
	{
		st_i = &system->m_staves[i];

		int decPortType_i;
		if (st_i->portNbLine == 4) 
			decPortType_i = m_layout->m_interl[st_i->staffSize]*5;
		else
			decPortType_i = m_layout->m_staffSize[st_i->staffSize];
		
		// on calcule l'epaisseur de la portee courante
		if (st_i->portNbLine == 1 || st_i->portNbLine == 4)
			portee = m_layout->m_staffSize[ st_i->staffSize ]*2;
		else
			portee = m_layout->m_staffSize[ st_i->staffSize ] + ((st_i->portNbLine-1) * m_layout->m_interl[st_i->staffSize]);
		// on place les marqueurs DEB et END des barres
		if (porteeAutonome || st_i->brace == START_END || st_i->vertBarre == START_END
			|| !st_i->brace || !pportee->noGrp)
		{	if (!accDeb)
			{	b = st_i->yrel - decPortType_i;
				bb = st_i->yrel - portee;//m_staffSize[st_ipTaille]*2;
				flLine = 1;
			}
		}
		else if (st_i->brace == START || st_i->vertBarre == START)
		{	b = st_i->yrel - decPortType_i;
			bb = st_i->yrel - portee;//m_staffSize[st_ipTaille]*2;
			if (st_i->brace)
				accDeb = ON;
		}
		else if (st_i->brace == END || st_i->vertBarre == END)
		{	bb = st_i->yrel - portee;//m_staffSize[st_ipTaille]*2;
			flLine = 1;
			accDeb=0;
		}

		if (flLine)
		{	//if (b > drawRect.top && bb < drawRect.bottom)
			//	b = drawRect.top;

			//if (!modMetafile || ( bb < drawRect.top && b > drawRect.bottom))
			{	
				if (cod > 2)	// barres plus epaisses qu'un 1/2 mm
					v_bline2( dc, (int)b, (int)bb, (int)a, cod); 
				else
					v_bline( dc, (int)b, (int)bb, (int)a, cod);
			}
			flLine = 0;
		}


	}
    
	return;
}


void MusRC::DrawSpecialBarline( MusDC *dc, MusSystem *system, int x, BarlineType code, bool porteeAutonome, MusLaidOutStaff *pportee)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !system->Check() )
		return;	
	
	int x1, x2;

	//if (modMetafile && !in (x, drawRect.left, drawRect.right) )
	//	return;

    /*
    int bardroite = 0;
	MusLaidOutStaff *st = NULL;
    
	if (code < 0) // -1 = bracket 
	{	
		bardroite = 1;
		code = abs (code); // -1 becomes 1
	}
	if ( in (code, 1, 64) )	// accolade simple
	{	
		st = pportee;
		while (system->GetNext( st ) && code > 0) // apparently designed to span over more than 2 staves
		{	
			st = system->GetNext( st );
			code--;
		}
		if (code == 0)
		{	
			int y1 =  pportee->yrel- m_staffSize[ pportee->staffSize ];
            int y2 = st->yrel- m_staffSize[ pportee->staffSize]*2;
			if (bardroite) {
                v_bline( dc, y1, y2, x, m_layout->m_parameters.m_barlineWidth);
                DrawBracket ( dc, system, x, y1, y2, 1, pportee->staffSize );
            }
			else {
				DrawBrace ( dc, system, x, y1, y2, pportee->staffSize);
            }
		}
		return;
	}
    */ // ax2 no support of brace (1) and brackets (-1) barlines
	x1 = x2 = 0;


	x1 = x - m_layout->m_beamWhiteWidth[0] - m_layout->m_beamWidth[0] - m_layout->m_env.m_barlineWidth;
	x2 = x + m_layout->m_beamWhiteWidth[0] + m_layout->m_beamWidth[0] - m_layout->m_env.m_barlineWidth;

 /* I, barre d'entree..., equivalente a barre de reprise ouvrante */

	if (code == BARLINE_RPTBOTH || code == BARLINE_RPTSTART || code == BARLINE_START)
	/* une grosse barre entre deux minces ou une grosse+mince qui "Ouvre" */
		DrawBarline ( dc, system, x2, m_layout->m_env.m_barlineWidth, porteeAutonome, pportee);

	/* une grosse barre */
	if (code == BARLINE_END || code == BARLINE_RPTBOTH || code == BARLINE_RPTEND || code == BARLINE_RPTSTART || code == BARLINE_START)
	{	DrawBarline ( dc, system, x, m_layout->m_beamWidth[0], porteeAutonome, pportee);

	}
	if (code == BARLINE_DBL)	/* deux barres minces */
	{	DrawBarline ( dc, system, x,  m_layout->m_env.m_barlineWidth, porteeAutonome, pportee);
		DrawBarline ( dc, system, x1 + m_layout->m_beamWidth[0],  m_layout->m_env.m_barlineWidth, porteeAutonome, pportee);
	}
	if (code == BARLINE_RPTBOTH || code == BARLINE_END || code == BARLINE_RPTEND)
		DrawBarline ( dc, system, x1,  m_layout->m_env.m_barlineWidth, porteeAutonome, pportee);

	if (code == BARLINE_RPTBOTH || code == BARLINE_RPTEND || code == BARLINE_RPTSTART)
	{	if (code == BARLINE_RPTEND)
			x2 = 0;
		if (code == BARLINE_RPTSTART)
			x1 = 0;
		if (x1)
			x1 -= (m_layout->m_step1 + (m_layout->m_env.m_barlineWidth/2));
		if (x2)
			x2 += m_layout->m_step1;
		//putDeuxpoints ( dc, x1 , x2);
	}

}


void MusRC::DrawPartialBarline ( MusDC *dc, MusSystem *system, int x, MusLaidOutStaff *pportee)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !system->Check() )
		return;

	int b, bb;

	MusLaidOutStaff *next = system->GetNext( false );	
	if ( next )
	{	
		b = pportee->yrel - m_layout->m_staffSize[ pportee->staffSize ]*2;
		bb = next->yrel - m_layout->m_staffSize[ next->staffSize];

		if (m_layout->m_env.m_barlineWidth > 2)	// barres plus epaisses qu'un 1/2 mm
			v_bline2 ( dc, b, bb, x,  m_layout->m_env.m_barlineWidth);
		else
			v_bline ( dc, b, bb, x,  m_layout->m_env.m_barlineWidth);
		
	}

}


//----------------------------------------------------------------------------
// MusRC - MusLaidOutStaff
//----------------------------------------------------------------------------

int MusRC::CalculateNeumePosY ( MusLaidOutStaff *staff, char note, int dec_clef, int oct)
{
    wxASSERT_MSG( staff, "Pointer to staff cannot be NULL" );

	static char notes[] = {1,2,3,4,5,6,7};
	int y_int;
	char *pnote, i;
	pnote = &notes[0] - 1;
	
	y_int = ((dec_clef + oct*7) - 17 ) * m_layout->m_halfInterl[staff->staffSize];
	if (staff->portNbLine > 4)
		y_int -= ((staff->portNbLine - 4) * 2) * m_layout->m_halfInterl[staff->staffSize];
	
	for (i=0; i<(signed)sizeof(notes); i++)
		if (*(pnote+i) == note)
			return(y_int += (i*m_layout->m_halfInterl[staff->staffSize]));
	return 0;
}

int MusRC::CalculatePitchPosY ( MusLaidOutStaff *staff, char pname, int dec_clef, int oct)
{
    wxASSERT_MSG( staff, "Pointer to staff cannot be NULL" );
	
    static char touches[] = {PITCH_C,PITCH_D,PITCH_E,PITCH_F,PITCH_G,PITCH_A,PITCH_B};
	int y_int;
	char *ptouche, i;
	ptouche=&touches[0];

	y_int = ((dec_clef + oct*7) - 17 ) * m_layout->m_halfInterl[staff->staffSize];
	if (staff->portNbLine > 5)
		y_int -= ((staff->portNbLine - 5) * 2) * m_layout->m_halfInterl[staff->staffSize];

	/* exprime distance separant yrel de
	position 1e Si, corrigee par dec_clef et oct. Elle est additionnee
	ensuite, donc elle doit etre NEGATIVE si plus bas que yrel */
	for (i=0; i<(signed)sizeof(touches); i++)
		if (*(ptouche+i) == pname)
			return(y_int += ((i+1)*m_layout->m_halfInterl[staff->staffSize]));
	return 0;
}

void MusRC::DrawStaffLines( MusDC *dc, MusLaidOutStaff *staff, MusSystem *system )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !staff->Check() )
		return;
        
    if (staff->invisible)
        return;

	int j, x1, x2, yy;

	yy = staff->yrel;
    
    /*
    dc->SetPen( AxRED );
    dc->DrawLine( system->m_xrel, ToRendererY(staff->yrel), system->m_xrel + 10, ToRendererY(staff->yrel) );
    dc->ResetPen();
    */
    
    if ( staff->portNbLine == 1 )
		yy  -= m_layout->m_interl[ staff->staffSize ]*2;
    else if ( staff->portNbLine == 4 )
		yy  -= m_layout->m_interl[ staff->staffSize ];
	yy = staff->yrel - m_layout->m_staffSize[ staff->staffSize ];

	x1 = system->indent ? system->indent*10 : 0;
	x2 = system->lrg_lign * 10;


	dc->SetPen( m_currentColour, ToRendererX( m_layout->m_env.m_staffLineWidth ), wxSOLID );
    dc->SetBrush( m_currentColour , wxSOLID );

	x1 = ToRendererX (x1);
	x2 = ToRendererX (x2);

	for(j = 0;j < staff->portNbLine; j++)
	{
		dc->DrawLine( x1 , ToRendererY ( yy ) , x2 , ToRendererY ( yy ) );
		yy -= m_layout->m_interl[staff->staffSize];
	}
    
    dc->ResetPen( );
    dc->ResetBrush( );
    
	return;
}



void MusRC::DrawStaff( MusDC *dc, MusLaidOutStaff *staff, MusSystem *system )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !staff->Check() )
		return;
    
    dc->StartGraphic( staff, "staff", wxString::Format("s_%d", staff->GetId()) );
    
    DrawStaffLines( dc, staff, system );
        
	MusLaidOutLayer *layer = NULL;
	int j;

	for(j = 0; j < staff->GetLayerCount(); j++)
	{
		layer = &staff->m_layers[j];
		//layer->Init( m_r );
		DrawLayer( dc, layer, staff );
	}
    
    dc->EndGraphic(staff); //RZ

}


//----------------------------------------------------------------------------
// MusRC - MusLaidOutLayer
//----------------------------------------------------------------------------




// a partir d'un y, trouve la hauteur d'une note exprimee en code touche et
// octave. Retourne code clavier, et situe l'octave. 

int MusRC::CalculatePitchCode ( MusLaidOutLayer *layer, int y_n, int x_pos, int *octave )
{
    wxASSERT_MSG( layer, "Pointer to layer cannot be NULL" );
    wxASSERT_MSG( layer->m_staff, "Pointer to staff cannot be NULL" );
	
    static int touches[] = {PITCH_C,PITCH_D,PITCH_E,PITCH_F,PITCH_G,PITCH_A,PITCH_B};
	int y_dec, yb, plafond;
	int degres, octaves, position, code;
	char clefId=0;

    int staffSize = layer->m_staff->staffSize;
	// calculer position du do central en fonction clef
	y_n += (int) m_layout->m_verticalUnit2[staffSize];
	yb = layer->m_staff->yrel -  m_layout->m_staffSize[layer->m_staff->staffSize]*2; // UT1 default 
	

	plafond = yb + 8 *  m_layout->m_octaveSize[staffSize];
	if (y_n > plafond)
		y_n = plafond;

	MusLaidOutLayerElement *pelement = layer->GetAtPos( x_pos );
	if ( layer->GetPrevious( pelement ) )
		pelement = layer->GetPrevious( pelement );

	MusClef *clef = layer->GetClef (pelement);
    if (clef) {
        clefId = clef->m_clefId;
        yb += (clef->GetClefOffset()) * m_layout->m_halfInterl[staffSize];	// UT1 reel
    }
	yb -= 4 *  m_layout->m_octaveSize[staffSize];	// UT, note la plus grave

	y_dec = y_n - yb;	// decalage par rapport a UT le plus grave

	if (y_dec< 0)
		y_dec = 0;

	degres = y_dec /  m_layout->m_halfInterl[staffSize];	// ecart en degres (PITCH_C..PITCH_B) par rapport a UT1
	octaves = degres / 7;
	position = degres % 7;

	code = touches[position];
	*octave = octaves /*- OCTBIT*/; // LP remove OCTBIT : oct 0 ‡ 7

	return (code);
}


MusPoint CalcPositionAfterRotation( MusPoint point , float rot_alpha, MusPoint center)
{
    int distCenterX = (point.x - center.x);
    int distCenterY = (point.y - center.y);
    // pythagore, distance entre le point d'origine et le centre
    int distCenter = (int)sqrt( pow( (double)distCenterX, 2 ) + pow( (double)distCenterY, 2 ) );
	
	// angle d'origine entre l'axe x et la droite passant par le point et le centre
    float alpha = atan ( (float)distCenterX / (float)(distCenterY) );
    
    MusPoint new_p = center;
    int new_distCenterX, new_distCenterY;

    new_distCenterX = ( (int)( sin( alpha - rot_alpha ) * distCenter ) );
	new_p.x += new_distCenterX;

    new_distCenterY = ( (int)( cos( alpha - rot_alpha ) * distCenter ) );
	new_p.y += new_distCenterY;

    return new_p;
}

/**
  x1 y1 = point de depart
  x2 y2 = point d'arrivee
  up = liaison vers le haut
  heigth = hauteur de la liaison ( ‡ plat )
  **/
void MusRC::DrawSlur( MusDC *dc, MusLaidOutLayer *layer, int x1, int y1, int x2, int y2, bool up, int height )
{
    wxASSERT_MSG( layer, "Pointer to layer cannot be NULL" );
    wxASSERT_MSG( layer->m_staff, "Pointer to staff cannot be NULL" );

    dc->SetPen( m_currentColour, 1, wxSOLID );
    dc->SetBrush( m_currentColour, wxSOLID );


    int distX = x1 - x2;
    int distY = y1 - y2;
    // pythagore, distance entre les points de depart et d'arrivee
    int dist = (int)sqrt( pow( (double)distX, 2 ) + pow( (double)distY, 2 ) );

	// angle
    float alpha2 = float( distY ) / float( distX );
    alpha2 = atan( alpha2 );
	MusPoint orig(0,0);

	int step = dist / 10; // espace entre les points
	int nbpoints = dist / step;
	dist += 2*step; // ajout d'un pas de chaque cote, supprime ‡ l'affichage
	if ( nbpoints <= 2)
		nbpoints = 3;
	else if ( !(nbpoints % 2) ) // nombre impair de points
		nbpoints++;

	int i,j, k;
	double a, b;
	a = dist / 2; // largeur de l'ellipse
	int nbp2 = nbpoints/2;
	MusPoint *points = new MusPoint[2*nbpoints]; // buffer double - aller retour

	// aller
	b = 100; // hauteur de l'ellipse
	points[nbp2].x = (int)a; // point central
	points[nbp2].y = (int)b;
	for(i = 0, j = nbpoints - 1, k = 1; i < nbp2; i++, j--, k++) // calcul de l'ellipse
	{
		points[i].x = k*step;
		points[i+nbp2+1].y = (int)sqrt( (1 - pow((double)points[i].x,2) / pow(a,2)) * pow(b,2) );
		points[j].x = dist - points[i].x;
		points[j-nbp2-1].y = points[i+nbp2+1].y;		
	}
	int dec_y = points[0].y; // decalage y ‡ cause des 2 pas ajoutes
	for(i = 0; i < nbpoints; i++)
	{
		points[i] = CalcPositionAfterRotation( points[i], alpha2, orig ); // rotation		
		points[i].x = ToRendererX( points[i].x + x1 - 1*step ); // transposition
		points[i].y = ToRendererY( points[i].y + y1 - dec_y );
	}
	dc->DrawSpline( nbpoints, points );

	// retour idem
	b = 90;	
	points[nbp2+nbpoints].x = (int)a;
	points[nbp2+nbpoints].y = (int)b;
	for(i = nbpoints, j = 2*nbpoints - 1, k = 1; i < nbp2+nbpoints; i++, j--, k++)
	{	
		points[j].x = k*step;
		points[i+nbp2+1].y = (int)sqrt( (1 - pow((double)points[j].x,2) / pow(a,2)) * pow(b,2) );
		points[i].x = dist - points[j].x;
		points[j-nbp2-1].y = points[i+nbp2+1].y;	
	}
	dec_y = points[nbpoints].y;

	for(i = nbpoints; i < 2*nbpoints; i++)
	{
		points[i] = CalcPositionAfterRotation( points[i], alpha2, orig );
		points[i].x = ToRendererX( points[i].x + x1 - 1*step );
		points[i].y = ToRendererY( points[i].y + y1 - dec_y );
	}
	dc->DrawSpline( nbpoints, points+nbpoints );

	// remplissage ?

    dc->ResetPen( );
    dc->ResetBrush( );

	delete[] points;

}

void MusRC::DrawLayer( MusDC *dc, MusLaidOutLayer *layer, MusLaidOutStaff *staff )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !layer->Check() )
		return;

	MusLaidOutLayerElement *pelement = NULL;
	int j;

	for(j = 0; j < layer->GetElementCount(); j++)
	{
		pelement = &layer->m_elements[j];
		//pelement->Init( m_r );
		DrawElement( dc, pelement, layer, staff );
	}

}






