/////////////////////////////////////////////////////////////////////////////
// Name:        musiobin.cpp
// Author:      Laurent Pugin
// Created:     2008
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musiobin.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"

#include "musiobin.h"

#include "muspage.h"
#include "mussystem.h"
#include "muslaidoutstaff.h"
#include "muslaidoutlayer.h"
#include "muslaidoutlayerelement.h"
#include "musbarline.h"
#include "musclef.h"
#include "musmensur.h"
#include "musnote.h"
#include "mussymbol.h"
#include "musrest.h"
#include "musneume.h"
#include "musneumesymbol.h"

#include "musstaff.h"
#include "muslayer.h"


//#include "app/axapp.h"
//#include "app/axfile.h"

//----------------------------------------------------------------------------
// MusBinOutput
//----------------------------------------------------------------------------

#ifdef PROUT

MusBinOutput::MusBinOutput( MusDoc *doc, wxString filename ) :
    MusFileOutputStream( doc, filename )
{
	m_filename = filename;
}

MusBinOutput::~MusBinOutput()
{
}

bool MusBinOutput::ExportFile( )
{    
	if ( !IsOk() )
	{
		wxLogMessage(_("Cannot write file '%s'"), m_filename.c_str() );
		return false;
	}
    
    m_doc->Save( this );
	return true;
}

void MusBinOutput::WriteObject( MusObject *object )
{
    //uuid_string_t uuidStr;
    //uuid_unparse( *object->GetUuid(), uuidStr );
    //wxLogDebug("Writing %s %s", object->MusClassName().c_str(), uuidStr );	
    //
    Write( *object->GetUuid(), 16 );
	int32 = wxINT32_SWAP_ON_BE( object->MusClassName().Length() );
	Write( &int32, 4 );
    Write( object->MusClassName().c_str(), object->MusClassName().Length() + 1 );
}

bool MusBinOutput::WriteDoc( MusDoc *doc )
{
    Write( &doc->m_env.m_landscape, 1);
    Write( &doc->m_env.m_staffLineWidth, 1);
    Write( &doc->m_env.m_stemWidth, 1);
    Write( &doc->m_env.m_barlineWidth, 1);
    Write( &doc->m_env.m_beamWidth, 1);
    Write( &doc->m_env.m_beamWhiteWidth, 1);
    Write( &doc->m_env.m_beamMaxSlope, 1);
    Write( &doc->m_env.m_beamMinSlope, 1);
    int32 = wxINT32_SWAP_ON_BE( doc->m_pageWidth );
    Write( &int32, 4);
    int32 = wxINT32_SWAP_ON_BE( doc->m_pageHeight );
    Write( &int32, 4);
    int16 = wxINT16_SWAP_ON_BE( doc->m_pageTopMar );
    Write( &int16, 2);
    int16 = wxINT16_SWAP_ON_BE( doc->m_env.m_leftMarginOddPage );
    Write( &int16, 2);
    int16 = wxINT16_SWAP_ON_BE( doc->m_env.m_leftMarginEvenPage );
    Write( &int16, 2);     
    Write( &doc->m_env.m_smallStaffNum, 1);
    Write( &doc->m_env.m_smallStaffDen, 1);
    Write( &doc->m_env.m_graceNum, 1);
    Write( &doc->m_env.m_graceDen, 1);
    Write( &doc->m_env.m_stemCorrection, 1);
    uint32 = wxINT32_SWAP_ON_BE( doc->m_env.m_headerType );
    Write( &uint32, 4);
    int32 = wxINT32_SWAP_ON_BE( doc->m_env.m_notationMode );
    Write( &int32, 4); 
    // also write the number of divs and layouts
    int32 = wxINT32_SWAP_ON_BE( (int)doc->m_divs.GetCount() );
    Write( &int32, 4 );
    int32 = wxINT32_SWAP_ON_BE( (int)doc->m_layouts.GetCount() );
    Write( &int32, 4 );
    
    return true;
}

bool MusBinOutput::WriteDiv( MusDiv *div )
{
    WriteObject( div );
    // also write that we have one score or one partset
    int32 = wxINT32_SWAP_ON_BE( (div->m_score) ? 1 : 0 );
    Write( &int32, 4 );
    int32 = wxINT32_SWAP_ON_BE( (div->m_partSet) ? 1 : 0 );
    Write( &int32, 4 );
    
    return true;    
}

bool MusBinOutput::WriteScore( MusScore *score )
{
    WriteObject( score );
    // also write the number of sections
    int32 = wxINT32_SWAP_ON_BE( (int)score->m_sections.GetCount() );
    Write( &int32, 4 );    
    
    return true;    
}

bool MusBinOutput::WritePartSet( MusPartSet *parts )
{
    WriteObject( parts );
    // also write the number of parts
    int32 = wxINT32_SWAP_ON_BE( (int)parts->m_parts.GetCount() );
    Write( &int32, 4 );
    
    return true;    
}

bool MusBinOutput::WritePart( MusPart *part )
{
    WriteObject( part  );
    // also write the number of sections
    int32 = wxINT32_SWAP_ON_BE( (int)part->m_sections.GetCount() );
    Write( &int32, 4 );
    
    return true;    
}

bool MusBinOutput::WriteSection( MusSection *section )
{
    WriteObject( section );
    // also write the number of measures or staves
    int measures = (int)section->m_measures.GetCount();
    Write( &measures, 4 );
    int staves = (int)section->m_staves.GetCount();
    Write( &staves, 4 ); 
    
    return true;    
}

bool MusBinOutput::WriteMeasure( MusMeasure *measure )
{
    WriteObject( measure );
    // also write the number of staves
    int32 = wxINT32_SWAP_ON_BE( (int)measure->m_staves.GetCount() );
    Write( &int32, 4 ); 
    
    return true;    
}

bool MusBinOutput::WriteStaff( MusStaff *staff )
{
    WriteObject( staff );
    // also write the number of layers
    int32 = wxINT32_SWAP_ON_BE( (int)staff->m_layers.GetCount() );
    Write( &int32, 4 );    
    
    return true;    
}

bool MusBinOutput::WriteLayer( MusLayer *layer )
{
    WriteObject( layer );
    // also write the number of elements
    int32 = wxINT32_SWAP_ON_BE( (int)layer->m_elements.GetCount() );
    Write( &int32, 4 );
    
    return true;    
}

