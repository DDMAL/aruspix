/////////////////////////////////////////////////////////////////////////////
// Name:        muswindow.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

/** 
 quick note about fonts: may 31, 2010
 
 I've tried to emulate what you've done with Leipzig and mimic this with Festa
 Dies throughout. It may not work as this is a very experimental approach.
 
 
 */



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

#include "RtMidi.h"

#include "app/axapp.h"
#include "muswindow.h"
#include "muspage.h"
#include "musfile.h"
#include "mustoolpanel.h"
#include "musiobin.h"

//#include "mus_wdr.h"

#include "app/axgotodlg.h"

#include <iostream>
#include <cstdlib>

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
	EVT_CHAR( MusWindow::OnChar )
    EVT_KEY_DOWN( MusWindow::OnKeyDown )
	EVT_KEY_UP( MusWindow::OnKeyUp )
    /*
	EVT_MENU_RANGE( ID_MS_N0, ID_MS_CT, MusWindow::OnPopupMenuNote ) // popup menu
	EVT_MENU_RANGE( ID_MS_R0, ID_MS_R7, MusWindow::OnPopupMenuNote ) // popup menu
	EVT_MENU_RANGE( ID_MS_G1, ID_MS_F5, MusWindow::OnPopupMenuSymbole ) // popup menu
	EVT_MENU_RANGE( ID_MS_MTPP, ID_MS_M2, MusWindow::OnPopupMenuSymbole ) // popup menu
	EVT_MENU_RANGE( ID_MS_PNT, ID_MS_BAR, MusWindow::OnPopupMenuSymbole ) // popup menu
    */
    // MIDI
    EVT_COMMAND  ( ID_MIDI_INPUT, AX_EVT_MIDI, MusWindow::OnMidiInput)
END_EVENT_TABLE()

MusWindow::MusWindow( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style, bool center ) :
    wxScrolledWindow( parent, id, position, size, style ), AxUndo( 100 )
{
	m_f = NULL;
    m_fh = NULL;
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
	m_notation_mode = MENSURAL_MODE;
	m_insertx = 0;
	m_insertcode = F6;
	m_insertoct = 4;
	m_dragging_x = 0;
	m_dragging_y_offset = 0;
	m_lyricMode = false;
	m_inputLyric = false;
	m_lyricCursor = 0;
	
	// keyboard entry
	m_keyEntryMode = false;
	m_keyEntryOctave = 0;

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
	//	m_toolpanel->SetMusWindow( NULL );
	if ( m_bufferElement )
		delete m_bufferElement;
}


// undo
void MusWindow::Load( AxUndoFile *undoPtr )
{
	wxASSERT_MSG( m_f, "MusFile should not be NULL in UNDO");

	if ( !m_f )
		return;
	
	int page, staff, element, lyric_element;
		
	MusBinInput *bin_input = new MusBinInput( m_f, undoPtr->GetFilename() );

	// keep current page, staff and element
	bin_input->Read( &page, sizeof( int ));
	bin_input->Read( &staff, sizeof( int ));
	bin_input->Read( &element, sizeof( int ));
    bin_input->Read( &lyric_element, sizeof( int ) );
    // edition state
    bin_input->Read( &m_editElement, sizeof( bool ) );
	bin_input->Read( &m_lyricMode, sizeof( bool ) );
	bin_input->Read( &m_inputLyric, sizeof( bool ) );
	bin_input->Read( &m_lyricCursor, sizeof( int ) );
		
	if ( undoPtr->m_flags == MUS_UNDO_FILE )
	{
	    bin_input->ImportFile();
		PaperSize();
	}
	else if ( undoPtr->m_flags == MUS_UNDO_PAGE )
	{	
		MusPage *musPage = new MusPage();
		bin_input->ReadPage( musPage );
		m_f->m_pages.RemoveAt( page );
		m_f->m_pages.Insert( musPage, page );
	}
	
	if ((page < 0) || (page > m_fh->nbpage - 1))
	{
		delete bin_input;
		return;
	}
	
	if ( (m_npage != page) || (undoPtr->m_flags != MUS_UNDO_STAFF) )
		SetPage( &m_f->m_pages[page] );
	m_npage = page;

	MusStaff *previous = NULL;
	if ( undoPtr->m_flags == MUS_UNDO_STAFF )
	{
		previous = m_currentStaff;
		if ( !previous || (previous->no == staff) )
			previous = NULL; // this staff will be deleted
		MusStaff *musStaff = new MusStaff();
		bin_input->ReadStaff( musStaff );
		// keep xrel and yrel
		musStaff->xrel = m_page->m_staves[ musStaff->no ].xrel;
		musStaff->yrel = m_page->m_staves[ musStaff->no ].yrel;
		musStaff->Init( this );
		// clear and remove previous staff
		m_page->m_staves.RemoveAt( musStaff->no );
		// replace
		m_page->m_staves.Insert( musStaff, musStaff->no );	
	}
	delete bin_input;
	
	m_currentElement = NULL;
	m_currentStaff = NULL;
	
	if ( staff != -1 ) {
		m_currentStaff = &m_page->m_staves[staff];
    }	
	if ( m_currentStaff && (element != -1) )
	{
        if ( (lyric_element != -1) && (&m_currentStaff->m_elements[element])->IsNote() ) {
            MusNote *note = (MusNote*)&m_currentStaff->m_elements[element];
            m_currentElement = note->GetLyricNo( lyric_element);
        } else { 
            m_currentElement = &m_currentStaff->m_elements[element];
        }

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

	// keep current page, staff and element and lyric
	int page = -1;
	int staff = -1;
	int element = -1;
    int lyric_element = -1;
    
	if ( m_page ) {
		page = m_page->npage - 1;
    }
	if ( m_currentStaff ) {
		staff = m_currentStaff->no;
    }
	if ( m_currentElement ) {
        if ( m_currentElement->IsSymbol() && ((MusSymbol*)m_currentElement)->IsLyric() ) {
            element = ((MusSymbol*)m_currentElement)->m_note_ptr->no;
            lyric_element = m_currentElement->no;
            wxLogDebug("lyric %d", lyric_element );
        } else {
            element = m_currentElement->no;
        }
    }
		
    MusBinOutput *bin_output = new MusBinOutput( m_f, undoPtr->GetFilename() );
	
	bin_output->Write( &page, sizeof( int ) );
	bin_output->Write( &staff, sizeof( int ) );
	bin_output->Write( &element, sizeof( int ) );
    bin_output->Write( &lyric_element, sizeof( int ) );
    // edition state
    bin_output->Write( &m_editElement, sizeof( bool ) );
	bin_output->Write( &m_lyricMode, sizeof( bool ) );
	bin_output->Write( &m_inputLyric, sizeof( bool ) );
	bin_output->Write( &m_lyricCursor, sizeof( int ) );
		
	if ( undoPtr->m_flags == MUS_UNDO_FILE )
	{
	    bin_output->ExportFile();
	}
	else if ( undoPtr->m_flags == MUS_UNDO_PAGE )
	{	
		wxASSERT_MSG( m_page, "MusPage should not be NULL in UNDO");
		bin_output->WritePage( m_page );

	}
	else if ( undoPtr->m_flags == MUS_UNDO_STAFF )
	{
		wxASSERT_MSG( m_currentStaff, "MusStaff should not be NULL in UNDO");
		bin_output->WriteStaff( m_currentStaff );
	
	}

    delete bin_output;

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
		m_page = NULL;
		zoomNum = 4;
		return;
	}

	m_f = file;
	m_fh = &file->m_fheader;
	m_npage = 0;
	zoomNum = 4;
	PaperSize();
	SetPage( &file->m_pages[m_npage] );
	//CheckPoint( UNDO_ALL, MUS_UNDO_FILE );
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
	
	info.FromString( wxGetApp().m_neumeFontDesc );
	m_ftFestaDies.SetNativeFontInfo( info );
	
	
	if ( !m_ftLeipzig.Ok() )
		wxLogWarning(_("Impossible to load font 'Leipzig'") );
	if ( !m_ftFestaDies.Ok() )
		wxLogWarning(_("Impossible to load font 'Festa Dies'") );
	
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
	
	m_activeChantFonts[0][0] = m_ftFestaDies;
    m_activeChantFonts[0][1] = m_ftFestaDies;
    m_activeChantFonts[1][0] = m_ftFestaDies;
    m_activeChantFonts[1][1] = m_ftFestaDies;
	
	
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

	m_activeChantFonts[0][0].SetPointSize( ToZoom( nTailleFont[0][0] ) );
    m_activeChantFonts[0][1].SetPointSize( ToZoom( nTailleFont[0][1] ) );
    m_activeChantFonts[1][0].SetPointSize( ToZoom( nTailleFont[1][0] ) );
    m_activeChantFonts[1][1].SetPointSize( ToZoom( nTailleFont[1][1] ) );
	
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
	
	printf("Char Defin: %d\n", m_charDefin);
}


