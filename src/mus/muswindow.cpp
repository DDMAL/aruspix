/////////////////////////////////////////////////////////////////////////////
// Name:        muswindow.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/fontdlg.h"
#include "wx/fontutil.h"

#include "RtMidi.h"

#include "app/axapp.h"

#include "muswindow.h"
#include "mustoolpanel.h"
#include "musiobin.h"

#include "muslaidoutlayerelement.h"
//#include "musstaff.h"
//#include "musnote.h"
////#include "mussymbol.h"

#include "app/axgotodlg.h"

#include "muswxdc.h"

#include <iostream>
#include <cstdlib>

int MusWindow::s_flats[] = {PITCH_C, PITCH_D, PITCH_D, PITCH_E, PITCH_E, PITCH_F, PITCH_G, PITCH_G, PITCH_A, PITCH_A, PITCH_B, PITCH_B};
int MusWindow::s_sharps[] = {PITCH_C, PITCH_C, PITCH_D, PITCH_D, PITCH_E, PITCH_F, PITCH_F, PITCH_G, PITCH_G, PITCH_A, PITCH_A, PITCH_B};

#define MUS_BORDER_AROUND_PAGE 40


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
    m_newElement = NULL;
	m_bufferElement = NULL;
    m_lastEditedElement = NULL;
	
	m_toolpanel = NULL;
    m_center = true;

	m_insert_x = 0;
	m_insert_pname = PITCH_G;
	m_insert_oct = 4;
	m_dragging_x = 0;
	m_dragging_y_offset = 0;
    m_has_been_dragged = false;
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
	wxASSERT_MSG( m_layout, "MusDoc should not be NULL in UNDO");

	if ( !m_layout )
		return;
	
	int page, system, staff, layer, element, lyric_element;

    // keep a pointer to the doc because we are going to change the m_layout value
    // if we want to handle more that one layout, we should keep the layout number too
    MusDoc *doc = m_layout->m_doc;
	MusBinInput *bin_input = new MusBinInput( doc, undoPtr->GetFilename() );


	// keep current page, staff and element
	bin_input->Read( &page, sizeof( int ));
	bin_input->Read( &system, sizeof( int ));
    bin_input->Read( &staff, sizeof( int ));
	bin_input->Read( &layer, sizeof( int ));
    bin_input->Read( &element, sizeof( int ));
    bin_input->Read( &lyric_element, sizeof( int ) );
    // edition state
    bool editor;
    bin_input->Read( &editor, sizeof( bool ) );
    m_editorMode = editor ? MUS_EDITOR_EDIT : MUS_EDITOR_INSERT;
	bin_input->Read( &m_lyricMode, sizeof( bool ) );
	bin_input->Read( &m_inputLyric, sizeof( bool ) );
	bin_input->Read( &m_lyricCursor, sizeof( int ) );
		
	//if ( undoPtr->m_flags == MUS_UNDO_FILE ) // this is now the only type of undo we handle anyway...
    {
        bin_input->ImportFile();
        SetLayout( &doc->m_layouts[0] ); // only one layout for now
	}

	if (page < 0)  //|| (page > m_fh->nbpage - 1))
	{
		delete bin_input;
		return;
	}
	
    SetPage( &m_layout->m_pages[page] );
	m_npage = page;

	delete bin_input;
	
    m_currentSystem = NULL;
	m_currentStaff = NULL;
    m_currentLayer = NULL;
    m_currentElement = NULL;

    if ( system != -1 ) {
		m_currentSystem = &m_page->m_systems[system];
    }	
	if ( m_currentSystem && (staff != -1) ) {
		m_currentStaff = &m_currentSystem->m_staves[staff];
    }	
	if ( m_currentStaff && (layer != -1) ) {
		m_currentLayer = &m_currentStaff->m_layers[layer];
    }		
	if ( m_currentLayer && (element != -1) )
	{
        
        //if ( (lyric_element != -1) && (&m_currentStaff->m_elements[element])->IsNote() ) {
        //    MusNote1 *note = (MusNote1*)&m_currentStaff->m_elements[element];
        //    m_currentElement = note->GetLyricNo( lyric_element);
        //} else { 
        //    m_currentElement = &m_currentStaff->m_elements[element];
        //}
        
        m_currentElement = &m_currentLayer->m_elements[element];

	}

	UpdateScroll();
	this->Refresh();
	OnEndEdition();
	SyncToolPanel();
}


void MusWindow::Store( AxUndoFile *undoPtr )
{
	wxASSERT_MSG( m_layout, "MusLayout should not be NULL in UNDO");

	if ( !m_layout )
		return;
	// keep current page, system, staff layer, and element and lyric
	int page = -1;
    int system = -1;
	int staff = -1;
    int layer = -1;
	int element = -1;
    int lyric_element = -1;
    
	if ( m_page ) {
		page = m_page->GetPageNo();
    }
	if ( m_currentSystem ) {
		system = m_currentSystem->GetSystemNo();
    }
	if ( m_currentStaff ) {
		staff = m_currentStaff->GetStaffNo();
    }
    if ( m_currentLayer ) {
        layer = m_currentLayer->GetLayerNo();
    }
	if ( m_currentElement ) {
        /*
        if ( m_currentElement->IsSymbol() && ((MusSymbol1*)m_currentElement)->IsLyric() ) {
            element = ((MusSymbol1*)m_currentElement)->m_note_ptr->no;
            lyric_element = m_currentElement->no;
            wxLogDebug("lyric %d", lyric_element );
        } else {
            element = m_currentElement->no;
        }
        */
        element = m_currentElement->GetElementNo();
    }
		
    MusBinOutput *bin_output = new MusBinOutput( m_layout->m_doc, undoPtr->GetFilename() );
	
	bin_output->Write( &page, sizeof( int ) );
    bin_output->Write( &system, sizeof( int ) );
	bin_output->Write( &staff, sizeof( int ) );
	bin_output->Write( &layer, sizeof( int ) );
    bin_output->Write( &element, sizeof( int ) );
    bin_output->Write( &lyric_element, sizeof( int ) );
    // edition state
    bool editor = m_editorMode == MUS_EDITOR_EDIT;
    bin_output->Write( &editor, sizeof( bool ) );
	bin_output->Write( &m_lyricMode, sizeof( bool ) );
	bin_output->Write( &m_inputLyric, sizeof( bool ) );
	bin_output->Write( &m_lyricCursor, sizeof( int ) );
		
	//if ( undoPtr->m_flags == MUS_UNDO_FILE ) // the only type of undo we handle
	{
	    bin_output->ExportFile();
	}

    delete bin_output;
}


void MusWindow::InitDC( wxDC *dc )
{
	//if ( m_center )
	//	dc->SetLogicalOrigin( - (margeMorteHor - m_leftMargin), -margeMorteVer );
	//else
        dc->SetLogicalOrigin( - (MUS_BORDER_AROUND_PAGE / 2) - m_layout->m_leftMargin,  - (MUS_BORDER_AROUND_PAGE / 2) );
        
	this->DoPrepareDC( *dc );
    
    dc->SetUserScale( double(GetZoom())/100.0,  double(GetZoom())/100.0 );
    dc->SetMapMode(wxMM_TEXT);
	dc->SetAxisOrientation( true, false );
}

void MusWindow::DoLyricCursor( int x, int y, MusDC *dc, wxString lyric )
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
    //m_neume = MusNeume(); // ax2
    ResetUndos();
    m_newElement = NULL;
}

