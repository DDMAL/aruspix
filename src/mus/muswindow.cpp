/////////////////////////////////////////////////////////////////////////////
// Name:        muswindow.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "muswindow.h"
#endif

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/fontdlg.h"
#include "wx/fontutil.h"
#include "wx/valgen.h"

#include "RtMidi.h"

#include "app/axapp.h"
#include "muswindow.h"
#include "mustoolpanel.h"
#include "musiobin.h"

#include "app/axgotodlg.h"
#include "app/axwxdc.h"

#include <iostream>
#include <cstdlib>

#include <mei/mei.h>

int MusWindow::s_flats[] = {F2, F3, F3, F4, F4, F5, F6, F6, F7, F7, F8, F8};
int MusWindow::s_sharps[] = {F2, F2, F3, F3, F4, F5, F5, F6, F6, F7, F7, F8};

#define MUS_BORDER_AROUND_PAGE 40

enum {
	NEW_NEUME_ANCUS = 0,
	NEW_NEUME_CAVUM,
	NEW_NEUME_CEPHALICUS,
	NEW_NEUME_CLIVIS,
	NEW_NEUME_COMPOUND,
	NEW_NEUME_CUSTOS,
	NEW_NEUME_EPIPHONUS,
	NEW_NEUME_PODATUS,
	NEW_NEUME_PORRECTUS,
	NEW_NEUME_PUNCTUM,
	NEW_NEUME_SALICUS,
	NEW_NEUME_SCANDICUS,
	NEW_NEUME_TORCULUS,
	NEW_NEUME_VIRGA
};

enum {
	NEW_NEUME_NONE = 0,
	NEW_NEUME_FLEXUS,
	NEW_NEUME_RESUPINUS
};

int NewNeumeDlg::s_neumetype = NEW_NEUME_PUNCTUM;
int NewNeumeDlg::s_variant = NEW_NEUME_NONE;
int NewNeumeDlg::s_nbpitches = 1;
bool NewNeumeDlg::s_inclinatum = false;
bool NewNeumeDlg::s_quilisma = false;


//----------------------------------------------------------------------------
// MusWindow
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(MusWindow,wxScrolledWindow)


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
    wxScrolledWindow( parent, id, position, size, style ), MusRC( ), AxUndo( 100 )
{
    parentwindow = parent;
	m_newElement = NULL;
	m_bufferElement = NULL;
    m_lastEditedElement = NULL;
	
	m_toolpanel = NULL;
    m_center = true;

	m_insertx = 0;
	m_insertcode = F6;
	m_insertoct = 4;
	m_dragging_x = 0;
	m_dragging_y_offset = 0;
	m_lyricCursor = 0;
    
    m_zoomNum = 10;
    m_zoomDen = 10;
	
	if ( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) == *wxWHITE )
		this->SetBackgroundColour( *wxLIGHT_GREY );
	else
		this->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) );
	this->SetForegroundColour( *wxBLACK );
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
    bool editor;
    bin_input->Read( &editor, sizeof( bool ) );
    m_editorMode = editor ? MUS_EDITOR_EDIT : MUS_EDITOR_INSERT;
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
    bool editor = m_editorMode == MUS_EDITOR_EDIT;
    bin_output->Write( &editor, sizeof( bool ) );
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
	//if ( m_center )
	//	dc->SetLogicalOrigin( - (margeMorteHor - mrgG), -margeMorteVer );
	//else
        dc->SetLogicalOrigin( - (MUS_BORDER_AROUND_PAGE / 2) - mrgG,  - (MUS_BORDER_AROUND_PAGE / 2) );
        
	this->DoPrepareDC( *dc );
    
    dc->SetUserScale( double(GetZoom())/100.0,  double(GetZoom())/100.0 );
    dc->SetMapMode(wxMM_TEXT);
	dc->SetAxisOrientation( true, false );
}

void MusWindow::DoLyricCursor( int x, int y, AxDC *dc, wxString lyric )
{
    int xCursor = x;
    if ( m_lyricCursor > 0 ){
        wxArrayInt lyricPos;
        // TODO dc->GetPartialTextExtents( s, lyricPos );
        if ( m_lyricCursor <= (int)lyricPos.GetCount() )
            xCursor += lyricPos[m_lyricCursor-1];			
    }
    // the cursor witdh
    int wCursor = max( 1, ToRendererX( 2 ) );
    
    // get the bounding box and draw it
    int wBox, hBox, wBox_empty;
    dc->GetTextExtent( lyric, &wBox, &hBox );
    if (lyric.Length() == 0) // we need the height of the BB even if the sting is empty
    {
        wxString empty = "d";
        dc->GetTextExtent( empty, &wBox_empty, &hBox );
    }
    dc->SetPen( AxBLACK, 1, wxSHORT_DASH );
    dc->DrawRectangle( x - 2 * wCursor, ToRendererY( y ) - wCursor, 
        wBox + 4 * wCursor, hBox + 2 * wCursor  ); 
    
    // draw the cursor
    xCursor -= wCursor / 2;
    dc->SetPen( AxBLACK, 1, wxSOLID );
    dc->SetBrush( m_currentColour, wxSOLID );
    
    dc->DrawRectangle( xCursor, ToRendererY( y ), wCursor , hBox  );

    // reset the pens
    dc->ResetPen();
    dc->ResetBrush();
}

void MusWindow::DoReset( )
{
    m_neume = MusNeume();
    ResetUndos();
    m_newElement = NULL;
}