void MusWindow::UpdatePageValues() 
{
	if ( !m_page || !m_fh ) 
		return;

	// margins
	int page = m_page->npage;
	if ( m_fh->param.entetePied & PAGINATION )
		page += m_f->m_pagination.numeroInitial;

	if (page % 2)	//pages impaires 
		mrgG = - m_fh->param.MargeGAUCHEIMPAIRE*10;
	else
		mrgG = - m_fh->param.MargeGAUCHEPAIRE*10;
	mrgG = ToZoom (mrgG);


	if (m_charDefin != m_page->defin)
		m_charDefin = 0;
    int defin = m_page->defin;

    RapportPortee[0] = m_fh->param.rapportPorteesNum;
    RapportPortee[1] = m_fh->param.rapportPorteesDen;
    RapportDimin[0] = m_fh->param.rapportDiminNum;
    RapportDimin[1] = m_fh->param.rapportDiminDen;

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


	
	//do I really have to do all this setup again for Festa Dies? maybe not
	
	
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

    largeurBreve[0] = (int)((w * 0.8 /*1.1667*/) / 2);
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

	if ( !m_page || !m_fh ) 
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
	m_toolpanel->SetMusWindow( this );
	SyncToolPanel();
}

void MusWindow::SetNotationMode( bool mode )
{
	m_notation_mode = mode;
}

void MusWindow::SetInsertMode( bool mode )
{
	if ( m_editElement == !mode )
		return; // nothing to change

    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
    kevent.SetId( this->GetId() );
    kevent.SetEventObject( this );
	kevent.m_keyCode = WXK_RETURN;
    this->ProcessEvent( kevent );
}

void MusWindow::SetToolType( int type )
{
    int value = '0';
    switch ( type )
    {
    case (MUS_TOOLS_NOTES): value = 'M'; break; // I changed this to 'M' so 'N' can be used by neumes
    case (MUS_TOOLS_CLEFS): value = 'K'; break;
    case (MUS_TOOLS_SIGNS): value = 'P'; break;
    case (MUS_TOOLS_OTHER): value = 'S'; break;
	case (NEUME_TOOLS_NOTES): value = 'N'; break;
	case (NEUME_TOOLS_CLEFS): value = 'K'; break;
	case (NEUME_TOOLS_OTHER): value = 'S'; break;
	}
        
	//we go to the EVT_KEY_DOWN event here... (MusWindow::OnKeyDown)
	
    wxKeyEvent kevent;
    kevent.SetEventType( wxEVT_KEY_DOWN );
	kevent.SetId( this->GetId() );
    kevent.SetEventObject( this );
    kevent.m_keyCode = value;
    kevent.m_controlDown = true;
    this->ProcessEvent( kevent );
}

/* I really don't know why this works... if the ternary operations check against MENSURAL_MODE
 * then clicking a control while in neume editing mode will change the toolbar back to the mensural one.
 * Weird and messy code.
 */

int MusWindow::GetToolType()
{
	if (m_notation_mode == MENSURAL_MODE)
		printf("we're in mensural mode	\n");
	else
		printf("we're in neumes mode\n");
	
	MusElement *sync = NULL;

	if (m_editElement)
		sync = m_currentElement;
	else
		sync = m_newElement;				//need to make a new element!! somehow?

	if (!sync) {
        return -1;
    }
    if ( sync->IsSymbol() )
    {
        if ( ((MusSymbol*)sync)->flag == CLE )
            return m_notation_mode == MENSURAL_MODE ? MUS_TOOLS_CLEFS : NEUME_TOOLS_CLEFS;
        else if ( ((MusSymbol*)sync)->flag == IND_MES )	
            return MUS_TOOLS_SIGNS;
        else
            return m_notation_mode == MENSURAL_MODE ? MUS_TOOLS_OTHER : NEUME_TOOLS_OTHER;
    } 
    else if (sync->IsNote() ) 
    {
        //return m_notation_mode == MENSURAL_MODE ? MUS_TOOLS_NOTES : NEUME_TOOLS_NOTES;
		return MUS_TOOLS_NOTES;
		
		//what should I do here?????
    }
	else if (sync->IsNeume() )
	{
		return NEUME_TOOLS_NOTES;
	}
    else {
        return -1;
    }

}

