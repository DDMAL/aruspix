/////////////////////////////////////////////////////////////////////////////
// Name:        musgraph.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musrc.h"
#include "muslaidoutstaff.h"
#include "musdoc.h"

#include <algorithm>
using std::min;
using std::max;

void MusRC::v_bline ( MusDC *dc, int y1, int y2, int x1, int nbr)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");

	if (discontinu)
		nbr = 1;

    dc->SetPen( m_currentColour, max( 1, ToRendererX(nbr) ), wxSOLID );
    dc->SetBrush( m_currentColour, wxSOLID );

	dc->DrawLine( ToRendererX(x1) , ToRendererY(y1), ToRendererX(x1), ToRendererY(y2) );

    dc->ResetPen();
    dc->ResetBrush();
	return;
}



void MusRC::v_bline2 ( MusDC *dc, int y1, int y2, int x1, int nbr)	
{
	wxASSERT_MSG( dc , "DC cannot be NULL");

	SwapY( &y1, &y2 );

	int x2;
  	x2 = x1 - nbr;
	if (ToRendererX(nbr) < 1)
		nbr = ToLogicalX(1);

    dc->SetPen( m_currentColour, 1, wxSOLID );
    dc->SetBrush( m_currentColour, wxSOLID );

	dc->DrawRectangle( ToRendererX( x2 ), ToRendererY(y1), ToRendererX(nbr) , ToRendererX( y1 - y2 ));

    dc->ResetPen();
    dc->ResetBrush();
	
	return;
}


void MusRC::h_bline ( MusDC *dc, int x1, int x2, int y1, int nbr)
{		
	wxASSERT_MSG( dc , "DC cannot be NULL");

	if (discontinu)
		nbr = 1;

    dc->SetPen( m_currentColour, max( 1, ToRendererX(nbr) ), wxSOLID );
    dc->SetBrush( m_currentColour, wxTRANSPARENT );

	dc->DrawLine( ToRendererX(x1) , ToRendererY(y1), ToRendererX(x2), ToRendererY(y1) );

    dc->ResetPen();
    dc->ResetBrush();
	return;
}

//draw a box, lol. It's like a rectangle, but not filled in.
void MusRC::box( MusDC *dc, int x1, int y1, int x2, int y2)
{
	wxASSERT_MSG( dc, "DC cannot be NULL");
	
	SwapY( &y1, &y2 );

    dc->SetPen( m_currentColour, 3, wxSOLID);
	
	dc->DrawRectangle( ToRendererX(x1), ToRendererY(y1), ToRendererX(x2 - x1), ToRendererX(y1 - y2));
	
    dc->ResetPen();
	
	return;
}

void MusRC::rect_plein2( MusDC *dc, int x1, int y1, int x2, int y2 )	/* dessine rectangle plein */
{	
	wxASSERT_MSG( dc , "DC cannot be NULL");

	SwapY( &y1, &y2 );

    dc->SetPen( m_currentColour, 1, wxSOLID  );
    dc->SetBrush( m_currentColour, wxSOLID );

	dc->DrawRectangle( ToRendererX( x1 ), ToRendererY(y1), ToRendererX(x2 - x1) , ToRendererX( y1 - y2 ));

    dc->ResetPen();
    dc->ResetBrush();

	return;
}

int MusRC::hGrosseligne ( MusDC *dc, int x1, int y1, int x2, int y2, int decal)
{	
	MusPoint p[4];
	int dec = decal;
  
    dc->SetPen( m_currentColour, 1, wxSOLID );
    dc->SetBrush( m_currentColour, wxSOLID );

	decal = ToRendererX(decal);
	p[0].x = ToRendererX(x1);
	p[0].y =  ToRendererY(y1);
	p[1].x = ToRendererX(x2);
	p[1].y =  ToRendererY(y2);
	p[2].x = p[1].x;
	p[2].y = p[1].y - decal;
	p[3].x = p[0].x;
	p[3].y = p[0].y - decal;

	dc->DrawPolygon ( 4, p); //(sizeof (bcoord)*2) / sizeof (POINT)); nbrInt*2+ 1;

    dc->ResetPen();
    dc->ResetBrush();
	return dec;
}


int MusRC::DrawDot ( MusDC *dc, int x, int b, int decal, MusLaidOutStaff *staff )
{	int y = b + staff->yrel;

	if (decal > 600 || in (y, (int)staff->yrel - m_doc->_portee[staff->staffSize], 
		(int)staff->yrel - m_doc->_portee[staff->staffSize]*2))
	{	decal += m_doc->_espace[staff->staffSize];
		if (decal > 600)
			decal -= 1000;
		if (0 == ((int)b % (int)m_doc->_interl[staff->staffSize]))
				y += decal;
	}

	int r = max( ToRendererX(4), 2 );
	
    dc->SetPen( m_currentColour, 1, wxSOLID );
    dc->SetBrush( m_currentColour, wxSOLID );

	dc->DrawCircle( ToRendererX(x) , ToRendererY(y), r );
		
    dc->ResetPen();
    dc->ResetBrush();

	return y;
}

/** 
 Adding experimental code for neumes drawing. Right now its just a copy of DrawLeipzigFont, but
 eventually neume-specific functionality will most likely be implemented.
	super-hack time
 */