void MusWindow::Resize( )
{
	wxWindow *parent = this->GetParent();
	if (!parent || !m_fh ) 
		return;
	
	Show( false );
	wxSize parent_s = parent->GetClientSize();
	int page_w = (ToRendererX(pageFormatHor) + MUS_BORDER_AROUND_PAGE) * GetZoom() / 100;
	int page_h = (ToRendererX(pageFormatVer) + MUS_BORDER_AROUND_PAGE) * GetZoom() / 100;
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

	//margeMorteHor = max( 0, ( parent_s.GetWidth() - win_w ) / 2 );
	//margeMorteVer = max( 0, ( parent_s.GetHeight() - win_h ) / 2 );
	//this->Move( x, y );
	
	Show( true );
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


bool MusWindow::CanZoom( bool zoomIn ) 
{ 
	if ( zoomIn )
		return ( m_f && (m_zoomNum/m_zoomDen < 1) );
	else
        return ( m_f && (m_zoomNum >= 2) );
		//return ( m_f && ((float)zoomNum/(float)zoomDen > 0.1) );
}

void MusWindow::Zoom( bool zoomIn )
{
	if ( !m_f || !m_fh )
		return;

	if ( zoomIn && this->CanZoom( true ) )
		m_zoomNum *= 2;
	else if	( !zoomIn && this->CanZoom( false ) )
		m_zoomNum /= 2;

	DoResize();
	SetPage( &m_f->m_pages[m_npage] );
}

void MusWindow::SetZoom( int percent )
{
	if ( !m_f || !m_fh )
		return;

	m_zoomNum = percent;
	m_zoomDen = 100;
	DoResize();
	SetPage( &m_f->m_pages[m_npage] );
}

void MusWindow::SetToolPanel( MusToolPanel *toolpanel )
{
	wxASSERT_MSG( toolpanel , "ToolPanel cannot be NULL ");
	m_toolpanel = toolpanel;
	m_toolpanel->SetMusWindow( this );
	SyncToolPanel();
}

void MusWindow::ToggleEditorMode() {
    if (m_editorMode == MUS_EDITOR_INSERT) {
        SetEditorMode(MUS_EDITOR_EDIT);
    } else {
        SetEditorMode(MUS_EDITOR_INSERT);
    }
}

void MusWindow::SetEditorMode( MusEditorMode mode )
{
	if ( mode == m_editorMode ) {
		return; // nothing to change
	}
	
	m_editorMode = mode;
	if ( m_editorMode == MUS_EDITOR_INSERT ) // edition -> insertion
	{
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
			else if ( m_currentElement->IsNeumeSymbol() )
			{
				m_neumesymbol = *(MusNeumeSymbol*)m_currentElement;
				m_newElement = &m_neumesymbol;
			}
			else if ( m_currentElement->IsNeume() || m_currentElement->IsNeumeElement() )
			{
				m_neume = MusNeume();
				m_newElement = &m_neume;
			}
		}
		else
		{
			switch (m_notation_mode) {
				case MUS_MENSURAL_MODE: m_newElement = &m_note; break;
				case MUS_NEUMATIC_MODE: m_newElement = &m_neume; break;
				default: break;
			}
			m_lastEditedElement = NULL;
		}
		m_currentElement = NULL;
	}
	else if ( m_newElement ) // insertion -> edition
	{
		m_currentElement = m_lastEditedElement;
		m_newElement = NULL;
	}
	UpdatePen();
	this->Refresh();
	OnEndEdition();
	SyncToolPanel();
}