void MusWindow::SyncToolPanel()
{
	int tool = this->GetToolType();

	if ( !m_toolpanel )
		return;

	if ( tool == -1 )
		tool = MUS_TOOLS_NOTES;
	m_toolpanel->SetTools( tool, this->m_editElement );

	this->SetFocus();
}

void MusWindow::Copy()
{
	if ( !m_currentElement )
		return;

	if ( m_bufferElement )
		delete m_bufferElement;

	if ( m_currentElement->IsSymbol() )
		m_bufferElement = new MusSymbol( *(MusSymbol*)m_currentElement );
	else if (m_currentElement->IsNote() )
		m_bufferElement = new MusNote( *(MusNote*)m_currentElement );
	else if (m_currentElement->IsNeume() )
		m_bufferElement = new MusNeume( *(MusNeume*)m_currentElement );
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
/*
	if ( !m_page || !m_currentStaff )
		return;

	MusSymbol *symbol = NULL;

	if ( m_editElement )
	{
		if ( !m_currentElement || ( !m_currentElement->IsSymbol()) )
			return;
		else
			 symbol = (MusSymbol*)m_currentElement;
	}
	else
	{
		symbol = new MusSymbol( );
		symbol->xrel = m_insertx;
	}

	int id = event.GetId();

	if ( in(id , ID_MS_G1, ID_MS_F5) )
	{
		symbol->flag = CLE; 
		switch ( id )
		{
		case ( ID_MS_G1 ): symbol->code = SOL1; break;
		case ( ID_MS_G2 ): symbol->code = SOL2; break;
		case ( ID_MS_U1 ): symbol->code = UT1; break;
		case ( ID_MS_U2 ): symbol->code = UT2; break;
		case ( ID_MS_U3 ): symbol->code = UT3; break;
		case ( ID_MS_U4 ): symbol->code = UT4; break;
		case ( ID_MS_U5 ): symbol->code = UT5; break;
		case ( ID_MS_F3 ): symbol->code = FA3; break;
		case ( ID_MS_F4 ): symbol->code = FA4; break;
		case ( ID_MS_F5 ): symbol->code = FA5; break;
		}
	}
	else if ( in(id , ID_MS_DIESE, ID_MS_DBEMOL) )
	{
		symbol->flag = ALTER;
		symbol->code = F6;
		symbol->oct = 4;
		switch ( id )
		{
		case ( ID_MS_DIESE): symbol->calte = DIESE; break;
		case ( ID_MS_BEMOL): symbol->calte = BEMOL; break;
		case ( ID_MS_BECAR): symbol->calte = BECAR; break;
		case ( ID_MS_DDIESE): symbol->calte = D_DIESE; break;
		case ( ID_MS_DBEMOL): symbol->calte = D_BEMOL; break;
		}
	}
	else if ( id == ID_MS_PNT )
	{
		symbol->flag = PNT;
		symbol->code = F6;
		symbol->oct = 4;
	}

	//if ( !m_editElement )
	//	m_currentStaff->Insert( symbol );

	this->Refresh();
*/
}