bool MusBinOutput::WriteLayerElement( MusLayerElement *element )
{
    WriteObject( element );
    
    if ( element->MusClassName() == "MusBarline") {
        MusBarline *barline = dynamic_cast<MusBarline*>(element);
        wxASSERT( barline );
        uc = barline->m_barlineType;
        Write( &uc, 1 );
        Write( &barline->m_partialBarline, 1);
        Write( &barline->m_onStaffOnly, 1 );
    }
    else if ( element->MusClassName() == "MusClef") {
        MusClef *clef = dynamic_cast<MusClef*>(element);
        wxASSERT( clef );
        uc = clef->m_clefId;
        Write( &uc, 1 );
    }
    else if ( element->MusClassName() == "MusMensur") {
        MusMensur *mensur = dynamic_cast<MusMensur*>(element);
        wxASSERT( mensur );
        Write( &mensur->m_dot, 1 );
        uc = mensur->m_meterSymb;
        Write( &uc, 1 );
        int32 = wxINT32_SWAP_ON_BE( mensur->m_num );
        Write( &int32, 4 );
        int32 = wxINT32_SWAP_ON_BE( mensur->m_numBase );
        Write( &int32, 4 );
        Write( &mensur->m_reversed, 1 );
        uc = mensur->m_sign;
        Write( &uc, 1 );   
        Write( &mensur->m_slash, 1);

    }
    else if ( element->MusClassName() == "MusNeume") {
        wxLogWarning( "Neumes are not saved in binary files" );
    }
    else if ( element->MusClassName() == "MusNeumeSymbol") {
        wxLogWarning( "NeumeSymbols are not saved in binary files" );
    }
    else if ( element->MusClassName() == "MusNote") {
        WriteDurationInterface( dynamic_cast<MusDurationInterface*>(element) );
        WritePitchInterface( dynamic_cast<MusPitchInterface*>(element) );
        MusNote *note = dynamic_cast<MusNote*>(element);
        wxASSERT( note );        
        Write( &note->m_artic, 1 );
        Write( &note->m_chord, 1 );
        Write( &note->m_colored, 1 );
        Write( &note->m_lig, 1 );
        Write( &note->m_headshape, 1 );
        Write( &note->m_ligObliqua, 1 );
        Write( note->m_slur, 6 );
        Write( &note->m_stemDir, 1 );
        Write( &note->m_stemLen, 1 );
    }
    else if ( element->MusClassName() == "MusRest") {
        WriteDurationInterface( dynamic_cast<MusDurationInterface*>(element) );
        WritePositionInterface( dynamic_cast<MusPositionInterface*>(element) );
    }
    else if ( element->MusClassName() == "MusSymbol") {
        WritePositionInterface( dynamic_cast<MusPositionInterface*>(element) );
        MusSymbol *symbol = dynamic_cast<MusSymbol*>(element);
        wxASSERT( symbol );  
        uc = symbol->m_type;
        Write( &uc, 1 );
        Write( &symbol->m_dot, 1 );
        Write( &symbol->m_accid, 1 );
    } 
    
    Write( &element->m_cueSize, 1);
    int32 = wxINT32_SWAP_ON_BE( element->m_hOffset );
    Write( &int32, 4 );
    Write( &element->m_staffShift, 1 );
    Write( &element->m_visible, 1 );
    
    return true;    
}

void MusBinOutput::WriteDurationInterface( MusDurationInterface *element )
{
    wxASSERT( element );
    Write( element->m_beam, 6 );
    Write( &element->m_breakSec, 1 );
    Write( &element->m_dots, 1);
    int32 = wxINT32_SWAP_ON_BE( element->m_dur );
    Write( &int32, 4 );
    int32 = wxINT32_SWAP_ON_BE( element->m_num );
    Write( &int32, 4 );
    int32 = wxINT32_SWAP_ON_BE( element->m_numBase );
    Write( &int32, 4 );
    Write( element->m_tuplet, 6 );   
}

void MusBinOutput::WritePitchInterface( MusPitchInterface *element )
{
    wxASSERT( element );
    Write( &element->m_oct, 1 );
    Write( &element->m_pname, 1 );    
}

void MusBinOutput::WritePositionInterface( MusPositionInterface *element )
{
    wxASSERT( element );
    Write( &element->m_oct, 1 );
    Write( &element->m_pname, 1 );   
}

bool MusBinOutput::WriteLayout( MusLayout *layout )
{
    WriteObject( layout );
    // also write the number of pages
    int32 = wxINT32_SWAP_ON_BE( (int)layout->m_pages.GetCount());
    Write( &int32, 4 );    
    
    return true;
}

bool MusBinOutput::WritePage( MusPage *page )
{
    WriteObject( page );
    Write( &page->defin, 1 );
    // also write the number of systems
    int32 = wxINT32_SWAP_ON_BE( (int)page->m_systems.GetCount());
    Write( &int32, 4 );
    
    return true;
}

bool MusBinOutput::WriteSystem( MusSystem *system )
{
    WriteObject( system );
	int32 = wxINT32_SWAP_ON_BE( system->indent );	
	Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_BE( system->indentDroite ); 
	Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_BE( system->lrg_lign );
    Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_BE( system->m_x_abs );
    Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_BE( system->m_y_abs );
    Write( &int32, 4 );
    // also write the number of staves
    int32 = wxINT32_SWAP_ON_BE( (int)system->m_staves.GetCount());
    Write( &int32, 4 );    
    
    return true;
}

bool MusBinOutput::WriteLaidOutStaff( MusLaidOutStaff *laidOutStaff )
{
    WriteObject( laidOutStaff );
	uint16 = wxUINT16_SWAP_ON_BE( laidOutStaff->noGrp );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( laidOutStaff->totGrp );
	Write( &uint16, 2 );
	Write( &laidOutStaff->armTyp, 1 );
	Write( &laidOutStaff->armNbr, 1 );
	Write( &laidOutStaff->notAnc, 1 );
	Write( &laidOutStaff->grise, 1 );
	Write( &laidOutStaff->invisible, 1 );
	Write( &laidOutStaff->vertBarre, 1 );
	Write( &laidOutStaff->brace, 1 );
	Write( &laidOutStaff->staffSize, 1 );
	Write( &laidOutStaff->portNbLine, 1 );
	Write( &laidOutStaff->accol, 1 );
	Write( &laidOutStaff->accessoire, 1 );
	int32 = wxINT32_SWAP_ON_BE( laidOutStaff->m_y_abs );
    Write( &int32, 4 );
    // also write the number of layers
    int32 = wxINT32_SWAP_ON_BE( (int)laidOutStaff->m_layers.GetCount());
    Write( &int32, 4 );    
    
    return true;
}

