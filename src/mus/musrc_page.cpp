/////////////////////////////////////////////////////////////////////////////
// Name:        musrc_page.cpp
// Author:      Laurent Pugin and Chris Niven
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musrc.h"

//----------------------------------------------------------------------------

#include <assert.h>
#include <math.h>
#include <typeinfo>

//----------------------------------------------------------------------------

#include "musbeam.h"
#include "musbarline.h"
#include "musclef.h"
#include "musdoc.h"
#include "muskeysig.h"
#include "muslayer.h"
#include "muslayerelement.h"
#include "musmeasure.h"
#include "musmensur.h"
#include "muspage.h"
#include "musstaff.h"
#include "mussystem.h"
#include "mustuplet.h"

//----------------------------------------------------------------------------
// MusRC - MusPage
//----------------------------------------------------------------------------

void MusRC::DrawPage( MusDC *dc, MusPage *page, bool background ) 
{
	assert( dc ); // DC cannot be NULL
    assert( page );
    
    int i;
	MusSystem *system = NULL;
    
    // Set the current score def to the page one
    // The page one has previously been set by MusObject::SetPageScoreDef
    m_drawing_scoreDef = page->m_drawing_scoreDef;

    if ( background )
        dc->DrawRectangle( 0, 0, m_doc->m_pageWidth, m_doc->m_pageHeight );
    
    dc->DrawBackgroundImage( );
    
    MusPoint origin = dc->GetLogicalOrigin();
    dc->SetLogicalOrigin( origin.x - m_doc->m_pageLeftMar, origin.y );

    dc->StartPage();

    for (i = 0; i < page->GetSystemCount(); i++) 
	{
		system = (MusSystem*)page->m_children[i];
        DrawSystem( dc, system );
        
        // TODO here: also update x_abs and m_y_drawing positions for system. How to calculate them?
    }
    
    dc->EndPage();
}

//----------------------------------------------------------------------------
// MusRC - MusSystem
//----------------------------------------------------------------------------


// drawing


void MusRC::DrawSystem( MusDC *dc, MusSystem *system ) 
{
	assert( system ); // other asserted before
    
	int i;
    MusMeasure *measure;
    
    dc->StartGraphic( system, "system", system->GetUuid() );
    
    
    if ( system->m_y_abs == AX_UNSET ) {
        assert( m_doc->GetType() == Raw );
        system->m_y_drawing = system->m_y_rel;
        system->m_x_drawing = system->m_x_rel;
    }
    else
    {
        assert( m_doc->GetType() == Transcription );
        system->m_y_drawing = system->m_y_abs;
        system->m_x_drawing = system->m_x_abs;
    }
    
    
    for (i = 0; i < (int)system->GetMeasureCount(); i++)
	{
		measure = (MusMeasure*)system->m_children[i];
        DrawMeasure( dc , measure, system );
	}

    // We draw the groups after the staves because we use the m_y_darwing member of the staves
    // that needs to be intialized.
    // Warning: we assume for now the scoreDef occuring in the system will not change the staffGrps content
    // and @symbol values, otherwise results will be unexpected...
    // First get the first measure of the system
    measure  = dynamic_cast<MusMeasure*>(system->GetFirstChild( &typeid(MusMeasure) ) );
    if ( measure ) {
        // NULL for the MusBarline parameters indicates that we are drawing the scoreDef
        DrawScoreDef( dc, &m_drawing_scoreDef, measure, system->m_x_drawing, NULL );
    }
    
    dc->EndGraphic(system, this );

}

void MusRC::DrawScoreDef( MusDC *dc, MusScoreDef *scoreDef, MusMeasure *measure, int x, MusBarline *barline  )
{
    assert( scoreDef ); // other asserted before

    // we need at least one measure to be able to draw the groups - we need access to the staff elements,
    assert( measure );
    
    MusStaffGrp *staffGrp = dynamic_cast<MusStaffGrp*>(scoreDef->GetFirstChild( &typeid(MusStaffGrp) ) );
    if ( !staffGrp ) {
        return;
    }
    
    if ( barline == NULL) {
        // Draw the first staffGrp and from there its children recursively
        DrawStaffGrp( dc, measure, staffGrp, x );
    }
    else{
        dc->StartGraphic( barline, "barline", barline->GetUuid() );
        DrawBarlines( dc, measure, staffGrp, x, barline );
        dc->EndGraphic( barline, this );
    }
    
	return;
}