void MusWindow::SetToolType( int type )
{
    int value = '0';
    switch ( type )
    {
    case (MUS_TOOLS_NOTES): value = 'N'; break;
    case (MUS_TOOLS_CLEFS): value = 'C'; break;
    case (MUS_TOOLS_PROPORTIONS): value = 'P'; break;
    case (MUS_TOOLS_OTHER): value = 'S'; break;
	case (NEUME_TOOLS_NOTES): value = 'N'; break;
	case (NEUME_TOOLS_SYMBOLS): value = 'S'; break;
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

void MusWindow::UpdatePen() {
	if ( m_editorMode == MUS_EDITOR_EDIT ) {
		this->SetCursor( wxCURSOR_ARROW );
	} else {
		this->SetCursor( wxCURSOR_PENCIL );
	}
}

int MusWindow::GetToolType()
{
	MusElement *sync = NULL;

	if (m_editorMode == MUS_EDITOR_EDIT)
		sync = m_currentElement;
	else
		sync = m_newElement;				//need to make a new element!! somehow?

	if (!sync) {
        return -1;
    }
    
    if (m_notation_mode == MUS_MENSURAL_MODE) {
        if ( sync->IsSymbol() )
        {
            if ( ((MusSymbol*)sync)->flag == CLE )
                return MUS_TOOLS_CLEFS;
            else if ( ((MusSymbol*)sync)->flag == IND_MES )	
                return MUS_TOOLS_PROPORTIONS;
            else
                return MUS_TOOLS_OTHER;
        } 
        else if (sync->IsNote() ) 
        {
            //return m_notation_mode == MENSURAL_MODE ? MUS_TOOLS_NOTES : NEUME_TOOLS_NOTES;
            return MUS_TOOLS_NOTES;
        }
    }
    else if (m_notation_mode == MUS_NEUMATIC_MODE) {
        if ( sync->IsNeumeSymbol() )
        {
			return NEUME_TOOLS_SYMBOLS;
        } 
        else if (sync->IsNeume() || sync->IsNeumeElement() )
        {
            return NEUME_TOOLS_NOTES;
        }
    }
    // just in case
    return -1;

}

void MusWindow::SyncToolPanel()
{
	int tool = this->GetToolType();

	if ( !m_toolpanel )
		return;
        
    //if ( tool == -1 )
    //    tool = MUS_TOOLS_NOTES;

	m_toolpanel->SetTools( tool, m_editorMode == MUS_EDITOR_EDIT );

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
	else if (m_currentElement->IsNeumeSymbol() )
		m_bufferElement = new MusNeumeSymbol( *(MusNeumeSymbol*)m_currentElement );
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

	if ( m_editorMode == MUS_EDITOR_INSERT ) // can paste in edition mode only
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
		x = ToRendererX( m_currentElement->xrel );
	int y = ToRendererY( kPos[m_currentStaff->no].yp + (m_currentStaff->portNbLine + 2)*_interl[m_currentStaff->pTaille]);
	// units
	int xu, yu;
	this->GetScrollPixelsPerUnit( &xu, &yu );
	// start
	int xs, ys;
	this->GetViewStart( &xs, &ys );
	xs *= xu;
	ys *= yu;
	ys += m_currentStaff->portNbLine + 2;
	// size
	int w, h;
	this->GetClientSize( &w, &h );
	//wxLogMessage("x %d y %d xs %d ys %d, w %d h %d", x, y, xs, ys, w, h );

	// check if necessary
	if ( (x > xs) && (x < xs + w) )
		x = -1;
	else
		x /= xu;
	if ( (y > ys ) && (y < ys + h - 2 * ToRendererX(_portee[0])) )
		y = -1;
	else {
		y /= yu;
		y -= m_currentStaff->portNbLine + 2 + m_currentStaff->no*2;
	}

	Scroll( x, y );
	OnSyncScroll( x, y );
}


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
	if (m_currentStaff && m_newElement) 
	{
		wxClientDC dc( this );
		InitDC( &dc );
		m_insertx = ToLogicalX( dc.DeviceToLogicalX( event.m_x ) ); //???
		int y = ToLogicalY( dc.DeviceToLogicalY( event.m_y ) );
		m_insertcode = m_currentStaff->trouveCodNote( y, m_insertx, &m_insertoct );
		m_newElement->xrel = m_insertx;
	}
	if ( m_editorMode == MUS_EDITOR_EDIT )
	{
		SetEditorMode(MUS_EDITOR_INSERT);
	}
	else  // insertion
	{
		if ( event.ButtonDClick( wxMOUSE_BTN_LEFT  ) && m_currentStaff && m_newElement )
		{
			if ( m_newElement->IsNote() || m_newElement->IsNeume() || m_newElement->IsNeumeElement() ||
				(((MusSymbol*)m_newElement)->flag == ALTER) || (((MusSymbol*)m_newElement)->flag == PNT)
				|| (((MusNeumeSymbol*)m_newElement)->getType() == NEUME_SYMB_FLAT) || (((MusNeumeSymbol*)m_newElement)->getType() == NEUME_SYMB_NATURAL) )
			{
				m_newElement->SetPitch(m_insertcode, m_insertoct);
			}
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			
			//make a new MeiElement with its own zone!
			if (m_f->GetMeiDocument()) {
				NewMeiNeumeElement(m_newElement);
			} else {
				m_lastEditedElement = m_currentStaff->Insert( m_newElement );
			}
			CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
			OnEndEdition();
		}

	}
	event.Skip();
}

void MusWindow::NewMeiNeumeElement(MusElement *element)
{
	MusElement *newelement = NULL;
	MusNeume neume;
	MusNeumeSymbol neume_symbol;
	if (element->IsNeume()) {
		neume = MusNeume(*(MusNeume*)element);
		neume.getPitches().clear();
		
		NewNeumeDlg dlg( parentwindow, -1, "Create a new neume" );
		dlg.Center(wxBOTH);
		if (dlg.ShowModal() != wxID_OK)
			return;
		
		NeumeType n_type;
		const short n = dlg.s_nbpitches;
		vector<int> pdiffs (n,0);
		
		switch(NewNeumeDlg::s_neumetype) {
			case NEW_NEUME_ANCUS: n_type = NEUME_TYPE_ANCUS; break;
			case NEW_NEUME_CAVUM: n_type = NEUME_TYPE_PUNCTUM_WHITE; break;
			case NEW_NEUME_CEPHALICUS: n_type = NEUME_TYPE_CEPHALICUS; pdiffs[1] = -1; break;
			case NEW_NEUME_CLIVIS: n_type = NEUME_TYPE_CLIVIS; pdiffs[1] = -1; break;
			case NEW_NEUME_COMPOUND: n_type = NEUME_TYPE_COMPOUND; break;
			case NEW_NEUME_CUSTOS: n_type = NEUME_TYPE_CUSTOS; break;
			case NEW_NEUME_EPIPHONUS: n_type = NEUME_TYPE_EPIPHONUS; pdiffs[1] = 1; break;
			case NEW_NEUME_PODATUS: n_type = NEUME_TYPE_PODATUS; pdiffs[1] = 2; break;
			case NEW_NEUME_PORRECTUS:
				switch (NewNeumeDlg::s_variant) {
					default:
					case NEW_NEUME_RESUPINUS:
					case NEW_NEUME_NONE: n_type = NEUME_TYPE_PORRECTUS; pdiffs[1] = -2; pdiffs[2] = 0; break;
					case NEW_NEUME_FLEXUS: n_type = NEUME_TYPE_PORRECTUS_FLEXUS; pdiffs[1] = -2; pdiffs[2] = -1; pdiffs[3] = -3; break;
				} break;
			case NEW_NEUME_PUNCTUM:
				if (NewNeumeDlg::s_inclinatum) {
					n_type = NEUME_TYPE_PUNCTUM_INCLINATUM;
				} else {
					n_type = NEUME_TYPE_PUNCTUM;
				} break;
			case NEW_NEUME_SALICUS: n_type = NEUME_TYPE_SALICUS; pdiffs[1] = 1; pdiffs[2] = 3; break;
			case NEW_NEUME_SCANDICUS:
				switch (NewNeumeDlg::s_variant) {
					default:
					case NEW_NEUME_RESUPINUS:
					case NEW_NEUME_NONE: n_type = NEUME_TYPE_SCANDICUS; break;
					case NEW_NEUME_FLEXUS: n_type = NEUME_TYPE_SCANDICUS_FLEXUS; break;
				} break;
			case NEW_NEUME_TORCULUS:
				switch (NewNeumeDlg::s_variant) {
					default:
					case NEW_NEUME_NONE:
					case NEW_NEUME_FLEXUS: n_type = NEUME_TYPE_TORCULUS; pdiffs[1] = 1; pdiffs[2] = 0; break;
					case NEW_NEUME_RESUPINUS: n_type = NEUME_TYPE_TORCULUS_RESUPINUS; pdiffs[1] = 1; pdiffs[2] = 0; pdiffs[3] = 1; break;
				} break;
			case NEW_NEUME_VIRGA: n_type = NEUME_TYPE_VIRGA; break;
		}
		neume.setType(n_type);
		for (int i = 1; i < NewNeumeDlg::s_nbpitches; i++) {
			MusNeumeElement* note = new MusNeumeElement(pdiffs[i]);
			neume.AppendNote(note);
		}
		if (NewNeumeDlg::s_inclinatum) {
			neume.inclinatum = true;
		}
		if (NewNeumeDlg::s_quilisma) {
			neume.quilisma = true;
		}
		
		neume.newmeielement = true;
		neume.CalcPitches(m_currentStaff);
		neume.SetPitch(neume.pitch, neume.oct);
		
		newelement = &neume;
	} else if (element->IsNeumeSymbol()) {
		neume_symbol = MusNeumeSymbol(*(MusNeumeSymbol*)element);
		newelement = &neume_symbol;
	} else {
		throw "MEI support only for neumatic notation!";
	}
	newelement->newmeielement = true;
	newelement->setMeiStaffZone(m_currentStaff->m_meiref->getZone());
	MusElement *tmp = m_currentStaff->GetFirst();
	while ( tmp && (tmp->xrel < newelement->xrel) )
	{
		if ( m_currentStaff->GetNext( tmp ) )
			tmp = m_currentStaff->GetNext( tmp );
		else
			break;
	}
	while ( tmp && tmp->IsNeumeElement() ) {
		if ( m_currentStaff->GetPrevious( tmp ) ) {
			tmp = m_currentStaff->GetPrevious( tmp );
		}
	}
	if (!tmp) {
		newelement->newMeiRef(m_currentStaff);
	}
	else {
		newelement->newMeiRef(tmp);
	}
	m_lastEditedElement = m_currentStaff->Insert( newelement );
	if (element->IsNeume() ) {
		((MusNeume*)element)->getPitches().clear();
	}
}

void MusWindow::OnMouseLeftUp(wxMouseEvent &event)
{
	if ( m_editorMode == MUS_EDITOR_EDIT || m_lyricMode )
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
    if ( m_editorMode == MUS_EDITOR_EDIT || m_lyricMode )
	{
		wxClientDC dc( this );
		InitDC( &dc );

		m_has_been_dragged = false;
		m_dragging_x  = ToLogicalX( dc.DeviceToLogicalX( event.m_x ) );
		int x = m_dragging_x - 3;
		int y = ToLogicalY( dc.DeviceToLogicalY( event.m_y ) );
		//wxLogMessage("x %d : y %d", x, y);
			
		/*** Picking element closest to mouse click location ***/
		
		// Default selection of closest note
		MusStaff *noteStaff = m_page->GetAtPos( x,y );
		MusElement *noteElement = noteStaff->GetAtPos( x );				

		// If we select a new item and the last item was a neume, close it
		//if (m_currentElement && m_currentElement->IsNeume()) {
		//	MusNeume *temp = (MusNeume*)m_currentElement;
		//	temp->SetClosed(true);
		//}
        
		m_lyricMode = false;
		m_inputLyric = false;
		m_editorMode = MUS_EDITOR_EDIT;
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
				m_editorMode = MUS_EDITOR_INSERT;
				
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

			int y = ToLogicalY( dc.DeviceToLogicalY( event.m_y ) );
			int x  = ToLogicalX( dc.DeviceToLogicalX( event.m_x ) );

			MusElement *tmp = NULL;
			if ( m_page->GetAtPos( x,y ) )
				tmp = m_page->GetAtPos( x,y )->GetAtPos( x );

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

			int y = ToLogicalY( dc.DeviceToLogicalY( event.m_y ) );
			int x  = ToLogicalX( dc.DeviceToLogicalX( event.m_x ) );
			if ( m_page->GetAtPos( x,y ) )
				m_currentStaff = m_page->GetAtPos( x,y );
		}
	}

	event.Skip();
}

