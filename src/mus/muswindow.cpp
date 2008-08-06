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
#include "wx/fontdlg.h"
#include "wx/fontutil.h"

#include "app/axapp.h"
#include "muswindow.h"
#include "muspage.h"
#include "musfile.h"
#include "mustoolpanel.h"
#include "musiowwg.h"

#include "mus_wdr.h"

#include "app/axgotodlg.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// MusWindow
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(MusWindow,wxScrolledWindow)

// WDR: event table for MusWindow

#include "wx/fontdlg.h"

BEGIN_EVENT_TABLE(MusWindow,wxScrolledWindow)
    EVT_LEFT_DOWN( MusWindow::OnMouseLeftDown )
	EVT_LEFT_UP( MusWindow::OnMouseLeftUp )
	EVT_LEFT_DCLICK( MusWindow::OnMouseDClick )
	EVT_MOTION( MusWindow::OnMouseMotion )
	EVT_LEAVE_WINDOW( MusWindow::OnMouseLeave )
    EVT_PAINT( MusWindow::OnPaint )
	EVT_SIZE( MusWindow::OnSize )
	EVT_KEY_DOWN( MusWindow::OnKeyDown )
	EVT_KEY_UP( MusWindow::OnKeyUp )
	EVT_MENU_RANGE( ID_MS_N0, ID_MS_CT, MusWindow::OnPopupMenuNote ) // popup menu
	EVT_MENU_RANGE( ID_MS_R0, ID_MS_R7, MusWindow::OnPopupMenuNote ) // popup menu
	EVT_MENU_RANGE( ID_MS_G1, ID_MS_F5, MusWindow::OnPopupMenuSymbole ) // popup menu
	EVT_MENU_RANGE( ID_MS_MTPP, ID_MS_M2, MusWindow::OnPopupMenuSymbole ) // popup menu
	EVT_MENU_RANGE( ID_MS_PNT, ID_MS_BAR, MusWindow::OnPopupMenuSymbole ) // popup menu
END_EVENT_TABLE()

MusWindow::MusWindow( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style, bool center ) :
    wxScrolledWindow( parent, id, position, size, style ), AxUndo( 100 )
{
	m_f = NULL;
    m_fh = NULL;
    m_p2 = NULL;
    m_page = NULL;
	m_npage = 0;
	m_center = center;

	_pas = 10;
    _bond = 60;
    _pas3 = 3 * _pas;
    _espace[0] = 10;
	_espace[1] = 8;
    _interl[0] = 20;
    _interl[1] = 16;
    _portee[0] = 80;
    _portee[1] = 64;
    _octave[0] = 70;
    _octave[1] = 56;
    hautFont = 100;
    RapportPortee[0] = 16;
    RapportPortee[1] = 20;
    RapportDimin[0] = 3;
    RapportDimin[1] = 4;
    DELTANbBAR[0] = 10;
    DELTANbBAR[1] = 6;
    DELTABLANC[0] = 6;
    DELTABLANC[1] = 4;
	DELTABAR = 16;
    nTailleFont[0][0] = 160;
    nTailleFont[0][1] = 120;
    nTailleFont[1][0] = 128;
    nTailleFont[1][1] = 100;
	hautFontCorr[0][0] = 0;
	hautFontCorr[0][1] = 0;
	hautFontCorr[1][0] = 0;
	hautFontCorr[1][1] = 0;

	discontinu = 0;
	MesVal=1.0;

    zoomNum = 4;
    zoomDen = 10;
    m_charDefin = 0;

	m_black = wxColour( 0, 0, 0 );
	m_currentColour = &m_black;
	m_currentElement = NULL;
	m_bufferElement = NULL;
	m_newElement = NULL;
	m_lastEditedElement = NULL;
	m_currentStaff = NULL;

	efface = false;
	
	m_toolpanel = NULL;

	m_editElement = true;
	m_insertx = 0;
	m_insertcode = F6;
	m_insertoct = 4;
	m_dragging_x = 0;
	m_dragging_y_offset = 0;
	m_lyricMode = false;
	m_inputLyric = false;

	m_str.Alloc(1000);

	if ( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) == *wxWHITE )
		this->SetBackgroundColour( *wxLIGHT_GREY );
	else
		this->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) );
	this->SetForegroundColour( *wxBLACK );

	this->UpdateFontValues();
}


MusWindow::MusWindow()
{
}

MusWindow::~MusWindow()
{
	//if ( m_toolpanel ) Do not do this here because m_toolpanel may have been deleted !!!
	// THIS HAS TO BE CORRECTED !!!!!!!!!!!!!!!!!!!!
	//	m_toolpanel->SetWgWindow( NULL );
	if ( m_bufferElement )
		delete m_bufferElement;
}


// undo
void MusWindow::Load( AxUndoFile *undoPtr )
{
	wxASSERT_MSG( m_f, "MusFile should not be NULL in UNDO");

	if ( !m_f )
		return;
	
	int page, staff, element;
		
	MusWWGInput *wwginput = new MusWWGInput( m_f, undoPtr->GetFilename() );

	// keep current page, staff and element
	wwginput->Read( &page, sizeof( int ));
	wwginput->Read( &staff, sizeof( int ));
	wwginput->Read( &element, sizeof( int ));
		
	if ( undoPtr->m_flags == WG_UNDO_FILE )
	{
	    wwginput->ImportFile();
		PaperSize();
	}
	else if ( undoPtr->m_flags == WG_UNDO_PAGE )
	{	
		MusPage *musPage = new MusPage();
		wwginput->ReadPage( musPage );
		m_f->m_pages.RemoveAt( page );
		m_f->m_pages.Insert( musPage, page );
	}
	
	if ((page < 0) || (page > m_fh->nbpage - 1))
	{
		delete wwginput;
		return;
	}
	
	if ( (m_npage != page) || (undoPtr->m_flags != WG_UNDO_STAFF) )
		SetPage( &m_f->m_pages[page] );
	m_npage = page;

	MusStaff *previous = NULL;
	if ( undoPtr->m_flags == WG_UNDO_STAFF )
	{
		previous = m_currentStaff;
		if ( !previous || (previous->no == staff) )
			previous = NULL; // this staff will be deleted
		MusStaff *musStaff = new MusStaff();
		wwginput->ReadStaff( musStaff );
		// keep xrel and yrel
		musStaff->xrel = m_page->m_staves[ musStaff->no ].xrel;
		musStaff->yrel = m_page->m_staves[ musStaff->no ].yrel;
		musStaff->Init( this );
		// clear and remove previous staff
		m_page->m_staves.RemoveAt( musStaff->no );
		// replace
		m_page->m_staves.Insert( musStaff, musStaff->no );	
	}
	delete wwginput;
	
	m_currentElement = NULL;
	m_currentStaff = NULL;
	
	if ( staff != -1 )
		m_currentStaff = &m_page->m_staves[staff];
	else
		return;
		
	if ( m_currentStaff && (element != -1) )
	{
		m_currentElement = &m_currentStaff->m_elements[element];
		this->SetInsertMode( false ); // force edition mode if current element
	}
		
	UpdateScroll();
	this->Refresh();
	OnEndEdition();
	SyncToolPanel();
}


