/////////////////////////////////////////////////////////////////////////////
// Name:        musdoc.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musdoc.h"

#include "muspage.h"
#include "mussystem.h"
#include "musstaff.h"
#include "muslayer.h"
#include "muslayerelement.h"

#include "musbboxdc.h"
#include "musrc.h"

#ifndef HEADLESS
#include "app/axapp.h"
#endif

//----------------------------------------------------------------------------
// MusDoc
//----------------------------------------------------------------------------

// Initialize static respource path
wxString MusDoc::m_respath = "/usr/local/share/aruspix";

MusDoc::MusDoc() :
    MusObject()
{
    Reset( Raw );
}

MusDoc::~MusDoc()
{
    
}

void MusDoc::Reset( DocType type )
{
    m_type = type;
    
    m_children.Clear();

    ResetPaperSize();
    
	m_step1 = 10;
    m_step3 = 60;
    m_step2 = 3 * m_step1;
    m_halfInterl[0] = 10;
	m_halfInterl[1] = 8;
    m_interl[0] = 20;
    m_interl[1] = 16;
    m_staffSize[0] = 80;
    m_staffSize[1] = 64;
    m_octaveSize[0] = 70;
    m_octaveSize[1] = 56;
    m_fontHeight = 100;
    m_smallStaffRatio[0] = 16;
    m_smallStaffRatio[1] = 20;
    m_graceRatio[0] = 3;
    m_graceRatio[1] = 4;
    m_beamWidth[0] = 10;
    m_beamWidth[1] = 6;
    m_beamWhiteWidth[0] = 6;
    m_beamWhiteWidth[1] = 4;
	m_barlineSpacing = 16;
    m_fontSize[0][0] = 160;
    m_fontSize[0][1] = 120;
    m_fontSize[1][0] = 128; 
    m_fontSize[1][1] = 100;
	m_fontHeightAscent[0][0] = 0;
	m_fontHeightAscent[0][1] = 0;
	m_fontHeightAscent[1][0] = 0;
	m_fontHeightAscent[1][1] = 0;
    
    m_charDefin = 20;
}

void MusDoc::ResetPaperSize()
{
    m_pageWidth = 2100;
    m_pageHeight = 2970;
    m_pageRightMar = 0;
    m_pageLeftMar = 0;
    m_pageTopMar = 0; 
}

bool MusDoc::Save( wxArrayPtrVoid params )
{  
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    return !output->WriteDoc( this );
}

void MusDoc::AddPage( MusPage *page )
{
	page->SetParent( this );
	m_children.Add( page );
}

void MusDoc::SpaceMusic() {
    
    // Calculate bounding boxes
    MusRC rc;
    MusBBoxDC bb_dc( &rc, 0, 0 );
    rc.SetDoc(this);
    rc.DrawPage(  &bb_dc, (MusPage*)&m_children[0], false );
    
    wxArrayPtrVoid params;
    int shift = 0;
    params.Add( &shift );
    /*
    MusLayerElementFunctor updateXPosition( &MusLayerElement::UpdateXPosition );
    this->Process( &updateXPosition, params );
    */ // ax2.3
    
    params.Clear();
    shift = m_pageHeight;
    params.Add( &shift );
    /*
    MusStaffFunctor updateYPosition( &MusStaff::UpdateYPosition );
    this->Process( &updateYPosition, params );
    */// ax2.3
    
    rc.DrawPage(  &bb_dc, (MusPage*)&m_children[0] , false );
    
    // Trim the page to the needed position
    ((MusPage*)&m_children[0])->m_pageWidth = 0; // first resest the page to 0
    ((MusPage*)&m_children[0])->m_pageHeight = m_pageHeight;
    params.Clear();
    /*
    MusSystemFunctor trimSystem(&MusSystem::Trim);
    this->Process( &trimSystem, params );
    */ // ax2.3
    
    rc.DrawPage(  &bb_dc, (MusPage*)&m_children[0] , false );
}

void MusDoc::PaperSize( MusPage *page )
{
    // we use the page members only if set (!= -1) 
    if ( page  && ( page->m_pageHeight != -1 ) ) {
        m_pageHeight = page->m_pageHeight;
        m_pageWidth = page->m_pageWidth;
        m_pageLeftMar = page->m_pageLeftMar;
        m_pageRightMar = page->m_pageRightMar;
	}
	else
	{	
        ResetPaperSize();
        if (this->m_env.m_landscape)
        {	
            int pageHeight = m_pageWidth;
            m_pageWidth = m_pageHeight;
            m_pageHeight = pageHeight;
            int pageRightMar = m_pageLeftMar;
            m_pageLeftMar = this->m_pageRightMar;
            m_pageRightMar = pageRightMar;
        }
    }
    
    
	m_beamMaxSlope = this->m_env.m_beamMaxSlope;
	m_beamMinSlope = this->m_env.m_beamMinSlope;
	m_beamMaxSlope /= 100;
	m_beamMinSlope /= 100;
    
	return;
}