void MusWindow::OnMouseMotion(wxMouseEvent &event)
{
	if ( event.Dragging() && event.LeftIsDown() && m_dragging_x && m_currentElement )
	{
		if ( !m_has_been_dragged )
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
		m_has_been_dragged = true;
		wxClientDC dc( this );
		InitDC( &dc );
		m_insertx = ToLogicalX( dc.DeviceToLogicalX( event.m_x ) );
		int y = ToLogicalY( dc.DeviceToLogicalY( event.m_y ) ) - m_dragging_y_offset;
		
		if ( m_editorMode == MUS_EDITOR_EDIT )
		{
			m_insertcode = m_currentStaff->trouveCodNote( y, m_insertx, &m_insertoct );
			m_currentElement->SetPitch( m_insertcode, m_insertoct );
		} 
		else if ( m_lyricMode )					// Movement of lyric element on y-axis
		{
			m_currentElement->dec_y = y - m_currentStaff->yrel;
		} 
		
		if ( m_insertx != m_dragging_x && m_currentElement->TYPE != NEUME_ELMT )		// If element has moved in the x-axis
		{
			// TODO m_currentElement->ClearElement( &dc, m_currentStaff );
			m_currentElement->xrel += ( m_insertx - m_dragging_x );
			if (m_currentElement->newmeielement) {
				m_currentElement->updateMeiRef();
			}
			m_dragging_x = m_insertx;
			if ( m_editorMode == MUS_EDITOR_EDIT ) {
				m_currentStaff->CheckIntegrity();
			}
		}
		this->Refresh();	
	} 

	event.Skip();
}