void MusWindow::OnMouseDClick(wxMouseEvent &event)
{
	
	//adding this up here because this happens in both edition and insertion modes
	if (m_currentStaff && m_newElement) 
	{
		wxClientDC dc( this );
		InitDC( &dc );
		m_insertx = ToReel( dc.DeviceToLogicalX( event.m_x ) ); //???
		int y = ToReelY( dc.DeviceToLogicalY( event.m_y ) );
		m_insertcode = m_currentStaff->trouveCodNote( y, m_insertx, &m_insertoct );
		m_newElement->xrel = m_insertx;
		printf("This is our xrel: %d\n is it a a note? %d\n", m_insertx, m_newElement->IsNote());
	}
	if ( m_editElement )
	{
        // TODO for cursor
        // Switch to insertion mode, which means that m_newElement will point to something (see OnKeyDown)
        // Get the x position for the cursor and use it for m_newElement (see m_insertx below in this method)
        // Also make sure we get a current staff, but this should not be a problem because we get it in OnMouseLeftDown, I think
        
		SetInsertMode(true);
		//get x position (for use later with drawing)
		/*
		if ( event.ButtonDClick( wxMOUSE_BTN_LEFT  ) && m_currentElement )
		{
			wxMenuBar *menubar = MusEditMenuFunc( );
			wxMenu *menu = menubar->Remove( 0 );

			int submenu_id = 0;
			if ( m_currentElement->TYPE == SYMB )
			{
				MusSymbol *symbol = (MusSymbol*)m_currentElement;
				if ( symbol->flag == CLE )
					submenu_id = ID_MS_KEYS;
				else if ( symbol->flag == IND_MES )
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
//			wxClientDC dc( this );
//			InitDC( &dc );
//			m_insertx = ToReel( dc.DeviceToLogicalX( event.m_x ) );
//			int y = ToReelY( dc.DeviceToLogicalY( event.m_y ) );
//			m_insertcode = m_currentStaff->trouveCodNote( y, m_insertx, &m_insertoct );
			
			
//			m_newElement->xrel = m_insertx;
			if ( m_newElement->IsNote() || m_newElement->IsNeume() ||
				(((MusSymbol*)m_newElement)->flag == ALTER) || (((MusSymbol*)m_newElement)->flag == PNT))
			{
				m_newElement->code = m_insertcode;
				m_newElement->oct = m_insertoct;
			}
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			m_lastEditedElement = m_currentStaff->Insert( m_newElement );
			
            // TODO for cursor
            // move the cursor on step forward
            // we will need to deal with staff and page break when reaching the end
            // we will probably have a method for this, because we need to do the same when inputing from the keyboard
            // for now, just increase the xrel in m_newElement

			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();

			//wxLogMessage("code %d oct %d", m_insertcode, m_insertoct );

			/*wxMenuBar *menubar = MusEditMenuFunc( );
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
	if ( m_editElement || m_lyricMode )
	{
		m_dragging_x = 0;
		m_dragging_y_offset = 0;
		if ( m_has_been_dragged == true ){
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
			SyncToolPanel();
			m_has_been_dragged = false;
		}
	}
	
	if ( m_currentElement && m_currentElement->IsSymbol() && ((MusSymbol*)m_currentElement)->IsLyric() ){
		MusNote *note = ((MusSymbol*)m_currentElement)->m_note_ptr;
		if (note) {
            note->CheckLyricIntegrity();
        }
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
	wxClientDC dc( this );
	InitDC( &dc );
	
	printf("clicked: {%d, %d}\n", ToReel(dc.DeviceToLogicalX(event.m_x)),
		   ToReelY(dc.DeviceToLogicalY(event.m_y)));
	
	if ( m_editElement || m_lyricMode )
	{
//		wxClientDC dc( this );
//		InitDC( &dc );
//		
//		printf("clicked: {%d, %d}\n", ToReel(dc.DeviceToLogicalX(event.m_x)),
//			   ToReelY(dc.DeviceToLogicalY(event.m_y)));
		
		if ( m_currentElement &&  m_currentStaff ) 
			m_currentElement->ClearElement( &dc, m_currentStaff );

		m_has_been_dragged = false;
		m_dragging_x  = ToReel( dc.DeviceToLogicalX( event.m_x ) );
		int x = m_dragging_x - 3;
		int y = ToReelY( dc.DeviceToLogicalY( event.m_y ) );
		//wxLogMessage("x %d : y %d", x, y);
			
		/*** Picking element closest to mouse click location ***/
		
		// Default selection of closest note
		MusStaff *noteStaff = m_page->GetAtPos( y );
		MusElement *noteElement = noteStaff->GetAtPos( x );				

		m_lyricMode = false;
		m_inputLyric = false;
		m_editElement = true;
		m_currentStaff = noteStaff;
		m_currentElement = noteElement;
		
		// Checking if there is a Lyric element closer to click location then default note
		MusStaff *lyricStaff;
		if ( noteStaff->yrel <= (uint)(y + 80)){
			if ((lyricStaff = m_page->GetPrevious( noteStaff )) == NULL) lyricStaff = noteStaff;
		} else {
			if ((lyricStaff = m_page->GetNext( noteStaff )) == NULL) lyricStaff = noteStaff;
		}		
		int y_note = noteStaff->yrel;
        if (noteElement) {
            y_note += noteElement->dec_y;
        }
		MusElement *lyricElement = lyricStaff->GetLyricAtPos( x );		// Lyric element closest to mouse click
		
		if ( lyricElement != NULL){
			int y_lyric = lyricElement->dec_y + lyricStaff->yrel;
			if ( abs( y_lyric - y ) <= abs( y_note - y ) ){				// Checking if lyric element is closer than note element
				m_lyricMode = true;
				m_inputLyric = false;
				m_editElement = false;
				
				m_currentStaff = lyricStaff;
				m_currentElement = lyricElement;
			}
		}
		
		// Track motion on y-axis
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
				else if ( tmp->IsSymbol() )
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
	wxClientDC dc( this );
//	printf("----- x: %d, y: %d -----\n", ToReel( dc.DeviceToLogicalX( event.m_x ) ),
//		   ToReelY( dc.DeviceToLogicalY( event.m_y ) ) );

	if ( event.Dragging() && event.LeftIsDown() && m_dragging_x && m_currentElement )
	{
		if ( !m_has_been_dragged )
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
		m_has_been_dragged = true;
		//wxClientDC dc( this );
		InitDC( &dc );
		m_insertx = ToReel( dc.DeviceToLogicalX( event.m_x ) );
		int y = ToReelY( dc.DeviceToLogicalY( event.m_y ) ) - m_dragging_y_offset;
		
		if ( m_editElement )
		{
			m_insertcode = m_currentStaff->trouveCodNote( y, m_insertx, &m_insertoct );
			m_currentElement->SetPitch( m_insertcode, m_insertoct, m_currentStaff );
		} 
		else if ( m_lyricMode )					// Movement of lyric element on y-axis
		{
			m_currentElement->dec_y = y - m_currentStaff->yrel;
		} 
		
		if ( m_insertx != m_dragging_x  )		// If element has moved in the x-axis
		{
			m_currentElement->ClearElement( &dc, m_currentStaff );
			m_currentElement->xrel += ( m_insertx - m_dragging_x );
			m_dragging_x = m_insertx;
			if ( m_editElement )
				m_currentStaff->CheckIntegrity();
			//OnEndEdition();
		}
		this->Refresh();	
	} 

	event.Skip();
}

// see TakeMidiEvent in WGCLE in Wolfgang
static int bem [] = {F2, F3, F3, F4, F4, F5, F6, F6, F7, F7, F8, F8};
static int die [] = {F2, F2, F3, F3, F4, F5, F5, F6, F6, F7, F7, F8};

void MusWindow::OnMidiInput(wxCommandEvent &event)
{
    //wxLogDebug("Midi %d", event.GetInt() ) ;
    int octave = event.GetInt() / 12;
	int hauteur = event.GetInt() - (octave * 12);
	
	printf("octave: %d, hauteur: %d\n", octave, hauteur);
    if ( m_currentElement && m_currentElement->IsNote() ) {
        m_currentElement->SetPitch( die[hauteur], octave, m_currentStaff );
    }
    
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
	
	//hijack the keyboard
	if (m_keyEntryMode) 
	{
		KeyboardEntry(event);
		return;
	}
	
	//otherwise, check if command-K is pressed
	
	if (event.GetKeyCode() == 'K' && event.CmdDown())
	{
		//printf("command K\n");
		m_keyEntryMode = !m_keyEntryMode;
	}

	int noteKeyCode = GetNoteValue( event.m_keyCode );

	// change mode edition -- insertion
	if ( !m_lyricMode && event.GetKeyCode() == WXK_RETURN )
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
				else if ( m_currentElement->IsSymbol() )
				{
					m_symbol = *(MusSymbol*)m_currentElement;
					m_newElement = &m_symbol;
				}
				else if ( m_currentElement->IsNeume() )
				{
					m_neume = *(MusNeume*)m_currentElement;
					m_newElement = &m_neume;
				}
                // TODO for cursor
                // increase the xrel of m_newElement. Where it will be tricky is when we are at the end of the staff,
                // but leave this problem for now
			}
			else
			{
				m_newElement = &m_note;
				m_lastEditedElement = NULL;
                // TODO for cursor
                // More to do here, because we know nothing about the position:
                // My suggestion: beginning of the staff (try with xrel = something like 10)
                // We also have to check that we have a current staff. If not, select the first one
				
				
				//testing..
//				printf("moving xrel to beginning?\n");
//				m_newElement->xrel = 10;
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
	else if ( m_editElement ) // mode edition
	{
		if ( ((event.m_keyCode == WXK_DELETE ) || (event.m_keyCode == WXK_BACK)) && m_currentElement) //"Delete or Backspace" event
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			MusElement *del = m_currentElement;
			MusStaff *delstaff = m_currentStaff;

			if (event.m_keyCode == WXK_DELETE )		//"Delete" event
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
			else									//"Backspace" event
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
				CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
				m_currentStaff = tmp;
				m_currentElement = del;
			}
			else
				CheckPoint( UNDO_PART, MUS_UNDO_STAFF );

			this->Refresh();
			OnEndEdition();
			SyncToolPanel();
		}
		else if ( in ( event.m_keyCode, WXK_F1, WXK_F9 ) && m_currentElement) // Change hauteur
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			m_insertcode = m_currentElement->filtrcod( event.m_keyCode - WXK_F1, &m_insertoct );
			m_currentElement->SetPitch( m_insertcode, m_insertoct, m_currentStaff );
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( event.m_controlDown && ( event.m_keyCode == WXK_UP ) && m_currentElement) // correction hauteur avec les fleches, up
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			m_insertcode = m_currentElement->filtrcod( m_currentElement->code + 1, &m_insertoct );
			m_currentElement->SetPitch( m_insertcode, m_insertoct, m_currentStaff );
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( event.m_controlDown && ( event.m_keyCode == WXK_DOWN ) && m_currentElement) // correction hauteur avec les fleches, down
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			m_insertcode = m_currentElement->filtrcod( m_currentElement->code - 1, &m_insertoct );
			m_currentElement->SetPitch( m_insertcode, m_insertoct, m_currentStaff );
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() && 
			( (event.m_keyCode == 'B') || (event.m_keyCode == 'D' ) ) ) // ajouter un bemol à une note
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
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
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == '.')  ) // ajouter un point
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			MusSymbol point;
			point.flag = PNT;
			point.code = m_currentElement->code;
			point.oct = m_currentElement->oct;
			point.xrel = m_currentElement->xrel + _pas * 3;
			// special case where we move forward
			m_currentElement = m_currentStaff->Insert( &point );
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() &&
			(in( noteKeyCode, 0, 7 ) || (noteKeyCode == CUSTOS))) // change duree sur une note ou un silence
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			int vflag = ( event.m_controlDown || (noteKeyCode == CUSTOS)) ? 1 : 0;
			m_currentElement->SetValue( noteKeyCode , m_currentStaff, vflag );
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == 'L')  ) // Ligature 
		{	
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			m_currentElement->SetLigature( m_currentStaff );
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == 'I')  ) // Change coloration
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			m_currentElement->ChangeColoration( m_currentStaff );
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );	
			OnEndEdition();
		}
		else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == 'A')  ) // Change stem direction
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			m_currentElement->ChangeStem( m_currentStaff );
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( event.m_controlDown && (( event.m_keyCode == WXK_LEFT ) || (event.m_keyCode == WXK_RIGHT )) && m_currentElement) // moving element
      {
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			if ( event.m_keyCode == WXK_LEFT )
				m_currentElement->xrel -=3;
			else
				m_currentElement->xrel +=3;
			this->Refresh();
			m_currentStaff->CheckIntegrity();
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
		}
		else if ( event.m_keyCode == 'T' )
		{
			m_editElement = false;
			m_lyricMode = true;
			
			if ( m_currentElement && m_currentElement->IsNote() && ((MusNote*)m_currentElement)->m_lyrics.GetCount() > 0 ){
				MusSymbol *lyric = &((MusNote*)m_currentElement)->m_lyrics[0];
				m_currentElement = lyric;
			}
			else if ( m_currentElement && m_currentElement->IsNote() ){
				MusSymbol *lyric = new MusSymbol();
				lyric->flag = CHAINE;
				lyric->fonte = LYRIC;
				lyric->m_debord_str = "";
				lyric->xrel = ((MusNote*)m_currentElement)->xrel - 10;
				lyric->dec_y = - STAFF_OFFSET;   //Add define for height
				lyric->offset = ((MusNote*)m_currentElement)->offset;
				lyric->m_note_ptr = (MusNote*)m_currentElement;
				((MusNote*)m_currentElement)->m_lyrics.Add( lyric );
				m_currentElement = lyric;
				m_inputLyric = true;
			}
			else if ( m_currentElement && m_currentStaff->GetLyricAtPos( m_currentElement->xrel ) )
				m_currentElement = m_currentStaff->GetLyricAtPos( m_currentElement->xrel );
			else if ( m_currentStaff->GetFirstLyric() )
				m_currentElement = m_currentStaff->GetFirstLyric();
			else{
				m_editElement = true;
				m_lyricMode = false;
			}
         this->Refresh();
		} 
		else if ( m_currentElement && m_currentElement->IsSymbol() &&
				 in( event.m_keyCode, 33, 125) ) // any other keycode on symbol (ascii codes)
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			int vflag = ( event.m_controlDown ) ? 1 : 0;
			m_currentElement->SetValue( event.m_keyCode, m_currentStaff, vflag );
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );	
			OnEndEdition();
		}		
		else // navigation avec les fleches
		{
			if ( event.GetKeyCode() == WXK_RIGHT || event.GetKeyCode() == WXK_SPACE ) 
			{
				if ( m_currentStaff->GetNext( m_currentElement ) )
					m_currentElement = m_currentStaff->GetNext( m_currentElement );
				else if ( m_page->GetNext( m_currentStaff ) )
				{
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
					m_currentStaff = m_page->GetNext( m_currentStaff );
					m_currentElement = m_currentStaff->GetAtPos(x);
					UpdateScroll();
				}
			}
			else if ( event.GetKeyCode() == WXK_HOME ) 
			{
				if ( m_currentStaff->GetFirst( ) )
					m_currentElement = m_currentStaff->GetFirst( );
			}
			else if ( event.GetKeyCode() == WXK_END ) 
			{
				if ( m_currentStaff->GetLast( ) )
					m_currentElement = m_currentStaff->GetLast( );
			}
			this->Refresh();
			OnEndEdition();
			SyncToolPanel();
		}
	}
	else if ( m_lyricMode && !m_inputLyric )	/*** Lyric Editing mode ***/
	{	
		if ( event.m_keyCode == 'T' )			//"T" event: Escape lyric navigation mode
		{
			m_lyricMode = false;
			m_inputLyric = false;
			m_editElement = true;
			if ( m_currentElement && m_currentElement->IsSymbol() && ((MusSymbol*)m_currentElement)->m_note_ptr )
				m_currentElement = ((MusSymbol*)m_currentElement)->m_note_ptr;
			else if ( m_currentElement && m_currentStaff->GetAtPos( m_currentElement->xrel ) )
				m_currentElement = m_currentStaff->GetAtPos( m_currentElement->xrel );
			else if ( m_currentStaff->GetFirst() )
				m_currentElement = m_currentStaff->GetFirst();
			else if ( m_page->GetFirst() ){
				m_currentStaff = m_page->GetFirst();
				m_currentElement = m_currentStaff->GetFirst();
			}
				
			this->Refresh();
		} 
		else if ( event.m_keyCode == WXK_RETURN && m_currentElement && m_currentElement->IsSymbol() )		//"Return" event: Enter lyric insertion mode
		{
			m_inputLyric = !m_inputLyric;
			m_lyricCursor = m_currentElement->m_debord_str.Length();
			this->Refresh();
		}
		else if ( event.m_controlDown && in( event.m_keyCode, WXK_LEFT, WXK_DOWN ) && m_currentElement)		//"Ctr + navigation arrow" event
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			if ( event.GetKeyCode() == WXK_RIGHT && m_currentElement->IsSymbol() )			//"Right arrow" event: switch lyric association to note to the right 
			{				
				MusNote *oldNote = ((MusSymbol*)m_currentElement)->m_note_ptr;
				MusNote *newNote = m_currentStaff->GetNextNote( (MusSymbol*)m_currentElement );
				m_currentStaff->SwitchLyricNoteAssociation( (MusSymbol*)m_currentElement, oldNote, newNote, true );
				oldNote->CheckLyricIntegrity();
				newNote->CheckLyricIntegrity();
			}
			else if ( event.GetKeyCode() == WXK_LEFT && m_currentElement->IsSymbol() )		//"Left arrow" event: switch lyric association to note to the left
			{
				MusNote *oldNote = ((MusSymbol*)m_currentElement)->m_note_ptr;
				MusNote *newNote = m_currentStaff->GetPreviousNote( (MusSymbol*)m_currentElement );				
				m_currentStaff->SwitchLyricNoteAssociation( (MusSymbol*)m_currentElement, oldNote, newNote, false );
                if ( oldNote ) {
                    oldNote->CheckLyricIntegrity();
                }
                if ( newNote ) {
                    newNote->CheckLyricIntegrity();
                }
			}
			else if ( event.GetKeyCode() == WXK_UP && m_currentElement->IsSymbol() )		//"Up arrow" event: Move lyric line up 
			{
				m_currentStaff->AdjustLyricLineHeight( 3 );
			}
			else if ( event.GetKeyCode() == WXK_DOWN && m_currentElement->IsSymbol() )		//"Down arrow" event: Move lyric line down 
			{
				m_currentStaff->AdjustLyricLineHeight( -3 );
			}
			CheckPoint(UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
			this->Refresh();
		}
		else if ( ( ( event.m_keyCode == WXK_DELETE ) || ( event.m_keyCode == WXK_BACK ) ) && m_currentElement 
			&& m_currentElement->IsSymbol() )												//"Delete or Backspace" event
		{
			
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			MusElement *del = m_currentElement;
			MusStaff *delstaff = m_currentStaff;
			
			// Find next element to select
			if (event.m_keyCode == WXK_DELETE )												//"Delete" event
			{
				if ( m_currentStaff->GetNextLyric( (MusSymbol*)del ) )
					m_currentElement = m_currentStaff->GetNextLyric( (MusSymbol*)del );
				else if ( m_page->GetNext( m_currentStaff ) )
				{
					m_currentStaff = m_page->GetNext( m_currentStaff );
					m_currentElement = m_currentStaff->GetFirstLyric();
				}
				else
					m_currentElement = NULL;
			}
			else																			//"Backspace" event
			{
				if ( m_currentStaff->GetPreviousLyric( (MusSymbol*)del ) )
					m_currentElement = m_currentStaff->GetPreviousLyric( (MusSymbol*)del );
				else if ( m_page->GetPrevious( m_currentStaff ) )
				{
					m_currentStaff = m_page->GetPrevious( m_currentStaff );
					m_currentElement = m_currentStaff->GetLastLyric();
				}
				else
					m_currentElement = NULL;
			}
			
			delstaff->DeleteLyric( (MusSymbol*)del );
			if ( m_currentStaff != delstaff )
			{
				// Reset previous staff with no element before checkpoint and then swap again
				MusStaff *tmp = m_currentStaff;
				m_currentStaff = delstaff;
				del = m_currentElement;
				m_currentElement = NULL;
				CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
				m_currentStaff = tmp;
				m_currentElement = del;
			}
			else
				CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			
			OnEndEdition();
			SyncToolPanel();
			this->Refresh();
		}
		else	// Navigation over lyrics using arrows
		{
			if ( event.GetKeyCode() == WXK_RIGHT && m_currentElement && m_currentElement->IsSymbol() )			//"Right arrow" event
			{
				if ( m_currentStaff->GetNextLyric( (MusSymbol*)m_currentElement ) )
					m_currentElement = m_currentStaff->GetNextLyric( (MusSymbol*)m_currentElement );
				else if ( m_page->GetNext( m_currentStaff ) )
				{
					m_currentStaff = m_page->GetNext( m_currentStaff );
					m_currentElement = m_currentStaff->GetFirstLyric();
				}
				UpdateScroll();
			}
			else if ( event.GetKeyCode() == WXK_LEFT && m_currentElement && m_currentElement->IsSymbol() )		//"Left arrow" event
			{
				if ( m_currentStaff->GetPreviousLyric( (MusSymbol*)m_currentElement ) )
					m_currentElement = m_currentStaff->GetPreviousLyric( (MusSymbol*)m_currentElement );
				else if ( m_page->GetPrevious( m_currentStaff ) )
				{
					m_currentStaff = m_page->GetPrevious( m_currentStaff );
					m_currentElement = m_currentStaff->GetLastLyric();
				}
				UpdateScroll();
			}
			else if ( event.GetKeyCode() == WXK_UP && m_currentElement && m_currentElement->IsSymbol() )		//"Up arrow" event
			{
				if ( m_page->GetPrevious( m_currentStaff ) )
				{
					int x = 0;
					if ( m_currentElement )
						x = m_currentElement->xrel;
					m_currentStaff = m_page->GetPrevious( m_currentStaff );
					m_currentElement = m_currentStaff->GetLyricAtPos(x);
					UpdateScroll();
				}
			}
			else if ( event.GetKeyCode() == WXK_DOWN && m_currentElement && m_currentElement->IsSymbol() )		//"Down arrow" event
			{
				if ( m_page->GetNext( m_currentStaff ) )
				{
					int x = 0;
					if ( m_currentElement )
						x = m_currentElement->xrel;
					m_currentStaff = m_page->GetNext( m_currentStaff );
					m_currentElement = m_currentStaff->GetLyricAtPos(x);
					UpdateScroll();
				}
			}
			else if ( event.GetKeyCode() == WXK_HOME )										//"Home" event
			{
				if ( m_currentStaff->GetFirstLyric() )
					m_currentElement = m_currentStaff->GetFirstLyric();
			}
			else if ( event.GetKeyCode() == WXK_END )										//"End" event
			{
				if ( m_currentStaff->GetLastLyric() )
					m_currentElement = m_currentStaff->GetLastLyric();
			}
			this->Refresh();
			OnEndEdition();
			SyncToolPanel();
		}
	}
	else if ( m_lyricMode && m_inputLyric )		/*** Lyric insertion mode ***/
	{
		event.Skip();  //Do further processing on the event in OnChar method
		
		if ( event.m_keyCode == WXK_RETURN )									// "Enter" event: Enter into lyric insertion mode
		{
			if ( m_currentElement->IsSymbol() && ((MusSymbol*)m_currentElement)->IsLyric()){
				PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
				MusSymbol* lyric = (MusSymbol*)m_currentElement;
				if ( lyric->IsLyricEmpty() ){
					if ( (m_currentElement = m_currentStaff->GetNextLyric( lyric )) == NULL ){
						m_currentElement = m_currentStaff->GetPreviousLyric( lyric );
					}
					lyric->m_note_ptr->DeleteLyricFromNote( lyric );
				}				
				CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
				OnEndEdition();
			}

			m_inputLyric = !m_inputLyric;
		}
		else if ( event.m_keyCode == WXK_BACK && m_currentElement->IsSymbol() )		//"Backspace" event
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			m_lyricCursor--;
			MusSymbol *element = (MusSymbol*)m_currentElement;
			if ( !element->DeleteCharInLyricAt( m_lyricCursor ) ){
				if ( element->m_debord_str.Length() == 0 ){
					m_currentElement = m_currentStaff->GetPreviousLyric( element );
					if ( !m_currentElement ) m_currentElement = m_currentStaff->GetNextLyric( element );
					m_lyricCursor = ((MusSymbol*)m_currentElement)->m_debord_str.Length();
					element->m_note_ptr->DeleteLyricFromNote( element );					
				} else{
					m_lyricCursor++;
				}
			} 
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition(); 
		}
		else if ( event.m_keyCode == WXK_DELETE && m_currentElement->IsSymbol() )		//"Delete" event
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			((MusSymbol*)m_currentElement)->DeleteCharInLyricAt( m_lyricCursor );
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition(); 			
		}
		else if ( event.m_keyCode == WXK_SPACE && m_currentElement->IsSymbol() )		//"Space" event
		{
			if ( m_lyricCursor == (int)((MusSymbol*)m_currentElement)->m_debord_str.Length() ){
				if ( ((MusSymbol*)m_currentElement)->IsLastLyricElementInNote() ){
					MusNote *note = m_currentStaff->GetNextNote( (MusSymbol*)m_currentElement );
					if ( !note )
						return;
					
                    PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
					//Add new lyric element to the next note
					MusSymbol *lyric = new MusSymbol( *((MusSymbol*)m_currentElement) );
					lyric->m_debord_str = "";
					lyric->xrel = note->xrel - 10;
					m_currentStaff->SwitchLyricNoteAssociation( lyric, ((MusSymbol*)m_currentElement)->m_note_ptr, note, true );				
					(((MusSymbol*)m_currentElement)->m_note_ptr)->CheckLyricIntegrity();
					m_currentElement = lyric;
					m_lyricCursor = 0;
                    CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
				} else {
					m_currentElement = m_currentStaff->GetNextLyric( (MusSymbol*)m_currentElement );
					m_lyricCursor = 0;
				}
				OnEndEdition();
			} 
		}
		else if ( event.m_keyCode == WXK_TAB && m_currentElement->IsSymbol() )			//"Tab" event
		{
			if ( m_lyricCursor == (int)((MusSymbol*)m_currentElement)->m_debord_str.Length() ){
				if ( ((MusSymbol*)m_currentElement)->IsLastLyricElementInNote() ){
					MusNote *note = m_currentStaff->GetNextNote( (MusSymbol*)m_currentElement );
					if ( !note )
						return;
					
                    PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
					//Add new lyric to next note
					MusSymbol *lyric = new MusSymbol( *((MusSymbol*)m_currentElement) );
					lyric->m_debord_str = "";
					lyric->xrel = note->xrel - 10;					
					m_currentStaff->SwitchLyricNoteAssociation( lyric, ((MusSymbol*)m_currentElement)->m_note_ptr, note, true );				
					
					//Add dash ("-") element between the two lyrics
					MusSymbol *lyric2 = new MusSymbol( *((MusSymbol*)m_currentElement) );
					lyric2->m_debord_str = "-";
					int length = ((MusSymbol*)m_currentElement)->m_debord_str.Length();
					lyric2->xrel = lyric->xrel - ( lyric->xrel - ( m_currentElement->xrel + length * 10 ) ) / 2;			
					m_currentStaff->SwitchLyricNoteAssociation( lyric2, ((MusSymbol*)m_currentElement)->m_note_ptr, note, true );				
                    
					m_currentElement = lyric;
					m_lyricCursor = 0;
                    CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
				} else {
					m_currentElement = m_currentStaff->GetNextLyric( (MusSymbol*)m_currentElement );
					m_lyricCursor = 0;
				}
				OnEndEdition();
			}
		}
		else		//Left and right cursor navigation within lyrics
		{
			if ( event.m_keyCode == WXK_RIGHT && m_currentElement->IsSymbol() )			//"Right arrow" event
			{
				if ( m_lyricCursor < (int)((MusSymbol*)m_currentElement)->m_debord_str.Length() )
					m_lyricCursor++;
				else if ( m_lyricCursor == (int)((MusSymbol*)m_currentElement)->m_debord_str.Length() )
				{
					MusSymbol *element = (MusSymbol*)m_currentElement;
					if ( m_currentStaff->GetNextLyric( element ) ){
						m_currentElement = m_currentStaff->GetNextLyric( element );
						m_lyricCursor = 0;
					} else if ( m_page->GetNext( m_currentStaff ) ){
						m_currentStaff = m_page->GetNext( m_currentStaff );
						m_currentElement = m_currentStaff->GetFirstLyric();
						m_lyricCursor = 0;
					}
					
					if ( element->m_debord_str.Length() == 0 )
						element->m_note_ptr->DeleteLyricFromNote( element );
				}  
					
			}
			else if ( event.m_keyCode == WXK_LEFT && m_currentElement->IsSymbol() )		//"Left arrow" event
			{
				if ( m_lyricCursor > 0 )
					m_lyricCursor--;
				else if ( m_lyricCursor == 0 ){
					MusSymbol *element = (MusSymbol*)m_currentElement;
					if ( m_currentStaff->GetPreviousLyric( element ) ){
						m_currentElement = m_currentStaff->GetPreviousLyric( element );
						m_lyricCursor = ((MusSymbol*)m_currentElement)->m_debord_str.Length();	
					} else if ( m_page->GetPrevious( m_currentStaff ) ){
						m_currentStaff = m_page->GetPrevious( m_currentStaff );
						m_currentElement = m_currentStaff->GetLastLyric();
						m_lyricCursor = ((MusSymbol*)m_currentElement)->m_debord_str.Length();
					}
					if ( element->m_debord_str.Length() == 0 )			
						element->m_note_ptr->DeleteLyricFromNote( element );
				}
			}
			UpdateScroll();
		}
		this->Refresh();
	}
	else /*** Note insertion mode ***/
	{
		if ( event.m_controlDown && (event.m_keyCode == 'M')) // change set (note, rests, key, signs, symbols, ....
			m_newElement = &m_note;	
		else if ( event.m_controlDown && (event.m_keyCode == 'N')) {
			m_newElement = &m_neume;
		}
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
		else if ( event.m_controlDown && (event.m_keyCode == 'S')) // symbols
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
		else if ( m_newElement && m_newElement->IsSymbol() &&
			 in( event.m_keyCode, 33, 125) ) // any other keycode on symbol (ascii codes)
		{
			int vflag = ( event.m_controlDown ) ? 1 : 0;
			m_newElement->SetValue( event.m_keyCode, NULL, vflag );
		}
		OnEndEdition();
		SyncToolPanel();
		
	}
	
}