void MusWindow::Store( AxUndoFile *undoPtr )
{
	wxASSERT_MSG( m_f, "MusFile should not be NULL in UNDO");

	if ( !m_f )
		return;

	// keep current page, staff and element
	int page = -1;
	int staff = -1;
	int element = -1;
	if ( m_page )
		page = m_page->npage - 1;
	if ( m_currentStaff )
		staff = m_currentStaff->no;
	if ( m_currentElement )
		element = m_currentElement->no;
		
    MusWWGOutput *wwgoutput = new MusWWGOutput( m_f, undoPtr->GetFilename() );
	
	wwgoutput->Write( &page, sizeof( int ) );
	wwgoutput->Write( &staff, sizeof( int ) );
	wwgoutput->Write( &element, sizeof( int ) );
		
	if ( undoPtr->m_flags == WG_UNDO_FILE )
	{
	    wwgoutput->ExportFile();
	}
	else if ( undoPtr->m_flags == WG_UNDO_PAGE )
	{	
		wxASSERT_MSG( m_page, "MusPage should not be NULL in UNDO");
		wwgoutput->WritePage( m_page );

	}
	else if ( undoPtr->m_flags == WG_UNDO_STAFF )
	{
		wxASSERT_MSG( m_currentStaff, "MusStaff should not be NULL in UNDO");
		wwgoutput->WriteStaff( m_currentStaff );
	
	}

    delete wwgoutput;

}

void MusWindow::InitDC( wxDC *dc )
{
	if ( m_center )
		dc->SetLogicalOrigin( - (margeMorteHor - mrgG), -margeMorteVer );
	else
		dc->SetLogicalOrigin( mrgG, 5 );

	dc->SetAxisOrientation( true, false );
	this->DoPrepareDC( *dc );
}

void MusWindow::Resize( )
{
	wxWindow *parent = this->GetParent();
	if (!parent || !m_fh ) 
		return;
	
	Show( false );
	wxSize parent_s = parent->GetClientSize();
	int page_w = ToZoom(pageFormatHor);
	int page_h = ToZoom(pageFormatVer) + ToZoom(40); // bord en bas
	int win_w = min( page_w, parent_s.GetWidth() );
	int win_h = min( page_h, parent_s.GetHeight() );

    wxScrollBar bar(this,-1);
    wxSize barSize = bar.GetSize();

	if ( (page_w > win_w) && (win_h + barSize.y < parent_s.GetHeight()) ) // scroll hor
		win_h += barSize.y;
	if ( (page_h > win_h) && (win_w + barSize.y < parent_s.GetWidth()) ) // scroll hor
		win_w += barSize.y;
	
	this->SetSize(  parent_s );
	//this->SetSize(  win_w, win_h );
	this->SetScrollbars( 20, 20, page_w / 20, page_h / 20 );

	margeMorteHor = max( 0, ( parent_s.GetWidth() - win_w ) / 2 );
	margeMorteVer = max( 0, ( parent_s.GetHeight() - win_h ) / 2 );
	//this->Move( x, y );
	
	Show( true );
}

void MusWindow::PaperSize( )
{
	if ( !m_fh ) 
		return;

	if (m_fh->param.orientation)
	{	
		pageFormatHor = m_fh->param.pageFormatHor*10;
		pageFormatVer = m_fh->param.pageFormatVer*10;
	}
	else
	{	
		pageFormatVer = m_fh->param.pageFormatHor*10;
		pageFormatHor = m_fh->param.pageFormatVer*10;
	}
	wymax = pageFormatVer-40;
	beamPenteMx = m_fh->param.beamPenteMax;
	beamPenteMin = m_fh->param.beamPenteMin;
	beamPenteMx /= 100;
	beamPenteMin /= 100;
	return;
}

void MusWindow::SetFile( MusFile *file )
{
	if ( file == NULL ) // unset file
	{
		m_f = NULL;
		m_fh = NULL;
		m_p2 = NULL;
		m_page = NULL;
		zoomNum = 4;
		return;
	}

	m_f = file;
	m_fh = &file->m_fheader;
	m_p2 = &file->m_param2;
	m_npage = 0;
	zoomNum = 4;
	PaperSize();
	SetPage( &file->m_pages[m_npage] );
	//CheckPoint( UNDO_ALL, WG_UNDO_FILE );
}


void MusWindow::SetPage( MusPage *page )
{
	wxASSERT_MSG( page, "MusPage cannot be NULL ");

	m_page = page;
    UpdatePageValues();
	if (m_charDefin == 0)
		UpdateZoomValues();
    UpdateStavesPos();

	m_currentElement = NULL;
	m_currentStaff = NULL;

	// selectionne le premier element
	if ( m_page->m_staves.GetCount() > 1 )
	{
		m_currentStaff = &m_page->m_staves[0];
		if (m_currentStaff && m_currentStaff->GetFirst())
			m_currentElement = m_currentStaff->GetFirst();
	}


    Refresh();
}

bool MusWindow::HasNext( bool forward ) 
{ 
	if ( forward )
		return ( m_f && ((int)m_f->m_pages.GetCount() - 1 > m_npage) );
	else
		return ( m_f && (m_npage > 0) );
		
}

void MusWindow::Next( bool forward ) 
{ 
	if ( !m_f || !m_fh )
		return;

	if ( forward && this->HasNext( true ) )
		m_npage++;
	else if ( !forward && this->HasNext( false ) )
		m_npage--;

	SetPage( &m_f->m_pages[m_npage] );		
}



int MusWindow::ToZoom( int i ) 
{ 
	return (i*zoomNum)/zoomDen; 
}

int MusWindow::ToReel( int i )
{ 
	return (i*zoomDen)/zoomNum;
}

int MusWindow::ToZoomY( int i ) 
{
	return ToZoom( wymax - i );
}

int MusWindow::ToReelY( int i )
{ 
	return wymax - ToReel( i ); 
}

bool MusWindow::CanZoom( bool zoomIn ) 
{ 
	if ( zoomIn )
		return ( m_f && (zoomNum/zoomDen < 1) );
	else
		return ( m_f && ((float)zoomNum/(float)zoomDen > 0.1) );
}


void MusWindow::Zoom( bool zoomIn )
{
	if ( !m_f || !m_fh )
		return;

	if ( zoomIn && this->CanZoom( true ) )
		zoomNum *= 2;
	else if	( !zoomIn && this->CanZoom( false ) )
		zoomNum /= 2;

	Resize();
	UpdateZoomValues();
	SetPage( &m_f->m_pages[m_npage] );
}

void MusWindow::SetZoom( int percent )
{
	if ( !m_f || !m_fh )
		return;

	zoomNum = percent;
	zoomDen = 100;
	Resize();
	UpdateZoomValues();
	SetPage( &m_f->m_pages[m_npage] );
}

bool MusWindow::CanGoto( )
{
	return ( m_fh && ( m_fh->nbpage > 1) );
}

void MusWindow::Goto( )
{
	if ( !m_f || !m_fh )
		return;

    AxGotoDlg *dlg = new AxGotoDlg(this, -1, _("Go to page ..."), m_fh->nbpage, m_npage );
    dlg->Center(wxBOTH);
    if ( dlg->ShowModal() == wxID_OK )
	{
		m_npage = dlg->GetPage();
		SetPage( &m_f->m_pages[m_npage] );
    }
	dlg->Destroy();
}

void MusWindow::Goto( int nopage )
{
	if ( !m_f || !m_fh )
		return;

	if ((nopage < 0) || (nopage > m_fh->nbpage - 1))
		return;

	m_npage = nopage;
	SetPage( &m_f->m_pages[m_npage] );
}