void MusWindow::OnMidiInput(wxCommandEvent &event)
{
    //wxLogDebug("Midi %d", event.GetInt() ) ;
    int octave = event.GetInt() / 12;
	int hauteur = event.GetInt() - (octave * 12);
	
	printf("octave: %d, hauteur: %d\n", octave, hauteur);
    if ( m_currentElement && m_currentElement->IsNote() ) {
//        m_currentElement->SetPitch( die[hauteur], octave, m_currentStaff );
		m_currentElement->SetPitch( MusWindow::s_sharps[hauteur], octave );
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

/**
 * Movement keys that are shared between all notation modes (next/prev/home/end/etc)
 */
void MusWindow::SharedEditOnKeyDown(wxKeyEvent &event) {
    if (event.m_controlDown) {
        return;
    }
    if ( event.GetKeyCode() == WXK_RIGHT || event.GetKeyCode() == WXK_SPACE ) 
    {
        if ( m_currentStaff->GetNext( m_currentElement )) {
            m_currentElement = m_currentStaff->GetNext( m_currentElement );
        }
        else if ( m_page->GetNext( m_currentStaff ) )
        {
            m_currentStaff = m_page->GetNext( m_currentStaff );
            m_currentElement = m_currentStaff->GetFirst();
        }
        //UpdateScroll();
    }
    else if ( event.GetKeyCode() == WXK_LEFT )
    {
        if ( m_currentStaff->GetPrevious( m_currentElement )) {
            m_currentElement = m_currentStaff->GetPrevious( m_currentElement );
        }
        else if ( m_page->GetPrevious( m_currentStaff ) )
        {
            m_currentStaff = m_page->GetPrevious( m_currentStaff );
            m_currentElement = m_currentStaff->GetLast();
        }
        //UpdateScroll();
    }
    else if ( event.GetKeyCode() == WXK_UP )
    {
        if ( m_page->GetPrevious( m_currentStaff ) )
        {
            int x = 0;
            if ( m_currentElement ) {
                x = m_currentElement->xrel;
            }
            m_currentStaff = m_page->GetPrevious( m_currentStaff );
            m_currentElement = m_currentStaff->GetAtPos(x);
            //UpdateScroll();
        }
    }
    else if ( event.GetKeyCode() == WXK_DOWN )
    {
        if ( m_page->GetNext( m_currentStaff ) )
        {
            int x = 0;
            if ( m_currentElement ) {
                x = m_currentElement->xrel;
            }
            m_currentStaff = m_page->GetNext( m_currentStaff );
            m_currentElement = m_currentStaff->GetAtPos(x);
            //UpdateScroll();
        }
    }
    else if ( event.GetKeyCode() == WXK_HOME ) 
    {
        if ( m_currentStaff->GetFirst( ) ) {
            m_currentElement = m_currentStaff->GetFirst( );
        }
    }
    else if ( event.GetKeyCode() == WXK_END ) 
    {
        if ( m_currentStaff->GetLast( ) ) {
            m_currentElement = m_currentStaff->GetLast( );
        }
    }
    this->Refresh();
    OnEndEdition();
    SyncToolPanel();
}

/**
 * Key handler for Neumatic editor / edit mode
 */
void MusWindow::NeumeEditOnKeyDown(wxKeyEvent &event) {
    int noteKeyCode = GetNoteValue( event.m_keyCode );
    if ( ((event.m_keyCode == WXK_DELETE ) || (event.m_keyCode == WXK_BACK)) && m_currentElement) //"Delete or Backspace" event
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        MusElement *del = m_currentElement;
        MusStaff *delstaff = m_currentStaff;
		delstaff->CheckIntegrity();
        
        if (event.m_keyCode == WXK_DELETE )		//"Delete" event
        {
			if ( m_currentElement->IsNeume() ) {
				int maxx = m_currentElement->no;
				for (vector<MusNeumeElement*>::iterator i = ((MusNeume*)m_currentElement)->getPitches().begin(); i != ((MusNeume*)m_currentElement)->getPitches().end(); i++) {
					if ( (*i)->no > maxx ) {
						maxx = (*i)->no;
						del = (*i);
					}
				}
			} else if ( m_currentElement->IsNeumeElement() ) {
				MusNeume *parent = ((MusNeumeElement*)m_currentElement)->getParent();
				int maxx = m_currentElement->no;
				for (vector<MusNeumeElement*>::iterator i = parent->getPitches().begin(); i != parent->getPitches().end(); i++) {
					if ( (*i)->no > maxx ) {
						maxx = (*i)->no;
						del = (*i);
					}
				}
			}
            if ( m_currentStaff->GetNext( del ) )
                m_currentElement = m_currentStaff->GetNext( del );
            else if ( m_page->GetNext( m_currentStaff ) )
            {
                
                m_currentStaff = m_page->GetNext( m_currentStaff );
                m_currentElement = m_currentStaff->GetFirst();
				//UpdateScroll();
            }
            else
                m_currentElement = NULL;
        }
        else                                    //"Backspace" event
        {
			if ( m_currentElement->IsNeumeElement() ) {
				MusNeume *parent = ((MusNeumeElement*)m_currentElement)->getParent();
				int minx = parent->no;
				for (vector<MusNeumeElement*>::iterator i = parent->getPitches().begin(); i != parent->getPitches().end(); i++) {
					if ( (*i)->no < minx ) {
						minx = (*i)->no;
						del = (*i);
					}
				}
			} else if ( m_currentElement->IsNeume() ) {
				int minx = m_currentElement->no;
				for (vector<MusNeumeElement*>::iterator i = ((MusNeume*)m_currentElement)->getPitches().begin(); i != ((MusNeume*)m_currentElement)->getPitches().end(); i++) {
					if ( (*i)->no < minx ) {
						minx = (*i)->no;
						del = (*i);
					}
				}
			}
            if ( m_currentStaff->GetPrevious( del ) )
                m_currentElement = m_currentStaff->GetPrevious( del );
            else if ( m_page->GetPrevious( m_currentStaff ) )
            {
                m_currentStaff = m_page->GetPrevious( m_currentStaff );
                m_currentElement = m_currentStaff->GetLast();
				UpdateScroll();
            }
            else
                m_currentElement = NULL;
        }
		if ( del->IsNeumeElement() ) { //since you can't just delete one note of a neume (for the time being, you must replace it with a new one of the correct type)
			MusNeumeElement *delel = (MusNeumeElement*)del;
			delel->getParent()->deleteMeiRef();
			delstaff->Delete(delel->getParent());
		} else {
			del->deleteMeiRef();
			delstaff->Delete( del );
		}
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
    }
    else if ( in ( event.m_keyCode, WXK_F1, WXK_F9 ) && m_currentElement) // Change hauteur
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        m_insertcode = m_currentElement->filtrcod( event.m_keyCode - WXK_F1, &m_insertoct );
        m_currentElement->SetPitch( m_insertcode, m_insertoct );
        CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
    }
    else if ( event.m_controlDown && ( event.m_keyCode == WXK_UP ) && m_currentElement) // correction hauteur avec les fleches, up
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        m_insertcode = (m_currentElement->TYPE==NEUME || m_currentElement->TYPE == NEUME_SYMB || m_currentElement->TYPE == NEUME_ELMT) ? (m_currentElement->filtrpitch( m_currentElement->pitch + 1, &m_insertoct )) : (m_currentElement->filtrcod( m_currentElement->code + 1, &m_insertoct ));
        m_currentElement->SetPitch( m_insertcode, m_insertoct );
        CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
    }
    else if ( event.m_controlDown && ( event.m_keyCode == WXK_DOWN ) && m_currentElement) // correction hauteur avec les fleches, down
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        m_insertcode = (m_currentElement->TYPE==NEUME || m_currentElement->TYPE == NEUME_SYMB || m_currentElement->TYPE == NEUME_ELMT) ? (m_currentElement->filtrpitch( m_currentElement->pitch - 1, &m_insertoct )) : (m_currentElement->filtrcod( m_currentElement->code - 1, &m_insertoct ));
        m_currentElement->SetPitch( m_insertcode, m_insertoct );
        CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
    }
    else if ( m_currentElement && m_currentElement->IsNeume() && 
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
    }
    else if ( m_currentElement && m_currentElement->IsNeume() && 
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
    }
    else if ( m_currentElement && m_currentElement->IsNeume() &&
             (in( noteKeyCode, 0, 5 )))
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        int vflag = ( event.m_controlDown || (noteKeyCode == CUSTOS)) ? 1 : 0;
        m_currentElement->SetValue( noteKeyCode , m_currentStaff, vflag );
        CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
    }
    else if ( event.m_controlDown && (( event.m_keyCode == WXK_LEFT ) || (event.m_keyCode == WXK_RIGHT )) && m_currentElement) // moving element
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        if (m_currentElement->TYPE != NEUME_ELMT ) {
			if ( event.m_keyCode == WXK_LEFT ) {
				m_currentElement->xrel -=3;
			} else {
				m_currentElement->xrel +=3;
			}
		}
		if (m_currentElement->newmeielement) {
			m_currentElement->updateMeiRef();
		}
        this->Refresh();
        m_currentStaff->CheckIntegrity();
        CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
    }
	/*else if ( event.m_keyCode == WXK_SHIFT )
	{
		PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
		MusElement *newelement;
		MusNeumeSymbol neumesymb = MusNeumeSymbol();
		MusNeume neume = MusNeume();
		if (m_currentElement->IsNeume())  {
			newelement = &neumesymb;
		} else if (m_currentElement->IsNeumeSymbol()) {
			newelement = &neume;
		} else if (m_currentElement->IsNeumeElement()) {
			m_currentElement = ((MusNeumeElement*)m_currentElement)->getParent();
			newelement = &neumesymb;
		}
		newelement->liaison = m_currentElement->liaison;
		newelement->dliai = m_currentElement->dliai;
		newelement->fliai = m_currentElement->fliai;
		newelement->lie_up = m_currentElement->lie_up;
		newelement->rel = m_currentElement->rel;
		newelement->drel = m_currentElement->drel;
		newelement->frel = m_currentElement->frel;
		newelement->oct = m_currentElement->oct;
		newelement->dimin = m_currentElement->dimin;
		newelement->grp = m_currentElement->grp;
		newelement->_shport = m_currentElement->_shport;
		newelement->ligat = m_currentElement->ligat;
		newelement->ElemInvisible = m_currentElement->ElemInvisible;
		newelement->pointInvisible = m_currentElement->pointInvisible;
		newelement->existDebord = m_currentElement->existDebord;
		newelement->fligat = m_currentElement->fligat;
		newelement->notschowgrp = m_currentElement->notschowgrp;
		newelement->cone = m_currentElement->cone;
		newelement->liaisonPointil = m_currentElement->liaisonPointil;
		newelement->reserve1 = m_currentElement->reserve1;
		newelement->reserve2 = m_currentElement->reserve2;
		newelement->ottava = m_currentElement->ottava;
		newelement->durNum = m_currentElement->durNum;
		newelement->durDen = m_currentElement->durDen;
		newelement->offset = m_currentElement->offset;
		newelement->xrel = m_currentElement->xrel;
		newelement->dec_y = m_currentElement->dec_y;
		newelement->debordCode = m_currentElement->debordCode;
		newelement->debordSize = m_currentElement->debordSize;
		newelement->no = m_currentElement->no;
		newelement->code = m_currentElement->code;
		newelement->pitch = m_currentElement->pitch;
		if (m_f->GetMeiDocument()) {
			newelement->newmeielement = true;
			newelement->setMeiRef(m_currentElement->getMeiRef());
			newelement->setMeiStaffZone(m_currentStaff->m_meiref->getZone());
			newelement->updateMeiRef();
		}
		m_currentStaff->Delete(m_currentElement);
		m_currentElement = newelement;
		if (m_f->GetMeiDocument()) {
			NewMeiNeumeElement(m_currentElement);
		} else {
			m_currentElement = m_currentStaff->Insert(newelement);
		}
		m_currentStaff->CheckIntegrity();
		CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
	}*/
    else if ( m_currentElement && m_currentElement->IsNeumeSymbol() &&
             in( event.m_keyCode, 33, 125) ) // any other keycode on symbol (ascii codes)
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        int vflag = ( event.m_controlDown ) ? 1 : 0;
        m_currentElement->SetValue( event.m_keyCode, m_currentStaff, vflag );
        CheckPoint( UNDO_PART, MUS_UNDO_STAFF );	
    }
	else if ( event.m_controlDown && event.m_keyCode == 'C' )
	{
		for (int i = 0; i < m_f->m_pages.GetCount(); i++) {
			for (int j = 0; j < m_f->m_pages[i].m_staves.GetCount(); j++) {
				for (int k = 0; k < m_f->m_pages[i].m_staves[j].m_elements.GetCount(); k++) {
					if ( m_f->m_pages[i].m_staves[j].m_elements[k].IsNeume() && ((MusNeume*)&(m_f->m_pages[i].m_staves[j].m_elements[k]))->getType() == NEUME_TYPE_CUSTOS )
						m_f->m_pages[i].m_staves[j].m_elements[k].SetPitch(m_f->m_pages[i].m_staves[j].m_elements[k].pitch, m_f->m_pages[i].m_staves[j].m_elements[k].oct - 1);
				}
			}
		}
	}
    OnEndEdition();
    SyncToolPanel();
}