void MusWindow::Resize( )
{
	wxWindow *parent = this->GetParent();
	if (!parent || !m_layout) 
		return;
	
	Show( false );
	wxSize parent_s = parent->GetClientSize();
	int page_w = (ToRendererX(m_layout->m_pageWidth) + MUS_BORDER_AROUND_PAGE) * GetZoom() / 100;
	int page_h = (ToRendererX(m_layout->m_pageHeight) + MUS_BORDER_AROUND_PAGE) * GetZoom() / 100;
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
	//return ( m_fh && ( m_fh->nbpage > 1) ); // ax2
    return false;
}

void MusWindow::Goto( )
{
	if ( !m_layout )
		return;
    AxGotoDlg *dlg = new AxGotoDlg(this, -1, _("Go to page ..."), m_layout->m_pages.GetCount(), m_npage );
    dlg->Center(wxBOTH);
    if ( dlg->ShowModal() == wxID_OK )
	{
		m_npage = dlg->GetPage();
		SetPage( &m_layout->m_pages[m_npage] );
    }
	dlg->Destroy();
    wxLogError( "MusWindow::Goto missing in ax2" );
}


bool MusWindow::CanZoom( bool zoomIn ) 
{ 
	if ( zoomIn )
		return ( m_layout && (m_zoomNum/m_zoomDen < 1) );
	else
        return ( m_layout && (m_zoomNum >= 2) );
		//return ( m_layout && ((float)zoomNum/(float)zoomDen > 0.1) );
}

void MusWindow::Zoom( bool zoomIn )
{
	if ( !m_layout )
		return;

	if ( zoomIn && this->CanZoom( true ) )
		m_zoomNum *= 2;
	else if	( !zoomIn && this->CanZoom( false ) )
		m_zoomNum /= 2;

	DoResize();
}

void MusWindow::SetZoom( int percent )
{
	if ( !m_layout )
		return;

	m_zoomNum = percent;
	m_zoomDen = 100;
	DoResize();
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
            if ( m_currentElement->IsBarline() )
			{
				m_barline = *(MusBarline*)m_currentElement->m_layerElement;
				m_newElement = &m_barline;
			}
            else if ( m_currentElement->IsClef() )
			{
				m_clef = *(MusClef*)m_currentElement->m_layerElement;
				m_newElement = &m_clef;
			}
			else if ( m_currentElement->IsMensur() )
			{
				m_mensur = *(MusMensur*)m_currentElement->m_layerElement;
				m_newElement = &m_mensur;
			}            
			else if ( m_currentElement->IsNote() )
			{
				m_note = *(MusNote*)m_currentElement->m_layerElement;
				m_newElement = &m_note;
			}
			else if ( m_currentElement->IsRest() )
			{
				m_rest = *(MusRest*)m_currentElement->m_layerElement;
				m_newElement = &m_rest;
			}
			else if ( m_currentElement->IsSymbol() )
			{
				m_symbol = *(MusSymbol*)m_currentElement->m_layerElement;
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
		m_currentElement = m_lastEditedElement; // we keep it for setting back focus later
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
	MusLayerElement *sync = NULL;

	if (m_editorMode == MUS_EDITOR_EDIT) {
        if (m_currentElement) {
            sync = m_currentElement->m_layerElement;
        }
    }
	else
		sync = m_newElement;				//need to make a new element!! somehow?

	if (!sync) {
        return -1;
    }
    
    if (m_notation_mode == MUS_MENSURAL_MODE) {
        if ( sync->IsClef() ) {
            return MUS_TOOLS_CLEFS;
        }
        else if ( sync->IsMensur() ) {
            return MUS_TOOLS_PROPORTIONS;
        }
        else if ( sync->IsNote() || sync->IsRest() ) {
            return MUS_TOOLS_NOTES;
        }
        else {
            return MUS_TOOLS_OTHER;
        } 
    }
    else if (m_notation_mode == MUS_NEUMATIC_MODE) {
        if ( sync->IsNeumeSymbol() )
        {
			return NEUME_TOOLS_SYMBOLS;
        } 
        else if (sync->IsNeume() )
        {
            return NEUME_TOOLS_NOTES;
        }
    }
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
    
    m_bufferElement = m_currentElement->m_layerElement->GetChildCopy();
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
	if ( !m_currentElement || !m_bufferElement || !m_currentLayer)
		return;

	if ( m_editorMode == MUS_EDITOR_INSERT ) // can paste in edition mode only
		return;
			
	// arbitrary x value after the current element
	m_currentElement = m_currentLayer->Insert( m_bufferElement, m_currentElement->m_x_abs + m_layout->m_step1 * 3 );

	this->Refresh();
	OnEndEdition();
}

void MusWindow::UpdateScroll()
{
	if (!m_currentStaff)
		return;
    
    // Needs to be fixed - zoom is not taken into account
		
	int x = 0;
	if ( m_currentElement )
		x = ToRendererX( m_currentElement->m_x_abs );
	int y = ToRendererY(  m_currentStaff->m_y_abs );
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
	if ( (y > ys ) && (y < ys + h - 2 * ToRendererX(m_layout->m_staffSize[0])) )
		y = -1;
	else
		y /= yu;

	Scroll( x, y );
	OnSyncScroll( x, y );
}


void MusWindow::OnPopupMenuNote( wxCommandEvent &event )
{
	/*if ( !m_page || !m_currentStaff )
		return;

	MusNote1 *note = NULL;

	if ( m_editElement )
	{
		if ( !m_currentElement || ( m_currentElement->TYPE == SYMB) )
			return;
		else
			 note = (MusNote1*)m_currentElement;
	}
	else
	{
		note = new MusNote1( false, DUR_LG, PITCH_G );
		note->m_x_abs = m_insert_x;
	}

	note->sil = false; // comming from custos

	switch ( event.GetId() )
	{
	case ( ID_MS_N0 ): note->val = DUR_LG; break;
	case ( ID_MS_N1 ): note->val = DUR_BR; break;
	case ( ID_MS_N2 ): note->val = DUR_1; break;
	case ( ID_MS_N3 ): note->val = DUR_2; break;
	case ( ID_MS_N4 ): note->val = DUR_4; break;
	case ( ID_MS_N5 ): note->val = DUR_8; break;
	case ( ID_MS_N6 ): note->val = DUR_16; break;
	case ( ID_MS_N7 ): note->val = DUR_32; break;
	case ( ID_MS_CT ): note->val = CUSTOS; note->sil = true; break;

	case ( ID_MS_R0 ): note->val = DUR_LG; note->sil = true; break;
	case ( ID_MS_R1 ): note->val = DUR_BR; note->sil = true; break;
	case ( ID_MS_R2 ): note->val = DUR_1; note->sil = true; break;
	case ( ID_MS_R3 ): note->val = DUR_2; note->sil = true; break;
	case ( ID_MS_R4 ): note->val = DUR_4; note->sil = true; break;
	case ( ID_MS_R5 ): note->val = DUR_8; note->sil = true; break;
	case ( ID_MS_R6 ): note->val = DUR_16; note->sil = true; break;
	case ( ID_MS_R7 ): note->val = DUR_32; note->sil = true; break;
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

	MusSymbol1 *symbol = NULL;

	if ( m_editElement )
	{
		if ( !m_currentElement || ( !m_currentElement->IsSymbol()) )
			return;
		else
			 symbol = (MusSymbol1*)m_currentElement;
	}
	else
	{
		symbol = new MusSymbol1( );
		symbol->m_x_abs = m_insert_x;
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
		symbol->code = PITCH_G;
		symbol->oct = 4;
		switch ( id )
		{
		case ( ID_MS_DIESE): symbol->calte = ACCID_SHARP; break;
		case ( ID_MS_BEMOL): symbol->calte = ACCID_FLAT; break;
		case ( ID_MS_BECAR): symbol->calte = ACCID_NATURAL; break;
		case ( ID_MS_DDIESE): symbol->calte = ACCID_DOUBLE_SHARP; break;
		case ( ID_MS_DBEMOL): symbol->calte = ACCID_DOUBLE_FLAT; break;
		}
	}
	else if ( id == ID_MS_PNT )
	{
		symbol->flag = PNT;
		symbol->code = PITCH_G;
		symbol->oct = 4;
	}

	//if ( !m_editElement )
	//	m_currentStaff->Insert( symbol );

	this->Refresh();
*/
}


void MusWindow::OnMouseDClick(wxMouseEvent &event)
{
	if (m_currentLayer && m_newElement) 
	{
		wxClientDC dc( this );
		InitDC( &dc );
		m_insert_x = ToLogicalX( dc.DeviceToLogicalX( event.m_x ) ); //???
		int y = ToLogicalY( dc.DeviceToLogicalY( event.m_y ) );
		m_insert_pname = CalculatePitchCode( m_currentLayer, y, m_insert_x, &m_insert_oct );
		//m_newElement->m_x_abs = m_insert_x;
	}
	if ( m_editorMode == MUS_EDITOR_EDIT )
	{
		SetEditorMode(MUS_EDITOR_INSERT);
	}
	else  // insertion
	{
		if ( event.ButtonDClick( wxMOUSE_BTN_LEFT  ) && m_currentLayer && m_newElement )
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
            OnBeginEditionClef();
			m_lastEditedElement = m_currentLayer->Insert( m_newElement, m_insert_x );
            OnEndEditionClef();
            m_lastEditedElement->m_layerElement->SetPitchOrPosition( m_insert_pname, m_insert_oct );
			CheckPoint( UNDO_PART, MUS_UNDO_FILE );
			OnEndEdition();
		}

	}
    this->Refresh();
	event.Skip();
}