void MusWindow::UpdateFontValues() 
{	
	wxNativeFontInfo info;
	info.FromString( wxGetApp().m_musicFontDesc );
	m_ftLeipzig.SetNativeFontInfo( info );

	if ( !m_ftLeipzig.Ok() )
		wxLogWarning(_("Impossible to load font 'Leipzig'") );
	
	//wxLogMessage(_("Size %d, Family %d, Style %d, Weight %d, Underline %d, Face %s, Desc %s"),
	//	m_ftLeipzig.GetPointSize(),
	//	m_ftLeipzig.GetFamily(),
	//	m_ftLeipzig.GetStyle(),
	//	m_ftLeipzig.GetWeight(),
	//	m_ftLeipzig.GetUnderlined(),
	//	m_ftLeipzig.GetFaceName().c_str(),
	//	m_ftLeipzig.GetNativeFontInfoDesc().c_str());

	m_activeFonts[0][0] = m_ftLeipzig;
    m_activeFonts[0][1] = m_ftLeipzig;
    m_activeFonts[1][0] = m_ftLeipzig;
    m_activeFonts[1][1] = m_ftLeipzig;
	
	// Lyrics
	info.FromString( wxGetApp().m_lyricFontDesc );
	m_ftLyrics.SetNativeFontInfo( info );

	if ( !m_ftLyrics.Ok() )
		wxLogWarning(_("Impossible to load font for the lyrics") );

	m_activeLyricFonts[0] = m_ftLyrics;
    m_activeLyricFonts[1] = m_ftLyrics;
}


void MusWindow::UpdateZoomValues() 
{
	if ( !m_page )
		return;	

	m_activeFonts[0][0].SetPointSize( ToZoom( nTailleFont[0][0] ) );
    m_activeFonts[0][1].SetPointSize( ToZoom( nTailleFont[0][1] ) );
    m_activeFonts[1][0].SetPointSize( ToZoom( nTailleFont[1][0] ) );
    m_activeFonts[1][1].SetPointSize( ToZoom( nTailleFont[1][1] ) );

	m_activeLyricFonts[0].SetPointSize( ToZoom( nTailleFont[0][0] * m_ftLyrics.GetPointSize() / 50 ) );
    m_activeLyricFonts[1].SetPointSize( ToZoom( nTailleFont[1][0] * m_ftLyrics.GetPointSize() / 50 ) );

	m_memDC.SetBackgroundMode( wxTRANSPARENT );
	//wxMask *mask;
	/*for (i = 0; i < 256; i++)
	{
		m_memDC.SetFont( m_activeFonts[0][0] );
		m_fontBitmaps[i][0][0].Create( ToZoom( nTailleFont[0][0]) ,ToZoom( 2*nTailleFont[0][0]), 1 );
		m_memDC.SelectObject( m_fontBitmaps[i][0][0] );
		m_memDC.Clear();
		m_str = (char)i;
		m_memDC.DrawText( m_str, 2, 0 );
		m_memDC.SelectObject( wxNullBitmap );
		mask = new wxMask( m_fontBitmaps[i][0][0], *wxWHITE );
		m_fontBitmaps[i][0][0].SetMask( mask );

		m_memDC.SetFont( m_activeFonts[1][0] );
		m_fontBitmaps[i][1][0].Create( ToZoom( nTailleFont[1][0]) ,ToZoom( 2*nTailleFont[1][0]) , 1 );
		m_memDC.SelectObject( m_fontBitmaps[i][1][0] );
		m_memDC.Clear();
		m_str = (char)i;
		m_memDC.DrawText( m_str, 2, 0 );
		m_memDC.SelectObject( wxNullBitmap );
		mask = new wxMask( m_fontBitmaps[i][1][0], *wxWHITE );
		m_fontBitmaps[i][1][0].SetMask( mask );

		m_memDC.SetFont( m_activeFonts[0][1] );
		m_fontBitmaps[i][0][1].Create( ToZoom( nTailleFont[0][1]) ,ToZoom( 2*nTailleFont[0][1]) , 1 );
		m_memDC.SelectObject( m_fontBitmaps[i][0][1] );
		m_memDC.Clear();
		m_str = (char)i;
		m_memDC.DrawText( m_str, 2, 0 );
		m_memDC.SelectObject( wxNullBitmap );
		mask = new wxMask( m_fontBitmaps[i][0][1], *wxWHITE );
		m_fontBitmaps[i][0][1].SetMask( mask );

		m_memDC.SetFont( m_activeFonts[1][1] );
		m_fontBitmaps[i][1][1].Create( ToZoom( nTailleFont[1][1]) ,ToZoom( 2*nTailleFont[1][1]) , 1 );
		m_memDC.SelectObject( m_fontBitmaps[i][1][1] );
		m_memDC.Clear();
		m_str = (char)i;
		m_memDC.DrawText( m_str, 2, 0 );
		m_memDC.SelectObject( wxNullBitmap );
		mask = new wxMask( m_fontBitmaps[i][1][1], *wxWHITE );
		m_fontBitmaps[i][1][1].SetMask( mask );
	}*/

	m_charDefin = m_page->defin;
}


void MusWindow::UpdatePageValues() 
{
	if ( !m_page || !m_p2 || !m_fh ) 
		return;

	// margins
	int page = m_page->npage;
	if ( m_p2->entetePied & PAGINATION )
		page += m_f->m_pagination.numeroInitial;

	if (page % 2)	//pages impaires 
		mrgG = - m_fh->param.MargeGAUCHEIMPAIRE*10;
	else
		mrgG = - m_fh->param.MargeGAUCHEPAIRE*10;
	mrgG = ToZoom (mrgG);


	if (m_charDefin != m_page->defin)
		m_charDefin = 0;
    int defin = m_page->defin;

    RapportPortee[0] = m_p2->rapportPorteesNum;
    RapportPortee[1] = m_p2->rapportPorteesDen;
    RapportDimin[0] = m_p2->rapportDiminNum;
    RapportDimin[1] = m_p2->rapportDiminDen;

    _espace[0] = defin/2;
    _espace[1] = (_espace[0] * RapportPortee[0]) / RapportPortee[1];
    _interl[0] = _espace[0] * 2;
    _interl[1] = _espace[1] * 2;
    _portee[0] = _interl[0] * 4;
    _portee[1] = _interl[1] * 4;
    _octave[0] = _espace[0] * 7;
    _octave[1] = _espace[1] * 7;

    _pas = _espace[0];
    _bond = _pas * 6;
    _pas3 = _pas * 3;

    DELTANbBAR[0] = m_fh->param.EpBarreValeur;
    DELTABLANC[0] = m_fh->param.EpBlancBarreValeur;
    DELTABAR = DELTANbBAR[0] + DELTABLANC[0];
    DELTANbBAR[1] = (DELTANbBAR[0] * RapportPortee[0]) / RapportPortee[1];
    DELTABLANC[1] = (DELTABLANC[0] * RapportPortee[0]) / RapportPortee[1];


	hautFont = _interl[0] * 8;
	m_ftLeipzig.SetPointSize( hautFont );
	wxClientDC dc(this);
	dc.SetMapMode( wxMM_TEXT );
	dc.SetFont( m_ftLeipzig );
	//wxCoord ch = dc.GetCharHeight();
	//wxCoord cw = dc.GetCharWidth();

	int w, h, d, l;
	dc.GetTextExtent( wxString((char)sSOL,1), &w, &h , &d, &l);

	hautFont *=  hautFont * wxGetApp().m_fontSizeCorrection;
	hautFont /= h * 100;
	
	hautFontCorr[0][0] = (_interl[0] * 45) / 10; //*/ hautFont * 765 / 1000;
	hautFontCorr[0][0] +=  wxGetApp().m_fontPosCorrection;

	hautFontCorr[0][1] = (hautFontCorr[0][0] * RapportDimin[0]) / RapportDimin[1];
	hautFontCorr[1][0] = (hautFontCorr[0][0] * RapportPortee[0]) / RapportPortee[1];
	hautFontCorr[1][1] = (hautFontCorr[1][0] * RapportDimin[0]) / RapportDimin[1];

	m_ftLeipzig.SetPointSize( hautFont );
	dc.SetFont( m_ftLeipzig );

    nTailleFont[0][0] = hautFont;
    nTailleFont[0][1] = (nTailleFont[0][0] * RapportDimin[0]) / RapportDimin[1];
    nTailleFont[1][0] = (nTailleFont[0][0] * RapportPortee[0]) / RapportPortee[1];
    nTailleFont[1][1]= (nTailleFont[1][0] * RapportDimin[0])/ RapportDimin[1];

    v_unit[0] = (float)_interl[0]/4;
    v4_unit[0] = (float)_interl[0]/8;
    v_unit[1] = (float)_interl[1]/4;
    v4_unit[1] = (float)_interl[1]/8;

	dc.GetTextExtent( wxString( (char)sBLANCHE, 1 ), &w, &h ) ;
    rayonNote[0][0] = w / 2;
    rayonNote[0][1] = (rayonNote[0][0] * RapportDimin[0])/RapportDimin[1];
    rayonNote[1][0] = (rayonNote[0][0] * RapportPortee[0])/RapportPortee[1];
    rayonNote[1][1] = (rayonNote[1][0] * RapportDimin[0])/RapportDimin[1];

    ledgerLine[0][0] = (int)(w * .72);
    ledgerLine[0][1] = (ledgerLine[0][0] * RapportDimin[0])/RapportDimin[1];
    ledgerLine[1][0] = (ledgerLine[0][0] * RapportPortee[0])/RapportPortee[1];
    ledgerLine[1][1] = (ledgerLine[1][0] * RapportDimin[0])/RapportDimin[1];

	dc.GetTextExtent( wxString( (char)sRONDE_B, 1 ), &w, &h ) ;
    ledgerLine[0][2] = (int)(w * .66);
    ledgerLine[1][2] = (ledgerLine[0][2] * RapportPortee[0]) /RapportPortee[1];

    largeurBreve[0] = (int)((w * 1.1667) / 2);
    largeurBreve[1] = (largeurBreve[0] * RapportPortee[0]) /RapportPortee[1];

	dc.GetTextExtent( wxString( (char)sDIESE, 1 ), &w, &h ) ;
    largAlter[0][0] = w;
    largAlter[0][1] = (largAlter[0][0] * RapportDimin[0])/RapportDimin[1];
    largAlter[1][0] = (largAlter[0][0] * RapportPortee[0]) /RapportPortee[1];
    largAlter[1][1] = (largAlter[1][0] * RapportDimin[0])/RapportDimin[1];
}