bool MusBinOutput::WriteLaidOutLayer( MusLaidOutLayer *laidOutLayer )
{
    int32 = wxINT32_SWAP_ON_BE( laidOutLayer->m_logLayerNb );   
    Write( &int32, 4 );
    int32 = wxINT32_SWAP_ON_BE( laidOutLayer->m_logStaffNb );  
    Write( &int32, 4 );
    
    Write( laidOutLayer->m_section->GetUuid(), 16 );
    // for the measure, we write a NULL uuid for un-measured music
    if ( laidOutLayer->m_measure ) { // measured music
        Write( laidOutLayer->m_measure->GetUuid(), 16 );    
    }
    else {
        uuid_t uuid;
        uuid_clear(uuid);
        Write( &uuid, 16 );         
    }
    
    WriteObject( laidOutLayer );
	uint16 = wxUINT16_SWAP_ON_BE( laidOutLayer->voix ); 
    Write( &uint16, 2 );
    // also write the number of elements
    int32 = wxINT32_SWAP_ON_BE( (int)laidOutLayer->m_elements.GetCount());
    Write( &int32, 4 );    
    
    return true;
}

bool MusBinOutput::WriteLaidOutLayerElement( MusLaidOutLayerElement *laidOutLayerElement )
{
    // first write the uuid of the layerElement
    Write( laidOutLayerElement->m_layerElement->GetUuid(), 16 );
    
    WriteObject( laidOutLayerElement );
	int32 = wxINT32_SWAP_ON_BE( laidOutLayerElement->m_x_abs );
    Write( &int32, 4 );
    return true;
}



//----------------------------------------------------------------------------
// MusBinInput
//----------------------------------------------------------------------------

MusBinInput::MusBinInput( MusDoc *doc, wxString filename ) :
    MusFileInputStream( doc, filename )
{
	m_filename = filename;
    m_nbDivs = 0;
    m_nbScores = 0;
    m_nbPartSets = 0;
    m_nbParts = 0;
    m_nbSections = 0;
    m_nbMeasures = 0;
    m_nbStaves = 0;
    m_nbLayers = 0;
    m_nbElements = 0;
    m_nbLayouts = 0;
    m_nbPages = 0;
    m_nbSystems = 0;
    m_nbLaidOutStaves = 0;
    m_nbLaidOutLayers = 0;
    m_nbLaidOutLayerElements = 0;
}

MusBinInput::~MusBinInput()
{
}

bool MusBinInput::ImportFile( )
{
	if ( !IsOk() )
	{
		wxLogMessage(_("Cannot read file '%s'"), m_filename.c_str() );
		return false;
	}
    m_doc->Load( this );
    
	return true;
}

void MusBinInput::ReadObject( MusObject *object )
{  
    Read( &m_uuid, 16 );
	Read( &int32, 4 );
	Read( m_className.GetWriteBuf( wxINT32_SWAP_ON_BE( int32 ) + 1 ) , wxINT32_SWAP_ON_BE( int32 ) + 1 );
	m_className.UngetWriteBuf();  
    
    if ( object ) {
        object->SetUuid( m_uuid );
    }
    
    //uuid_string_t uuidStr;
    //uuid_unparse( m_uuid, uuidStr );
    //wxLogDebug("Reading %s %s", m_className.c_str(), uuidStr ); 
    
}

bool MusBinInput::ReadDoc( MusDoc *doc )
{
    Read( &doc->m_env.m_landscape, 1);
    Read( &doc->m_env.m_staffLineWidth, 1);
    Read( &doc->m_env.m_stemWidth, 1);
    Read( &doc->m_env.m_barlineWidth, 1);
    Read( &doc->m_env.m_beamWidth, 1);
    Read( &doc->m_env.m_beamWhiteWidth, 1);
    Read( &doc->m_env.m_beamMaxSlope, 1);
    Read( &doc->m_env.m_beamMinSlope, 1);
    Read( &int32, 4 );
    doc->m_pageWidth = wxINT32_SWAP_ON_BE( int32 );
    Read( &int32, 4 );
    doc->m_pageHeight = wxINT32_SWAP_ON_BE( int32 );
    Read( &int16, 2 );
    doc->m_env.m_topMargin = wxINT32_SWAP_ON_BE( int16 );
    Read( &int16, 2 );
    doc->m_env.m_leftMarginOddPage = wxINT16_SWAP_ON_BE( int16 );
    Read( &int16, 2 );
    doc->m_env.m_leftMarginEvenPage = wxINT16_SWAP_ON_BE( int16 );       
    Read( &doc->m_env.m_smallStaffNum, 1);
    Read( &doc->m_env.m_smallStaffDen, 1);
    Read( &doc->m_env.m_graceNum, 1);
    Read( &doc->m_env.m_graceDen, 1);
    Read( &doc->m_env.m_stemCorrection, 1);
    Read( &uint32, 4 );
    doc->m_env.m_headerType = wxINT32_SWAP_ON_BE( uint32 );
    Read( &int32, 4 );
    doc->m_env.m_notationMode = wxINT32_SWAP_ON_BE( int32 );
    // also read the number of divs and layouts
    Read( &int32, 4 );
    m_nbDivs = wxINT32_SWAP_ON_BE( int32 );
    Read( &int32, 4 );
    m_nbLayouts= wxINT32_SWAP_ON_BE( int32 );
    
    return true;
}

MusDiv* MusBinInput::ReadDiv( )
{
    if ( !m_nbDivs ) {
        return NULL;
    }
    m_nbDivs--;     
    
    MusDiv *div = new MusDiv();
    ReadObject( div );
    // also read the number of scores and partSets
    Read( &int32, 4 );
    m_nbScores = wxINT32_SWAP_ON_BE( int32 );
    Read( &int32, 4 );
    m_nbPartSets= wxINT32_SWAP_ON_BE( int32 ); 
    
    return div;    
}

MusScore* MusBinInput::ReadScore( )
{
    if ( !m_nbScores ) {
        return NULL;
    }
    m_nbScores--; 
    
    MusScore *score = new MusScore();
    ReadObject( score );
    // also read the number of sections
    Read( &int32, 4 );
    m_nbSections = wxINT32_SWAP_ON_BE( int32 );   
    
    return score;    
}

MusPartSet* MusBinInput::ReadPartSet( )
{
    if ( !m_nbPartSets ) {
        return NULL;
    }
    m_nbPartSets--; 
    
    MusPartSet *parts = new MusPartSet();
    ReadObject( parts );
    // also read the number of parts
    Read( &int32, 4 );
    m_nbParts = wxINT32_SWAP_ON_BE( int32 );    
    
    return parts;    
}

MusPart* MusBinInput::ReadPart( )
{
    if ( !m_nbParts ) {
        return NULL;
    }
    m_nbParts--;     
    
    MusPart *part = new MusPart();
    ReadObject( part  );
    // also read the number of sections
    Read( &int32, 4 );
    m_nbSections = wxINT32_SWAP_ON_BE( int32 );     
    
    return part;    
}