void MusRC::festa_string ( MusDC *dc, int x, int y, const wxString str, 
							  MusLaidOutStaff *staff, int dimin ) {
	int staffSize = staff->staffSize;
	int fontCorr = m_doc->hautFontAscent[staffSize][dimin];
	
	wxASSERT_MSG( dc , "DC cannot be NULL");
	
	//need to add handling for festa dies font
	// m_activeChantFonts
	if (staff->notAnc)
	{	
		dc->SetFont( &m_doc->m_activeChantFonts[ staffSize][0] );			
		fontCorr = m_doc->hautFontAscent[ staffSize][0];
	}
	else
	{
		dc->SetFont( &m_doc->m_activeChantFonts[ staffSize][ dimin ] );
	}
	
	if ( dc)
	{	
		dc->SetBackground( AxBLUE );
		dc->SetBackgroundMode( wxTRANSPARENT );
		
		dc->SetTextForeground( m_currentColour );
        dc->SetPen( m_currentColour, 1, wxSOLID );
        dc->SetBrush( m_currentColour, wxSOLID );
		
		//printf("Drawing text here, x: %d, y: %d, y (zoomed): %d, y + fontcorr: %d\n"
		//	   , ToRendererX(x), y, ToRendererY(y), ToRendererY(y + fontCorr));

		dc->DrawText( wxString::Format(wxT("%s"), str.c_str()), ToRendererX(x), ToRendererY(y + fontCorr - 4) );
		
        dc->ResetPen();
        dc->ResetBrush();
	}
	
	return;
}


void MusRC::DrawLeipzigFont ( MusDC *dc, int x, int y, unsigned char c, 
						 MusLaidOutStaff *staff, bool dimin )
{  
	int staffSize = staff->staffSize;
	int fontCorr = 0;
    if (dc->CorrectMusicAscent()) {
        fontCorr = m_doc->hautFontAscent[staffSize][dimin];
    }

	wxASSERT_MSG( dc , "DC cannot be NULL");

    // Font offset management for clef in mensural mode - needs improvement (font modification?)
	if (staff->notAnc && (unsigned char)c >= LEIPZIG_OFFSET_IN_FONT)
	{	
		c+= LEIPZIG_OFFSET_MENSURAL;
		if (dimin && in (c, 227, 229))	// les trois clefs
		{	
			c+= 14;	// les cles d===e tablature
            if (dc->CorrectMusicAscent()) {
                fontCorr = m_doc->hautFontAscent[ staffSize][0];
            }
		}
	}
	if (!staff->notAnc || !in (c, 241, 243))	// tout sauf clefs de tablature
	{
        dc->SetFont( &m_doc->m_activeFonts[ staffSize ][ dimin ] );
	}

	if ( dc)
	{	
		dc->SetBackground( AxBLUE );
		dc->SetBackgroundMode( wxTRANSPARENT );
	
		m_str = (char)c;
		dc->SetTextForeground( m_currentColour );
        dc->SetPen( m_currentColour, 1, wxSOLID );
        dc->SetBrush( m_currentColour, wxSOLID );

		//printf("Drawing text here, x: %d, y: %d, y (zoomed): %d, y + fontcorr: %d\n"
		//	   , ToRendererX(x), y, ToRendererY(y), ToRendererY(y + fontCorr));
        // DrawMusicText is the same with AxWxDc but different with MusSvgDC
		dc->DrawMusicText( m_str, ToRendererX(x), ToRendererY(y + fontCorr) );

        dc->ResetPen();
        dc->ResetBrush();
	}

	return;
}


/**
  Utilise une table de bitmap m_fontsBitmaps
  Plus rapide que DrawText
  **/
/*
void MusRC::putfontfast ( MusDC *dc, int x, int y, unsigned char c )
{  
	//wxColour col = dc->GetTextForeground();
	dc->SetTextForeground( m_currentColour );
	dc->DrawText("",0,0); // needed to flush current colour

	dc->DrawBitmap( m_fontBitmaps[c][0][0], ToRendererX(x) - 2 , ToRendererY(y + this->hautFontAscent[0][0]), true );

	return;
}
*/


void MusRC::putstring ( MusDC *dc, int x, int y, wxString s, int centrer, int staffSize)
{ 
	wxASSERT_MSG( dc , "DC cannot be NULL");

    dc->SetFont( &m_doc->m_activeFonts[ staffSize ][0] );
    x = ToRendererX(x);

	if ( centrer )
	{
        wxLogDebug("Centering string not implemented with MusDC");
		/*
        int w, h;
		dc->GetTextExtent( s, &w, &h );
		x -= w / 2;
        */
	}
	dc->SetTextForeground( m_currentColour );
	dc->DrawText( s, x, ToRendererY(y + m_doc->hautFontAscent[staffSize][0]) );
}

void MusRC::putlyric ( MusDC *dc, int x, int y, wxString s, int staffSize, bool cursor)
{ 
	wxASSERT_MSG( dc , "DC cannot be NULL");

    dc->SetFont( &m_doc->m_activeLyricFonts[ staffSize ] );
	x = ToRendererX(x);

	dc->SetTextForeground( m_currentColour );
	dc->DrawText( s, x, ToRendererY( y ) );
    
    if (cursor)
        DoLyricCursor( x, y, dc, s );	
}