void MusRC::DrawStaffGrp( MusDC *dc, MusMeasure *measure, MusStaffGrp *staffGrp, int x )
{
    assert( measure );
    assert( staffGrp );
    
    ListOfMusObjects *staffDefs = staffGrp->GetList( staffGrp );
    if ( staffDefs->empty() ) {
        return;
    }
    
    // Get the first and last staffDef of the staffGrp
    MusStaffDef *firstDef = dynamic_cast<MusStaffDef*>(staffDefs->front());
    MusStaffDef *lastDef = dynamic_cast<MusStaffDef*>(staffDefs->back());
    
    if (!firstDef || !lastDef ) {
        Mus::LogDebug("Could not get staffDef while drawing staffGrp - Mus::DrawStaffGrp");
        return;
    }
    
    // Get the corresponding staff looking at the previous (or first) measure
    MusStaff *first = measure->GetStaffWithNo( firstDef->GetStaffNo() );
    MusStaff *last = measure->GetStaffWithNo( lastDef->GetStaffNo() );
    
    if (!first || !last ) {
        Mus::LogDebug("Could not get staff (%d; %d) while drawing staffGrp - Mus::DrawStaffGrp", firstDef->GetStaffNo(), lastDef->GetStaffNo() );
        return;
    }
    
    int y_top = first->m_y_drawing;
    // for the bottom position we need to take into account the number of lines and the staff size
    int y_bottom = last->m_y_drawing - (last->portNbLine - 1) * m_doc->m_interl[last->staffSize];
    
    // actually draw the line, the brace or the bracket
    if ( staffGrp->GetSymbol() == STAFFGRP_LINE ) {
        v_bline( dc , y_top, y_bottom, x, m_doc->m_env.m_barlineWidth);
    }
    else if ( staffGrp->GetSymbol() == STAFFGRP_BRACE ) {
        DrawBrace ( dc, x, y_top, y_bottom, last->staffSize );
    }
    else if ( staffGrp->GetSymbol() == STAFFGRP_BRACKET ) {
        DrawBracket( dc, x, y_top, y_bottom, last->staffSize );
        x -= 2 * m_doc->m_beamWidth[0] - m_doc->m_beamWhiteWidth[0];
    }
    
    // recursively draw the children
    int i;
    MusStaffGrp *childStaffGrp = NULL;
    for (i = 0; i < staffGrp->GetChildCount(); i++) {
        childStaffGrp = dynamic_cast<MusStaffGrp*>(staffGrp->GetChild( i ));
        if ( childStaffGrp ) {
            DrawStaffGrp( dc, measure, childStaffGrp, x );
        }
    }
}


void MusRC::DrawBracket ( MusDC *dc, int x, int y1, int y2, int staffSize)
{
	int xg, xd, yg, yd, ecart, centre;

    dc->SetPen( m_currentColour , 2, AxSOLID );
    dc->SetBrush( m_currentColour , AxTRANSPARENT );
    
    ecart = m_doc->m_barlineSpacing;
    centre = x - ecart;
    
    xg = centre - ecart*2;
    xd = centre + ecart*2;
    
    yg = y1 + m_doc->m_interl[ staffSize ] * 2;
    yd = y1;
    SwapY( &yg, &yd );
    dc->DrawEllipticArc( ToRendererX(xg), ToRendererY(yg), ToRendererX(xd-xg), ToRendererX(yg-yd), 90, 40 );
    
    yg = y2;
    yd = y2 - m_doc->m_interl[ staffSize ] * 2;
    SwapY( &yg, &yd );
    dc->DrawEllipticArc( ToRendererX(xg), ToRendererY(yg), ToRendererX(xd-xg), ToRendererX(yg-yd), 320, 270 );
    
    dc->ResetPen();
    dc->ResetBrush();
    
    xg = x - (m_doc->m_beamWhiteWidth[0] + 1);
    // determine le blanc entre barres grosse et mince
    v_bline2( dc, y1, y2, xg, m_doc->m_beamWidth[0]);

	return;
}