MusSection* MusBinInput::ReadSection( )
{
    if ( !m_nbSections ) {
        return NULL;
    }
    m_nbSections--;   
    
    MusSection *section = new MusSection();
    ReadObject( section );
    // also read the number of measures and staves
    Read( &int32, 4 );
    m_nbMeasures = wxINT32_SWAP_ON_BE( int32 );  
    Read( &int32, 4 );
    m_nbStaves = wxINT32_SWAP_ON_BE( int32 );    
    
    return section;    
}

MusMeasure* MusBinInput::ReadMeasure( )
{
    if ( !m_nbMeasures ) {
        return NULL;
    }
    m_nbMeasures--;  
    
    MusMeasure *measure = new MusMeasure();
    ReadObject( measure );
    // also read the number of staves
    Read( &int32, 4 );
    m_nbStaves = wxINT32_SWAP_ON_BE( int32 );     
    
    return measure;    
}

MusStaff* MusBinInput::ReadStaff( )
{
    if ( !m_nbStaves ) {
        return NULL;
    }
    m_nbStaves--;  
    
    MusStaff *staff = new MusStaff();
    ReadObject( staff );
    // also read the number of layers
    Read( &int32, 4 );
    m_nbLayers = wxINT32_SWAP_ON_BE( int32 );     
    
    return staff;    
}

MusLayer* MusBinInput::ReadLayer( )
{
    if ( !m_nbLayers ) {
        return NULL;
    }
    m_nbLayers--;   
    
    MusLayer *layer = new MusLayer();
    ReadObject( layer );
    // also read the number of elements
    Read( &int32, 4 );
    m_nbElements = wxINT32_SWAP_ON_BE( int32 );    
    
    return layer;    
}

MusLayerElement *MusBinInput::ReadLayerElement( )
{
    if ( !m_nbElements ) {
        return NULL;
    }
    m_nbElements--;   
    
    MusLayerElement *element = NULL;
    ReadObject( );
    
    if ( m_className == "MusBarline") {
        MusBarline *barline = new MusBarline();
        Read( &uc, 1 );
        barline->m_barlineType = (BarlineType)uc;
        Read( &barline->m_partialBarline, 1);
        Read( &barline->m_onStaffOnly, 1 );
        element = barline;
    }
    else if ( m_className == "MusClef") {
        MusClef *clef = new MusClef();
        Read( &uc, 1 );
        clef->m_clefId = (ClefId)uc;
        element = clef;
    }
    else if ( m_className == "MusMensur") {
        MusMensur *mensur = new MusMensur();
        Read( &mensur->m_dot, 1 );
        Read( &uc, 1 );
        mensur->m_meterSymb = (MeterSymb)uc;
        Read( &int32, 4 );
        mensur->m_num  = wxINT32_SWAP_ON_BE( int32 );
        Read( &int32, 4 );
        mensur->m_numBase  = wxINT32_SWAP_ON_BE( int32 );
        Read( &mensur->m_reversed, 1 );
        Read( &uc, 1 );
        mensur->m_sign = (MensurSign)uc;  
        Read( &mensur->m_slash, 1);
        element = mensur;
    }
    else if ( m_className == "MusNeume") {
        wxLogWarning( "Neumes are not saved in binary files" );
        MusNeume *neume = new MusNeume();
        element = neume;
    }
    else if ( m_className == "MusNeumeSymbol") {
        wxLogWarning( "NeumeSymbols are not saved in binary files" );
        MusNeumeSymbol *neumeSymbol = new MusNeumeSymbol();
        element = neumeSymbol;
    }
    else if ( m_className == "MusNote") {
        MusNote *note = new MusNote();
        ReadDurationInterface( note );
        ReadPitchInterface( note );
        Read( &note->m_artic, 1 );
        Read( &note->m_chord, 1 );
        Read( &note->m_colored, 1 );
        Read( &note->m_lig, 1 );
        Read( &note->m_headshape, 1 );
        Read( &note->m_ligObliqua, 1 );
        Read( note->m_slur, 6 );
        Read( &note->m_stemDir, 1 );
        Read( &note->m_stemLen, 1 );
        element = note;
    }
    else if ( m_className == "MusRest") {
        MusRest *rest = new MusRest();
        ReadDurationInterface( rest );
        ReadPositionInterface( rest );
        element = rest;
    }
    else if ( m_className == "MusSymbol") {
        MusSymbol *symbol = new MusSymbol();
        ReadPositionInterface( symbol );
        Read( &uc, 1 );
        symbol->m_type = (SymbolType)uc;
        Read( &symbol->m_dot, 1 );
        Read( &symbol->m_accid, 1 );
        element = symbol;
    }
    // we have it, set the uuid we read
    if ( element ) {
        Read( &element->m_cueSize, 1);
        Read( &int32, 4 );
        element->m_hOffset = wxINT32_SWAP_ON_BE( int32 );
        Read( &element->m_staffShift, 1 );
        Read( &element->m_visible, 1 );
        element->SetUuid( m_uuid );
    }
    else {
        wxLogWarning( "Element class %s not found", m_className.c_str() );
    }
    return element;    
}

void MusBinInput::ReadDurationInterface( MusDurationInterface *element )
{
    Read( element->m_beam, 6 );
    Read( &element->m_breakSec, 1 );
    Read( &element->m_dots, 1);
    Read( &int32, 4 );
    element->m_dur = wxINT16_SWAP_ON_BE( int32 );
    Read( &int32, 4 );
    element->m_num = wxINT16_SWAP_ON_BE( int32 );
    Read( &int32, 4 );
    element->m_numBase = wxINT16_SWAP_ON_BE( int32 );
    Read( element->m_tuplet, 6 );    
}

void MusBinInput::ReadPitchInterface( MusPitchInterface *element )
{
    Read( &element->m_oct, 1 );
    Read( &element->m_pname, 1 );    
}

void MusBinInput::ReadPositionInterface( MusPositionInterface *element )
{
    Read( &element->m_oct, 1 );
    Read( &element->m_pname, 1 );        
}

MusLayout* MusBinInput::ReadLayout( )
{
    if ( !m_nbLayouts ) {
        return NULL;
    }
    m_nbLayouts--;   
    
    MusLayout *layout = new MusLayout( Raw );
    ReadObject( layout );
    // also read the number of pages
    Read( &int32, 4 );
    m_nbPages = wxINT32_SWAP_ON_BE( int32 );     
    
    return layout;
}

MusPage* MusBinInput::ReadPage( )
{
    if ( !m_nbPages ) {
        return NULL;
    }
    m_nbPages--;   
    
    MusPage *page = new MusPage();
    ReadObject( page );
    Read( &page->defin, 1 );
    // also read the number of systems
    Read( &int32, 4 );
    m_nbSystems = wxINT32_SWAP_ON_BE( int32 );   
    
    return page;
}