void MusWindow::OnChar(wxKeyEvent &event)
{
    if ( m_lyricMode && m_inputLyric )
	{
		if ( ((event.m_keyCode >= 65 && event.m_keyCode <= 90) || (event.m_keyCode >= 97 && event.m_keyCode <= 122 ))
			&& m_currentElement->IsSymbol() )
		{	
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			((MusSymbol*)m_currentElement)->InsertCharInLyricAt( m_lyricCursor, (char)event.m_keyCode );
			m_lyricCursor++;
            (((MusSymbol*)m_currentElement)->m_note_ptr)->CheckLyricIntegrity();
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
		}
    }
}


void MusWindow::OnPaint(wxPaintEvent &event)
{
	if ( !m_page || !m_fh )
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
    // TODO for cursor
    // Draw the cursor if we are in insertion mode, we have a m_newElement and a m_currentStaff
    // We can add a DrawCursor method, use the y position of the staff and the x of the element
    // What shape to draw??
	
	
	if (!m_editElement && m_newElement && m_currentStaff) {
		m_currentColour = wxRED;
		//drawing code here
		
		printf("staff y: %d\n", m_currentStaff->yrel);
		
		this->rect_plein2(&dc, m_newElement->xrel+35, m_currentStaff->yrel-200, 
						  m_newElement->xrel+40, m_currentStaff->yrel-40);
		
		m_currentColour = wxBLACK;
	}
}