void MusRC::DrawBrace ( MusDC *dc, int x, int y1, int y2, int staffSize)
{	
    int new_coords[2][6];
    
	assert( dc ); // DC cannot be NULL

	SwapY( &y1, &y2 );
	
	int ymed, xdec, fact, nbrInt;
    
    dc->SetPen( m_currentColour , 1, AxSOLID );
    dc->SetBrush( m_currentColour , AxSOLID );
    
	x -= m_doc->m_beamWhiteWidth[ staffSize ];  // distance entre barre et debut accolade
    
	nbrInt = BEZIER_NB_POINTS;
    
	ymed = (y1 + y2) / 2;
	fact = m_doc->m_beamWidth[ staffSize ]-1 + m_doc->m_env.m_barlineWidth;
	xdec = ToRendererX(fact);
    
	point_[0].x = ToRendererX(x);
	point_[0].y = ToRendererY(y1);
	point_[1].x = ToRendererX(x - m_doc->m_step2);
	point_[1].y = point_[0].y - ToRendererX( m_doc->m_interl[ staffSize ]*3);
	point_[3].x = ToRendererX(x - m_doc->m_step1*2);
	point_[3].y = ToRendererY(ymed);
	point_[2].x = ToRendererX(x + m_doc->m_step1);
	point_[2].y = point_[3].y + ToRendererX( m_doc->m_interl[ staffSize ]);
    
    new_coords[0][0] = point_[1].x;
    new_coords[0][1] = point_[1].y;
    new_coords[0][2] = point_[2].x;
    new_coords[0][3] = point_[2].y;
    new_coords[0][4] = point_[3].x;
    new_coords[0][5] = point_[3].y;
    
	pntswap (&point_[0], &point_[3]);
	pntswap (&point_[1], &point_[2]);
	
	point_[1].x += xdec;
	point_[2].x += xdec;
	point_[1].y = point_[0].y + ToRendererX( m_doc->m_interl[ staffSize ]*2);
    
    new_coords[1][0] = point_[1].x;
    new_coords[1][1] = point_[1].y;
    new_coords[1][2] = point_[2].x;
    new_coords[1][3] = point_[2].y;
    new_coords[1][4] = point_[3].x;
    new_coords[1][5] = point_[3].y;
    
    dc->DrawComplexBezierPath(ToRendererX(x), ToRendererY(y1), new_coords[0], new_coords[1]);
    
	// on produit l'image reflet vers le bas: 0 est identique 
	point_[1].y = point_[0].y - ToRendererX( m_doc->m_interl[ staffSize ]*2);
	point_[3].y = ToRendererY(y2);
	point_[2].y = point_[3].y + ToRendererX( m_doc->m_interl[ staffSize ]*3);
    
    new_coords[0][0] = point_[1].x;
    new_coords[0][1] = point_[1].y;
    new_coords[0][2] = point_[2].x;
    new_coords[0][3] = point_[2].y;
    new_coords[0][4] = point_[3].x;
    new_coords[0][5] = point_[3].y;
    
	pntswap (&point_[0], &point_[3]);
	pntswap (&point_[1], &point_[2]);
	
	point_[1].x -= xdec;
	point_[2].x -= xdec;
	point_[2].y = point_[3].y - ToRendererX( m_doc->m_interl[ staffSize ]);
    
    new_coords[1][0] = point_[1].x;
    new_coords[1][1] = point_[1].y;
    new_coords[1][2] = point_[2].x;
    new_coords[1][3] = point_[2].y;
    new_coords[1][4] = point_[3].x;
    new_coords[1][5] = point_[3].y;
    
    dc->DrawComplexBezierPath(point_[3].x, point_[3].y, new_coords[0], new_coords[1]);
    
    dc->ResetPen();
    dc->ResetBrush();
        
	return;
}