MusSystem* MusBinInput::ReadSystem( )
{
    if ( !m_nbSystems ) {
        return NULL;
    }
    m_nbSystems--;     
    
    MusSystem *system = new MusSystem();
    ReadObject( system );
    Read( &int32, 4 );
	system->indent = wxINT32_SWAP_ON_BE( int32 );	
	Read( &int32, 4 );
	system->indentDroite = wxINT32_SWAP_ON_BE( int32 ); 
	Read( &int32, 4 );
	system->lrg_lign = wxINT32_SWAP_ON_BE( int32 );
	Read( &int32, 4 );
	system->m_x_abs = wxINT32_SWAP_ON_BE( int32 );
	Read( &int32, 4 );
	system->m_y_abs = wxINT32_SWAP_ON_BE( int32 );
    // also read the number of staves
    Read( &int32, 4 );
    m_nbLaidOutStaves = wxINT32_SWAP_ON_BE( int32 );    
    
    return system;
}

MusLaidOutStaff* MusBinInput::ReadLaidOutStaff( )
{
    if ( !m_nbLaidOutStaves ) {
        return NULL;
    }
    m_nbLaidOutStaves--;   
    
    MusLaidOutStaff *laidOutStaff = new MusLaidOutStaff();
    ReadObject( laidOutStaff );
	Read( &uint16, 2 );
	laidOutStaff->noGrp = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	laidOutStaff->totGrp = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &laidOutStaff->armTyp, 1 );
	Read( &laidOutStaff->armNbr, 1 );
	Read( &laidOutStaff->notAnc, 1 );
	Read( &laidOutStaff->grise, 1 );
	Read( &laidOutStaff->invisible, 1 );
	Read( &laidOutStaff->vertBarre, 1 );
	Read( &laidOutStaff->brace, 1 );
	Read( &laidOutStaff->staffSize, 1 );
	Read( &laidOutStaff->portNbLine, 1 );
	Read( &laidOutStaff->accol, 1 );
	Read( &laidOutStaff->accessoire, 1 ); 
	Read( &int32, 4 );
	laidOutStaff->m_y_abs = wxINT32_SWAP_ON_BE( int32 );
    // also read the number of layers
    Read( &int32, 4 );
    m_nbLaidOutLayers = wxINT32_SWAP_ON_BE( int32 );    
    
    return laidOutStaff;
}

MusLaidOutLayer* MusBinInput::ReadLaidOutLayer( )
{
    if ( !m_nbLaidOutLayers ) {
        return NULL;
    }
    m_nbLaidOutLayers--;  
        
    Read( &int32, 4 );
    int logLayerNb = wxINT32_SWAP_ON_BE( int32 ); 
    Read( &int32, 4 );
    int logStaffNb = wxINT32_SWAP_ON_BE( int32 ); 

    Read( &m_uuid, 16 ); // this is the uuid of the Section element
    // we need to find the section it points to based on the uuid    
    MusFunctor findSectionUuid( &MusObject::FindWithUuid );
    MusObject *o = m_doc->FindLogicalObject( &findSectionUuid, m_uuid );
    MusSection *section = dynamic_cast<MusSection*>( o  );

    Read( &m_uuid, 16 ); // this is the uuid of the Measure element
    // we need to find the measure (if any?) it points to based on the uuid    
    MusFunctor findMeasureUuid( &MusObject::FindWithUuid );
    // measure will be NULL if uuid is NULL (for un measured music)
    MusMeasure *measure = dynamic_cast<MusMeasure*>( m_doc->FindLogicalObject( &findMeasureUuid, m_uuid ) );
    
    MusLaidOutLayer *laidOutLayer = new MusLaidOutLayer(logLayerNb, logStaffNb, section, measure);
    ReadObject( laidOutLayer );    
	Read( &uint16, 2 );
	laidOutLayer->voix = wxUINT16_SWAP_ON_BE( uint16 );        
    // also read the number of elements
    Read( &int32, 4 );
    m_nbLaidOutLayerElements = wxINT32_SWAP_ON_BE( int32 );     
    
    return laidOutLayer;
}

MusLaidOutLayerElement* MusBinInput::ReadLaidOutLayerElement( )
{
    if ( !m_nbLaidOutLayerElements ) {
        return NULL;
    }
    m_nbLaidOutLayerElements--;  
    
    Read( &m_uuid, 16 ); // this is the uuid of the layer element element
    // we need to find the it in the logical tree 
    MusFunctor findElementUuid( &MusObject::FindWithUuid );
    MusLayerElement *element = dynamic_cast<MusLayerElement*>( m_doc->FindLogicalObject( &findElementUuid, m_uuid ) );
    
    MusLaidOutLayerElement *laidOutLayerElement = new MusLaidOutLayerElement( element );
    ReadObject( laidOutLayerElement );
	Read( &int32, 4 );
	laidOutLayerElement->m_x_abs = wxINT32_SWAP_ON_BE( int32 );
    
    return laidOutLayerElement;
}

#endif

//----------------------------------------------------------------------------
// MusBinInput_1_X
//----------------------------------------------------------------------------

#define BIN_SEPARATOR "#\376\364\365\376#"  // "#˛Ùı˛#"

/* note types */
#define NOTE 0	/* pour type (bit) de struct element */
#define SYMB 1

/* symbol flags */
#define BARRE 0
#define ALTER 1
#define PNT	2
#define IND_MES 5
#define CLE 6

#define CUSTOS 11 // ax2 double check in 1.6


MusBinInput_1_X::MusBinInput_1_X( MusDoc *doc, wxString filename, int flag ) :
	MusFileInputStream( doc, filename )
{
	m_flag = flag;
	m_vmaj = m_vmin = m_vrev = 10000; // arbitrary version, we assume we will never reach version 10000...
    
	m_section = NULL;
	m_logStaff = NULL;
	m_logLayer = NULL;
}

MusBinInput_1_X::~MusBinInput_1_X()
{
}

bool MusBinInput_1_X::ImportFile( )
{
	int i;

	if ( !IsOk() )
	{
		wxLogMessage(_("Cannot read file '%s'"), m_doc->m_fname.c_str() );
		return false;
	}
    
    // reset the MusDoc and create the logical tree
    m_doc->Reset();	
    MusDiv *div = new MusDiv( );
    MusScore *score = new MusScore( );
    m_section = new MusSection( );
    // create a new layout  (we will get only one of them in a WWG)
    MusLayout *layout = new MusLayout( Transcription );

    unsigned short nbpage;
    ReadFileHeader( &nbpage ); // fileheader
    
    for (i = 0; i < nbpage; i++ )
	{
		MusPage *page = new MusPage();
		ReadPage( page );
        layout->AddPage( page );
    }
    
	//wxLogMessage("OK %d",  m_doc->m_pages.GetCount() );
    m_doc->AddLayout( layout );	
    score->AddSection( m_section );
    div->AddScore( score );
    m_doc->AddDiv( div );
    
    
    // update the system and staff y positions
    layout->PaperSize();
    int j, k, l, m;
    for (j = 0; j < layout->GetPageCount(); j++)
    {
        MusPage *page = &layout->m_pages[j];
        m = 0; // staff number on the page
        int yy =  layout->m_pageHeight;
        for (k = 0; k < page->GetSystemCount(); k++) 
        {
            MusSystem *system = &page->m_systems[k];
            MusLaidOutStaff *staff = NULL;
            
            for (l = 0; l < system->GetStaffCount(); l++) 
            {
                staff = &system->m_staves[l];
                yy -= ecarts[m] * layout->m_interl[ staff->staffSize ];
                staff->m_y_abs = yy;
                m++;
                
                // we are handling the first staff - update the position of the system as well
                if ( l == 0 ) { 
                    system->m_y_abs = yy;
                }
            }
        }
    }
    
	return true;
}