void MusWindow::OnMouseLeftUp(wxMouseEvent &event)
{
	if ( m_editorMode == MUS_EDITOR_EDIT || m_lyricMode )
	{
		m_dragging_x = 0;
		m_dragging_y_offset = 0;
		if ( m_has_been_dragged == true ){
			CheckPoint( UNDO_PART, MUS_UNDO_FILE );
			OnEndEdition();
			SyncToolPanel();
			m_has_been_dragged = false;
		}
	}
	
    /*
	if ( m_currentElement && m_currentElement->IsSymbol() && ((MusSymbol1*)m_currentElement)->IsLyric() ){
		MusNote1 *note = ((MusSymbol1*)m_currentElement)->m_note_ptr;
		if (note) {
            note->CheckLyricIntegrity();
        }
	}
	*/ // ax2
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
        // If we select a new item and the last item was a neume, close it
		//if (m_currentElement && m_currentElement->IsNeume()) {
		//	MusNeume *temp = (MusNeume*)m_currentElement;
		//	temp->SetClosed(true);
		//}
        
		wxClientDC dc( this );
		InitDC( &dc );

		m_has_been_dragged = false;
		m_dragging_x  = ToLogicalX( dc.DeviceToLogicalX( event.m_x ) );
		int x = m_dragging_x - 3;
		int y = ToLogicalY( dc.DeviceToLogicalY( event.m_y ) );
		//wxLogMessage("x %d : y %d", x, y);
			
		// Picking element closest to mouse click location
		
		// Default selection of closest note
        m_currentSystem = m_page->GetAtPos( y );
        // we certainly need to check pointers here!
        m_currentStaff = m_currentSystem->GetAtPos( y );
        m_currentLayer = m_currentStaff->GetFirst();
		m_currentElement = m_currentLayer->GetAtPos( x );				
        
		m_lyricMode = false;
		m_inputLyric = false;
		m_editorMode = MUS_EDITOR_EDIT;
		
        /* ax2
		// Checking if there is a Lyric element closer to click location then default note
		MusLaidOutStaff *lyricStaff;
		if ( noteStaff->m_y_drawing <= (uint)(y + 80)){
			if ((lyricStaff = m_page->GetPrevious( noteStaff )) == NULL) lyricStaff = noteStaff;
		} else {
			if ((lyricStaff = m_page->GetNext( noteStaff )) == NULL) lyricStaff = noteStaff;
		}		
		int y_note = noteStaff->m_y_drawing;
        if (noteElement) {
            y_note += noteElement->dec_y;
        }
		MusElement *lyricElement = lyricStaff->GetLyricAtPos( x );		// Lyric element closest to mouse click
		
		if ( lyricElement != NULL){
			int y_lyric = lyricElement->dec_y + lyricStaff->m_y_drawing;
			if ( abs( y_lyric - y ) <= abs( y_note - y ) ){				// Checking if lyric element is closer than note element
				m_lyricMode = true;
				m_inputLyric = false;
				m_editorMode = MUS_EDITOR_INSERT;
				
				m_currentStaff = lyricStaff;
				m_currentElement = lyricElement;
			}
		}
        */
		
		// Track motion on y-axis
		if ( m_currentElement )
			m_dragging_y_offset = y - m_currentStaff->m_y_drawing - m_currentElement->m_y_drawing;
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
            wxLogDebug( "MusWindow::OnMouseLeftDown missing in insert mode in ax2" );
            // Do we need this?
            /*
			wxClientDC dc( this );
			InitDC( &dc );

			int y = ToLogicalY( dc.DeviceToLogicalY( event.m_y ) );
			int x  = ToLogicalX( dc.DeviceToLogicalX( event.m_x ) );

			MusElement *tmp = NULL;
			if ( m_page->GetAtPos( y ) )
				tmp = m_page->GetAtPos( y )->GetAtPos( x );

			if ( tmp )
			{
				if ( tmp->IsNote() )
				{
					m_note = *(MusNote1*)tmp;
					m_newElement = &m_note;
				}
				else if ( tmp->IsSymbol() )
				{
					m_symbol = *(MusSymbol1*)tmp;
					m_newElement = &m_symbol;
				}
			}
            */
		}
		else // update current staff
		{
			wxClientDC dc( this );
			InitDC( &dc );

			int y = ToLogicalY( dc.DeviceToLogicalY( event.m_y ) );
            // Default selection of closest note
            m_currentSystem = m_page->GetAtPos( y );
            // we certainly need to check pointers here!
            m_currentStaff = m_currentSystem->GetAtPos( y );
            m_currentLayer = m_currentStaff->GetFirst();
		}
    }
	event.Skip();    
}