void MusRC::DrawBarlines( MusDC *dc, MusMeasure *measure, MusStaffGrp *staffGrp, int x, MusBarline *barline )
{
    assert( measure );
    assert( staffGrp );
    
    if ( !staffGrp->GetBarthru() ) {
        // recursively draw the children (staffDef or staffGrp)
        int i;
        MusStaffGrp *childStaffGrp = NULL;
        MusStaffDef *childStaffDef = NULL;
        for (i = 0; i < staffGrp->GetChildCount(); i++) {
            childStaffGrp = dynamic_cast<MusStaffGrp*>(staffGrp->GetChild( i ));
            childStaffDef = dynamic_cast<MusStaffDef*>(staffGrp->GetChild( i ));
            if ( childStaffGrp ) {
                DrawBarlines( dc, measure, childStaffGrp, x, barline );
            }
            else if ( childStaffDef ) {
                MusStaff *staff = measure->GetStaffWithNo( childStaffDef->GetStaffNo() );
                if (!staff ) {
                    Mus::LogDebug("Could not get staff (%d) while drawing staffGrp - Mus::DrawBarlines", childStaffDef->GetStaffNo() );
                    continue;
                }
                int y_top = staff->m_y_drawing;
                // for the bottom position we need to take into account the number of lines and the staff size
                int y_bottom = staff->m_y_drawing - (staff->portNbLine - 1) * m_doc->m_interl[staff->staffSize];
                DrawBarline( dc, x, y_top, y_bottom, barline );
                if ( barline->HasRepetitionDots() ) {
                    DrawBarlineDots( dc, x, staff, barline );
                }
            }
        }
    }
    else {
 
        ListOfMusObjects *staffDefs = staffGrp->GetList( staffGrp );
        if ( staffDefs->empty() ) {
            return;
        }
        
        // Get the first and last staffDef of the staffGrp
        MusStaffDef *firstDef = dynamic_cast<MusStaffDef*>(staffDefs->front());
        MusStaffDef *lastDef = dynamic_cast<MusStaffDef*>(staffDefs->back());
        
        if (!firstDef || !lastDef ) {
            Mus::LogDebug("Could not get staffDef while drawing staffGrp - Mus::DrawStaffGrp");
            return;
        }
        
        // Get the corresponding staff looking at the previous (or first) measure
        MusStaff *first = measure->GetStaffWithNo( firstDef->GetStaffNo() );
        MusStaff *last = measure->GetStaffWithNo( lastDef->GetStaffNo() );
        
        if (!first || !last ) {
            Mus::LogDebug("Could not get staff (%d; %d) while drawing staffGrp - Mus::DrawStaffGrp", firstDef->GetStaffNo(), lastDef->GetStaffNo() );
            return;
        }
        
        int y_top = first->m_y_drawing;
        // for the bottom position we need to take into account the number of lines and the staff size
        int y_bottom = last->m_y_drawing - (last->portNbLine - 1) * m_doc->m_interl[last->staffSize];
        
        DrawBarline( dc, x, y_top, y_bottom, barline );
        
        // Now we have a barthru barline, but we have dots so we still need to go through each staff
        if ( barline->HasRepetitionDots() ) {
            int i;
            MusStaffDef *childStaffDef = NULL;
            for (i = 0; i < staffGrp->GetChildCount(); i++) {
                childStaffDef = dynamic_cast<MusStaffDef*>(staffGrp->GetChild( i ));
                if ( childStaffDef ) {
                    MusStaff *staff = measure->GetStaffWithNo( childStaffDef->GetStaffNo() );
                    if (!staff ) {
                        Mus::LogDebug("Could not get staff (%d) while drawing staffGrp - Mus::DrawBarlines", childStaffDef->GetStaffNo() );
                        continue;
                    }
                    DrawBarlineDots( dc, x, staff, barline );
                }
            }
        }
    }
}