bool MusBinInput_1_X::ReadFileHeader( unsigned short *nbpage )
{
	Read( &int32, 4 ); 
	m_flag = wxINT32_SWAP_ON_BE( int32 ); 
	Read( &int32, 4 ); 
	m_vmaj = wxINT32_SWAP_ON_BE( int32 ); 
    Read( &int32, 4 ); 
	m_vmin = wxINT32_SWAP_ON_BE( int32 ); 
    Read( &int32, 4 ); 
	m_vrev = wxINT32_SWAP_ON_BE( int32 ); 
	Read( &uint16, 2 ); 
	*nbpage = wxUINT16_SWAP_ON_BE( uint16 ); // nbpage / ignored
	Read( &uint16, 2 ); 
	//header->nopage = wxUINT16_SWAP_ON_BE( uint16 ); // nopage / ignored
	Read( &uint16, 2 ); 
	//header->noligne = wxUINT16_SWAP_ON_BE( uint16 ); // noligne / ignored
	Read( &uint32, 4 );
	//header->xpos = wxUINT32_SWAP_ON_BE( uint32 );  // xpso / ignored
	Read( &m_doc->m_env.m_landscape, 1 ); // param - orientation
    m_doc->m_env.m_landscape = !m_doc->m_env.m_landscape; // reverse it?
	Read( &m_doc->m_env.m_staffLineWidth, 1 ); // param - epLignesPortee
    m_doc->m_env.m_staffLineWidth = 1;	// force it
	Read( &m_doc->m_env.m_stemWidth, 1 ); // param - epQueueNotes
    m_doc->m_env.m_stemWidth = 2; // force it
	Read( &m_doc->m_env.m_barlineWidth, 1 ); // param - epBarreMesure
	Read( &m_doc->m_env.m_beamWidth, 1 ); // param - epBarreValeur
	Read( &m_doc->m_env.m_beamWhiteWidth, 1 ); // param - epBlancBarreValeur
	Read( &int32, 4 );
	m_doc->m_pageHeight = wxINT32_SWAP_ON_BE( int32 ) * 10; // param - pageFormatHor
	Read( &int32, 4 );
	m_doc->m_pageWidth = wxINT32_SWAP_ON_BE( int32 ) * 10; // param - pageFormatVer
	Read( &int16, 2 );
	m_doc->m_pageTopMar = wxINT16_SWAP_ON_BE( int16 ); // param - margeSommet
	Read( &int16, 2 );
	m_doc->m_env.m_leftMarginOddPage = wxINT16_SWAP_ON_BE( int16 ); // param - margeGaucheImpaire
	Read( &int16, 2 );
	m_doc->m_env.m_leftMarginEvenPage = wxINT16_SWAP_ON_BE( int16 ); // param - margeGauchePaire
    
	Read( &m_doc->m_env.m_smallStaffNum, 1 ); // rpPorteesNum
	Read( &m_doc->m_env.m_smallStaffDen, 1 ); // rpPorteesDen
	Read( &m_doc->m_env.m_graceNum, 1 ); // rpDiminNum
	Read( &m_doc->m_env.m_graceDen, 1 ); // rpDiminDen	
	Read( &m_doc->m_env.m_stemCorrection, 1 ); // hampesCorr
    m_doc->m_env.m_stemCorrection = 1;	// force it 

	if ( MusDoc::GetFileVersion(m_vmaj, m_vmin, m_vrev) < MusDoc::GetFileVersion(1, 6, 1) )
		return true; // following values where added in 1.6.1
    // 1.6.1
    Read( &int32, 4 );
    m_doc->m_env.m_notationMode = wxINT32_SWAP_ON_BE( int32 );

	return true;
}


bool MusBinInput_1_X::ReadSeparator( )
{
	char buffer[7];

	Read( buffer, 7 );
	wxString str1( buffer, 0, 6 );
	wxString str2( BIN_SEPARATOR );
	if ( str1 != str2 ) {
		wxLogDebug("'%s' - '%s' - Error while reading separator", str1.c_str(), str2.c_str() );
		return false;
    }
	else
		return true;
}

