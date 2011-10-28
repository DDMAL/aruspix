/////////////////////////////////////////////////////////////////////////////
// Name:        musdoc.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musdoc.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "app/axapp.h"

#include "musdoc.h"
#include "musdiv.h"

#include "muslayout.h"
#include "muspage.h"
#include "mussystem.h"
#include "muslaidoutlayer.h"


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
// MusParameters
//----------------------------------------------------------------------------

MusParameters::MusParameters()
{
    orientation = true;
    EpLignesPortee = 2;
    EpQueueNote = 3;
    EpBarreMesure = 3;
    EpBarreValeur = 10;
    EpBlancBarreValeur = 5;
    beamPenteMax = 30;
    beamPenteMin = 10;
    pageFormatHor = 210;
    pageFormatVer = 297;
    MargeSOMMET = 0;
    MargeGAUCHEIMPAIRE = 10;
    MargeGAUCHEPAIRE = 10;

    // originally in MusParameters2
    rapportPorteesNum = 16;
    rapportPorteesDen = 20;
    rapportDiminNum = 3;
    rapportDiminDen = 4;
	hampesCorr = 0;
    entetePied = 0;
    
    // additional parameters
    notationMode = MUS_MENSURAL_MODE;
}

MusParameters::~MusParameters()
{
}



//----------------------------------------------------------------------------
// MusDoc
//----------------------------------------------------------------------------

MusDoc::MusDoc()
{
    Reset();
    m_meidoc = NULL;
}

MusDoc::~MusDoc()
{
}

void MusDoc::Reset()
{
    m_layouts.Clear();
    m_divs.Clear();
    // what about m_meidoc ?
    
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
    
	MesVal=1.0;

    m_charDefin = 20;
}

void MusDoc::AddDiv( MusDiv *div )
{
	div->SetDoc( this );
	m_divs.Add( div );
}


void MusDoc::AddLayout( MusLayout *layout )
{
	layout->SetDoc( this );
	m_layouts.Add( layout );
}

void MusDoc::GetNumberOfVoices( int *min_voice, int *max_voice )
{
	wxArrayPtrVoid params; // tableau de pointeurs pour parametres

    (*max_voice) = 0;
    (*min_voice) = 100000; // never more than 10000 voices ?

	params.Add( min_voice );
    params.Add( max_voice );
    MusPageFunctor countVoices( &MusPage::CountVoices );
    this->Process( &countVoices, params );
}


void MusDoc::PaperSize( )
{
	if (this->m_parameters.orientation)
	{	
		pageFormatHor = this->m_parameters.pageFormatHor*10;
		pageFormatVer = this->m_parameters.pageFormatVer*10;
	}
	else
	{	
		pageFormatVer = this->m_parameters.pageFormatHor*10;
		pageFormatHor = this->m_parameters.pageFormatVer*10;
	}
    
	beamPenteMx = this->m_parameters.beamPenteMax;
	beamPenteMin = this->m_parameters.beamPenteMin;
	beamPenteMx /= 100;
	beamPenteMin /= 100;
    
	return;
}


int MusDoc::CalcMusicFontSize( )
{
    // we just have the Leipzig font for now
    return round((double)m_charDefin * LEIPZIG_UNITS_PER_EM / LEIPZIG_WHOLE_NOTE_HEAD_HEIGHT);
}


int MusDoc::CalcNeumesFontSize( )
{
    return 100;
}

void MusDoc::UpdateFontValues() 
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
        
    
MusLaidOutStaff *MusDoc::GetVoice( int i )
{
	wxArrayPtrVoid params; // tableau de pointeurs pour parametres
    
    MusLaidOutLayerFunctor copyElements( &MusLaidOutLayer::CopyElements );
    wxArrayPtrVoid staffParams; // idem for staff functor
    MusLaidOutStaff *staff = new MusLaidOutStaff();
    staffParams.Add( staff );
    
    params.Add(&copyElements);
    params.Add(&staffParams);
    params.Add(&i);

    MusPageFunctor processVoices( &MusPage::ProcessVoices );
    this->Process( &processVoices, params );
    return staff;
}