void MusWindow::UpdateStavesPos() 
{
	int i,mPortTaille;
    MusStaff *staff;

	if ( !m_page || !m_p2 || !m_fh ) 
        return;
       
	int yy = wymax; //JwgDef.MRGMORTE;    // sommet utile "dessinable" de la page (bord - 5mm)
    for (i = 0; i < m_page->nbrePortees; i++) 
	{
         staff = &m_page->m_staves[i];
         mPortTaille = staff->pTaille;
         yy -= staff->ecart * _interl[mPortTaille];
         kPos[i].compte = 0;
         // calcul du point d'ancrage des curseurs au-dessus de la ligne superieure
		 kPos[i].yp = yy + _portee[mPortTaille];
		 staff->yrel = (int)kPos[i].yp;
         // portees à 1 ou 4 lignes
         if (staff->portNbLine == 1)
            kPos[i].yp  += _interl[mPortTaille]*2;
         else if (staff->portNbLine == 4)
			kPos[i].yp  += _interl[mPortTaille];
         staff->xrel = m_fh->param.MargeGAUCHEIMPAIRE; //+JwgDef.MRGMORTE;
    }
}

void MusWindow::SetToolPanel( MusToolPanel *toolpanel )
{
	wxASSERT_MSG( toolpanel , "ToolPanel cannot be NULL ");
	m_toolpanel = toolpanel;
	m_toolpanel->SetWgWindow( this );
	SyncToolPanel();
}

void MusWindow::SetInsertMode( bool mode )
{
	if ( m_editElement == !mode )
		return; // nothing to change

    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.SetId( this->GetId() );
    kevent.SetEventObject( this );
    kevent.m_keyCode = WXK_HELP;
	kevent.m_keyCode = WXK_RETURN;
    this->ProcessEvent( kevent );
}

void MusWindow::SetToolType( int type )
{
    int value = '0';
    switch ( type )
    {
    case (MUS_TOOLS_NOTES): value = 'N'; break;
    case (MUS_TOOLS_CLEFS): value = 'K'; break;
    case (MUS_TOOLS_SIGNS): value = 'P'; break;
    case (MUS_TOOLS_OTHER): value = 'S'; break;
    }
        
    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
	kevent.SetId( this->GetId() );
    kevent.SetEventObject( this );
    kevent.m_keyCode = value;
    kevent.m_controlDown = true;
    this->ProcessEvent( kevent );
}


int MusWindow::GetToolType()
{
	MusElement *sync = NULL;

	if (m_editElement)
		sync = m_currentElement;
	else
		sync = m_newElement;

	if (sync)
	{
		if ( sync->TYPE == SYMB )
		{
			if ( ((MusSymbol*)sync)->flag == CLE )
				return MUS_TOOLS_CLEFS;
			else if ( ((MusSymbol*)sync)->flag == IND_MES )
				return MUS_TOOLS_SIGNS;
			else
				return MUS_TOOLS_OTHER;
		}
		else
		{
			return MUS_TOOLS_NOTES;
		}
	}
	else
		return -1;
}

void MusWindow::SyncToolPanel()
{
	int tool = this->GetToolType();

	if ( !m_toolpanel )
		return;

	if ( tool == -1 )
		tool = MUS_TOOLS_NOTES;
	m_toolpanel->SetTools( tool, this->m_editElement);

	this->SetFocus();
}

void MusWindow::Copy()
{
	if ( !m_currentElement )
		return;

	if ( m_bufferElement )
		delete m_bufferElement;

	if ( m_currentElement->IsSymbole() )
		m_bufferElement = new MusSymbol( *(MusSymbol*)m_currentElement );
	else
		m_bufferElement = new MusNote( *(MusNote*)m_currentElement );
}

void MusWindow::Cut()
{
	if ( !m_currentElement )
		return;
	
	this->Copy();
    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.SetId( this->GetId() );
    kevent.SetEventObject( this );
    kevent.m_keyCode = WXK_DELETE;
    this->ProcessEvent( kevent );
}

void MusWindow::Paste()
{
	if ( !m_currentElement || !m_bufferElement)
		return;

	if ( !m_editElement ) // can paste in edition mode only
		return;
			
	m_bufferElement->xrel = m_currentElement->xrel + this->_pas * 3; // valeur arbitraire
	m_currentElement = m_currentStaff->Insert( m_bufferElement );

	this->Refresh();
	OnEndEdition();
}

