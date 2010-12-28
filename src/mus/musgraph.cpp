/////////////////////////////////////////////////////////////////////////////
// Name:        musgraph.cpp
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

void MusRC::v_bline ( AxDC *dc, int y1, int y2, int x1, int nbr)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");

	if (discontinu)
		nbr = 1;

    dc->SetPen( m_currentColour, max( 1, ToZoom(nbr) ), wxSOLID );
    dc->SetBrush( m_currentColour, wxSOLID );

	dc->DrawLine( ToZoom(x1) , ToZoomY(y1), ToZoom(x1), ToZoomY(y2) );

    dc->ResetPen();
    dc->ResetBrush();
	return;
}



void MusRC::v_bline2 ( AxDC *dc, int y1, int y2, int x1, int nbr)	
{
	wxASSERT_MSG( dc , "DC cannot be NULL");

	SwapY( &y1, &y2 );

	int x2;
  	x2 = x1 - nbr;
	if (ToZoom(nbr) < 1)
		nbr = ToReel(1);

    dc->SetPen( m_currentColour, 1, wxSOLID );
    dc->SetBrush( m_currentColour, wxSOLID );

	dc->DrawRectangle( ToZoom( x2 ), ToZoomY(y1), ToZoom(nbr) , ToZoom( y1 - y2 ));

    dc->ResetPen();
    dc->ResetBrush();
	
	return;
}


void MusRC::h_bline ( AxDC *dc, int x1, int x2, int y1, int nbr)
{		
	wxASSERT_MSG( dc , "DC cannot be NULL");

	if (discontinu)
		nbr = 1;

    dc->SetPen( m_currentColour, max( 1, ToZoom(nbr) ), wxSOLID );
    dc->SetBrush( m_currentColour, wxTRANSPARENT );

	dc->DrawLine( ToZoom(x1) , ToZoomY(y1), ToZoom(x2), ToZoomY(y1) );

    dc->ResetPen();
    dc->ResetBrush();
	return;
}

//draw a box, lol. It's like a rectangle, but not filled in.
void MusRC::box( AxDC *dc, int x1, int y1, int x2, int y2)
{
	wxASSERT_MSG( dc, "DC cannot be NULL");
	
	SwapY( &y1, &y2 );

    dc->SetPen( m_currentColour, 3, wxSOLID);
	
	dc->DrawRectangle( ToZoom(x1), ToZoomY(y1), ToZoom(x2 - x1), ToZoom(y1 - y2));
	
    dc->ResetPen();
	
	return;
}

void MusRC::rect_plein2( AxDC *dc, int x1, int y1, int x2, int y2 )	/* dessine rectangle plein */
{	
	wxASSERT_MSG( dc , "DC cannot be NULL");

	SwapY( &y1, &y2 );

    dc->SetPen( m_currentColour, 1, wxSOLID  );
    dc->SetBrush( m_currentColour, wxSOLID );

	dc->DrawRectangle( ToZoom( x1 ), ToZoomY(y1), ToZoom(x2 - x1) , ToZoom( y1 - y2 ));

    dc->ResetPen();
    dc->ResetBrush();

	return;
}

int MusRC::hGrosseligne ( AxDC *dc, int x1, int y1, int x2, int y2, int decal)
{	
	AxPoint p[4];
	int dec = decal;
  
    dc->SetPen( m_currentColour, 1, wxSOLID );
    dc->SetBrush( m_currentColour, wxSOLID );

	decal = ToZoom(decal);
	p[0].x = ToZoom(x1);
	p[0].y =  ToZoomY(y1);
	p[1].x = ToZoom(x2);
	p[1].y =  ToZoomY(y2);
	p[2].x = p[1].x;
	p[2].y = p[1].y - decal;
	p[3].x = p[0].x;
	p[3].y = p[0].y - decal;

	dc->DrawPolygon ( 4, p); //(sizeof (bcoord)*2) / sizeof (POINT)); nbrInt*2+ 1;

    dc->ResetPen();
    dc->ResetBrush();
	return dec;
}


int MusRC::pointer ( AxDC *dc, int x, int b, int decal, MusStaff *staff )
{	int y = b + staff->yrel;

	if (decal > 600 || in (y, (int)staff->yrel - _portee[staff->pTaille], 
		(int)staff->yrel - _portee[staff->pTaille]*2))
	{	decal += _espace[staff->pTaille];
		if (decal > 600)
			decal -= 1000;
		if (0 == ((int)b % (int)_interl[staff->pTaille]))
				y += decal;
	}

	int r = max( ToZoom(4), 2 );
	
    dc->SetPen( m_currentColour, 1, wxSOLID );
    dc->SetBrush( m_currentColour, wxSOLID );

	dc->DrawCircle( ToZoom(x) , ToZoomY(y), r );
		
    dc->ResetPen();
    dc->ResetBrush();

	return y;
}

/** 
 Adding experimental code for neumes drawing. Right now its just a copy of putfont, but
 eventually neume-specific functionality will most likely be implemented.
	super-hack time
 */

