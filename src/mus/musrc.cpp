/////////////////////////////////////////////////////////////////////////////
// Name:        musrc.cpp
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
    #pragma implementation "musrc.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "musrc.h"
#include "muspage.h"
#include "musfile.h"
#include "musiobin.h"

#include "app/axapp.h"


/*
    The following values have been obtain using FreeType2
    See ./varia/glyph_info.c
    For now, hardcoded values are ok because we do not change the music/neumes fonts
    
    If we want to enable this (changing font at runtime) we would need to
    add a method similar to glyph_info.c to get the glyph size for the note head (and others)
*/

#define LEIPZIG_UNITS_PER_EM 2048.0
#define LEIPZIG_ASCENT 1183.0
#define LEIPZIG_WHOLE_NOTE_HEAD_HEIGHT 266.0
#define LEIPZIG_WHOLE_NOTE_HEAD_WIDTH 405.0
#define LEIPZIG_HALF_NOTE_HEAD_WIDTH 314.0
#define LEIPZIG_SHARP_WIDTH 197.0


//----------------------------------------------------------------------------
// MusRC
//----------------------------------------------------------------------------

MusRC::MusRC( )
{
	m_f = NULL;
    m_fh = NULL;
    m_page = NULL;
	m_npage = 0;

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
	hautFontAscent[0][0] = 0;
	hautFontAscent[0][1] = 0;
	hautFontAscent[1][0] = 0;
	hautFontAscent[1][1] = 0;

	discontinu = 0;
	MesVal=1.0;

    m_charDefin = 0;
    
    m_pageMaxX = 0;
    m_pageMaxY = 0;

	m_currentColour = AxBLACK;
	m_currentElement = NULL;
	m_currentStaff = NULL;

	m_lyricMode = false;
	m_inputLyric = false;
	m_editorMode = MUS_EDITOR_INSERT;
	m_eraseElement = false;
	
	m_notation_mode = MUS_MENSURAL_MODE;
	
	m_str.Alloc(1000);
    
    UpdateFontValues();
}


MusRC::~MusRC()
{
}


void MusRC::PaperSize( )
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
	m_pageMaxY = pageFormatVer-40; // is this a dead margin?
    
	beamPenteMx = m_fh->param.beamPenteMax;
	beamPenteMin = m_fh->param.beamPenteMin;
	beamPenteMx /= 100;
	beamPenteMin /= 100;
    
	return;
}

void MusRC::SetFile( MusFile *file )
{
	if ( file == NULL ) // unset file
	{
		m_f = NULL;
		m_fh = NULL;
		m_page = NULL;
        m_currentStaff = NULL;
        m_currentElement = NULL;
        DoReset();
		return;
	}

	m_f = file;
	m_fh = &file->m_fheader;
    m_notation_mode = m_fh->param.notationMode;
	m_npage = 0;
	PaperSize();
	SetPage( &file->m_pages[m_npage] );
	//CheckPoint( UNDO_ALL, MUS_UNDO_FILE );
}


void MusRC::SetPage( MusPage *page )
{
	wxASSERT_MSG( page, "MusPage cannot be NULL ");

	m_page = page;
    UpdatePageValues();
	if (m_charDefin == 0)
		UpdatePageFontValues();
    m_pageMaxX = m_page->lrg_lign*10;
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

    DoRefresh();
}

bool MusRC::HasNext( bool forward ) 
{ 
	if ( forward )
		return ( m_f && ((int)m_f->m_pages.GetCount() - 1 > m_npage) );
	else
		return ( m_f && (m_npage > 0) );
		
}

void MusRC::Next( bool forward ) 
{ 
	if ( !m_f || !m_fh )
		return;

	if ( forward && this->HasNext( true ) )
		m_npage++;
	else if ( !forward && this->HasNext( false ) )
		m_npage--;

	SetPage( &m_f->m_pages[m_npage] );		
}

void MusRC::LoadPage( int nopage )
{
	if ( !m_f || !m_fh )
		return;

	if ((nopage < 0) || (nopage > m_fh->nbpage - 1))
		return;

	m_npage = nopage;
	SetPage( &m_f->m_pages[m_npage] );
}