bool MusBinInput_1_X::ReadPage( MusPage *page )
{
	int j;
    
    int indent;
    int indentDroite;
    int lrg_lign; 
    char unused;

    if ( !ReadSeparator() )
		return false;

	Read( &int32, 4 );
	//page->npage = wxINT32_SWAP_ON_BE( int32 );
	Read( &int16, 2 );
	short nbrePortees = wxINT16_SWAP_ON_BE( int16 );
    Read( &unused, 1 );	
	Read( &unused, 1 );
	Read( &unused, 1 );
	Read( &page->defin, 1 );
	Read( &int32, 4 );
	indent = wxINT32_SWAP_ON_BE( int32 );	
	Read( &int32, 4 );
	indentDroite = wxINT32_SWAP_ON_BE( int32 ); 
	Read( &int32, 4 );
	lrg_lign = wxINT32_SWAP_ON_BE( int32 );
    
    /*
    for (j = 0; j < page->nbrePortees; j++) 
	{
		MusStaff *staff = new MusStaff();
		ReadStaff( staff );
        staff->voix = (j % 2 == 0) ? 1 : 0; // add voices
		page->m_staves.Add( staff );
	}
    */
    
    MusSystem *system = new MusSystem(); // first system of the page 
    system->m_systemLeftMar = indent;
    system->m_systemRightMar = indentDroite;
    //system->lrg_lign = lrg_lign;  
    int system_no = 0; // we don't have no members in system anymore 
    
    for (j = 0; j < nbrePortees; j++) 
	{
        // create or get the current MusStaff in the logical tree;
        /*
        // this creates one staff in the logical tree per staff on the page
        if (j >= (int)m_section->m_staves.GetCount()) {
            MusStaff *staff = new MusStaff();
            MusLayer *layer = new MusLayer();
            staff->AddLayer( layer );
            m_section->AddStaff( staff );
        }
        // we ignore voice numbers here
        m_logStaff = dynamic_cast<MusStaff*> (&m_section->m_staves[j]);
        wxASSERT_MSG( m_logStaff, "MusStaff cannot be NULL" );
        m_logLayer = dynamic_cast<MusLayer*> (&m_logStaff->m_layers[0]);
        wxASSERT_MSG( m_logLayer, "MusLayer cannot be NULL" );
        */
        // the alternate option is to create one single staff in the logical tree
        if ((int)m_section->m_staves.GetCount() == 0) {
            MusStaff *staff = new MusStaff();
            MusLayer *layer = new MusLayer();
            staff->AddLayer( layer );
            m_section->AddStaff( staff );
        }
        m_logStaff = dynamic_cast<MusStaff*> (&m_section->m_staves[0]);
        wxASSERT_MSG( m_logStaff, "MusStaff cannot be NULL" );
        m_logLayer = dynamic_cast<MusLayer*> (&m_logStaff->m_layers[0]);
        wxASSERT_MSG( m_logLayer, "MusLayer cannot be NULL" );
        
		MusLaidOutStaff *staff = new MusLaidOutStaff( j + 1 );
        MusLaidOutLayer *layer = new MusLaidOutLayer( 1, j + 1, m_section, NULL ); // we have always on layer per staff
		ReadStaff( staff, layer, j );
        if ( m_noLigne > system_no + 1 ) { // we have a new system
            page->AddSystem( system ); // add the current one
            system = new MusSystem(); // create the next one
            system_no = m_noLigne - 1; 
            system->m_systemRightMar = indentDroite;
            //system->lrg_lign = lrg_lign;
        }
        if ( m_indent ) {
			system->m_systemLeftMar = indent;
        }
        if ( m_indentDroite ) {      
            system->m_systemRightMar = m_indentDroite;
        }
        staff->AddLayer( layer );
        system->AddStaff( staff );
	}
    // add the last system
    page->AddSystem( system );    

	return true;

}
bool MusBinInput_1_X::ReadStaff( MusLaidOutStaff *staff, MusLaidOutLayer *layer, int staffNo )
{
	unsigned int k;

	Read( &uint32, 4 );
	unsigned int nblement  = wxUINT32_SWAP_ON_BE( uint32 );
	Read( &uint16, 2 );
	layer->voix = wxUINT16_SWAP_ON_BE( uint16 );
	//wxLogDebug("voix: %d", layer->voix);
	Read( &uint16, 2 );
	staff->noGrp = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	staff->totGrp = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	m_noLigne = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	//staff->no = wxUINT16_SWAP_ON_BE( uint16 ); // ignored
	Read( &staff->armTyp, 1 );
	Read( &staff->armNbr, 1 );
	Read( &staff->notAnc, 1 );
	//staff->notAnc = true;// force notation ancienne
	Read( &staff->grise, 1 );
	Read( &staff->invisible, 1 );
	Read( &uint16, 2 );
	ecarts[staffNo] = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &staff->vertBarre, 1 );
	Read( &staff->brace, 1 );
	Read( &staff->staffSize, 1 );
    Read( &int32, 4 );
    m_indent = wxINT32_SWAP_ON_BE( int32 );
	Read( &m_indentDroite, 1 );
	Read( &staff->portNbLine, 1 );
	Read( &staff->accol, 1 );
	Read( &staff->accessoire, 1 );
	Read( &uint16, 2 );
	//staff->reserve = wxUINT16_SWAP_ON_BE( uint16 ); // ignored
	
	unsigned char c;
	for ( k = 0; k < nblement; k++ )
	{
		Read( &c, 1 );
		if ( c == NOTE )
		{
			ReadNote( layer );
		}
		else if ( c == SYMB )
		{
			ReadSymbol( layer );
            // For the lyrics, we must attach them to the notes
            // We keep it and
		}
        /*
        else if ( c == NEUME )
		{
			MusNeume *neume = new MusNeume();
			ReadNeume( neume );
			staff->m_elements.Add(  neume );
		}
		if ( m_flag == MUS_BIN_ARUSPIX_CMP )
		{		
			MusElement *elem = &staff->m_elements.Last();
			Read( &int32, 4 );
			Read( elem->m_im_docname.GetWriteBuf( wxINT32_SWAP_ON_BE( int32 ) + 1 ) , wxINT32_SWAP_ON_BE( int32 ) + 1 );
			elem->m_im_docname.UngetWriteBuf();
			Read( &int32, 4 );
			elem->m_im_staff = wxINT32_SWAP_ON_BE( int32 );
			Read( &int32, 4 );
			elem->m_im_pos = wxINT32_SWAP_ON_BE( int32 );
			Read( &int32, 4 );
			elem->m_cmp_flag = wxINT32_SWAP_ON_BE( int32 );
		}
        */
	}
			
	return true;
}


bool MusBinInput_1_X::ReadSymbol( MusLaidOutLayer *layer, bool isLyric )
{
	ReadElementAttr(  );
	Read( &flag , 1 );
	Read( &calte , 1 );
	Read( &carStyle , 1 );
	Read( &carOrient , 1 );
	Read( &fonte , 1 );
	Read( &s_lie_l , 1 );
	Read( &symbol_point , 1 );
	Read( &uint16, 2 );
	code = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	l_ptch = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &int32, 4 );
	dec_y = wxINT32_SWAP_ON_BE( int32 );
    
    MusLayerElement *layer_element = NULL;        
    if ( flag == CLE ) {
        MusClef *clef = new MusClef();
        clef->m_clefId = (ClefId)code;
        layer_element = clef;
        
    } 
    else if ( flag == ALTER ) {
        MusSymbol *alter = new MusSymbol( SYMBOL_ACCID );
        alter->m_oct = oct;
        alter->m_pname = code;
        alter->m_accid = calte;
        layer_element = alter;
    }
    else if ( flag == BARRE ) {
        MusBarline *barline = new MusBarline();
        layer_element = barline;
    }
    else if ( flag == IND_MES ) {
        MusMensur *mensur = new MusMensur( );
		if ( code & 64 )
		{
			//symbol->code = 64; ?? // ax2
			if ( calte == 0 )
				mensur->m_meterSymb = METER_SYMB_COMMON;
			else if ( calte == 1 )
				mensur->m_meterSymb = METER_SYMB_CUT;
			else if ( calte == 2 )
				mensur->m_meterSymb = METER_SYMB_2;
			else if ( calte == 3 )
				mensur->m_meterSymb = METER_SYMB_3;
			else if ( calte == 4 )
				mensur->m_meterSymb = METER_SYMB_2_CUT;
			else if ( calte == 5 )
				mensur->m_meterSymb = METER_SYMB_3_CUT;
			else
				wxLogWarning( _("Unkown mesure signe indication") );
		}
		else if ( code != 1 )
		{
			if ( code & 32  )
                mensur->m_sign = MENSUR_SIGN_O;
			else if ( code & 16 )
				mensur->m_sign = MENSUR_SIGN_C;
            else if ( code & 8 ) {
				mensur->m_sign = MENSUR_SIGN_C;
                mensur->m_reversed = true;
            }
			else
				wxLogWarning( _("Unkown mesure indication") );
			if ( code & 4 )
				mensur->m_slash = true;
			if ( code & 2 )
				mensur->m_dot = true;
		}
		else
		{
			mensur->m_num = durNum;
			mensur->m_numBase = durDen;
		}
        layer_element = mensur;
    }
    else if ( flag == PNT ) {
        MusSymbol *dot = new MusSymbol( SYMBOL_DOT );
        dot->m_oct = oct;
        dot->m_pname = code;
        layer_element = dot;
    }
    
    if ( layer_element ) {
        m_logLayer->AddLayerElement( layer_element );
        MusLaidOutLayerElement *element = new MusLaidOutLayerElement( layer_element );
        element->m_x_abs = x_abs;
        layer->AddElement( element );
    }
    
	if ( isLyric ) {
        // this will read the lyric but it will be ignored
        ReadLyric( );
    }
     
	return true;
}