void MusWindow::OnMouseMotion(wxMouseEvent &event)
{
	if ( event.Dragging() && event.LeftIsDown() && m_dragging_x && m_currentElement && m_currentLayer )
	{
		if ( !m_has_been_dragged )
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
		m_has_been_dragged = true;
		wxClientDC dc( this );
		InitDC( &dc );
		m_insert_x = ToLogicalX( dc.DeviceToLogicalX( event.m_x ) );
		int y = ToLogicalY( dc.DeviceToLogicalY( event.m_y ) ) - m_dragging_y_offset;
		
		if ( m_editorMode == MUS_EDITOR_EDIT )
		{
			m_insert_pname = CalculatePitchCode( m_currentLayer, y, m_insert_x, &m_insert_oct );
			m_currentElement->m_layerElement->SetPitchOrPosition( m_insert_pname, m_insert_oct );
		} 
		else if ( m_lyricMode )					// Movement of lyric element on y-axis
		{
			m_currentElement->m_y_drawing = y - m_currentStaff->m_y_drawing;
		} 
		
		if ( m_insert_x != m_dragging_x  )		// If element has moved in the x-axis
		{
			// TODO m_currentElement->ClearElement( &dc, m_currentStaff );
            wxLogDebug("End dragging m_dragging_x %d; m_insert_x %d", m_dragging_x, m_insert_x );
			m_currentElement->m_x_abs += ( m_insert_x - m_dragging_x );
			m_dragging_x = m_insert_x;
			if ( m_editorMode == MUS_EDITOR_EDIT ) {
				//m_currentStaff->CheckIntegrity();
			}
		}
		this->Refresh();	
	}

	event.Skip();
}

void MusWindow::OnMidiInput(wxCommandEvent &event)
{
    /*
    //wxLogDebug("Midi %d", event.GetInt() ) ;
    int octave = event.GetInt() / 12;
	int hauteur = event.GetInt() - (octave * 12);
	
	printf("octave: %d, hauteur: %d\n", octave, hauteur);
    if ( m_currentElement && m_currentElement->IsNote() ) {
//        m_currentElement->SetPitch( die[hauteur], octave, m_currentStaff );
		m_currentElement->SetPitch( MusWindow::s_sharps[hauteur], octave );
    }
    */ // ax2
    
}

bool MusWindow::MoveUpDown( bool up )
{
	if ( !m_page || !m_currentSystem || !m_currentStaff || !m_currentLayer ) {
		return false;
    } 
    
    MusPage *page = m_page;
    MusSystem *system = m_currentSystem;
    MusLaidOutStaff *staff = m_currentStaff;
    MusLaidOutLayer *layer = m_currentLayer;
    
    int x = 0;
    if ( m_currentElement ) {
        x = m_currentElement->m_x_abs;
    }
    
    if ( up ) {
        // layer up in the staff
        if ( m_currentStaff->GetPrevious( m_currentLayer ) )
        {
            layer = m_currentStaff->GetPrevious( m_currentLayer );
        }
        // staff up in the system
        else if ( m_currentSystem->GetPrevious( m_currentStaff ) )
        {
            staff = m_currentSystem->GetPrevious( m_currentStaff );
            layer = staff->GetLast();
        }
        // previous system
        else if ( m_page->GetPrevious( m_currentSystem ) )
        {
            system = m_page->GetPrevious( m_currentSystem );
            staff = system->GetLast();
            if ( staff ) {
                layer = staff->GetLast();
            }
        }
        // previous page
        // TODO - we would also to set the page in the MusWindow
    }
    else {
        // layer up in the staff
        if ( m_currentStaff->GetNext( m_currentLayer ) )
        {
            layer = m_currentStaff->GetNext( m_currentLayer );
        }
        // staff up in the system
        else if ( m_currentSystem->GetNext( m_currentStaff ) )
        {
            staff = m_currentSystem->GetNext( m_currentStaff );
            layer = staff->GetFirst();
        }
        // previous system
        else if ( m_page->GetNext( m_currentSystem ) )
        {
            system = m_page->GetNext( m_currentSystem );
            staff = system->GetFirst();
            if ( staff ) {
                layer = staff->GetFirst();
            }
        }
        // previous page
        // TODO - we would also to set the page in the MusWindow
    }
    
    if ( !layer || !staff || !system || !page ) {
        return false;
    }
    
    m_currentLayer = layer;
    m_currentStaff = staff;
    m_currentSystem = system;
    m_page = page;
    
    // now try to get the element - still success if no element at all
    m_currentElement = m_currentLayer->GetAtPos(x);
    return true;
}