/**
 * Key handler for Neumatic editor / insert mode
 */
void MusWindow::NeumeInsertOnKeyDown(wxKeyEvent &event) {
    int noteKeyCode = GetNoteValue( event.m_keyCode );

    if ( event.m_controlDown && (event.m_keyCode == 'N')) {
        m_newElement = &m_neume;
    }	
    else if ( event.m_controlDown && (event.m_keyCode == 'S')) // symbols
    {
		m_neumesymbol.ResetToNeumeSymbol();
        m_newElement = &m_neumesymbol;
    }
    else if ( m_newElement && m_newElement->IsNeume() &&
             (in( noteKeyCode, 0, 5 )))
    {
        int vflag = ( event.m_controlDown || (noteKeyCode == CUSTOS)) ? 1 : 0;
        m_newElement->SetValue( noteKeyCode , NULL, vflag );
    }
    else if ( m_newElement && m_newElement->IsNeumeSymbol() &&
			 in( event.m_keyCode, 33, 125) ) // any other keycode on symbol (ascii codes)
    {
        int vflag = ( event.m_controlDown ) ? 1 : 0;
        m_newElement->SetValue( event.m_keyCode, NULL, vflag );
    }
    UpdatePen();
    OnEndEdition();
    SyncToolPanel();
}

/**
 * Key handler for Mensural editor / edit mode
 */