//keyboard entry is based off GarageBand's 'Musical Typing' concept
//I'm not sure if its very practical to have accidentals other than Bb however...

void MusWindow::KeyboardEntry(wxKeyEvent &event) {
	
	if (event.GetKeyCode() == 'K' && event.CmdDown())
	{
		
		//change from edition to insertion mode?
		
		m_keyEntryMode = !m_keyEntryMode;
		return;
	}
	
	
	// it would be nice if octave switching happened 'instantly'
	
	switch (event.GetKeyCode()) {
		case WXK_RETURN: 
			//toggle neume type, if in neumes mode (possibly apply to mensural as well)
			printf("testing keyboard hijack\n");
			break;
		case WXK_SPACE:
			//single space: enter next pitch in compound neume
			//double space: 'break' the neume
			if (lastKeyEntered == WXK_SPACE) {
				
			} else {
				
			}
			printf("breaking group\n");
			break;
				case 'Z':
			//octave down
			m_keyEntryOctave--;
			printf("Moving down an octave: %d\n", m_keyEntryOctave);
			break;
			//ocave up
		case 'X':
			m_keyEntryOctave++;
			printf("Moving up an octave: %d\n", m_keyEntryOctave);
			break;
		
	}
	
	//for changing pitch
	
	int pitch;
	char keyPitches[] = { 'A', 'W', 'S', 'E', 'D', 'F',			//should move this somewhere else
		'T', 'G', 'Y', 'H', 'U', 'J',
	'K', 'O', 'L', 'P', ';', '\'' };
	
	int octave;
	int hauteur;
	
	for (int i = 0; i < 17; i++) {
		if (event.GetKeyCode() == keyPitches[i]) {
			pitch = 60 + i + (12 * m_keyEntryOctave);
			octave = pitch / 12;
			hauteur = pitch - (octave * 12);
			
			if ( m_currentElement && (m_currentElement->IsNote() || m_currentElement->IsNeume())) {
				m_currentElement->SetPitch( die[hauteur], octave, m_currentStaff );
			}
			
			break;
		}
	}
	
	lastKeyEntered = event.GetKeyCode();
	
}


void MusWindow::OnSize(wxSizeEvent &event)
{
	Resize();
}