void MusWindow::UpdateScroll()
{
	if (!m_currentStaff)
		return;
		
	
		
	int x = 0;
	if ( m_currentElement )
		x = ToZoom( m_currentElement->xrel );
	int y = ToZoomY(  kPos[m_currentStaff->no].yp );
	// units
	int xu, yu;
	this->GetScrollPixelsPerUnit( &xu, &yu );
	// start
	int xs, ys;
	this->GetViewStart( &xs, &ys );
	xs *= xu;
	ys *= yu;
	// size
	int w, h;
	this->GetClientSize( &w, &h );
	//wxLogMessage("x %d y %d xs %d ys %d, w %d h %d", x, y, xs, ys, w, h );

	// check if necessary
	if ( (x > xs) && (x < xs + w) )
		x = -1;
	else
		x /= xu;
	if ( (y > ys ) && (y < ys + h - 2 * ToZoom(_portee[0])) )
		y = -1;
	else
		y /= yu;

	Scroll( x, y );
	OnSyncScroll( x, y );
}

// WDR: handler implementations for MusWindow

void MusWindow::OnPopupMenuNote( wxCommandEvent &event )
{
	/*if ( !m_page || !m_currentStaff )
		return;

	MusNote *note = NULL;

	if ( m_editElement )
	{
		if ( !m_currentElement || ( m_currentElement->TYPE == SYMB) )
			return;
		else
			 note = (MusNote*)m_currentElement;
	}
	else
	{
		note = new MusNote( false, LG, F6 );
		note->xrel = m_insertx;
	}

	note->sil = false; // comming from custos

	switch ( event.GetId() )
	{
	case ( ID_MS_N0 ): note->val = LG; break;
	case ( ID_MS_N1 ): note->val = BR; break;
	case ( ID_MS_N2 ): note->val = RD; break;
	case ( ID_MS_N3 ): note->val = BL; break;
	case ( ID_MS_N4 ): note->val = NR; break;
	case ( ID_MS_N5 ): note->val = CR; break;
	case ( ID_MS_N6 ): note->val = DC; break;
	case ( ID_MS_N7 ): note->val = TC; break;
	case ( ID_MS_CT ): note->val = CUSTOS; note->sil = true; break;

	case ( ID_MS_R0 ): note->val = LG; note->sil = true; break;
	case ( ID_MS_R1 ): note->val = BR; note->sil = true; break;
	case ( ID_MS_R2 ): note->val = RD; note->sil = true; break;
	case ( ID_MS_R3 ): note->val = BL; note->sil = true; break;
	case ( ID_MS_R4 ): note->val = NR; note->sil = true; break;
	case ( ID_MS_R5 ): note->val = CR; note->sil = true; break;
	case ( ID_MS_R6 ): note->val = DC; note->sil = true; break;
	case ( ID_MS_R7 ): note->val = TC; note->sil = true; break;
	}

	//if ( !m_editElement )
	//	m_currentStaff->Insert( note );

	this->Refresh();
	event.Skip();
	*/
}

void MusWindow::OnPopupMenuSymbole( wxCommandEvent &event )
{
	if ( !m_page || !m_currentStaff )
		return;

	MusSymbol *symbole = NULL;

	if ( m_editElement )
	{
		if ( !m_currentElement || ( m_currentElement->TYPE != SYMB) )
			return;
		else
			 symbole = (MusSymbol*)m_currentElement;
	}
	else
	{
		symbole = new MusSymbol( );
		symbole->xrel = m_insertx;
	}

	int id = event.GetId();

	if ( in(id , ID_MS_G1, ID_MS_F5) )
	{
		symbole->flag = CLE; 
		switch ( id )
		{
		case ( ID_MS_G1 ): symbole->code = SOL1; break;
		case ( ID_MS_G2 ): symbole->code = SOL2; break;
		case ( ID_MS_U1 ): symbole->code = UT1; break;
		case ( ID_MS_U2 ): symbole->code = UT2; break;
		case ( ID_MS_U3 ): symbole->code = UT3; break;
		case ( ID_MS_U4 ): symbole->code = UT4; break;
		case ( ID_MS_U5 ): symbole->code = UT5; break;
		case ( ID_MS_F3 ): symbole->code = FA3; break;
		case ( ID_MS_F4 ): symbole->code = FA4; break;
		case ( ID_MS_F5 ): symbole->code = FA5; break;
		}
	}
	else if ( in(id , ID_MS_DIESE, ID_MS_DBEMOL) )
	{
		symbole->flag = ALTER;
		symbole->code = F6;
		symbole->oct = 4;
		switch ( id )
		{
		case ( ID_MS_DIESE): symbole->calte = DIESE; break;
		case ( ID_MS_BEMOL): symbole->calte = BEMOL; break;
		case ( ID_MS_BECAR): symbole->calte = BECAR; break;
		case ( ID_MS_DDIESE): symbole->calte = D_DIESE; break;
		case ( ID_MS_DBEMOL): symbole->calte = D_BEMOL; break;
		}
	}
	else if ( id == ID_MS_PNT )
	{
		symbole->flag = PNT;
		symbole->code = F6;
		symbole->oct = 4;
	}

	//if ( !m_editElement )
	//	m_currentStaff->Insert( symbole );

	this->Refresh();
}


void MusWindow::OnMouseDClick(wxMouseEvent &event)
{
	if ( m_editElement )
	{
		/*
		if ( event.ButtonDClick( wxMOUSE_BTN_LEFT  ) && m_currentElement )
		{
			wxMenuBar *menubar = WgEditMenuFunc( );
			wxMenu *menu = menubar->Remove( 0 );

			int submenu_id = 0;
			if ( m_currentElement->TYPE == SYMB )
			{
				MusSymbol *symbole = (MusSymbol*)m_currentElement;
				if ( symbole->flag == CLE )
					submenu_id = ID_MS_KEYS;
				else if ( symbole->flag == IND_MES )
					submenu_id = ID_MS_SIGNS;
				else
					submenu_id = ID_MS_SYMBOLES;
			}
			else
			{
				MusNote *note = (MusNote*)m_currentElement;
				if ( note->sil != _SIL )
					submenu_id = ID_MS_NOTES;
				else
					submenu_id = ID_MS_RESTS;
			}
			wxMenuItem *submenu = menu->Remove( submenu_id );
			delete menu;
			menu = submenu->GetSubMenu();

			this->PopupMenu( menu );
			delete menu;
			menubar->Destroy();
		}
		*/
	}
	else  // insertion
	{
		if ( event.ButtonDClick( wxMOUSE_BTN_LEFT  ) && m_currentStaff && m_newElement )
		{
			wxClientDC dc( this );
			InitDC( &dc );
			m_insertx = ToReel( dc.DeviceToLogicalX( event.m_x ) );
			int y = ToReelY( dc.DeviceToLogicalY( event.m_y ) );
			m_insertcode = m_currentStaff->trouveCodNote( y, m_insertx, &m_insertoct );
			
			m_newElement->xrel = m_insertx;
			if ( m_newElement->IsNote() || 
				(((MusSymbol*)m_newElement)->flag == ALTER) || (((MusSymbol*)m_newElement)->flag == PNT))
			{
				m_newElement->code = m_insertcode;
				m_newElement->oct = m_insertoct;
			}
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			m_lastEditedElement = m_currentStaff->Insert( m_newElement );
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );
			OnEndEdition();

			//wxLogMessage("code %d oct %d", m_insertcode, m_insertoct );

			/*wxMenuBar *menubar = WgEditMenuFunc( );
			wxMenu *menu = menubar->Remove( 0 );

			this->PopupMenu( menu );
			delete menu;
			menubar->Destroy();*/
		}

	}
	event.Skip();
}

void MusWindow::OnMouseLeftUp(wxMouseEvent &event)
{
	if ( m_editElement )
	{
		m_dragging_x = 0;
		m_dragging_y_offset = 0;
		if ( m_has_been_dragged == true )
		{
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );
			OnEndEdition();
			SyncToolPanel();
			m_has_been_dragged = false;
		}
	}
	else  // not edit
	{
	}

	event.Skip();
}