void MusDoc::UpdatePageFontValues() 
{
	m_activeFonts[0][0].SetPointSize( nTailleFont[0][0] ); //160
    m_activeFonts[0][1].SetPointSize( nTailleFont[0][1] ); //120
    m_activeFonts[1][0].SetPointSize( nTailleFont[1][0] ); //128
    m_activeFonts[1][1].SetPointSize( nTailleFont[1][1] ); //100

	//experimental font size for now
	//they can all be the same size, seeing as the 'grace notes' are built in the font
	
	m_activeChantFonts[0][0].SetPointSize( 110 ); // change this following the Leipzig method
    m_activeChantFonts[0][1].SetPointSize( 110 );
    m_activeChantFonts[1][0].SetPointSize( 110 );
    m_activeChantFonts[1][1].SetPointSize( 110 );
	
	m_activeLyricFonts[0].SetPointSize( m_ftLyrics.GetPointSize() );
    m_activeLyricFonts[1].SetPointSize( m_ftLyrics.GetPointSize() );
}


void MusDoc::UpdatePageValues() 
{
	// margins
    /*
	int page = m_page->npage;
	if (page % 2)	//pages impaires 
		mrgG = this->m_parameters.MargeGAUCHEIMPAIRE*10;
	else
		mrgG = this->m_parameters.MargeGAUCHEPAIRE*10;
    */
	mrgG = this->m_parameters.MargeGAUCHEPAIRE*10;

    RapportPortee[0] = this->m_parameters.rapportPorteesNum;
    RapportPortee[1] = this->m_parameters.rapportPorteesDen;
    RapportDimin[0] = this->m_parameters.rapportDiminNum;
    RapportDimin[1] = this->m_parameters.rapportDiminDen;

    // half of the space between two lines
    _espace[0] = m_charDefin/2;
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
    DELTANbBAR[0] = this->m_parameters.EpBarreValeur;
    DELTABLANC[0] = this->m_parameters.EpBlancBarreValeur;
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

void MusDoc::CheckIntegrity()
{
    /*
	this->m_parameters.nbpage = (int)this->m_pages.GetCount(); // ax2

	MusPage *page;
	int i;
    for (i = 0; i < m_parameters.nbpage; i++) 
	{
		page = &m_pages[i];
		page->npage = i + 1;
		page->CheckIntegrity();
	}
    */
    wxLogDebug( "MusDoc::CheckIntegrity missing in ax2");
}

void MusDoc::Process(MusPageFunctor *functor, wxArrayPtrVoid params )
{
    /*
    MusPage *page;
	int i;
    for (i = 0; i < m_parameters.nbpage; i++) 
	{
		page = &m_pages[i];
        functor->Call( page, params );
	}
    */
    wxLogDebug( "MusDoc::Process missing in ax2");
}

MeiDocument *MusDoc::GetMeiDocument() {
    return m_meidoc;
}

void MusDoc::SetMeiDocument(MeiDocument *doc) {
    m_meidoc = doc;
}


//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

MusFileOutputStream::MusFileOutputStream( MusDoc *doc, wxString filename ) :
	wxFileOutputStream( filename )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_doc = doc;
}

MusFileOutputStream::MusFileOutputStream( MusDoc *doc, int fd ) :
	wxFileOutputStream( fd )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_doc = doc;
}

/*
MusFileOutputStream::MusFileOutputStream( MusDoc *doc, wxFile *wxfile ) :
	wxFileOutputStream( *wxfile )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_doc = doc;
}
*/

MusFileOutputStream::~MusFileOutputStream()
{
}



//----------------------------------------------------------------------------
// MusFileInputStream
//----------------------------------------------------------------------------

MusFileInputStream::MusFileInputStream( MusDoc *doc, wxString filename  ) :
	wxFileInputStream( filename )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_doc = doc;
}

MusFileInputStream::MusFileInputStream( MusDoc *doc, int fd ) :
	wxFileInputStream( fd )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_doc = doc;
}

MusFileInputStream::~MusFileInputStream()
{
}