void MusWindow::MensuralEditOnKeyDown(wxKeyEvent &event) {
    int noteKeyCode = GetNoteValue( event.m_keyCode );
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
        else                                    //"Backspace" event
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
        else {
            CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        }
        
        this->Refresh();
        OnEndEdition();
        SyncToolPanel();
    }
    else if ( in ( event.m_keyCode, WXK_F1, WXK_F9 ) && m_currentElement) // Change hauteur
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        m_insertcode = m_currentElement->filtrcod( event.m_keyCode - WXK_F1, &m_insertoct );
        m_currentElement->SetPitch( m_insertcode, m_insertoct );
        CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        OnEndEdition();
    }
    else if ( event.m_controlDown && ( event.m_keyCode == WXK_UP ) && m_currentElement) // correction hauteur avec les fleches, up
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        m_insertcode = m_currentElement->filtrcod( m_currentElement->code + 1, &m_insertoct );
        m_currentElement->SetPitch( m_insertcode, m_insertoct );
        CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        OnEndEdition();
    }
    else if ( event.m_controlDown && ( event.m_keyCode == WXK_DOWN ) && m_currentElement) // correction hauteur avec les fleches, down
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        m_insertcode = m_currentElement->filtrcod( m_currentElement->code - 1, &m_insertoct );
        m_currentElement->SetPitch( m_insertcode, m_insertoct );
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
        if ( event.m_keyCode == WXK_LEFT ) {
            m_currentElement->xrel -=3;
        } else {
            m_currentElement->xrel +=3;
        }
        this->Refresh();
        m_currentStaff->CheckIntegrity();
        CheckPoint( UNDO_PART, MUS_UNDO_STAFF );
        OnEndEdition();
    }
    else if ( (event.m_keyCode == 'T') && m_currentElement && m_currentElement->IsNote() )
    {
        m_editorMode = MUS_EDITOR_INSERT;
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
            m_editorMode = MUS_EDITOR_EDIT;
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
}

/**
 * Key handler for Mensural editor / insert mode
 */