bool MusBinInput_1_X::ReadNote( MusLaidOutLayer *layer )
{
	ReadElementAttr( );
	Read( &sil, 1 );
	Read( &val, 1 );
	Read( &inv_val, 1 );
	Read( &note_point, 1 );
	Read( &stop_rel, 1 );
	Read( &acc, 1 );
	Read( &accInvis, 1 );
	Read( &q_auto, 1 );
	Read( &queue, 1 );
	Read( &stacc, 1 );
	Read( &oblique, 1 );
	Read( &queue_lig, 1 );
	Read( &chord, 1 );
	Read( &fchord, 1 );
	Read( &lat, 1 );
	Read( &haste, 1 );
	unsigned char c;
	Read( &c, 1 );
	code = (unsigned short)c; // code deplace dans element, unsigned SHORT !!
	Read( &tetenot, 1 );
	Read( &typStac, 1 );
	
    MusLayerElement *layer_element = NULL;
    if (val == CUSTOS) {
        MusSymbol *custos = new MusSymbol( SYMBOL_CUSTOS );
        custos->m_oct = oct;
        custos->m_pname = code;
        layer_element = custos;
    }
    else if ( !sil ) { // note
        MusNote *note = new MusNote( );
        note->m_dur = val;
        note->m_oct = oct;
        note->m_pname = code;
        // stem
        if ( !q_auto && ((note->m_dur ==  DUR_LG) || (note->m_dur > DUR_1 ))) {
            note->m_stemDir = 1;
        }
        // ligature
        if ( ligat && (note->m_dur >  DUR_LG) && (note->m_dur < DUR_2 )) {
            note->m_lig = LIG_INITIAL;
        }
        // coloration
        if ( inv_val && (note->m_dur < DUR_2 )) {
            note->m_colored = true;
        }
        else if (inv_val && (note->m_dur > DUR_4 )) {
            note->m_ligObliqua = true;
        }
        layer_element = note;
    }
    else { // rest
        MusRest *rest = new MusRest();
        rest->m_dur = val;
        rest->m_oct = oct;
        rest->m_pname = code;
        layer_element = rest;
    }
    
    // if we got something, add it to the LaidOutLayer
    if ( layer_element ) {
        m_logLayer->AddLayerElement( layer_element );
        MusLaidOutLayerElement *element = new MusLaidOutLayerElement( layer_element );
        element->m_x_abs = x_abs;
        layer->AddElement( element );
    }
    
	char count;
	Read( &count, 1 );
	
	for ( int i = 0; i < count; i++ ) {
        char TYPE;
		Read( &TYPE, 1 );
		ReadSymbol( NULL, true ); // the lyric will be ignored
	}
	
	return true;
}

bool MusBinInput_1_X::ReadNeume( )
{
    // ...
	return true;
}

bool MusBinInput_1_X::ReadElementAttr( )
{
	Read( &liaison , 1 );
	Read( &dliai , 1 );
	Read( &fliai , 1 );
	Read( &lie_up , 1 );
	Read( &rel , 1 );
	Read( &drel , 1 );
	Read( &frel , 1 );
	Read( &oct , 1 );
	Read( &dimin , 1 );
	Read( &grp , 1 );
	Read( &_shport , 1 );
	Read( &ligat , 1 );
	Read( &ElemInvisible , 1 );
	Read( &pointInvisible , 1 );
	Read( &existDebord , 1 );
	Read( &fligat , 1 );
	Read( &notschowgrp , 1 );
	Read( &cone , 1 );
	Read( &liaisonPointil , 1 );
	Read( &reserve1 , 1 );
	Read( &reserve2 , 1 );
	Read( &ottava , 1 );
	Read( &uint16, 2 );
	durNum = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	durDen = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	offset = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &int32, 4 );
	x_abs = wxINT32_SWAP_ON_BE( int32 );
	
	return true;
}


bool MusBinInput_1_X::ReadLyric( )
{
	Read( &int32, 4 );
    wxString to_be_ignored;
	Read( to_be_ignored.GetWriteBuf( wxINT32_SWAP_ON_BE( int32 ) + 1 ) , wxINT32_SWAP_ON_BE( int32 ) + 1 );
	to_be_ignored.UngetWriteBuf();
	return true;
}

/*
bool MusBinInput_1_X::ReadPagination( MusPagination *pagination )
{
	Read( &int16, 2 );
	pagination->numeroInitial = wxINT16_SWAP_ON_BE( int16 );
	Read( &pagination->aussiPremierPage, 1 );
	Read( &pagination->position, 1 );
	Read( &pagination->numeroFonte, 1 );
	Read( &pagination->carStyle, 1 );
	Read( &pagination->taille, 1 );
	Read( &pagination->offsetDuBord, 1 );
	return true;
}

bool MusBinInput_1_X::ReadHeaderFooter( MusHeaderFooter *headerfooter)
{
	char buffer[HEADER_FOOTER_TEXT + 1];
	Read( buffer, HEADER_FOOTER_TEXT );
	headerfooter->texte = buffer;
	Read( &headerfooter->aussiPremierPage, 1 );
	Read( &headerfooter->position, 1 );
	Read( &headerfooter->numeroFonte, 1 );
	Read( &headerfooter->carStyle, 1 );
	Read( &headerfooter->taille, 1 );
	Read(&headerfooter->offsetDuBord, 1 );
	return true;
}
*/




