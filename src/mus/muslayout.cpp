/////////////////////////////////////////////////////////////////////////////
// Name:        muslayout.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

//#include "app/axapp.h" // in order to get wxGetApp - to be modified

#include "muslayout.h"
#include "muspage.h"
#include "mussystem.h"
#include "muslaidoutstaff.h"
#include "muslaidoutlayer.h"
#include "muslaidoutlayerelement.h"

#include "musio.h"
#include "musdoc.h"
#include "musdiv.h"
#include "mussection.h"
#include "musmeasure.h"
#include "musstaff.h"
#include "muslayer.h"
#include "musrc.h"
#include "musbboxdc.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusLayouts );

//----------------------------------------------------------------------------
// MusLayout
//----------------------------------------------------------------------------

MusLayout::MusLayout( LayoutType type ) :
	MusLayoutObject()
{
    Clear();
    m_type = type;
    m_doc = NULL;
}

MusLayout::~MusLayout()
{
}

bool MusLayout::Check()
{
    wxASSERT( m_doc );
    return ( m_doc && MusLayoutObject::Check());
}

void MusLayout::Clear( )
{
	m_pages.Clear( );
    
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
    
	//MesVal=1.0;
    
    m_charDefin = 20;
}

void MusLayout::SetDoc( MusDoc *doc )
{
    wxArrayPtrVoid params;
	params.Add( this );
    
    MusFunctor layout( &MusObject::SetLayout );
    this->Process( &layout, params ); 
    
    m_layout = this; // just for integrity
    m_doc = doc;
    m_env = doc->m_env;
}

void MusLayout::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WriteLayout( this );

    // save pages
    MusPageFunctor page( &MusPage::Save );
    this->Process( &page, params );
}

void MusLayout::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileInputStream *input = (MusFileInputStream*)params[0];       
    
    // load pages
    MusPage *page;
    while ( (page = input->ReadPage()) ) {
        page->Load( params );
        this->AddPage( page );
    }
}


void MusLayout::AddPage( MusPage *page )
{
	//page->SetLayout( this );
	m_pages.Add( page );
}

void MusLayout::Realize( MusScore *score )
{
	Clear();
	
	wxASSERT( score );
	
	MusPage *page = new MusPage( );
	MusSystem *system = new MusSystem();

    int x = 0; // hardcoded spacing
	
	int i, j, k, l, m;
	for (i = 0; i < (int)score->m_sections.GetCount(); i++) {
		MusSection *section = &score->m_sections[i];
        // measured music
		for (j = 0; j < (int)section->m_measures.GetCount(); j++) {
			MusMeasure *measure = &section->m_measures[j];
            for (k = 0; k < (int)measure->m_staves.GetCount(); k++) {
                MusStaff *staff = &measure->m_staves[k];
                MusLaidOutStaff *laidOutStaff;
                if (k >= (int)system->m_staves.GetCount()) {
                    system->AddStaff( new MusLaidOutStaff( k ));
                }
                laidOutStaff = &system->m_staves[k];
                for (l = 0; l < (int)staff->m_layers.GetCount(); l++) {
                    MusLayer *layer = &staff->m_layers[l];
                    MusLaidOutLayer *laidOutLayer;
                    if (l >= laidOutStaff->GetLayerCount()) {
                        laidOutStaff->AddLayer( new MusLaidOutLayer( l, k, section, measure ));
                    }
                    laidOutLayer = &laidOutStaff->m_layers[l];
                    for (m = 0; m < (int)layer->m_elements.GetCount(); m++) {
                        MusLaidOutLayerElement *element = new MusLaidOutLayerElement( &layer->m_elements[m] );
                        element->m_x_abs = x;
                        x += 40;
                        laidOutLayer->AddElement( element );
                        //wxLogDebug("element %d added", m);
                    }			
				}
			}
		}
        // unmeasured music
        for (k = 0; k < (int)section->m_staves.GetCount(); k++) {
            MusStaff *staff = &section->m_staves[k];
            MusLaidOutStaff *laidOutStaff;
            if (k >= (int)system->m_staves.GetCount()) {
                system->AddStaff( new MusLaidOutStaff( k ));
            }
            laidOutStaff = &system->m_staves[k];
            for (l = 0; l < (int)staff->m_layers.GetCount(); l++) {
                MusLayer *layer = &staff->m_layers[l];
                MusLaidOutLayer *laidOutLayer;
                if (l >= laidOutStaff->GetLayerCount()) {
                    laidOutStaff->AddLayer( new MusLaidOutLayer( l, k, section, NULL ));
                }
                laidOutLayer = &laidOutStaff->m_layers[l];
                for (m = 0; m < (int)layer->m_elements.GetCount(); m++) {
                    MusLaidOutLayerElement *element = new MusLaidOutLayerElement( &layer->m_elements[m] );
                    element->m_x_abs = x;
                    x += 40;
                    laidOutLayer->AddElement( element );
                    //wxLogDebug("element %d added", m);
                }			
			}
		}
    }

	page->AddSystem( system );
	this->AddPage( page );
}