void MusWindow::OnMouseLeave(wxMouseEvent &event)
{
	this->OnMouseLeftUp( event );
	//event.Skip();
}

void MusWindow::OnMouseLeftDown(wxMouseEvent &event)
{
	if ( m_editElement )
	{
		wxClientDC dc( this );
		InitDC( &dc );
		
		if ( m_currentElement &&  m_currentStaff )
			m_currentElement->ClearElement( &dc, m_currentStaff );

		m_has_been_dragged = false;
		m_dragging_x  = ToReel( dc.DeviceToLogicalX( event.m_x ) );
		int x = m_dragging_x - 3;
		int y = ToReelY( dc.DeviceToLogicalY( event.m_y ) );
			//wxLogMessage("x %d : y %d", x, y);

		MusStaff *previous = NULL;
		if ( m_page->GetAtPos( y ) )
		{
			previous = m_currentStaff;
			m_currentStaff = m_page->GetAtPos( y );
		}
		if ( m_currentStaff )
			m_currentElement = m_currentStaff->GetAtPos(x);

		if ( m_currentElement )
			m_dragging_y_offset = y - m_currentStaff->yrel - m_currentElement->dec_y;
		else
			m_dragging_y_offset = 0;

		this->Refresh();
		OnEndEdition();
		SyncToolPanel();
		
	}
	else  // not edit
	{
		if ( event.RightIsDown() ) // copier l'element à la position du click 
		{
			wxClientDC dc( this );
			InitDC( &dc );

			int y = ToReelY( dc.DeviceToLogicalY( event.m_y ) );
			int x  = ToReel( dc.DeviceToLogicalX( event.m_x ) );

			MusElement *tmp = NULL;
			if ( m_page->GetAtPos( y ) )
				tmp = m_page->GetAtPos( y )->GetAtPos( x );

			if ( tmp )
			{
				if ( tmp->IsNote() )
				{
					m_note = *(MusNote*)tmp;
					m_newElement = &m_note;
				}
				else
				{
					m_symbol = *(MusSymbol*)tmp;
					m_newElement = &m_symbol;
				}
			}
		}
		else // update current staff
		{
			wxClientDC dc( this );
			InitDC( &dc );

			int y = ToReelY( dc.DeviceToLogicalY( event.m_y ) );
			if ( m_page->GetAtPos( y ) )
				m_currentStaff = m_page->GetAtPos( y );
		}
	}

	event.Skip();
}

void MusWindow::OnMouseMotion(wxMouseEvent &event)
{
	if ( m_editElement )
	{
		if ( event.Dragging() && event.LeftIsDown() && m_dragging_x && m_currentElement )
		{
			if ( !m_has_been_dragged )
				PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			m_has_been_dragged = true;
			wxClientDC dc( this );
			InitDC( &dc );
			m_insertx = ToReel( dc.DeviceToLogicalX( event.m_x ) );
			int y = ToReelY( dc.DeviceToLogicalY( event.m_y ) ) - m_dragging_y_offset;
			m_insertcode = m_currentStaff->trouveCodNote( y, m_insertx, &m_insertoct );
			m_currentElement->SetPitch( m_insertcode, m_insertoct, m_currentStaff );
			//OnEndEdition();

			/*if (m_currentElement->IsNote() ) // duree
			{
				int ecartx = (m_dragging_x - m_insertx);
				if ( ecartx && !(ecartx % _pas ) )
				{
					ecartx = (ecartx > 1) ? 1 : -1;
					m_dragging_x = m_insertx;
					int vflag = ( ((MusNote*)m_currentElement)->sil == _SIL ) ? 1 : 0;
					m_currentElement->SetValue( ((MusNote*)m_currentElement)->val - ecartx, m_currentStaff, vflag );
					//OnEndEdition();
				}
			}*/
			// move x position
			{
				if ( m_insertx != m_dragging_x  )
				{
					//m_has_been_dragged = true;
					m_currentElement->ClearElement( &dc, m_currentStaff );
					m_currentElement->xrel += ( m_insertx - m_dragging_x );
					m_dragging_x = m_insertx;
					m_currentStaff->CheckIntegrity();
					this->Refresh();
					//OnEndEdition();
				}
			}

		}
	}
	else  // not edit
	{
	}

	event.Skip();
}

void MusWindow::OnKeyUp(wxKeyEvent &event)
{
    //if ( event.GetKeyCode() == WXK_CONTROL )
	//	m_ctrlDown = false;
}

int GetNoteValue( int keycode )
{
	int note = keycode;
	switch (keycode)
	{
	case ( 48 ):
	case ( WXK_NUMPAD0 ): note = 0; break;
	case ( 49 ):
	case ( WXK_NUMPAD1 ): note = 1; break;
	case ( 50 ):
	case ( WXK_NUMPAD2 ): note = 2; break;
	case ( 51 ):
	case ( WXK_NUMPAD3): note = 3; break;
	case ( 52 ):
	case ( WXK_NUMPAD4 ): note = 4; break;
	case ( 53 ):
	case ( WXK_NUMPAD5 ): note = 5; break;
	case ( 54 ):
	case ( WXK_NUMPAD6 ): note = 6; break;
	case ( 55 ):
	case ( WXK_NUMPAD7 ): note = 7; break;
	case ( 57 ):
	case ( WXK_NUMPAD9 ): note = 9; break;
	case ( 'C' ): note = CUSTOS; break;
	}
	return note;
}