void MusWindow::MensuralInsertOnKeyDown(wxKeyEvent &event) {
    int noteKeyCode = GetNoteValue( event.m_keyCode );
    if ( event.m_controlDown && (event.m_keyCode == 'N')) {// change set (note, rests, key, signs, symbols, ....
        m_newElement = &m_note;
    }
    else if ( event.m_controlDown && (event.m_keyCode == 'C')) // clefs
    {	
        m_symbol.ResetToClef();
        m_newElement = &m_symbol;
    }	
    else if ( event.m_controlDown && (event.m_keyCode == 'P')) // proportions
    {	
        m_symbol.ResetToProportion();
        m_newElement = &m_symbol;
    }	
    else if ( event.m_controlDown && (event.m_keyCode == 'S')) // symbols
    {
        m_symbol.ResetToSymbol();
        m_newElement = &m_symbol;
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
    UpdatePen();
    OnEndEdition();
    SyncToolPanel();
}

void MusWindow::OnKeyDown(wxKeyEvent &event)
{
	if ( !m_page || !m_currentStaff ) {
		return;
    }

    if ( m_lyricMode ) {
        LyricEntry( event );
    }
	// change mode edition -- insertion
	else if ( event.GetKeyCode() == WXK_RETURN )
	{
		ToggleEditorMode();
	}
	else if ( m_editorMode == MUS_EDITOR_EDIT ) // mode edition
	{
        SharedEditOnKeyDown(event);
        if (m_notation_mode == MUS_MENSURAL_MODE) {
            MensuralEditOnKeyDown(event);
        } else if (m_notation_mode == MUS_NEUMATIC_MODE) {
            NeumeEditOnKeyDown(event);
        }
	}
	else /*** Note insertion mode ***/
	{
        if (m_notation_mode == MUS_MENSURAL_MODE) {
            MensuralInsertOnKeyDown(event);
        } else if (m_notation_mode == MUS_NEUMATIC_MODE) {
            NeumeInsertOnKeyDown(event);
        }
	}
	
}

void MusWindow::LyricEntry(wxKeyEvent &event) 
{
    if ( !m_inputLyric )	/*** Lyric Editing mode ***/
	{	
		if ( event.m_keyCode == 'T' )			//"T" event: Escape lyric navigation mode
		{
			m_lyricMode = false;
			m_inputLyric = false;
			m_editorMode = MUS_EDITOR_EDIT;
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
	else		/*** Lyric insertion mode ***/
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

	// calculate scroll position
    int scrollX, scrollY;
    this->GetViewStart(&scrollX,&scrollY);
    int unitX, unitY;
    this->GetScrollPixelsPerUnit(&unitX,&unitY);
	wxSize csize = GetClientSize();
    scrollX *= unitX;
    scrollY *= unitY;
    
	wxPaintDC dc( this );
    //if ( m_center )
	//	dc.SetLogicalOrigin( - (margeMorteHor - mrgG), -margeMorteVer );
	//else
		dc.SetLogicalOrigin( - MUS_BORDER_AROUND_PAGE / 2, - MUS_BORDER_AROUND_PAGE / 2);
    
	this->PrepareDC( dc );
	dc.SetTextForeground( *wxBLACK );
	dc.SetMapMode( wxMM_TEXT );
	dc.SetAxisOrientation( true, false );
    dc.SetUserScale( double(GetZoom())/100.0,  double(GetZoom())/100.0 );
	
	m_page->Init( this );
    AxWxDC ax_dc( &dc );
    m_page->DrawPage( &ax_dc );

    // TODO for cursor
    // Draw the cursor if we are in insertion mode, we have a m_newElement and a m_currentStaff
    // We can add a DrawCursor method, use the y position of the staff and the x of the element
    // What shape to draw??
    

	// hitting return in keyboard entry mode sends us here for some reason
	
	//if (!m_editElement && m_newElement && m_currentStaff) {
	//	m_currentColour = AxRED;
		//drawing code here
		
		//printf("staff y: %d\n", m_currentStaff->yrel);
		
	// TODO	this->rect_plein2(&dc, m_newElement->xrel+35, m_currentStaff->yrel-200, 
	//					  m_newElement->xrel+40, m_currentStaff->yrel-40);
		
	//	m_currentColour = AxBLACK;
	//}
}

void MusWindow::OnSize(wxSizeEvent &event)
{
	Resize();
}

//----------------------------------------------------------------------------
// NewNeumeDlg
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(NewNeumeDlg,wxDialog)
EVT_COMBOBOX(ID7_NEUME_TYPE, NewNeumeDlg::OnNeumeTypeChange)
END_EVENT_TABLE()

NewNeumeDlg::NewNeumeDlg( wxWindow *parent, wxWindowID id, const wxString &title,
					 const wxPoint &position, const wxSize& size, long style ) :
wxDialog( parent, id, title, position, size, style )
{
    NewNeumeDlgFunc( this, TRUE );
    
    this->GetCbNeumeType()->SetValidator(wxGenericValidator(&NewNeumeDlg::s_neumetype));
	this->GetRbVariant()->SetValidator(wxGenericValidator(&NewNeumeDlg::s_variant));
    this->GetScNbPitches()->SetValidator(wxGenericValidator(&NewNeumeDlg::s_nbpitches));
    this->GetCxInclinatum()->SetValidator(wxGenericValidator(&NewNeumeDlg::s_inclinatum));
    this->GetCxQuilisma()->SetValidator(wxGenericValidator(&NewNeumeDlg::s_quilisma)); 
}

void NewNeumeDlg::OnNeumeTypeChange(wxCommandEvent& event)
{
	int id = event.GetInt();
	wxSpinCtrl* spin = (wxSpinCtrl*) FindWindow( ID7_NB_PITCHES );
	switch (id) {
		case NEW_NEUME_ANCUS: spin->SetValue(1); break;
		case NEW_NEUME_CAVUM: spin->SetValue(1); break;
		case NEW_NEUME_CEPHALICUS: spin->SetValue(2); break;
		case NEW_NEUME_CLIVIS: spin->SetValue(2); break;
		case NEW_NEUME_COMPOUND: spin->SetValue(4); break;
		case NEW_NEUME_CUSTOS: spin->SetValue(1); break;
		case NEW_NEUME_EPIPHONUS: spin->SetValue(2); break;
		case NEW_NEUME_PODATUS: spin->SetValue(2); break;
		case NEW_NEUME_PORRECTUS: spin->SetValue(3); break;
		case NEW_NEUME_PUNCTUM: spin->SetValue(1); break;
		case NEW_NEUME_SALICUS: spin->SetValue(3); break;
		case NEW_NEUME_SCANDICUS: spin->SetValue(3); break;
		case NEW_NEUME_TORCULUS: spin->SetValue(3); break;
		case NEW_NEUME_VIRGA: spin->SetValue(1); break;
		default: break;
	}
}