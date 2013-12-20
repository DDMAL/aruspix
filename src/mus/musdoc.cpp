/////////////////////////////////////////////////////////////////////////////
// Name:        musdoc.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musdoc.h"

//----------------------------------------------------------------------------

#include "mus.h"
#include "muslayer.h"
#include "muslayerelement.h"
#include "muspage.h"
#include "musstaff.h"
#include "mussystem.h"

//----------------------------------------------------------------------------

#include <math.h>

//----------------------------------------------------------------------------
// MusDoc
//----------------------------------------------------------------------------

MusDoc::MusDoc() :
    MusObject("doc-")
{
    Reset( Raw );
}

MusDoc::~MusDoc()
{
    
}

void MusDoc::Reset( DocType type )
{
    m_type = type;
    
    ClearChildren();
    
    // there is no reason why this is treated differently
    // show be in MusEnv and set in UpdatePageValues
    m_charDefin = 18;
    
    UpdatePageValues();

    ResetPaperSize();
    
    m_scoreDef.Clear();
}

void MusDoc::ResetPaperSize()
{
    m_pageWidth = 2100;
    m_pageHeight = 2970;
    m_pageRightMar = 0;
    m_pageLeftMar = 0;
    m_pageTopMar = 0; 
}

int MusDoc::Save( ArrayPtrVoid params )
{  
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    if (!output->WriteDoc( this )) {
        return FUNCTOR_STOP;
    }
    return FUNCTOR_CONTINUE;
}

void MusDoc::AddPage( MusPage *page )
{
	page->SetParent( this );
	m_children.push_back( page );
    Modify();
}

void MusDoc::Refresh()
{
    RefreshViews();
}

void MusDoc::Layout( )
{
    MusScoreDef currentScoreDef;
    currentScoreDef = m_scoreDef;
    MusStaffDef *staffDef = NULL;
    ArrayPtrVoid params;
    params.push_back( &currentScoreDef );
    params.push_back( &staffDef );
    MusFunctor setPageScoreDef( &MusObject::SetPageScoreDef );
    this->Process( &setPageScoreDef, params );
    
    int i;
	MusPage *page = NULL;
    for (i = 0; i < this->GetPageCount(); i++)
	{
		page = (MusPage*)this->m_children[i];
        PaperSize( page );
        page->Layout( );
        break;
     }
}

void MusDoc::PaperSize( MusPage *page )
{
    // we use the page members only if set (!= -1) 
    if ( page  && ( page->m_pageHeight != -1 ) ) {
        m_pageHeight = page->m_pageHeight;
        m_pageWidth = page->m_pageWidth;
        m_pageLeftMar = page->m_pageLeftMar;
        m_pageRightMar = page->m_pageRightMar;
        m_pageTopMar = page->m_pageTopMar;
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
	if ( !m_ftLeipzig.FromString( Mus::GetMusicFontDescStr() ) )
        Mus::LogWarning( "Impossible to load font 'Leipzig'" );
	
	//Mus::LogMessage( "Size %d, Family %d, Style %d, Weight %d, Underline %d, Face %s, Desc %s",
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
	if ( !m_ftLyrics.FromString( Mus::GetLyricFontDescStr() ) )
		Mus::LogWarning( "Impossible to load font for the lyrics" );
    
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
	m_fontHeightAscent[0][0] +=  Mus::GetFontPosCorrection();
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
    
    float glyph_size;
    glyph_size = (LEIPZIG_HALF_NOTE_HEAD_WIDTH * (float)m_fontHeight / LEIPZIG_UNITS_PER_EM);
    m_noteRadius[0][0] = ceil(glyph_size / 2);
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


int MusDoc::Save( MusFileOutputStream *output )
{
    ArrayPtrVoid params;
	params.push_back( output );

    MusFunctor save( &MusObject::Save );
    this->Process( &save, params );
    
    return true;
}

void MusDoc::GetNumberOfVoices( int *min_voice, int *max_voice )
{
	ArrayPtrVoid params; // tableau de pointeurs pour parametres

    (*max_voice) = 0;
    (*min_voice) = 100000; // never more than 10000 voices ?

	params.push_back( min_voice );
    params.push_back( max_voice );
    //MusPageFunctor countVoices( &MusPage::CountVoices ); // ax2.3
    //this->Process( &countVoices, params ); // ax2
}
        
    
MusStaff *MusDoc::GetVoice( int i )
{
    /*
	ArrayPtrVoid params; // tableau de pointeurs pour parametres
    
    MusLayerFunctor copyElements( &MusLayer::CopyElements );
    ArrayPtrVoid staffParams; // idem for staff functor
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