void MusRC::festa_string ( AxDC *dc, int x, int y, const wxString str, 
							  MusStaff *staff, int dimin ) {
	int pTaille = staff->pTaille;
	int fontCorr = this->hautFontAscent[pTaille][dimin];
	
	wxASSERT_MSG( dc , "DC cannot be NULL");
	
	//need to add handling for festa dies font
	// m_activeChantFonts
	if (staff->notAnc)
	{	
		dc->SetFont( &m_activeChantFonts[ pTaille][0] );			
		fontCorr = this->hautFontAscent[ pTaille][0];
	}
	else
	{
		dc->SetFont( &m_activeChantFonts[ pTaille][ dimin ] );
	}
	
	if ( dc)
	{	
		dc->SetBackground( AxBLUE );
		dc->SetBackgroundMode( wxTRANSPARENT );
		
		dc->SetTextForeground( m_currentColour );
        dc->SetPen( m_currentColour, 1, wxSOLID );
        dc->SetBrush( m_currentColour, wxSOLID );
		
		//printf("Drawing text here, x: %d, y: %d, y (zoomed): %d, y + fontcorr: %d\n"
		//	   , ToZoom(x), y, ToZoomY(y), ToZoomY(y + fontCorr));

		dc->DrawText( wxString::Format(wxT("%s"), str.c_str()), ToZoom(x), ToZoomY(y + fontCorr - 4) );
		
        dc->ResetPen();
        dc->ResetBrush();
	}
	
	return;
}


void MusRC::putfont ( AxDC *dc, int x, int y, unsigned char c, 
						 MusStaff *staff, int dimin, int font_flag)
{  
	int pTaille = staff->pTaille;
	int fontCorr = this->hautFontAscent[pTaille][dimin];

	wxASSERT_MSG( dc , "DC cannot be NULL");

	//need to add handling for festa dies font
	// m_activeChantFonts
	if (staff->notAnc && (unsigned char)c >= OFFSETNOTinFONT)
	{	
		c+= N_FONT;
		if (dimin && in (c, 227, 229))	// les trois clefs
		{	
			if (font_flag == NEUME)
			{
				dc->SetFont( &m_activeChantFonts[ pTaille][0] );
			} else dc->SetFont( &m_activeFonts[ pTaille][0] ); 
			
			c+= 14;	// les cles d===e tablature
			fontCorr = this->hautFontAscent[ pTaille][0];
		}
	}
	if (!staff->notAnc || !in (c, 241, 243))	// tout sauf clefs de tablature
	{
		if (font_flag == NEUME)
			dc->SetFont( &m_activeChantFonts[ pTaille][ dimin ] );
		else 
            dc->SetFont( &m_activeFonts[ pTaille ][ dimin ] );
	}
		/*	
	//if ( !pTaille && !dimin && !illumine)
#if defined (__WXMSW__)
	if ( !pTaille && !dimin )
#else
	if ( !pTaille && !dimin &&  (*m_currentColour != m_black ) )
#endif
	{
		putfontfast( dc, x, y, c );
		return;
	}
*/

	if ( dc)
	{	
		dc->SetBackground( AxBLUE );
		dc->SetBackgroundMode( wxTRANSPARENT );
	
		m_str = (char)c;
		dc->SetTextForeground( m_currentColour );
        dc->SetPen( m_currentColour, 1, wxSOLID );
        dc->SetBrush( m_currentColour, wxSOLID );

		//printf("Drawing text here, x: %d, y: %d, y (zoomed): %d, y + fontcorr: %d\n"
		//	   , ToZoom(x), y, ToZoomY(y), ToZoomY(y + fontCorr));
		dc->DrawText( m_str, ToZoom(x), ToZoomY(y + fontCorr) );

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
void MusRC::putfontfast ( AxDC *dc, int x, int y, unsigned char c )
{  
	//wxColour col = dc->GetTextForeground();
	dc->SetTextForeground( m_currentColour );
	dc->DrawText("",0,0); // needed to flush current colour

	dc->DrawBitmap( m_fontBitmaps[c][0][0], ToZoom(x) - 2 , ToZoomY(y + this->hautFontAscent[0][0]), true );

	return;
}
*/


void MusRC::putstring ( AxDC *dc, int x, int y, wxString s, int centrer, int pTaille)
{ 
	wxASSERT_MSG( dc , "DC cannot be NULL");

    dc->SetFont( &m_activeFonts[ pTaille ][0] );
    x = ToZoom(x);

	if ( centrer )
	{
        wxLogDebug("Centering string not implemented with AxDC");
		/*
        int w, h;
		dc->GetTextExtent( s, &w, &h );
		x -= w / 2;
        */
	}
	dc->SetTextForeground( m_currentColour );
	dc->DrawText( s, x, ToZoomY(y + this->hautFontAscent[pTaille][0]) );
}

void MusRC::putlyric ( AxDC *dc, int x, int y, wxString s, int pTaille, bool cursor)
{ 
	wxASSERT_MSG( dc , "DC cannot be NULL");

    dc->SetFont( &m_activeLyricFonts[ pTaille ] );
	x = ToZoom(x);

	dc->SetTextForeground( m_currentColour );
	dc->DrawText( s, x, ToZoomY( y ) );
    
    if (cursor)
        DoLyricCursor( x, y, dc, s );	
}