int MusRC::CalcMusicFontSize( )
{
    // we just have the Leipzig font for now
    return round((double)m_page->defin * LEIPZIG_UNITS_PER_EM / LEIPZIG_WHOLE_NOTE_HEAD_HEIGHT);
}

int MusRC::CalcNeumesFontSize( )
{
    return 100;
}

void MusRC::UpdateFontValues() 
{	
	if ( !m_ftLeipzig.FromString( wxGetApp().m_musicFontDesc ) )
        wxLogWarning(_("Impossible to load font 'Leipzig'") );
	
	if ( !m_ftFestaDiesA.FromString( wxGetApp().m_neumeFontDesc ) )
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
	
	m_activeChantFonts[0][0] = m_ftFestaDiesA;
    m_activeChantFonts[0][1] = m_ftFestaDiesA;
    m_activeChantFonts[1][0] = m_ftFestaDiesA;
    m_activeChantFonts[1][1] = m_ftFestaDiesA;
	
	
	// Lyrics
	if ( !m_ftLyrics.FromString( wxGetApp().m_lyricFontDesc ) )
		wxLogWarning(_("Impossible to load font for the lyrics") );

	m_activeLyricFonts[0] = m_ftLyrics;
    m_activeLyricFonts[1] = m_ftLyrics;
}


void MusRC::UpdatePageFontValues() 
{
	if ( !m_page )
		return;	

	m_activeFonts[0][0].SetPointSize( ToRendererX( nTailleFont[0][0] ) ); //160
    m_activeFonts[0][1].SetPointSize( ToRendererX( nTailleFont[0][1] ) ); //120
    m_activeFonts[1][0].SetPointSize( ToRendererX( nTailleFont[1][0] ) ); //128
    m_activeFonts[1][1].SetPointSize( ToRendererX( nTailleFont[1][1] ) ); //100

	//experimental font size for now
	//they can all be the same size, seeing as the 'grace notes' are built in the font
	
	m_activeChantFonts[0][0].SetPointSize( ToRendererX( 110 ) ); // change this following the Leipzig method
    m_activeChantFonts[0][1].SetPointSize( ToRendererX( 110 ) );
    m_activeChantFonts[1][0].SetPointSize( ToRendererX( 110 ) );
    m_activeChantFonts[1][1].SetPointSize( ToRendererX( 110 ) );
	
	m_activeLyricFonts[0].SetPointSize( ToRendererX( m_ftLyrics.GetPointSize() ) );
    m_activeLyricFonts[1].SetPointSize( ToRendererX( m_ftLyrics.GetPointSize() ) );

	m_charDefin = m_page->defin;
}