bool MusWindow::MoveLeftRight( bool left )
{
	if ( !m_page || !m_currentSystem || !m_currentStaff || !m_currentLayer ) {
		return false;
    } 
    
    MusPage *page = m_page;
    MusSystem *system = m_currentSystem;
    MusLaidOutStaff *staff = m_currentStaff;
    MusLaidOutLayer *layer = m_currentLayer;
    
    if ( left ) {
        // previous element
        if ( m_currentLayer->GetPrevious( m_currentElement )) {
            m_currentElement = m_currentLayer->GetPrevious( m_currentElement );
        }
        // previous system
        else if ( m_page->GetPrevious( m_currentSystem ) )
        {
            int currentStaffNo = m_currentStaff->GetStaffNo();
            int currentLayerNo = m_currentLayer->GetLayerNo();
            system = m_page->GetPrevious( m_currentSystem );
            staff = system->GetStaff( currentStaffNo );
            if ( staff ) {
                layer = staff->GetLayer( currentLayerNo );
                if ( layer ) {
                    m_currentElement = layer->GetLast();
                }
            }
        }
        // previous page
        // TODO - we would also to set the page in the MusWindow
    }
    else {
        // next element
        if ( m_currentLayer->GetNext( m_currentElement )) {
            m_currentElement = m_currentLayer->GetNext( m_currentElement );
        }
        // next system
        else if ( m_page->GetNext( m_currentSystem ) )
        {
            int currentStaffNo = m_currentStaff->GetStaffNo();
            int currentLayerNo = m_currentLayer->GetLayerNo();
            system = m_page->GetNext( m_currentSystem );
            staff = system->GetStaff( currentStaffNo );
            if ( staff ) {
                layer = staff->GetLayer( currentLayerNo );
                if ( layer ) {
                    m_currentElement = layer->GetFirst();
                }
            }
        }
        // next page
        // TODO - we would also to set the page in the MusWindow
    }
    
    if ( !layer || !staff || !system || !page ) {
        return false;
    }
    
    m_currentLayer = layer;
    m_currentStaff = staff;
    m_currentSystem = system;
    m_page = page;
    
    return true;
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
	//case ( 'C' ): note = CUSTOS; break; // ax2
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
        MoveLeftRight( false );
        UpdateScroll();
    }
    else if ( event.GetKeyCode() == WXK_LEFT )
    {
        MoveLeftRight( true );
        UpdateScroll();
    }
    else if ( event.GetKeyCode() == WXK_UP )
    {
        MoveUpDown( true );
        UpdateScroll();
    }
    else if ( event.GetKeyCode() == WXK_DOWN )
    {
        MoveUpDown( false );
        UpdateScroll();
    }
    else if ( event.GetKeyCode() == WXK_HOME ) 
    {
        if ( m_currentLayer->GetFirst( ) ) {
            m_currentElement = m_currentLayer->GetFirst( );
        }
    }
    else if ( event.GetKeyCode() == WXK_END ) 
    {
        if ( m_currentLayer->GetLast( ) ) {
            m_currentElement = m_currentLayer->GetLast( );
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
    /*
    int noteKeyCode = GetNoteValue( event.m_keyCode );
    if ( ((event.m_keyCode == WXK_DELETE ) || (event.m_keyCode == WXK_BACK)) && m_currentElement) //"Delete or Backspace" event
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        MusElement *del = m_currentElement;
        MusLaidOutStaff *delstaff = m_currentStaff;
        
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
        del->deleteMeiRef();
        delstaff->Delete( del );
        if ( m_currentStaff != delstaff )
        {
            // reset previous staff with no element before checkpoint and then swap again
            MusLaidOutStaff *tmp = m_currentStaff;
            m_currentStaff = delstaff;
            del = m_currentElement;
            m_currentElement = NULL;
            CheckPoint( UNDO_PART, MUS_UNDO_FILE );
            m_currentStaff = tmp;
            m_currentElement = del;
        }
        else
            CheckPoint( UNDO_PART, MUS_UNDO_FILE );
        
        this->Refresh();
    }
    else if ( in ( event.m_keyCode, WXK_F2, WXK_F8 ) && m_currentElement) // Change hauteur
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        m_insert_pname = m_currentElement->filtrcod( event.m_keyCode - WXK_F1, &m_insert_oct );
        m_currentElement->SetPitch( m_insert_pname, m_insert_oct );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
    }
    else if ( event.m_controlDown && ( event.m_keyCode == WXK_UP ) && m_currentElement) // correction hauteur avec les fleches, up
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        m_insert_pname = ((m_currentElement->TYPE==NEUME) || (m_currentElement->TYPE == NEUME_SYMB)) ? (m_currentElement->filtrpitch( m_currentElement->pitch + 1, &m_insert_oct )) : (m_currentElement->filtrcod( m_currentElement->code + 1, &m_insert_oct ));
        m_currentElement->SetPitch( m_insert_pname, m_insert_oct );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
    }
    else if ( event.m_controlDown && ( event.m_keyCode == WXK_DOWN ) && m_currentElement) // correction hauteur avec les fleches, down
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        m_insert_pname = ((m_currentElement->TYPE==NEUME) || (m_currentElement->TYPE == NEUME_SYMB)) ? (m_currentElement->filtrpitch( m_currentElement->pitch - 1, &m_insert_oct )) : (m_currentElement->filtrcod( m_currentElement->code - 1, &m_insert_oct ));
        m_currentElement->SetPitch( m_insert_pname, m_insert_oct );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
    }
    else if ( m_currentElement && m_currentElement->IsNeume() && 
             ( (event.m_keyCode == 'B') || (event.m_keyCode == 'D' ) ) ) // ajouter un bemol à une note
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        MusSymbol1 alteration;
        alteration.flag = ALTER;
        alteration.code = m_currentElement->code;
        alteration.oct = m_currentElement->oct;
        if ( event.m_keyCode == 'B') 
            alteration.calte = ACCID_FLAT;
        else
            alteration.calte = ACCID_SHARP;
        alterationm_x_abs = m_currentElement->m_x_abs - m_step1 * 3;
        m_currentStaff->Insert( &alteration );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
    }
    else if ( m_currentElement && m_currentElement->IsNeume() && 
			 (event.m_keyCode == '.')  ) // ajouter un point
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        MusSymbol1 point;
        point.flag = PNT;
        point.code = m_currentElement->code;
        point.oct = m_currentElement->oct;
        pointm_x_abs = m_currentElement->m_x_abs + m_step1 * 3;
        // special case where we move forward
        m_currentElement = m_currentStaff->Insert( &point );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
    }
    else if ( m_currentElement && m_currentElement->IsNeume() &&
             (in( noteKeyCode, 0, 5 )))
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        int vflag = ( event.m_controlDown || (noteKeyCode == CUSTOS)) ? 1 : 0;
        m_currentElement->SetValue( noteKeyCode , m_currentStaff, vflag );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
    }
    else if ( event.m_controlDown && (( event.m_keyCode == WXK_LEFT ) || (event.m_keyCode == WXK_RIGHT )) && m_currentElement) // moving element
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        if ( event.m_keyCode == WXK_LEFT ) {
            m_currentElement->m_x_abs -=3;
        } else {
            m_currentElement->m_x_abs +=3;
        }
        this->Refresh();
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
    }
    else if ( m_currentElement && m_currentElement->IsNeumeSymbol() &&
             in( event.m_keyCode, 33, 125) ) // any other keycode on symbol (ascii codes)
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        int vflag = ( event.m_controlDown ) ? 1 : 0;
        m_currentElement->SetValue( event.m_keyCode, m_currentStaff, vflag );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );	
    }
    OnEndEdition();
    SyncToolPanel();
    */
    wxLogDebug( "MusWindow::NeumeEditOnKeyDown missing in ax2" );
}

/**
 * Key handler for Neumatic editor / insert mode
 */
void MusWindow::NeumeInsertOnKeyDown(wxKeyEvent &event) {
    /*
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
    */ // ax2
}

/**
 * Key handler for Mensural editor / edit mode
 */
void MusWindow::MensuralEditOnKeyDown(wxKeyEvent &event) {
    
    // the pointers validity is checked in OnKeyDown
    int noteKeyCode = GetNoteValue( event.m_keyCode );
    
    if ( ((event.m_keyCode == WXK_DELETE ) || (event.m_keyCode == WXK_BACK)) && m_currentElement) //"Delete or Backspace" event
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        MusLaidOutLayerElement *del = m_currentElement;
        MusLaidOutLayer *delLayer = m_currentLayer;
        
        if (event.m_keyCode == WXK_DELETE )		//"Delete" event
        {
            MoveLeftRight( false );
        }
        else                                    //"Backspace" event
        {
            MoveLeftRight( true );
        }
        
        // we have moved, currentLayer is now the layer we will be after deletion
        MusLaidOutLayer *nextLayer = m_currentLayer;
        m_currentLayer = delLayer; // we swap it to do the deletion
        
        // in case we failed moving
        if ( del == m_currentElement ) {
            m_currentElement = NULL;
        }
        
        OnBeginEditionClef();
        m_currentLayer->Delete( del );
        OnEndEditionClef();
        
        m_currentLayer = nextLayer;
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
        
        OnEndEdition();
        SyncToolPanel();
    }
    else if ( in ( event.m_keyCode, WXK_F2, WXK_F8 ) && m_currentElement ) // Change hauteur
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        m_currentElement->m_layerElement->GetPitchOrPosition( &m_insert_pname, &m_insert_oct);
        m_insert_pname = event.m_keyCode - WXK_F1;
        MusLayerElement::AdjustPname( &m_insert_pname, &m_insert_oct );
        m_currentElement->m_layerElement->SetPitchOrPosition( m_insert_pname, m_insert_oct );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
        OnEndEdition();
    }
    else if ( event.m_controlDown && ( event.m_keyCode == WXK_UP ) && m_currentElement) // correction hauteur avec les fleches, up
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        m_currentElement->m_layerElement->GetPitchOrPosition( &m_insert_pname, &m_insert_oct);
        m_insert_pname++;
        MusLayerElement::AdjustPname( &m_insert_pname, &m_insert_oct );
        m_currentElement->m_layerElement->SetPitchOrPosition( m_insert_pname, m_insert_oct );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
        OnEndEdition();
    }
    else if ( event.m_controlDown && ( event.m_keyCode == WXK_DOWN ) && m_currentElement) // correction hauteur avec les fleches, down
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        m_currentElement->m_layerElement->GetPitchOrPosition( &m_insert_pname, &m_insert_oct);
        m_insert_pname--;
        MusLayerElement::AdjustPname( &m_insert_pname, &m_insert_oct );
        m_currentElement->m_layerElement->SetPitchOrPosition( m_insert_pname, m_insert_oct );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
        OnEndEdition();
    }
    else if ( m_currentElement && m_currentElement->IsNote() && 
             ( (event.m_keyCode == 'F') || (event.m_keyCode == 'S' ) ) ) // ajouter un bemol à une note
    {
        MusNote *note = dynamic_cast<MusNote*>(m_currentElement->m_layerElement);
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        MusSymbol alteration( SYMBOL_ACCID );
        alteration.m_pname = note->m_pname;
        alteration.m_oct = note->m_oct;
        if ( event.m_keyCode == 'F') {
            alteration.m_accid = ACCID_FLAT;
        }
        else {
            alteration.m_accid = ACCID_SHARP;
        }
        m_currentLayer->Insert( &alteration, m_currentElement->m_x_abs - m_layout->m_step1 * 3 );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
        OnEndEdition();
    }
    else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == '.')  ) // ajouter un point
    {
        MusNote *note = dynamic_cast<MusNote*>(m_currentElement->m_layerElement);
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        MusSymbol dot( SYMBOL_DOT );
        dot.m_pname = note->m_pname;
        dot.m_oct = note->m_oct;
        m_currentLayer->Insert( &dot, m_currentElement->m_x_abs + m_layout->m_step1 * 3 );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
        OnEndEdition();
    }
    else if ( m_currentElement && (m_currentElement->IsNote() || m_currentElement->IsRest()) &&
             ( in( noteKeyCode, 0, 7 ) ) ) // change duration on a note or a rest
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        //int vflag = ( event.m_controlDown || (noteKeyCode == CUSTOS)) ? 1 : 0;
        m_currentElement->m_layerElement->SetValue( noteKeyCode );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
        OnEndEdition();
    }
    else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == 'L')  ) // Ligature 
    {	
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        m_currentElement->m_layerElement->SetLigature( );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
        OnEndEdition();
    }
    else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == 'I')  ) // Change coloration
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        m_currentElement->m_layerElement->ChangeColoration( );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );	
        OnEndEdition();
    }
    else if ( m_currentElement && m_currentElement->IsNote() && 
			 (event.m_keyCode == 'A')  ) // Change stem direction
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        m_currentElement->m_layerElement->ChangeStem(  );
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
        OnEndEdition();
    }
    /*
    else if ( event.m_controlDown && (( event.m_keyCode == WXK_LEFT ) || (event.m_keyCode == WXK_RIGHT )) && m_currentElement) // moving element
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        if ( event.m_keyCode == WXK_LEFT ) {
            m_currentElement->m_x_abs -=3;
        } else {
            m_currentElement->m_x_abs +=3;
        }
        this->Refresh();
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );
        OnEndEdition();
    }
    else if ( (event.m_keyCode == 'T') && m_currentElement && m_currentElement->IsNote() )
    {
        m_editorMode = MUS_EDITOR_INSERT;
        m_lyricMode = true;
        
        if ( m_currentElement && m_currentElement->IsNote() && ((MusNote1*)m_currentElement)->m_lyrics.GetCount() > 0 ){
            MusSymbol1 *lyric = &((MusNote1*)m_currentElement)->m_lyrics[0];
            m_currentElement = lyric;
        }
        else if ( m_currentElement && m_currentElement->IsNote() ){
            MusSymbol1 *lyric = new MusSymbol1();
            lyric->flag = CHAINE;
            lyric->fonte = LYRIC;
            lyric->m_debord_str = "";
            lyric->m_x_abs = ((MusNote1*)m_currentElement)->m_x_abs - 10;
            lyric->dec_y = - STAFF_OFFSET;   //Add define for height
            lyric->offset = ((MusNote1*)m_currentElement)->offset;
            lyric->m_note_ptr = (MusNote1*)m_currentElement;
            ((MusNote1*)m_currentElement)->m_lyrics.Add( lyric );
            m_currentElement = lyric;
            m_inputLyric = true;
        }
        else if ( m_currentElement && m_currentStaff->GetLyricAtPos( m_currentElement->m_x_abs ) )
            m_currentElement = m_currentStaff->GetLyricAtPos( m_currentElement->m_x_abs );
        else if ( m_currentStaff->GetFirstLyric() )
            m_currentElement = m_currentStaff->GetFirstLyric();
        else{
            m_editorMode = MUS_EDITOR_EDIT;
            m_lyricMode = false;
        }
        this->Refresh();
    } 
    */
    else if ( m_currentElement && (m_currentElement->IsMensur() || m_currentElement->IsClef() || m_currentElement->IsSymbol())
        && in( event.m_keyCode, 33, 125) ) // any other keycode on symbol (ascii codes)
    {
        PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
        // we might be editing a clef - see method for doc
        OnBeginEditionClef();
        int vflag = ( event.m_controlDown ) ? 1 : 0;
        m_currentElement->m_layerElement->SetValue( event.m_keyCode, vflag );
        OnEndEditionClef();
        CheckPoint( UNDO_PART, MUS_UNDO_FILE );	
        OnEndEdition();
    }
    else {
        return;
    }
    this->Refresh();
}