void MusRC::DrawBarline( MusDC *dc, int x, int y_top, int y_bottom, MusBarline *barline )
{
    assert( dc );

	int x1 = x - m_doc->m_beamWidth[0] - m_doc->m_env.m_barlineWidth;
	int x2 = x + m_doc->m_beamWidth[0] + m_doc->m_env.m_barlineWidth;
    
	if (barline->m_barlineType == BARLINE_SINGLE)
    {
        v_bline( dc , y_top, y_bottom, x, m_doc->m_env.m_barlineWidth);
    }
    else if (barline->m_barlineType == BARLINE_RPTBOTH)
    {
        v_bline( dc , y_top, y_bottom, x1, m_doc->m_env.m_barlineWidth);
        v_bline( dc , y_top, y_bottom, x, m_doc->m_beamWidth[0]);
        v_bline( dc , y_top, y_bottom, x2, m_doc->m_env.m_barlineWidth);
    }
    else if (barline->m_barlineType  == BARLINE_RPTSTART)
    {
        v_bline( dc , y_top, y_bottom, x, m_doc->m_beamWidth[0]);
        v_bline( dc , y_top, y_bottom, x2, m_doc->m_env.m_barlineWidth);
    }
    else if (barline->m_barlineType == BARLINE_RPTEND)
	{
        v_bline( dc , y_top, y_bottom, x1, m_doc->m_env.m_barlineWidth);
        v_bline( dc , y_top, y_bottom, x, m_doc->m_beamWidth[0]);
	}
	else if (barline->m_barlineType  == BARLINE_DBL)
	{
        // Narrow the bars a little bit - should be centered?
        x1 += m_doc->m_env.m_barlineWidth;
        v_bline( dc , y_top, y_bottom, x, m_doc->m_env.m_barlineWidth);
        v_bline( dc , y_top, y_bottom, x1, m_doc->m_env.m_barlineWidth);
	}
	else if (barline->m_barlineType  == BARLINE_END)
    {
        v_bline( dc , y_top, y_bottom, x1, m_doc->m_env.m_barlineWidth);
        v_bline( dc , y_top, y_bottom, x, m_doc->m_beamWidth[0]);
    }
}

 
void MusRC::DrawBarlineDots ( MusDC *dc, int x, MusStaff *staff, MusBarline *barline )
{
	assert( dc ); // DC cannot be NULL
    
	int x1 = x - 2 * m_doc->m_beamWidth[0] - m_doc->m_env.m_barlineWidth;
	int x2 = x + 2 * m_doc->m_beamWidth[0] + m_doc->m_env.m_barlineWidth;
    
    int y_bottom = staff->m_y_drawing - staff->portNbLine  * m_doc->m_halfInterl[staff->staffSize];
    int y_top = y_bottom + m_doc->m_interl[staff->staffSize];
 
    if ((barline->m_barlineType  == BARLINE_RPTSTART) || (barline->m_barlineType == BARLINE_RPTBOTH))
    {
        DoDrawDot(dc, x2, y_bottom );
        DoDrawDot(dc, x2, y_top );

    }
    if ((barline->m_barlineType == BARLINE_RPTEND) || (barline->m_barlineType == BARLINE_RPTBOTH))
	{
        DoDrawDot(dc, x1, y_bottom );
        DoDrawDot(dc, x1, y_top );
	}
    
	return;
}


void MusRC::DrawPartialBarline ( MusDC *dc, MusSystem *system, int x, MusStaff *pportee)
{
	assert( dc ); // DC cannot be NULL

	int b, bb;

    /* ax3
	MusStaff *next = system->GetNext( NULL );
	if ( next )
	{	
		b = pportee->m_y_drawing - m_doc->m_staffSize[ pportee->staffSize ]*2;
		bb = next->m_y_drawing - m_doc->m_staffSize[ next->staffSize];

		if (m_doc->m_env.m_barlineWidth > 2)	// barres plus epaisses qu'un 1/2 mm
			v_bline2 ( dc, b, bb, x,  m_doc->m_env.m_barlineWidth);
		else
			v_bline ( dc, b, bb, x,  m_doc->m_env.m_barlineWidth);
		
	}
    */

}