void MusRC::UpdatePageValues() 
{
	if ( !m_page || !m_fh ) 
		return;

	// margins
	int page = m_page->npage;
	if ( m_fh->param.entetePied & PAGINATION )
		page += m_f->m_pagination.numeroInitial;

	if (page % 2)	//pages impaires 
		mrgG = m_fh->param.MargeGAUCHEIMPAIRE*10;
	else
		mrgG = m_fh->param.MargeGAUCHEPAIRE*10;
	mrgG = ToRendererX (mrgG);

	if (m_charDefin != m_page->defin)
		m_charDefin = 0; // the charDefin changed - reset to 0 to force UpdatePageFontValues in SetPage
    int defin = m_page->defin;

    RapportPortee[0] = m_fh->param.rapportPorteesNum;
    RapportPortee[1] = m_fh->param.rapportPorteesDen;
    RapportDimin[0] = m_fh->param.rapportDiminNum;
    RapportDimin[1] = m_fh->param.rapportDiminDen;

    // half of the space between two lines
    _espace[0] = defin/2;
    // same for small staves
    _espace[1] = (_espace[0] * RapportPortee[0]) / RapportPortee[1];
    // space between two lines
    _interl[0] = _espace[0] * 2;
    // same for small staves
    _interl[1] = _espace[1] * 2;
    // staff (with five lines)
    _portee[0] = _interl[0] * 4;
    _portee[1] = _interl[1] * 4;
    // 
    _octave[0] = _espace[0] * 7;
    _octave[1] = _espace[1] * 7;

    _pas = _espace[0];
    _bond = _pas * 6;
    _pas3 = _pas * 3;

    // values for beams
    DELTANbBAR[0] = m_fh->param.EpBarreValeur;
    DELTABLANC[0] = m_fh->param.EpBlancBarreValeur;
    DELTABAR = DELTANbBAR[0] + DELTABLANC[0];
    DELTANbBAR[1] = (DELTANbBAR[0] * RapportPortee[0]) / RapportPortee[1];
    DELTABLANC[1] = (DELTABLANC[0] * RapportPortee[0]) / RapportPortee[1];
    
    hautFont = CalcMusicFontSize();
    hautFontAscent[0][0] = floor(LEIPZIG_ASCENT * (double)hautFont / LEIPZIG_UNITS_PER_EM);
	hautFontAscent[0][0] +=  wxGetApp().m_fontPosCorrection;
	hautFontAscent[0][1] = (hautFontAscent[0][0] * RapportDimin[0]) / RapportDimin[1];
    hautFontAscent[1][0] = (hautFontAscent[0][0] * RapportPortee[0]) / RapportPortee[1];
	hautFontAscent[1][1] = (hautFontAscent[1][0] * RapportDimin[0]) / RapportDimin[1];    

    nTailleFont[0][0] = hautFont;
    nTailleFont[0][1] = (nTailleFont[0][0] * RapportDimin[0]) / RapportDimin[1];
    nTailleFont[1][0] = (nTailleFont[0][0] * RapportPortee[0]) / RapportPortee[1];
    nTailleFont[1][1]= (nTailleFont[1][0] * RapportDimin[0])/ RapportDimin[1];

    v_unit[0] = (float)_interl[0]/4;
    v4_unit[0] = (float)_interl[0]/8;
    v_unit[1] = (float)_interl[1]/4;
    v4_unit[1] = (float)_interl[1]/8;

    int glyph_size;
    glyph_size = round(LEIPZIG_HALF_NOTE_HEAD_WIDTH * (double)hautFont / LEIPZIG_UNITS_PER_EM);
    rayonNote[0][0] = glyph_size / 2;
    rayonNote[0][1] = (rayonNote[0][0] * RapportDimin[0])/RapportDimin[1];
    rayonNote[1][0] = (rayonNote[0][0] * RapportPortee[0])/RapportPortee[1];
    rayonNote[1][1] = (rayonNote[1][0] * RapportDimin[0])/RapportDimin[1];

    ledgerLine[0][0] = (int)(glyph_size * .72);
    ledgerLine[0][1] = (ledgerLine[0][0] * RapportDimin[0])/RapportDimin[1];
    ledgerLine[1][0] = (ledgerLine[0][0] * RapportPortee[0])/RapportPortee[1];
    ledgerLine[1][1] = (ledgerLine[1][0] * RapportDimin[0])/RapportDimin[1];

    glyph_size = round(LEIPZIG_WHOLE_NOTE_HEAD_WIDTH * (double)hautFont / LEIPZIG_UNITS_PER_EM);
    ledgerLine[0][2] = (int)(glyph_size * .66);
    ledgerLine[1][2] = (ledgerLine[0][2] * RapportPortee[0]) /RapportPortee[1];

    largeurBreve[0] = (int)((glyph_size * 0.8) / 2);
    largeurBreve[1] = (largeurBreve[0] * RapportPortee[0]) /RapportPortee[1];

    glyph_size = round(LEIPZIG_SHARP_WIDTH * (double)hautFont / LEIPZIG_UNITS_PER_EM);
    largAlter[0][0] = glyph_size;
    largAlter[0][1] = (largAlter[0][0] * RapportDimin[0])/RapportDimin[1];
    largAlter[1][0] = (largAlter[0][0] * RapportPortee[0]) /RapportPortee[1];
    largAlter[1][1] = (largAlter[1][0] * RapportDimin[0])/RapportDimin[1];
}

void MusRC::UpdateStavesPos() 
{
	int i,mPortTaille;
    MusStaff *staff;

	if ( !m_page || !m_fh ) 
        return;
       
	int yy = m_pageMaxY; //
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
         staff->xrel = m_fh->param.MargeGAUCHEIMPAIRE;
    }
}


bool MusRC::IsNoteSelected() 
{ 
	if (!m_currentElement) 
		return false;
	else
		return m_currentElement->IsNote() || m_currentElement->IsNeume();
}