void MusLayout::SpaceMusic() {
    
    // Calculate bounding boxes
    MusRC rc;
    MusBBoxDC bb_dc( &rc, 0, 0 );
    rc.SetLayout(this);
    rc.DrawPage(  &bb_dc, &m_pages[0], false );
    
    wxArrayPtrVoid params;
    int shift = 0;
    params.Add( &shift );
    MusLaidOutLayerElementFunctor updateXPosition( &MusLaidOutLayerElement::UpdateXPosition );
    m_doc->ProcessLayout( &updateXPosition, params );
    
    params.Clear();
    shift = m_pageHeight;
    params.Add( &shift );
    MusLaidOutStaffFunctor updateYPosition( &MusLaidOutStaff::UpdateYPosition );
    m_doc->ProcessLayout( &updateYPosition, params );
    
    rc.DrawPage(  &bb_dc, &m_pages[0] , false );
    
    // Trim the system to the needed position
    params.Clear();
    MusSystemFunctor trimSystem(&MusSystem::Trim);
    m_doc->ProcessLayout( &trimSystem, params );
    
    rc.DrawPage(  &bb_dc, &m_pages[0] , false );
}

void MusLayout::PaperSize( )
{
	if (this->m_env.m_landscape)
	{	
		m_pageWidth = this->m_env.m_paperWidth*10;
		m_pageHeight = this->m_env.m_paperHeight*10;
	}
	else
	{	
		m_pageHeight = this->m_env.m_paperWidth*10;
		m_pageWidth = this->m_env.m_paperHeight*10;
	}
    
	m_beamMaxSlope = this->m_env.m_beamMaxSlope;
	m_beamMinSlope = this->m_env.m_beamMinSlope;
	m_beamMaxSlope /= 100;
	m_beamMinSlope /= 100;
    
	return;
}


int MusLayout::CalcMusicFontSize( )
{
    // we just have the Leipzig font for now
    return round((double)m_charDefin * LEIPZIG_UNITS_PER_EM / LEIPZIG_WHOLE_NOTE_HEAD_HEIGHT);
}

int MusLayout::CalcNeumesFontSize( )
{
    return 100;
}

void MusLayout::UpdateFontValues() 
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



void MusLayout::UpdatePageValues() 
{
	// margins
    /*
     int page = m_page->npage;
     if (page % 2)	//pages impaires 
     m_leftMargin = this->m_env.m_leftMarginOddPage*10;
     else
     m_leftMargin = this->m_env.m_leftMarginEvenPage*10;
     */
	m_leftMargin = this->m_env.m_leftMarginEvenPage*10;
    
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

// functors for MusLayout

void MusLayout::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusPageFunctor *pageFunctor = dynamic_cast<MusPageFunctor*>(functor);
    MusPage *page;
	int i;
    for (i = 0; i < (int)m_pages.GetCount(); i++) 
	{
        page = &m_pages[i];
        functor->Call( page, params );
        if (pageFunctor) { // is is a MusSystemFunctor, call it
            pageFunctor->Call( page, params );
        }
        else { // process it further
            page->Process( functor, params );
        }
	}
}