/**
 * Key handler for Mensural editor / insert mode
 */
void MusWindow::MensuralInsertOnKeyDown(wxKeyEvent &event) {
    
    int noteKeyCode = GetNoteValue( event.m_keyCode );
    // changing category
    if ( event.m_controlDown && (event.m_keyCode == 'N')) {// change set (note, rests, key, signs, symbols, ....
        m_newElement = &m_note;
    }
    else if ( event.m_controlDown && (event.m_keyCode == 'C')) // clefs
    {	
        m_newElement = &m_clef;
    }	
    else if ( event.m_controlDown && (event.m_keyCode == 'P')) // proportions
    {	
        m_newElement = &m_mensur;
    }	
    else if ( event.m_controlDown && (event.m_keyCode == 'S')) // symbols
    {
        m_newElement = &m_symbol;
    }
    // changing values within a category
    else if ( m_newElement && (m_newElement->IsNote() || m_newElement->IsRest()) ) 
    // change duree sur une note ou un silence
    {
        if ( event.m_controlDown && in( noteKeyCode, 0, 6 ) ) {
            m_newElement = &m_rest;
            m_newElement->SetValue( noteKeyCode , 0 );
        }
        else if ( in( noteKeyCode, 0, 7 ) ){
            m_newElement = &m_note;
            m_newElement->SetValue( noteKeyCode , 0 );
        }
    }
    else if ( m_newElement && m_newElement->IsNote() && (noteKeyCode == 'L') )
        m_newElement->SetLigature();
    else if ( m_newElement && m_newElement->IsNote() && (noteKeyCode == 'I') )
        m_newElement->ChangeColoration( );
    else if ( m_newElement && m_newElement->IsNote() && (noteKeyCode == 'A') )
        m_newElement->ChangeStem( );
    else if ( m_newElement && (m_newElement->IsClef() || m_newElement->IsMensur()) &&
        in( event.m_keyCode, 33, 125) ) // any other keycode on clef and mensur (ascii codes)
    {
        m_newElement->SetValue( event.m_keyCode, 0 );
    }
    else if ( m_newElement && (m_newElement->IsSymbol() || m_newElement->IsBarline()) &&
             in( event.m_keyCode, 33, 125) ) // any other keycode on symbol (ascii codes)
    {
        if (noteKeyCode == '|') {
            m_newElement = &m_barline;
        }
        else {
            m_newElement = &m_symbol;
            int vflag = ( event.m_controlDown ) ? 1 : 0;
            m_newElement->SetValue( event.m_keyCode, vflag );
        }
    }
    UpdatePen();
    OnEndEdition();
    SyncToolPanel();
}

