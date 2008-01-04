/////////////////////////////////////////////////////////////////////////////
// Name:        wgwindow.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_WG

#ifdef __GNUG__
    #pragma implementation "wgwindow.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wgwindow.h"
#include "wgpage.h"
#include "wgfile.h"

// WDR: class implementations

void WgWindow::v_bline ( wxDC *dc, int y1, int y2, int x1, int nbr)
{
	wxASSERT_MSG( dc , "DC cannot be NULL");

	if (discontinu)
		nbr = 1;

	wxPen pen( *m_currentColour, nbr, wxSOLID );
	dc->SetPen( pen );
	wxBrush brush( *m_currentColour, wxTRANSPARENT );
	dc->SetBrush( brush );

	dc->DrawLine( ToZoom(x1) , ToZoomY(y1), ToZoom(x1), ToZoomY(y2) );

	dc->SetPen( wxNullPen );
	dc->SetBrush( wxNullBrush );
	return;
}



void WgWindow::v_bline2 ( wxDC *dc, int y1, int y2, int x1, int nbr)	
{
	wxASSERT_MSG( dc , "DC cannot be NULL");

	SwapY( &y1, &y2 );

	int x2;
  	x2 = x1 - nbr;
	if (ToZoom(nbr) < 1)
		nbr = ToReel(1);

	wxPen pen( *m_currentColour, 1, wxSOLID );
	dc->SetPen( pen );
	wxBrush brush( *m_currentColour, wxSOLID );
	dc->SetBrush( brush );

	dc->DrawRectangle( ToZoom( x2 ), ToZoomY(y1), ToZoom(nbr) , ToZoom( y1 - y2 ));

	dc->SetPen( wxNullPen );
	dc->SetBrush( wxNullBrush );
	
	return;
}


void WgWindow::h_bline ( wxDC *dc, int x1, int x2, int y1, int nbr)
{		
	wxASSERT_MSG( dc , "DC cannot be NULL");

	if (discontinu)
		nbr = 1;

	wxPen pen( *m_currentColour, nbr, wxSOLID );
	dc->SetPen( pen );
	wxBrush brush( *m_currentColour, wxTRANSPARENT );
	dc->SetBrush( brush );

	dc->DrawLine( ToZoom(x1) , ToZoomY(y1), ToZoom(x2), ToZoomY(y1) );

	dc->SetPen( wxNullPen );
	dc->SetBrush( wxNullBrush );
	return;
}


void WgWindow::rect_plein2( wxDC *dc, int x1, int y1, int x2, int y2)	/* dessine rectangle plein */
{	
	wxASSERT_MSG( dc , "DC cannot be NULL");

	SwapY( &y1, &y2 );

	wxPen pen( *m_currentColour, 1, wxSOLID );
	dc->SetPen( pen );
	wxBrush brush( *m_currentColour, wxSOLID );
	dc->SetBrush( brush );

	dc->DrawRectangle( ToZoom( x1 ), ToZoomY(y1), ToZoom(x2 - x1) , ToZoom( y1 - y2 ));

	dc->SetPen( wxNullPen );
	dc->SetBrush( wxNullBrush );

	return;

}


int WgWindow::hGrosseligne ( wxDC *dc, int x1, int y1, int x2, int y2, int decal)
{	
	wxPoint p[4];
	int dec = decal;
  
	wxPen pen( *m_currentColour, 1, wxSOLID );
	dc->SetPen( pen );
	wxBrush brush( *m_currentColour, wxSOLID );
	dc->SetBrush( brush );

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

	dc->SetPen( wxNullPen );
	dc->SetBrush( wxNullBrush );
	return dec;
}


int WgWindow::pointer ( wxDC *dc, int x, int b, int decal, WgStaff *staff )
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
	
	wxPen pen( *m_currentColour, 1, wxSOLID );
	dc->SetPen( pen );
	wxBrush brush( *m_currentColour, wxSOLID );
	dc->SetBrush( brush );

	dc->DrawCircle( ToZoom(x) , ToZoomY(y), r );
		
	dc->SetPen( wxNullPen );
	dc->SetBrush( wxNullBrush );

	return y;
}


void WgWindow::putfont ( wxDC *dc, int x, int y, unsigned char c, WgStaff *staff, int dimin)
{  
	int pTaille = staff->pTaille;
	int fontCorr = this->hautFontCorr[pTaille][dimin];

	wxASSERT_MSG( dc , "DC cannot be NULL");

	if (staff->notAnc && (unsigned char)c >= OFFSETNOTinFONT)
	{	
		c+= N_FONT;
		if (dimin && in (c, 227, 229))	// les trois clefs
		{	
			c+= 14;	// les cles de tablature
			dc->SetFont( m_activeFonts[ pTaille ][0] );
			fontCorr = this->hautFontCorr[ pTaille ][0];
		}
	}
	if (!staff->notAnc || !in (c, 241, 243))	// tout sauf clefs de tablature
		dc->SetFont( m_activeFonts[ pTaille ][ dimin ] );
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
		dc->SetBackground( *wxBLUE );
		dc->SetBackgroundMode( wxTRANSPARENT );
	
		m_str = (char)c;
		dc->SetTextForeground( *m_currentColour );
		wxPen pen( *m_currentColour, 1, wxSOLID );
		dc->SetPen( pen );
		wxBrush brush( *m_currentColour, wxSOLID );

		dc->SetBrush( brush );

		dc->DrawText( m_str, ToZoom(x), ToZoomY(y + fontCorr) );

		dc->SetPen( wxNullPen );
		dc->SetBrush( wxNullBrush );
	}

	return;
}


/**
  Utilise une table de bitmap m_fontsBitmaps
  Plus rapide que DrawText
  **/
void WgWindow::putfontfast ( wxDC *dc, int x, int y, unsigned char c )
{  
	//wxColour col = dc->GetTextForeground();
	dc->SetTextForeground( *m_currentColour );
	dc->DrawText("",0,0); // needed to flush current colour

	dc->DrawBitmap( m_fontBitmaps[c][0][0], ToZoom(x) - 2 , ToZoomY(y + this->hautFontCorr[0][0]), true );

	return;
}


void WgWindow::putstring ( wxDC *dc, int x, int y, wxString s, int centrer, int pTaille)
{ 
	wxASSERT_MSG( dc , "DC cannot be NULL");

    dc->SetFont( m_activeFonts[ pTaille ][0] );
    x = ToZoom(x);

	if ( centrer )
	{
		int w, h;
		dc->GetTextExtent( s, &w, &h );
		x -= w / 2;
	}
	dc->SetTextForeground( *m_currentColour );
	dc->DrawText( s, x, ToZoomY(y + this->hautFontCorr[pTaille][0]) );
}



#endif // AX_WG