//----------------------------------------------------------------------------
// MusRC - MusMeasure
//----------------------------------------------------------------------------

void MusRC::DrawMeasure( MusDC *dc, MusMeasure *measure, MusSystem *system )
{
	assert( dc ); // DC cannot be NULL
    
    // This is a special case where we do not draw (SVG, Bounding boxes, etc.) the measure if un-measured music
    if ( measure->IsMeasuredMusic()) {
        dc->StartGraphic( measure, "measure", measure->GetUuid() );
    }
    
    // Here we set the appropriate y value to be used for drawing
    // With Raw documents, we use m_x_rel that is calculated by the layout algorithm
    // With Transcription documents, we use the m_x_abs
    if ( measure->m_x_abs == AX_UNSET ) {
        assert( m_doc->GetType() == Raw );
        measure->m_x_drawing = measure->m_x_rel + system->m_x_drawing;
    }
    else
    {
        assert( m_doc->GetType() == Transcription );
        measure->m_x_drawing = measure->m_x_abs;
    }
    
	MusStaff *staff = NULL;
	int j;
    
	for(j = 0; j < measure->GetStaffCount(); j++)
	{
		staff = (MusStaff*)measure->m_children[j];
		DrawStaff( dc, staff, measure, system );
    }

    if ( measure->GetLeftBarlineType() != BARLINE_NONE) {
        DrawScoreDef( dc, &m_drawing_scoreDef, measure, measure->m_x_drawing, measure->GetLeftBarline() );
    }
    if ( measure->GetRightBarlineType() != BARLINE_NONE) {
        DrawScoreDef( dc, &m_drawing_scoreDef, measure, measure->m_x_drawing + measure->GetXRelRight(), measure->GetRightBarline() );
    }
    
    if ( measure->IsMeasuredMusic()) {
        dc->EndGraphic( measure, this );
    }
}


//----------------------------------------------------------------------------
// MusRC - MusStaff
//----------------------------------------------------------------------------

int MusRC::CalculateNeumePosY ( MusStaff *staff, char note, int dec_clef, int oct)
{
    assert(staff); // Pointer to staff cannot be NULL"

	static char notes[] = {1,2,3,4,5,6,7};
	int y_int;
	char *pnote, i;
	pnote = &notes[0] - 1;
	
	y_int = ((dec_clef + oct*7) - 17 ) * m_doc->m_halfInterl[staff->staffSize];
	if (staff->portNbLine > 4)
		y_int -= ((staff->portNbLine - 4) * 2) * m_doc->m_halfInterl[staff->staffSize];
	
	for (i=0; i<(signed)sizeof(notes); i++)
		if (*(pnote+i) == note)
			return(y_int += (i*m_doc->m_halfInterl[staff->staffSize]));
	return 0;
}

int MusRC::CalculatePitchPosY ( MusStaff *staff, char pname, int dec_clef, int oct)
{
    assert(staff); // Pointer to staff cannot be NULL"
	
    static char touches[] = {PITCH_C,PITCH_D,PITCH_E,PITCH_F,PITCH_G,PITCH_A,PITCH_B};
	int y_int;
	char *ptouche, i;
	ptouche=&touches[0];

	y_int = ((dec_clef + oct*7) - 9 ) * m_doc->m_halfInterl[staff->staffSize];
	if (staff->portNbLine > 5)
		y_int -= ((staff->portNbLine - 5) * 2) * m_doc->m_halfInterl[staff->staffSize];

	/* exprime distance separant m_y_drawing de
	position 1e Si, corrigee par dec_clef et oct. Elle est additionnee
	ensuite, donc elle doit etre NEGATIVE si plus bas que m_y_drawing */
	for (i=0; i<(signed)sizeof(touches); i++)
		if (*(ptouche+i) == pname)
			return(y_int += ((i+1)*m_doc->m_halfInterl[staff->staffSize]));
	return 0;
}