void MusWindow::OnKeyDown(wxKeyEvent &event)
{
	if ( !m_page || !m_currentStaff )
		return;

	int noteKeyCode = GetNoteValue( event.m_keyCode );

	// change mode edition -- insertion
	if ( event.GetKeyCode() == WXK_RETURN )
	{
		m_editElement = !m_editElement;
		if ( !m_editElement ) // edition -> insertion
		{
			this->SetCursor( wxCURSOR_PENCIL );
			if ( m_currentElement )
			{
				// keep the last edited element for when we come back to edition mode
				m_lastEditedElement = m_currentElement;
				if ( m_currentElement->IsNote() )
				{
					m_note = *(MusNote*)m_currentElement;
					m_newElement = &m_note;
				}
				else
				{
					m_symbol = *(MusSymbol*)m_currentElement;
					m_newElement = &m_symbol;
				}
			}
			else
			{
				m_newElement = &m_note;
				m_lastEditedElement = NULL;
			}
			m_currentElement = NULL;
		}
		else if ( m_newElement ) // insertion -> edition
		{
			m_currentElement = m_lastEditedElement;
			this->SetCursor( wxCURSOR_ARROW );
			m_newElement = NULL;
		}
		this->Refresh();
		OnEndEdition();
		SyncToolPanel();
	}
	else if ( m_editElement && !m_lyricMode ) // mode edition
	{
		if ( ((event.m_keyCode == WXK_DELETE ) || (event.m_keyCode == WXK_BACK)) && m_currentElement) // delete
		{
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			MusElement *del = m_currentElement;
			MusStaff *delstaff = m_currentStaff;

			if (event.m_keyCode == WXK_DELETE )
			{
				if ( m_currentStaff->GetNext( del ) )
					m_currentElement = m_currentStaff->GetNext( del );
				else if ( m_page->GetNext( m_currentStaff ) )
				{

					m_currentStaff = m_page->GetNext( m_currentStaff );
					m_currentElement = m_currentStaff->GetFirst();
				}
				else
					m_currentElement = NULL;
			}
			else
			{
				if ( m_currentStaff->GetPrevious( del ) )
					m_currentElement = m_currentStaff->GetPrevious( del );
				else if ( m_page->GetPrevious( m_currentStaff ) )
				{
					m_currentStaff = m_page->GetPrevious( m_currentStaff );
					m_currentElement = m_currentStaff->GetLast();
				}
				else
					m_currentElement = NULL;
			}

			delstaff->Delete( del );
			if ( m_currentStaff != delstaff )
			{
				// reset previous staff with no element before checkpoint and then swap again
				MusStaff *tmp = m_currentStaff;
				m_currentStaff = delstaff;
				del = m_currentElement;
				m_currentElement = NULL;
				CheckPoint( UNDO_PART, WG_UNDO_STAFF );
				m_currentStaff = tmp;
				m_currentElement = del;
			}
			else
				CheckPoint( UNDO_PART, WG_UNDO_STAFF );

			if ( m_currentStaff  )
			{
				wxClientDC dc(this);
				InitDC( &dc );
				m_currentStaff->DrawStaff( &dc );
				if (m_currentStaff != delstaff)
					delstaff->DrawStaff( &dc );
			}
			OnEndEdition();
			SyncToolPanel();
		}
		else if ( in ( event.m_keyCode, WXK_F1, WXK_F9 ) && m_currentElement) // change hauteur
		{
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			m_insertcode = m_currentElement->filtrcod( event.m_keyCode - WXK_F1, &m_insertoct );
			m_currentElement->SetPitch( m_insertcode, m_insertoct, m_currentStaff );
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( event.m_controlDown && in( event.m_keyCode, WXK_LEFT, WXK_DOWN) && m_currentElement) // correction hauteur / duree avec les fleches
		{
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			if ( (event.GetKeyCode() == WXK_RIGHT) && m_currentElement->IsNote() && !m_currentElement->ligat ) 
			{
				m_currentElement->SetValue( ((MusNote*)m_currentElement)->val + 1, m_currentStaff );	
			}
			else if ( (event.GetKeyCode() == WXK_LEFT ) && m_currentElement->IsNote() && !m_currentElement->ligat ) 
			{
				m_currentElement->SetValue( ((MusNote*)m_currentElement)->val - 1, m_currentStaff );	
			}
			else if ( event.GetKeyCode() == WXK_UP )
			{
				m_insertcode = m_currentElement->filtrcod( m_currentElement->code + 1, &m_insertoct );
				m_currentElement->SetPitch( m_insertcode, m_insertoct, m_currentStaff );
			}
			else if ( event.GetKeyCode() == WXK_DOWN )
			{
				m_insertcode = m_currentElement->filtrcod( m_currentElement->code - 1, &m_insertoct );
				m_currentElement->SetPitch( m_insertcode, m_insertoct, m_currentStaff );
			}
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() && 
			( (event.m_keyCode == 'B') || (event.m_keyCode == 'D' ) ) ) // ajouter un bemol à une note
		{
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			MusSymbol alteration;
			alteration.flag = ALTER;
			alteration.code = m_currentElement->code;
			alteration.oct = m_currentElement->oct;
			if ( event.m_keyCode == 'B') 
				alteration.calte = BEMOL;
			else
				alteration.calte = DIESE;
			alteration.xrel = m_currentElement->xrel - _pas * 3;
			m_currentStaff->Insert( &alteration );
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == '.')  ) // ajouter un point
		{
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			MusSymbol point;
			point.flag = PNT;
			point.code = m_currentElement->code;
			point.oct = m_currentElement->oct;
			point.xrel = m_currentElement->xrel + _pas * 3;
			// special case where we move forward
			m_currentElement = m_currentStaff->Insert( &point );
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() &&
			(in( noteKeyCode, 0, 7 ) || (noteKeyCode == CUSTOS))) // change duree sur une note ou un silence
		{
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			int vflag = ( event.m_controlDown || (noteKeyCode == CUSTOS)) ? 1 : 0;
			m_currentElement->SetValue( noteKeyCode , m_currentStaff, vflag );
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == 'L')  ) // Ligature 
		{	
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			m_currentElement->SetLigature( m_currentStaff );
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == 'I')  ) // Change coloration
		{
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			m_currentElement->ChangeColoration( m_currentStaff );
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );	
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == 'A')  ) // Change stem direction
		{
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			m_currentElement->ChangeStem( m_currentStaff );
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsSymbole() &&
			 in( event.m_keyCode, 33, 125) ) // any other keycode on symbole (ascii codes)
		{
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			int vflag = ( event.m_controlDown ) ? 1 : 0;
			m_currentElement->SetValue( event.m_keyCode, m_currentStaff, vflag );
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );	
			OnEndEdition();
		}
		else if ( event.m_keyCode == WXK_SPACE && m_currentElement) // deplacement
		{
			PrepareCheckPoint( UNDO_PART, WG_UNDO_STAFF );
			if ( event.m_controlDown )
				m_currentElement->xrel -=3;
			else
				m_currentElement->xrel +=3;
			this->Refresh();
			m_currentStaff->CheckIntegrity();
			CheckPoint( UNDO_PART, WG_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( event.m_keyCode == 'T' && m_currentElement->IsNote() )
		{
			m_lyricMode = true;
			if ( ((MusNote*)m_currentElement)->m_lyric_ptr )
				m_currentElement = ((MusNote*)m_currentElement)->m_lyric_ptr;
			else 
				m_currentElement = m_currentStaff->GetFirstLyric();

			this->Refresh();
		} 
		else // navigation avec les fleches
		{
			MusStaff *previous = NULL;

			if ( event.GetKeyCode() == WXK_RIGHT ) 
			{
				if ( m_currentStaff->GetNext( m_currentElement ) )
					m_currentElement = m_currentStaff->GetNext( m_currentElement );
				else if ( m_page->GetNext( m_currentStaff ) )
				{
					previous = m_currentStaff;
					m_currentStaff = m_page->GetNext( m_currentStaff );
					m_currentElement = m_currentStaff->GetFirst();
				}
				UpdateScroll();
			}
			else if ( event.GetKeyCode() == WXK_LEFT )
			{
				if ( m_currentStaff->GetPrevious( m_currentElement ) )
					m_currentElement = m_currentStaff->GetPrevious( m_currentElement );
				else if ( m_page->GetPrevious( m_currentStaff ) )
				{
					previous = m_currentStaff;
					m_currentStaff = m_page->GetPrevious( m_currentStaff );
					m_currentElement = m_currentStaff->GetLast();
				}
				UpdateScroll();
			}
			else if ( event.GetKeyCode() == WXK_UP )
			{
				if ( m_page->GetPrevious( m_currentStaff ) )
				{
					int x = 0;
					if ( m_currentElement )
						x = m_currentElement->xrel;
					previous = m_currentStaff;
					m_currentStaff = m_page->GetPrevious( m_currentStaff );
					m_currentElement = m_currentStaff->GetAtPos(x);
					UpdateScroll();
				}
			}
			else if ( event.GetKeyCode() == WXK_DOWN )
			{
				if ( m_page->GetNext( m_currentStaff ) )
				{
					int x = 0;
					if ( m_currentElement )
						x = m_currentElement->xrel;
					previous = m_currentStaff;
					m_currentStaff = m_page->GetNext( m_currentStaff );
					m_currentElement = m_currentStaff->GetAtPos(x);
					UpdateScroll();
				}
			}
			if ( event.GetKeyCode() == WXK_HOME ) 
			{
				if ( m_currentStaff->GetFirst( ) )
					m_currentElement = m_currentStaff->GetFirst( );
			}
			if ( event.GetKeyCode() == WXK_END ) 
			{
				if ( m_currentStaff->GetLast( ) )
					m_currentElement = m_currentStaff->GetLast( );
			}
			this->Refresh();
			OnEndEdition();
			SyncToolPanel();
			
		}
	}
	else if ( m_editElement && m_lyricMode )		// Lyric navigation mode
	{	
		if ( event.m_keyCode == WXK_ESCAPE )	// Escape lyric navigaition mode
		{
			m_lyricMode = false;
			if ( ((MusSymbol*)m_currentElement)->m_note_ptr )
				m_currentElement = ((MusSymbol*)m_currentElement)->m_note_ptr;
			else 
				m_currentElement = m_currentStaff->GetFirst();
			
			this->Refresh();
		} 
		else if ( event.m_keyCode == WXK_RETURN )	// Enter lyric insertion mode
		{
			m_inputLyric = true;
		}
		else	// Navigation over lyrics using arrow keys
		{
			MusStaff *previous = NULL;
			
			if ( event.GetKeyCode() == WXK_RIGHT ) 
			{
				if ( m_currentStaff->GetNextLyric( (MusSymbol*)m_currentElement ) )
					m_currentElement = m_currentStaff->GetNextLyric( (MusSymbol*)m_currentElement );
				else if ( m_page->GetNext( m_currentStaff ) )
				{
					previous = m_currentStaff;
					m_currentStaff = m_page->GetNext( m_currentStaff );
					m_currentElement = m_currentStaff->GetFirstLyric();
				}
				UpdateScroll();
			}
			else if ( event.GetKeyCode() == WXK_LEFT )
			{
				if ( m_currentStaff->GetPreviousLyric( (MusSymbol*)m_currentElement ) )
					m_currentElement = m_currentStaff->GetPreviousLyric( (MusSymbol*)m_currentElement );
				else if ( m_page->GetPrevious( m_currentStaff ) )
				{
					previous = m_currentStaff;
					m_currentStaff = m_page->GetPrevious( m_currentStaff );
					m_currentElement = m_currentStaff->GetLastLyric();
				}
				UpdateScroll();
			}
			else if ( event.GetKeyCode() == WXK_UP )
			{
				if ( m_page->GetPrevious( m_currentStaff ) )
				{
					int x = 0;
					if ( m_currentElement )
						x = m_currentElement->xrel;
					previous = m_currentStaff;
					m_currentStaff = m_page->GetPrevious( m_currentStaff );
					m_currentElement = m_currentStaff->GetLyricAtPos(x);
					UpdateScroll();
				}
			}
			else if ( event.GetKeyCode() == WXK_DOWN )
			{
				if ( m_page->GetNext( m_currentStaff ) )
				{
					int x = 0;
					if ( m_currentElement )
						x = m_currentElement->xrel;
					previous = m_currentStaff;
					m_currentStaff = m_page->GetNext( m_currentStaff );
					m_currentElement = m_currentStaff->GetLyricAtPos(x);
					UpdateScroll();
				}
			}
			if ( event.GetKeyCode() == WXK_HOME ) 
			{
				if ( m_currentStaff->GetFirstLyric( ) )
					m_currentElement = m_currentStaff->GetFirstLyric( );
			}
			if ( event.GetKeyCode() == WXK_END ) 
			{
				if ( m_currentStaff->GetLastLyric( ) )
					m_currentElement = m_currentStaff->GetLastLyric( );
			}
			this->Refresh();
			OnEndEdition();
			SyncToolPanel();
		}
	}
	else // mode insertion
	{
		if ( event.m_controlDown && (event.m_keyCode == 'N')) // change set (note, rests, key, signs, symbols, ....
			m_newElement = &m_note;	
		else if ( event.m_controlDown && (event.m_keyCode == 'K')) // keys
		{	
			m_symbol.ResetToKey();
			m_newElement = &m_symbol ;	
		}	
		else if ( event.m_controlDown && (event.m_keyCode == 'P')) // proportions
		{	
			m_symbol.ResetToProportion() ;
			m_newElement = &m_symbol ;	
		}	
		else if ( event.m_controlDown && (event.m_keyCode == 'S')) // symboles
		{
			m_symbol.ResetToSymbol() ;
			m_newElement = &m_symbol ;	
		}	
		else if ( m_newElement && m_newElement->IsNote() &&
			(in( noteKeyCode, 0, 7 ) || (noteKeyCode == CUSTOS))) // change duree sur une note ou un silence
		{
			int vflag = ( event.m_controlDown || (noteKeyCode == CUSTOS)) ? 1 : 0;
			m_newElement->SetValue( noteKeyCode , NULL, vflag );
		}
		else if ( m_newElement && m_newElement->IsNote() && (noteKeyCode == 'L') )
			m_newElement->SetLigature();
		else if ( m_newElement && m_newElement->IsNote() && (noteKeyCode == 'I') )
			m_newElement->ChangeColoration( );
		else if ( m_newElement && m_newElement->IsNote() && (noteKeyCode == 'A') )
			m_newElement->ChangeStem( );
		else if ( m_newElement && m_newElement->IsSymbole() &&
			 in( event.m_keyCode, 33, 125) ) // any other keycode on symbole (ascii codes)
		{
			int vflag = ( event.m_controlDown ) ? 1 : 0;
			m_newElement->SetValue( event.m_keyCode, NULL, vflag );
		}
		OnEndEdition();
		SyncToolPanel();
		
	}
    //else if ( event.GetKeyCode() == WXK_CONTROL )
	//		m_ctrlDown = !m_ctrlDown;

	//event.Skip();
}