void MusWindow::OnKeyDown(wxKeyEvent &event)
{
	if ( !m_page || !m_currentSystem || !m_currentStaff || !m_currentLayer ) {
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
    /*
    if ( !m_inputLyric )	// Lyric Editing mode //
	{	
		if ( event.m_keyCode == 'T' )			//"T" event: Escape lyric navigation mode
		{
			m_lyricMode = false;
			m_inputLyric = false;
			m_editorMode = MUS_EDITOR_EDIT;
			if ( m_currentElement && m_currentElement->IsSymbol() && ((MusSymbol1*)m_currentElement)->m_note_ptr )
				m_currentElement = ((MusSymbol1*)m_currentElement)->m_note_ptr;
			else if ( m_currentElement && m_currentStaff->GetAtPos( m_currentElement->m_x_abs ) )
				m_currentElement = m_currentStaff->GetAtPos( m_currentElement->m_x_abs );
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
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
			if ( event.GetKeyCode() == WXK_RIGHT && m_currentElement->IsSymbol() )			//"Right arrow" event: switch lyric association to note to the right 
			{				
				MusNote1 *oldNote = ((MusSymbol1*)m_currentElement)->m_note_ptr;
				MusNote1 *newNote = m_currentStaff->GetNextNote( (MusSymbol1*)m_currentElement );
				m_currentStaff->SwitchLyricNoteAssociation( (MusSymbol1*)m_currentElement, oldNote, newNote, true );
				oldNote->CheckLyricIntegrity();
				newNote->CheckLyricIntegrity();
			}
			else if ( event.GetKeyCode() == WXK_LEFT && m_currentElement->IsSymbol() )		//"Left arrow" event: switch lyric association to note to the left
			{
				MusNote1 *oldNote = ((MusSymbol1*)m_currentElement)->m_note_ptr;
				MusNote1 *newNote = m_currentStaff->GetPreviousNote( (MusSymbol1*)m_currentElement );				
				m_currentStaff->SwitchLyricNoteAssociation( (MusSymbol1*)m_currentElement, oldNote, newNote, false );
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
			CheckPoint(UNDO_PART, MUS_UNDO_FILE );
			OnEndEdition();
			this->Refresh();
		}
		else if ( ( ( event.m_keyCode == WXK_DELETE ) || ( event.m_keyCode == WXK_BACK ) ) && m_currentElement 
			&& m_currentElement->IsSymbol() )												//"Delete or Backspace" event
		{
			
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
			MusElement *del = m_currentElement;
			MusLaidOutStaff *delstaff = m_currentStaff;
			
			// Find next element to select
			if (event.m_keyCode == WXK_DELETE )												//"Delete" event
			{
				if ( m_currentStaff->GetNextLyric( (MusSymbol1*)del ) )
					m_currentElement = m_currentStaff->GetNextLyric( (MusSymbol1*)del );
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
				if ( m_currentStaff->GetPreviousLyric( (MusSymbol1*)del ) )
					m_currentElement = m_currentStaff->GetPreviousLyric( (MusSymbol1*)del );
				else if ( m_page->GetPrevious( m_currentStaff ) )
				{
					m_currentStaff = m_page->GetPrevious( m_currentStaff );
					m_currentElement = m_currentStaff->GetLastLyric();
				}
				else
					m_currentElement = NULL;
			}
			
			delstaff->DeleteLyric( (MusSymbol1*)del );
			if ( m_currentStaff != delstaff )
			{
				// Reset previous staff with no element before checkpoint and then swap again
				MusLaidOutStaff *tmp = m_currentStaff;
				m_currentStaff = delstaff;
				del = m_currentElement;
				m_currentElement = NULL;
				CheckPoint( UNDO_PART, MUS_UNDO_FILE );
				m_currentStaff = tmp;
				m_currentElement = del;
			}
			else
				CheckPoint( UNDO_PART, MUS_UNDO_FILE );
			
			OnEndEdition();
			SyncToolPanel();
			this->Refresh();
		}
		else	// Navigation over lyrics using arrows
		{
			if ( event.GetKeyCode() == WXK_RIGHT && m_currentElement && m_currentElement->IsSymbol() )			//"Right arrow" event
			{
				if ( m_currentStaff->GetNextLyric( (MusSymbol1*)m_currentElement ) )
					m_currentElement = m_currentStaff->GetNextLyric( (MusSymbol1*)m_currentElement );
				else if ( m_page->GetNext( m_currentStaff ) )
				{
					m_currentStaff = m_page->GetNext( m_currentStaff );
					m_currentElement = m_currentStaff->GetFirstLyric();
				}
				UpdateScroll();
			}
			else if ( event.GetKeyCode() == WXK_LEFT && m_currentElement && m_currentElement->IsSymbol() )		//"Left arrow" event
			{
				if ( m_currentStaff->GetPreviousLyric( (MusSymbol1*)m_currentElement ) )
					m_currentElement = m_currentStaff->GetPreviousLyric( (MusSymbol1*)m_currentElement );
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
						x = m_currentElement->m_x_abs;
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
						x = m_currentElement->m_x_abs;
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
	else		// Lyric insertion mode //
	{
		event.Skip();  //Do further processing on the event in OnChar method
		
		if ( event.m_keyCode == WXK_RETURN )									// "Enter" event: Enter into lyric insertion mode
		{
			if ( m_currentElement->IsSymbol() && ((MusSymbol1*)m_currentElement)->IsLyric()){
				PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
				MusSymbol1* lyric = (MusSymbol1*)m_currentElement;
				if ( lyric->IsLyricEmpty() ){
					if ( (m_currentElement = m_currentStaff->GetNextLyric( lyric )) == NULL ){
						m_currentElement = m_currentStaff->GetPreviousLyric( lyric );
					}
					lyric->m_note_ptr->DeleteLyricFromNote( lyric );
				}				
				CheckPoint( UNDO_PART, MUS_UNDO_FILE );
				OnEndEdition();
			}

			m_inputLyric = !m_inputLyric;
		}
		else if ( event.m_keyCode == WXK_BACK && m_currentElement->IsSymbol() )		//"Backspace" event
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
			m_lyricCursor--;
			MusSymbol1 *element = (MusSymbol1*)m_currentElement;
			if ( !element->DeleteCharInLyricAt( m_lyricCursor ) ){
				if ( element->m_debord_str.Length() == 0 ){
					m_currentElement = m_currentStaff->GetPreviousLyric( element );
					if ( !m_currentElement ) m_currentElement = m_currentStaff->GetNextLyric( element );
					m_lyricCursor = ((MusSymbol1*)m_currentElement)->m_debord_str.Length();
					element->m_note_ptr->DeleteLyricFromNote( element );					
				} else{
					m_lyricCursor++;
				}
			} 
			CheckPoint( UNDO_PART, MUS_UNDO_FILE );
			OnEndEdition(); 
		}
		else if ( event.m_keyCode == WXK_DELETE && m_currentElement->IsSymbol() )		//"Delete" event
		{
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
			((MusSymbol1*)m_currentElement)->DeleteCharInLyricAt( m_lyricCursor );
			CheckPoint( UNDO_PART, MUS_UNDO_FILE );
			OnEndEdition(); 			
		}
		else if ( event.m_keyCode == WXK_SPACE && m_currentElement->IsSymbol() )		//"Space" event
		{
			if ( m_lyricCursor == (int)((MusSymbol1*)m_currentElement)->m_debord_str.Length() ){
				if ( ((MusSymbol1*)m_currentElement)->IsLastLyricElementInNote() ){
					MusNote1 *note = m_currentStaff->GetNextNote( (MusSymbol1*)m_currentElement );
					if ( !note )
						return;
					
                    PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
					//Add new lyric element to the next note
					MusSymbol1 *lyric = new MusSymbol1( *((MusSymbol1*)m_currentElement) );
					lyric->m_debord_str = "";
					lyric->m_x_abs = note->m_x_abs - 10;
					m_currentStaff->SwitchLyricNoteAssociation( lyric, ((MusSymbol1*)m_currentElement)->m_note_ptr, note, true );				
					(((MusSymbol1*)m_currentElement)->m_note_ptr)->CheckLyricIntegrity();
					m_currentElement = lyric;
					m_lyricCursor = 0;
                    CheckPoint( UNDO_PART, MUS_UNDO_FILE );
				} else {
					m_currentElement = m_currentStaff->GetNextLyric( (MusSymbol1*)m_currentElement );
					m_lyricCursor = 0;
				}
				OnEndEdition();
			} 
		}
		else if ( event.m_keyCode == WXK_TAB && m_currentElement->IsSymbol() )			//"Tab" event
		{
			if ( m_lyricCursor == (int)((MusSymbol1*)m_currentElement)->m_debord_str.Length() ){
				if ( ((MusSymbol1*)m_currentElement)->IsLastLyricElementInNote() ){
					MusNote1 *note = m_currentStaff->GetNextNote( (MusSymbol1*)m_currentElement );
					if ( !note )
						return;
					
                    PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
					//Add new lyric to next note
					MusSymbol1 *lyric = new MusSymbol1( *((MusSymbol1*)m_currentElement) );
					lyric->m_debord_str = "";
					lyric->m_x_abs = note->m_x_abs - 10;					
					m_currentStaff->SwitchLyricNoteAssociation( lyric, ((MusSymbol1*)m_currentElement)->m_note_ptr, note, true );				
					
					//Add dash ("-") element between the two lyrics
					MusSymbol1 *lyric2 = new MusSymbol1( *((MusSymbol1*)m_currentElement) );
					lyric2->m_debord_str = "-";
					int length = ((MusSymbol1*)m_currentElement)->m_debord_str.Length();
					lyric2->m_x_abs = lyric->m_x_abs - ( lyric->m_x_abs - ( m_currentElement->m_x_abs + length * 10 ) ) / 2;			
					m_currentStaff->SwitchLyricNoteAssociation( lyric2, ((MusSymbol1*)m_currentElement)->m_note_ptr, note, true );				
                    
					m_currentElement = lyric;
					m_lyricCursor = 0;
                    CheckPoint( UNDO_PART, MUS_UNDO_FILE );
				} else {
					m_currentElement = m_currentStaff->GetNextLyric( (MusSymbol1*)m_currentElement );
					m_lyricCursor = 0;
				}
				OnEndEdition();
			}
		}
		else		//Left and right cursor navigation within lyrics
		{
			if ( event.m_keyCode == WXK_RIGHT && m_currentElement->IsSymbol() )			//"Right arrow" event
			{
				if ( m_lyricCursor < (int)((MusSymbol1*)m_currentElement)->m_debord_str.Length() )
					m_lyricCursor++;
				else if ( m_lyricCursor == (int)((MusSymbol1*)m_currentElement)->m_debord_str.Length() )
				{
					MusSymbol1 *element = (MusSymbol1*)m_currentElement;
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
					MusSymbol1 *element = (MusSymbol1*)m_currentElement;
					if ( m_currentStaff->GetPreviousLyric( element ) ){
						m_currentElement = m_currentStaff->GetPreviousLyric( element );
						m_lyricCursor = ((MusSymbol1*)m_currentElement)->m_debord_str.Length();	
					} else if ( m_page->GetPrevious( m_currentStaff ) ){
						m_currentStaff = m_page->GetPrevious( m_currentStaff );
						m_currentElement = m_currentStaff->GetLastLyric();
						m_lyricCursor = ((MusSymbol1*)m_currentElement)->m_debord_str.Length();
					}
					if ( element->m_debord_str.Length() == 0 )			
						element->m_note_ptr->DeleteLyricFromNote( element );
				}
			}
			UpdateScroll();
		}
		this->Refresh();
    }
    */
    wxLogDebug( "MusWindow::LyricEntry missing in ax2" );
}

void MusWindow::OnChar(wxKeyEvent &event)
{
    /*
    if ( m_lyricMode && m_inputLyric )
	{
		if ( ((event.m_keyCode >= 65 && event.m_keyCode <= 90) || (event.m_keyCode >= 97 && event.m_keyCode <= 122 ))
			&& m_currentElement->IsSymbol() )
		{	
			PrepareCheckPoint( UNDO_PART, MUS_UNDO_FILE );
			((MusSymbol1*)m_currentElement)->InsertCharInLyricAt( m_lyricCursor, (char)event.m_keyCode );
			m_lyricCursor++;
            (((MusSymbol1*)m_currentElement)->m_note_ptr)->CheckLyricIntegrity();
			CheckPoint( UNDO_PART, MUS_UNDO_FILE );
			OnEndEdition();
		}
    }
    */ // ax2
}


void MusWindow::OnPaint(wxPaintEvent &event)
{
	if ( !m_page )
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
	//	dc.SetLogicalOrigin( - (margeMorteHor - m_leftMargin), -margeMorteVer );
	//else
		dc.SetLogicalOrigin( - MUS_BORDER_AROUND_PAGE / 2, - MUS_BORDER_AROUND_PAGE / 2);
    
	this->PrepareDC( dc );
	dc.SetTextForeground( *wxBLACK );
	dc.SetMapMode( wxMM_TEXT );
	dc.SetAxisOrientation( true, false );
    dc.SetUserScale( double(GetZoom())/100.0,  double(GetZoom())/100.0 );
	
	//m_page->Init( this );
    MusWxDC ax_dc( &dc );
    DrawPage( &ax_dc, m_page );

    // TODO for cursor
    // Draw the cursor if we are in insertion mode, we have a m_newElement and a m_currentStaff
    // We can add a DrawCursor method, use the y position of the staff and the x of the element
    // What shape to draw??
    

	// hitting return in keyboard entry mode sends us here for some reason
	
	//if (!m_editElement && m_newElement && m_currentStaff) {
	//	m_currentColour = AxRED;
		//drawing code here
		
		//printf("staff y: %d\n", m_currentStaff->m_y_drawing);
		
	// TODO	this->rect_plein2(&dc, m_newElement->m_x_abs+35, m_currentStaff->m_y_drawing-200, 
	//					  m_newElement->m_x_abs+40, m_currentStaff->m_y_drawing-40);
		
	//	m_currentColour = AxBLACK;
	//}
}

void MusWindow::OnSize(wxSizeEvent &event)
{
	Resize();
}