int MusDoc::GetSystemRightX( MusSystem *system )
{
    return m_pageWidth - m_pageLeftMar - m_pageRightMar - system->m_systemRightMar;
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
	if ( !m_ftLeipzig.FromString( MusDoc::GetMusicFontDescStr() ) )
        wxLogWarning(_("Impossible to load font 'Leipzig'") );
	
	if ( !m_ftFestaDiesA.FromString( MusDoc::GetNeumeFontDescStr() ) )
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
	if ( !m_ftLyrics.FromString( MusDoc::GetLyricFontDescStr() ) )
		wxLogWarning(_("Impossible to load font for the lyrics") );
    
	m_activeLyricFonts[0] = m_ftLyrics;
    m_activeLyricFonts[1] = m_ftLyrics;
}



void MusDoc::UpdatePageValues() 
{
    m_smallStaffRatio[0] = this->m_env.m_smallStaffNum;
    m_smallStaffRatio[1] = this->m_env.m_smallStaffDen;
    m_graceRatio[0] = this->m_env.m_graceNum;
    m_graceRatio[1] = this->m_env.m_graceDen;
    
    // half of the space between two lines
    m_halfInterl[0] = m_charDefin/2;
    // same for small staves
    m_halfInterl[1] = (m_halfInterl[0] * m_smallStaffRatio[0]) / m_smallStaffRatio[1];
    // space between two lines
    m_interl[0] = m_halfInterl[0] * 2;
    // same for small staves
    m_interl[1] = m_halfInterl[1] * 2;
    // staff (with five lines)
    m_staffSize[0] = m_interl[0] * 4;
    m_staffSize[1] = m_interl[1] * 4;
    // 
    m_octaveSize[0] = m_halfInterl[0] * 7;
    m_octaveSize[1] = m_halfInterl[1] * 7;
    
    m_step1 = m_halfInterl[0];
    m_step2 = m_step1 * 3;
    m_step3 = m_step1 * 6;
    
    // values for beams
    m_beamWidth[0] = this->m_env.m_beamWidth;
    m_beamWhiteWidth[0] = this->m_env.m_beamWhiteWidth;
    m_barlineSpacing = m_beamWidth[0] + m_beamWhiteWidth[0];
    m_beamWidth[1] = (m_beamWidth[0] * m_smallStaffRatio[0]) / m_smallStaffRatio[1];
    m_beamWhiteWidth[1] = (m_beamWhiteWidth[0] * m_smallStaffRatio[0]) / m_smallStaffRatio[1];
    
    m_fontHeight = CalcMusicFontSize();
    m_fontHeightAscent[0][0] = floor(LEIPZIG_ASCENT * (double)m_fontHeight / LEIPZIG_UNITS_PER_EM);
	m_fontHeightAscent[0][0] +=  MusDoc::GetFontPosCorrection();
	m_fontHeightAscent[0][1] = (m_fontHeightAscent[0][0] * m_graceRatio[0]) / m_graceRatio[1];
    m_fontHeightAscent[1][0] = (m_fontHeightAscent[0][0] * m_smallStaffRatio[0]) / m_smallStaffRatio[1];
	m_fontHeightAscent[1][1] = (m_fontHeightAscent[1][0] * m_graceRatio[0]) / m_graceRatio[1];    
    
    m_fontSize[0][0] = m_fontHeight;
    m_fontSize[0][1] = (m_fontSize[0][0] * m_graceRatio[0]) / m_graceRatio[1];
    m_fontSize[1][0] = (m_fontSize[0][0] * m_smallStaffRatio[0]) / m_smallStaffRatio[1];
    m_fontSize[1][1]= (m_fontSize[1][0] * m_graceRatio[0])/ m_graceRatio[1];
    
	m_activeFonts[0][0].SetPointSize( m_fontSize[0][0] ); //160
    m_activeFonts[0][1].SetPointSize( m_fontSize[0][1] ); //120
    m_activeFonts[1][0].SetPointSize( m_fontSize[1][0] ); //128
    m_activeFonts[1][1].SetPointSize( m_fontSize[1][1] ); //100
    
	//experimental font size for now
	//they can all be the same size, seeing as the 'grace notes' are built in the font
	
	m_activeChantFonts[0][0].SetPointSize( 110 ); // change this following the Leipzig method
    m_activeChantFonts[0][1].SetPointSize( 110 );
    m_activeChantFonts[1][0].SetPointSize( 110 );
    m_activeChantFonts[1][1].SetPointSize( 110 );
	
	m_activeLyricFonts[0].SetPointSize( m_ftLyrics.GetPointSize() );
    m_activeLyricFonts[1].SetPointSize( m_ftLyrics.GetPointSize() );
    
    m_verticalUnit1[0] = (float)m_interl[0]/4;
    m_verticalUnit2[0] = (float)m_interl[0]/8;
    m_verticalUnit1[1] = (float)m_interl[1]/4;
    m_verticalUnit2[1] = (float)m_interl[1]/8;
    
    int glyph_size;
    glyph_size = round(LEIPZIG_HALF_NOTE_HEAD_WIDTH * (double)m_fontHeight / LEIPZIG_UNITS_PER_EM);
    m_noteRadius[0][0] = glyph_size / 2;
    m_noteRadius[0][1] = (m_noteRadius[0][0] * m_graceRatio[0])/m_graceRatio[1];
    m_noteRadius[1][0] = (m_noteRadius[0][0] * m_smallStaffRatio[0])/m_smallStaffRatio[1];
    m_noteRadius[1][1] = (m_noteRadius[1][0] * m_graceRatio[0])/m_graceRatio[1];
    
    m_ledgerLine[0][0] = (int)(glyph_size * .72);
    m_ledgerLine[0][1] = (m_ledgerLine[0][0] * m_graceRatio[0])/m_graceRatio[1];
    m_ledgerLine[1][0] = (m_ledgerLine[0][0] * m_smallStaffRatio[0])/m_smallStaffRatio[1];
    m_ledgerLine[1][1] = (m_ledgerLine[1][0] * m_graceRatio[0])/m_graceRatio[1];
    
    glyph_size = round(LEIPZIG_WHOLE_NOTE_HEAD_WIDTH * (double)m_fontHeight / LEIPZIG_UNITS_PER_EM);
    m_ledgerLine[0][2] = (int)(glyph_size * .66);
    m_ledgerLine[1][2] = (m_ledgerLine[0][2] * m_smallStaffRatio[0]) /m_smallStaffRatio[1];
    
    m_brevisWidth[0] = (int)((glyph_size * 0.8) / 2);
    m_brevisWidth[1] = (m_brevisWidth[0] * m_smallStaffRatio[0]) /m_smallStaffRatio[1];
    
    glyph_size = round(LEIPZIG_SHARP_WIDTH * (double)m_fontHeight / LEIPZIG_UNITS_PER_EM);
    m_accidWidth[0][0] = glyph_size;
    m_accidWidth[0][1] = (m_accidWidth[0][0] * m_graceRatio[0])/m_graceRatio[1];
    m_accidWidth[1][0] = (m_accidWidth[0][0] * m_smallStaffRatio[0]) /m_smallStaffRatio[1];
    m_accidWidth[1][1] = (m_accidWidth[1][0] * m_graceRatio[0])/m_graceRatio[1];
}


