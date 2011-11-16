/////////////////////////////////////////////////////////////////////////////
// Name:        musrc.cpp
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musrc.h"
#include "muslaidoutlayerelement.h"
#include "musnote.h"
#include "musiobin.h"

#include "app/axapp.h"

#include <typeinfo>

//----------------------------------------------------------------------------
// MusRC
//----------------------------------------------------------------------------

MusRC::MusRC( )
{
	m_doc = NULL;
    m_page = NULL;
	m_npage = 0;

	m_currentColour = AxBLACK;
	m_currentElement = NULL;
	m_currentStaff = NULL;

	m_lyricMode = false;
	m_inputLyric = false;
	m_editorMode = MUS_EDITOR_INSERT;
	
	m_notation_mode = MUS_MENSURAL_MODE;
    
    m_pageMaxX = 0;
    m_pageMaxY = 0;
    
	discontinu = 0;
	
	m_str.Alloc(1000);
}


MusRC::~MusRC()
{
}

void MusRC::SetDoc( MusDoc *doc )
{
	if ( doc == NULL ) // unset file
	{
		m_doc = NULL;
		m_page = NULL;
        m_currentStaff = NULL;
        m_currentElement = NULL;
        DoReset();
		return;
	}
    else {
        m_doc = doc;
        m_notation_mode = m_doc->m_parameters.notationMode;
        m_npage = 0;
        m_doc->PaperSize();
        m_doc->UpdateFontValues();
        // for now we just get the first layout
        SetPage( &doc->m_layouts[0].m_pages[m_npage] );
        //CheckPoint( UNDO_ALL, MUS_UNDO_FILE ); // ax2
    }
}


void MusRC::SetPage( MusPage *page )
{
	wxASSERT_MSG( page, "MusPage cannot be NULL ");

	m_page = page;
    /*
    UpdatePageValues();
	if (m_charDefin == 0)
		UpdatePageFontValues();
    //m_pageMaxX = m_page->lrg_lign*10; // ax2 set in PaperSize
    UpdateStavesPos();
    */
    m_pageMaxY = m_doc->pageFormatVer-40; // is this a dead margin?
    m_pageMaxX = m_doc->pageFormatHor-40; // is this a dead margin?

	m_currentElement = NULL;
	m_currentStaff = NULL;

	// selectionne le premier element
    /*
	if ( m_page->m_staves.GetCount() > 1 )
	{
		m_currentStaff = &m_page->m_staves[0];
		if (m_currentStaff && m_currentStaff->GetFirst())
			m_currentElement = m_currentStaff->GetFirst();
	}
    */
    // ax2

    DoRefresh();
}

bool MusRC::HasNext( bool forward ) 
{ 
    /*
	if ( forward )
		return ( m_doc && ((int)m_doc->m_pages.GetCount() - 1 > m_npage) );
	else
		return ( m_doc && (m_npage > 0) );
    */
    //wxLogDebug( "MusRC::HasNext missing in ax2" );
    return false;
		
}

void MusRC::Next( bool forward ) 
{ 
	if ( !m_doc )
		return;

	if ( forward && this->HasNext( true ) )
		m_npage++;
	else if ( !forward && this->HasNext( false ) )
		m_npage--;

	//SetPage( &m_doc->m_pages[m_npage] );	 // ax2	
}

void MusRC::LoadPage( int nopage )
{
    /*
	if ( !m_doc )
		return;

	if ((nopage < 0) || (nopage > m_fh->nbpage - 1))
		return;

	m_npage = nopage;
	SetPage( &m_doc->m_pages[m_npage] );
    */
    wxLogDebug( "MusRC::LoadPage missing in ax2" );
}

int MusRC::ToRendererX( int i ) { return i; }; // the same

/** x value in the Logical world */
int MusRC::ToLogicalX( int i )  { return i; };

/** y value in the Renderer */
int MusRC::ToRendererY( int i )  { return m_pageMaxY - i; }; // flipped

/** y value in the Logical world  */
int MusRC::ToLogicalY( int i )  { return m_pageMaxY - i; }; // flipped

void MusRC::UpdateStavesPos() 
{
	int i, j, mPortTaille;
    MusSystem *system;
    MusLaidOutStaff *staff;

	if ( !m_page ) 
        return;
       
	int yy = m_pageMaxY; //
    for (i = 0; i < m_page->GetSystemCount(); i++) {
        system = &m_page->m_systems[i];
        for (j = 0; j < system->GetStaffCount(); j++) {
             staff = &system->m_staves[j];
             mPortTaille = staff->staffSize;
             yy -= staff->ecart * m_doc->_interl[mPortTaille];
             //staff->clefIndex.compte = 0;
             // calcul du point d'ancrage des curseurs au-dessus de la ligne superieure
             staff->yrel = yy + m_doc->_portee[mPortTaille];
             //kPos[j].yp = staff->yrel;
             // portees à 1 ou 4 lignes // LP why ??
             //if (staff->portNbLine == 1)
             //   kPos[j].yp  += _interl[mPortTaille]*2;
             //else if (staff->portNbLine == 4)
             //   kPos[j].yp  += _interl[mPortTaille];
             staff->xrel = m_doc->m_parameters.MargeGAUCHEIMPAIRE;
        }
    }
}


bool MusRC::IsNoteSelected() 
{ 
	if (!m_currentElement) 
		return false;
	else
		return m_currentElement->IsNote() || m_currentElement->IsNeume();
}