int MusRC::CalculateRestPosY ( MusStaff *staff, char duration)
{
    assert(staff); // Pointer to staff cannot be NULL"

	int staff_space = m_doc->m_halfInterl[staff->staffSize];
    int base = -17 * staff_space; // -17 is a magic number copied from above
    int offset;
    
    switch (duration) {
        case DUR_LG: offset = 12; break;
        case DUR_BR: offset = 13; break;
        case DUR_1: offset = 15; break;
        case DUR_2: offset = 13; break;
        case DUR_4: offset = 11; break;
        case DUR_8: offset = 11; break;
        case DUR_16: offset = 11; break;
        case DUR_32: offset = 11; break;
        case DUR_64: offset = 10; break;
        case DUR_128: offset = 10; break;
        case DUR_256: offset = 9; break;
        case VALSilSpec: offset = 15; break; // MM rests have same height as wholes
            
        default: offset = 12; break; // Signal an error, put the clef up high
    }
    return base + staff_space * offset;
}

void MusRC::DrawStaffLines( MusDC *dc, MusStaff *staff, MusMeasure *measure, MusSystem *system )
{
	assert( dc ); // DC cannot be NULL
        
    if (staff->invisible)
        return;

	int j, x1, x2, yy;

	yy = staff->m_y_drawing;

	x1 = system->m_systemLeftMar;
    x2 = m_doc->GetSystemRightX( system );

	dc->SetPen( m_currentColour, ToRendererX( m_doc->m_env.m_staffLineWidth ), AxSOLID );
    dc->SetBrush( m_currentColour , AxSOLID );

	x1 = ToRendererX (x1);
	x2 = ToRendererX (x2);

	for(j = 0;j < staff->portNbLine; j++)
	{
		dc->DrawLine( x1 , ToRendererY ( yy ) , x2 , ToRendererY ( yy ) );
		yy -= m_doc->m_interl[staff->staffSize];
	}
    
    dc->ResetPen( );
    dc->ResetBrush( );
    
	return;
}



void MusRC::DrawStaff( MusDC *dc, MusStaff *staff, MusMeasure *measure, MusSystem *system )
{
	assert( dc ); // DC cannot be NULL
    
    dc->StartGraphic( staff, "staff", staff->GetUuid());
    
    // Here we set the appropriate y value to be used for drawing
    // With Raw documents, we use m_y_rel that is calculated by the layout algorithm
    // With Transcription documents, we use the m_y_abs
    if ( staff->m_y_abs == AX_UNSET ) {
        assert( m_doc->GetType() == Raw );
        staff->m_y_drawing = staff->GetYRel() + system->m_y_drawing;
    }
    else
    {
        assert( m_doc->GetType() == Transcription );
        staff->m_y_drawing = staff->m_y_abs;
    }
    
    DrawStaffLines( dc, staff, measure, system );
        
	MusLayer *layer = NULL;
	int j;
    
	for(j = 0; j < staff->GetLayerCount(); j++)
	{
		layer = (MusLayer*)staff->m_children[j];
		DrawLayer( dc, layer, staff, measure );
	}
    
    dc->EndGraphic( staff, this );
}


//----------------------------------------------------------------------------
// MusRC - MusLayer
//----------------------------------------------------------------------------


// a partir d'un y, trouve la hauteur d'une note exprimee en code touche et
// octave. Retourne code clavier, et situe l'octave. 