bool MusDoc::Save( MusFileOutputStream *output )
{
    wxArrayPtrVoid params;
	params.Add( output );

    MusFunctor save( &MusObject::Save );
    this->Process( &save, params );
    
    return true;
}

void MusDoc::GetNumberOfVoices( int *min_voice, int *max_voice )
{
	wxArrayPtrVoid params; // tableau de pointeurs pour parametres

    (*max_voice) = 0;
    (*min_voice) = 100000; // never more than 10000 voices ?

	params.Add( min_voice );
    params.Add( max_voice );
    //MusPageFunctor countVoices( &MusPage::CountVoices ); // ax2.3
    //this->Process( &countVoices, params ); // ax2
}
        
    
MusStaff *MusDoc::GetVoice( int i )
{
    /*
	wxArrayPtrVoid params; // tableau de pointeurs pour parametres
    
    MusLayerFunctor copyElements( &MusLayer::CopyElements );
    wxArrayPtrVoid staffParams; // idem for staff functor
    MusStaff *staff = new MusStaff();
    staffParams.Add( staff );
    
    params.Add(&copyElements);
    params.Add(&staffParams);
    params.Add(&i);

    MusPageFunctor processVoices( &MusPage::ProcessVoices );
    this->Process( &processVoices, params ); 
    return staff;
    */ // ax2
    return NULL;
}


wxString MusDoc::GetAxVersion() {
#ifdef HEADLESS
    return wxString("command line"); // we need to add versioning
#else
    return AxApp::s_version;
#endif
}

wxString MusDoc::GetResourcesPath() {
#ifdef HEADLESS
    //hardcode galore
    return m_respath;
#else
    return wxGetApp().m_resourcesPath;
#endif
}    
    
wxString MusDoc::GetMusicFontDescStr() {
#ifdef HEADLESS
    return wxString("0;13;70;90;90;0;Leipzig 4.7;33");
#else
    return wxGetApp().m_musicFontDesc;
#endif
}
    
wxString MusDoc::GetNeumeFontDescStr() {
#ifdef HEADLESS
    return wxString("0;53;70;90;90;0;Festa Dies A;0");
#else
    return wxGetApp().m_neumeFontDesc;
#endif
}
        
wxString MusDoc::GetLyricFontDescStr() {
#ifdef HEADLESS
    return wxString("0;12;70;93;90;0;Garamond;0");
#else
    return wxGetApp().m_lyricFontDesc;
#endif
}


int MusDoc::GetFontPosCorrection(){
#ifdef HEADLESS
    return 0;
#else
    return wxGetApp().m_fontPosCorrection;
#endif
}

wxString MusDoc::GetFileVersion(int vmaj, int vmin, int vrev) {
    return wxString::Format("%04d.%04d.%04d", vmaj, vmin, vrev );
}