void MusWindow::OnPaint(wxPaintEvent &event)
{
	if ( !m_page || !m_p2 || !m_fh )
		return;

	// marge
	//UpdateMargins( m_npage );

	// dans fonction ruler
	wxmax = m_page->lrg_lign*10;

	// calculate scroll position
    int scrollX, scrollY;
    this->GetViewStart(&scrollX,&scrollY);
    int unitX, unitY;
    this->GetScrollPixelsPerUnit(&unitX,&unitY);
	wxSize csize = GetClientSize();
    scrollX *= unitX;
    scrollY *= unitY;
	winwxg = max(0, scrollX - (margeMorteHor - mrgG) );
	winwyg = max(0, scrollY - margeMorteVer) ;
	wxg = ToReel(winwxg);
	wyg = ToReelY(winwyg);
	wxd = min( ToReel( csize.GetWidth() ), wxmax );
	wyd = min( ToReel( csize.GetHeight() ), wymax );
	drawRect.x = wxg;
	drawRect.width = wxd;
	drawRect.y = wyg;
	drawRect.height = - wyd;
	//wxLogDebug("x=%d y=%d right=%d bottom=%d", drawRect.x, drawRect.y, drawRect.GetRight(), drawRect.GetBottom());
	//wxLogDebug("x=%d y=%d width=%d height=%d", drawRect.x, drawRect.y, drawRect.width, drawRect.height);

	//mrgG = 0;
	wxPaintDC dc( this );
	
	if ( m_center )
		dc.SetLogicalOrigin( - (margeMorteHor - mrgG), -margeMorteVer );
	else
		dc.SetLogicalOrigin( mrgG, 5 );

	this->PrepareDC( dc );
	dc.SetTextForeground( *wxBLACK );
	//dc.SetMapMode( wxMM_TEXT );
	dc.SetAxisOrientation( true, false );

	m_page->Init( this );
	m_page->DrawPage( &dc );
}

void MusWindow::OnSize(wxSizeEvent &event)
{
	Resize();
}