int MusRC::CalculatePitchCode ( MusLayer *layer, int y_n, int x_pos, int *octave )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(layer->m_parent); // Pointer to staff cannot be NULL"
	
    static int touches[] = {PITCH_C,PITCH_D,PITCH_E,PITCH_F,PITCH_G,PITCH_A,PITCH_B};
	int y_dec, yb, plafond;
	int degres, octaves, position, code;
	char clefId=0;

    int staffSize = ((MusStaff*)layer->m_parent)->staffSize;
	// calculer position du do central en fonction clef
	y_n += (int) m_doc->m_verticalUnit2[staffSize];
	yb = ((MusStaff*)layer->m_parent)->m_y_drawing -  m_doc->m_staffSize[((MusStaff*)layer->m_parent)->staffSize]*2; // UT1 default
	

	plafond = yb + 8 *  m_doc->m_octaveSize[staffSize];
	if (y_n > plafond)
		y_n = plafond;

    MusLayerElement *previous = NULL;
	MusLayerElement *pelement = layer->GetAtPos( x_pos );
	if ( (previous = layer->GetPrevious( pelement ) ) )
		pelement = previous;

	MusClef *clef = layer->GetClef (pelement);
    if (clef) {
        clefId = clef->m_clefId;
        yb += (clef->GetClefOffset()) * m_doc->m_halfInterl[staffSize];	// UT1 reel
    }
	yb -= 4 *  m_doc->m_octaveSize[staffSize];	// UT, note la plus grave

	y_dec = y_n - yb;	// decalage par rapport a UT le plus grave

	if (y_dec< 0)
		y_dec = 0;

	degres = y_dec /  m_doc->m_halfInterl[staffSize];	// ecart en degres (PITCH_C..PITCH_B) par rapport a UT1
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
void MusRC::DrawSlur( MusDC *dc, MusLayer *layer, int x1, int y1, int x2, int y2, bool up, int height )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(layer->m_parent); // Pointer to staff cannot be NULL"

    dc->SetPen( m_currentColour, 1, AxSOLID );
    dc->SetBrush( m_currentColour, AxSOLID );


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

void MusRC::DrawLayer( MusDC *dc, MusLayer *layer, MusStaff *staff, MusMeasure *measure)
{
	assert( dc ); // DC cannot be NULL

	MusLayerElement *element = NULL;
	int j;
    
    // first we need to clear the drawing list of postponed elements
    layer->ResetDrawingList();
    
    if (layer->GetClefAttr()) {
        DrawElement(dc, layer->GetClefAttr(), layer, measure, staff);
    }
    if (layer->GetKeySigAttr()) {
        DrawElement(dc, layer->GetKeySigAttr(), layer, measure, staff);
    }
    if (layer->GetMensurAttr()) {
        DrawElement(dc, layer->GetMensurAttr(), layer, measure, staff);
    }
    
	for(j = 0; j < layer->GetElementCount(); j++)
	{
		element = (MusLayerElement*)layer->m_children[j];
        
        if ( !element->m_in_layer_app ) {
            DrawElement( dc, element, layer, measure, staff );
        }
	}
    
    // first draw the beams
    DrawLayerList(dc, layer, staff, measure, &typeid(MusBeam) );
    // then tuplets
    DrawLayerList(dc, layer, staff, measure, &typeid(MusTuplet) );
    
}


void MusRC::DrawLayerList( MusDC *dc, MusLayer *layer, MusStaff *staff, MusMeasure *measure, const std::type_info *elementType )
{
	assert( dc ); // DC cannot be NULL
    
    ListOfMusObjects *drawingList = layer->GetDrawingList();
	MusLayerElement *element = NULL;
    
    ListOfMusObjects::iterator iter;
    
    for (iter = drawingList->begin(); iter != drawingList->end(); ++iter)
    {
        element = dynamic_cast<MusLayerElement*>(*iter);
        if ( (typeid(*element) == *elementType) &&  (*elementType == typeid(MusBeam) ) ) {
            MusBeam *beam = dynamic_cast<MusBeam*>(element);
            DrawBeam( dc, layer, beam, staff );
        }
        else if ( (typeid(*element) == *elementType) &&  (*elementType == typeid(MusTuplet) ) ) {
            MusTuplet *tuplet = dynamic_cast<MusTuplet*>(element);
            DrawTuplet( dc, tuplet, layer, staff );
        }
            
    }
}






