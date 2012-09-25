/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei.cpp
// Author:      Laurent Pugin
// Created:     2008
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////



#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wx.h"

#include "wx/filename.h"
#include "wx/txtstrm.h"

#include "musiomei.h"

#include <mei/xmlimport.h>
#include <mei/xmlexport.h>
#include <mei/cmnornaments.h>

#include "musbarline.h"
#include "musbeam.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musneumesymbol.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"

#include "muslaidoutstaff.h"
#include "muslaidoutlayer.h"
#include "muslaidoutlayerelement.h"

//#include "app/axapp.h"

#include <vector>
using std::vector;

//----------------------------------------------------------------------------
// MusMeiOutput
//----------------------------------------------------------------------------

MusMeiOutput::MusMeiOutput( MusDoc *doc, wxString filename ) :
    // This is pretty bad. We open a bad fileoutputstream as we don't use it
	MusFileOutputStream( doc, -1 )
{
    m_filename = filename;
    m_mei = NULL;
    m_music = NULL;
    m_body = NULL;
    m_div = NULL;
    m_score = NULL;
    m_parts = NULL;
    m_part = NULL;
    m_section = NULL;
    m_measure = NULL;
    m_staff = NULL;
    m_layer = NULL;
    m_rdgLayer = NULL;
    m_currentLayer = NULL;
    m_layouts = NULL;
    m_layout = NULL;
    m_page = NULL;
    m_system = NULL;
    m_laidOutStaff = NULL;
    m_laidOutLayer = NULL;
}

MusMeiOutput::~MusMeiOutput()
{
}

bool MusMeiOutput::ExportFile( )
{
    try {
        
        
        XmlInstructions procinst;
        
        //std::string name1 = "xml-model";
        //std::string value1 = "href=\"mei-2012.rng\" type=\"application/xml\" schematypens=\"http://purl.oclc.org/dsdl/schematron\"";
        
        std::string name2 = "xml-model";
        std::string value2 = "href=\"http://www.aruspix.net/mei-layout-2012-09-25.rng\" type=\"application/xml\" schematypens=\"http://relaxng.org/ns/structure/1.0\"";
        
        //XmlProcessingInstruction *xpi1 = new XmlProcessingInstruction(name1, value1);
        XmlProcessingInstruction *xpi2 = new XmlProcessingInstruction(name2, value2);
        
        //procinst.push_back(xpi1);
        procinst.push_back(xpi2);
        
        mei::MeiDocument *meiDoc = new mei::MeiDocument();
        
        m_mei = new MeiElement("mei");
        // this starts the call of all the functors
        
        m_doc->Save( this );

        meiDoc->setRootElement(m_mei);
        XmlExport::meiDocumentToFile( meiDoc, m_filename.c_str(), procinst );
    }
    catch( char * str ) {
        wxLogError("%s", str );
        return false;
    }
	return true;    
}

std::string MusMeiOutput::UuidToMeiStr( MusObject *element )
{
    // RZ uuid_string_t does not exist on freebsd
    char uuidStr[37];
    uuid_unparse( *element->GetUuid(), uuidStr );
    string out;    
    // xml IDs can't start with a number, so we prepend "m-" to every ID.
    out = "m-" + string(uuidStr);
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

bool MusMeiOutput::WriteDoc( MusDoc *doc )
{
    wxASSERT( m_mei );
    
    // ---- header ----
    MeiHead *meiHead = new MeiHead();
    
    MeiElement *fileDesc = new MeiElement("fileDesc");
    MeiElement *titleStmt = new MeiElement("titleStmt");
    fileDesc->addChild(titleStmt);
    MeiElement *title = new MeiElement("title");
    titleStmt->addChild(title);
    MeiElement *pubStmt = new MeiElement("pubStmt");
    fileDesc->addChild(pubStmt);
    MeiElement *date = new MeiElement("date");
    pubStmt->addChild(date);
    
    MeiElement *encodingDesc = new MeiElement("encodingDesc");
    MeiElement *projectDesc = new MeiElement("projectDesc");
    encodingDesc->addChild(projectDesc);
    MeiElement *p1 = new MeiElement("p");
    projectDesc->addChild(p1);
    
    p1->setValue( wxString::Format( "Encoded with Aruspix version %s",  MusDoc::GetAxVersion().c_str() ).c_str() );
    date->setValue( wxNow().c_str() );
    
    meiHead->addChild(fileDesc);
    meiHead->addChild(encodingDesc);
    m_mei->addChild( meiHead );
    
    // ---- music ---- 
    m_music = new Music();
    m_layouts = new Layouts();
    m_body = new Body();
    // add them, layout first, the body containing divs
    m_music->addChild( m_layouts );
    m_music->addChild( m_body );
    m_mei->addChild( m_music );
    
    return true;
}

bool MusMeiOutput::WriteDiv( MusDiv *div )
//bool MusMeiOutput::WriteMeiDiv( Mdiv *meiDiv, MusDiv *div )
{
    wxASSERT( m_body );
    m_div = new Mdiv();
    m_div->setId( UuidToMeiStr( div ));
    m_body->addChild( m_div );
    return true;
}

bool MusMeiOutput::WriteScore( MusScore *score )
//bool MusMeiOutput::WriteMeiScore( Score *meiScore, MusScore *score )
{
    wxASSERT( m_div );
    m_score = new Score();
    m_score->setId( UuidToMeiStr( score ));
    m_div->addChild( m_score );
    return true;
}

bool MusMeiOutput::WritePartSet( MusPartSet *partSet )
//bool MusMeiOutput::WriteMeiParts( Parts * meiParts, MusPartSet *partSet )
{
    wxASSERT( m_div );
    m_parts = new Parts();
    m_parts->setId( UuidToMeiStr( partSet ));
    m_div->addChild( m_parts );
    return true;
}

bool MusMeiOutput::WritePart( MusPart *part )
//bool MusMeiOutput::WriteMeiPart( Part *meiPart, MusPart *part )
{
    wxASSERT( m_parts );
    m_part = new Part();
    m_part->setId( UuidToMeiStr( part ));
    m_parts->addChild( m_part );
    return true;
}

bool MusMeiOutput::WriteSection( MusSection *section )
//bool MusMeiOutput::WriteMeiSection( Section *meiSection, MusSection *section )
{
    wxASSERT( m_score || m_part );
    m_section = new Section();
    m_section->setId( UuidToMeiStr( section ));
    if ( m_score ) {
        m_score->addChild( m_section );
    }
    else {
        m_part->addChild( m_section );
    }
    return true;
}

bool MusMeiOutput::WriteMeasure( MusMeasure *measure )
//bool MusMeiOutput::WriteMeiMeasure( Measure *meiMeasure, MusMeasure *measure )
{
    wxASSERT( m_section );
    m_measure = new Measure();
    m_measure->setId( UuidToMeiStr( measure ));
    m_section->addChild( m_measure );
    return true;
}

bool MusMeiOutput::WriteStaff( MusStaff *staff )
//bool MusMeiOutput::WriteMeiStaff( Staff *meiStaff, MusStaff *staff )
{
    wxASSERT( m_section || m_measure );
    m_staff = new Staff();
    m_staff->setId( UuidToMeiStr( staff ));
    // measured music, we have a measure object
    if ( m_measure ) {
        m_measure->addChild( m_staff );
    }
    // unmeasured music
    else {
        m_section->addChild( m_staff );
    }
    return true;
}

bool MusMeiOutput::WriteLayer( MusLayer *layer )
//bool MusMeiOutput::WriteMeiLayer( Layer *meiLayer, MusLayer *layer )
{
    wxASSERT( m_staff );
    m_layer = new Layer();
    m_currentLayer = m_layer;
    m_layer->setId( UuidToMeiStr( layer ));
    m_staff->addChild( m_layer );
    return true;
}

bool MusMeiOutput::WriteLayerElement( MusLayerElement *element )
{
    wxASSERT( m_currentLayer );
    
    MeiElement *meiElement = NULL;
    if (dynamic_cast<MusBarline*>(element)) {
        BarLine *barline = new BarLine();
        WriteMeiBarline( barline, dynamic_cast<MusBarline*>(element) );
        meiElement = barline;
    }
    else if (dynamic_cast<MusBeam*>(element)) {
        Beam *beam = new Beam();
        WriteMeiBeam( beam, dynamic_cast<MusBeam*>(element) );
        meiElement = beam;
    }
    else if (dynamic_cast<MusClef*>(element)) {
        Clef *clef = new Clef();
        WriteMeiClef( clef, dynamic_cast<MusClef*>(element) );
        meiElement = clef;
    }
    else if (dynamic_cast<MusMensur*>(element)) {
        Mensur *mensur = new Mensur();
        WriteMeiMensur( mensur, dynamic_cast<MusMensur*>(element) );
        meiElement = mensur;
    }
    else if (dynamic_cast<MusNeume*>(element)) {
        wxLogWarning( "Neume are not saved in MEI files" );
    }
    else if (dynamic_cast<MusNeumeSymbol*>(element)) {
        wxLogWarning( "NeumeSymbol are not saved in MEI files" );
    }
    else if (dynamic_cast<MusNote*>(element)) {
        if ( dynamic_cast<MusNote*>(element)->m_beam[0] ) {
            // the note will be saved from the beam element;
            return true;
        }
        Note *note = new Note();
        WriteMeiNote( note, dynamic_cast<MusNote*>(element) );
        meiElement = note;
    }
    else if (dynamic_cast<MusRest*>(element)) {
        if ( dynamic_cast<MusRest*>(element)->m_beam[0] ) {
            // the rest will be saved from the beam element;
            return true;
        }
        Rest *rest = new Rest();
        WriteMeiRest( rest, dynamic_cast<MusRest*>(element) );
        meiElement = rest;
    }
    else if (dynamic_cast<MusSymbol*>(element)) {        
        meiElement = WriteMeiSymbol( dynamic_cast<MusSymbol*>(element) );
    }
    
    // we have it, set the uuid we read
    if ( meiElement ) {
        meiElement->setId( UuidToMeiStr( element ));
        m_currentLayer->addChild( meiElement );
        return true;
    }
    else {
        wxLogWarning( "Element class %s could not be saved", element->MusClassName().c_str() );
        return false;
    }    
}

void MusMeiOutput::WriteMeiBarline( BarLine *meiBarline, MusBarline *barline )
{
    return;
}


void MusMeiOutput::WriteMeiBeam( Beam *meiBeam, MusBeam *beam )
{
    int i = 0;
    for (i = 0; i < (int)beam->m_notes.Count(); i++) {
        if ( dynamic_cast<MusNote*>(&beam->m_notes[i]) ) {
            MusNote *musNote = dynamic_cast<MusNote*>( &beam->m_notes[i] );
            Note *note = new Note();
            WriteMeiNote( note, musNote );
            note->setId( UuidToMeiStr( musNote ));
            meiBeam->addChild( note );
        }
        else if (dynamic_cast<MusRest*>(&beam->m_notes[i]) ) {
            MusRest *musRest = dynamic_cast<MusRest*>( &beam->m_notes[i] );
            Rest *rest = new Rest();
            WriteMeiRest( rest, musRest );
            rest->setId( UuidToMeiStr( musRest ));
            meiBeam->addChild( rest );
        }
    }
    return;
}


void MusMeiOutput::WriteMeiClef( Clef *meiClef, MusClef *clef )
{
    meiClef->m_Lineloc.setLine( ClefLineToStr( clef->m_clefId ) );
    meiClef->m_Clefshape.setShape( ClefShapeToStr( clef->m_clefId ) );
    return;
}


void MusMeiOutput::WriteMeiMensur( Mensur *meiMensur, MusMensur *mensur )
{
    if ( mensur->m_sign ) {
        meiMensur->m_MensurLog.setSign( MensurSignToStr( mensur->m_sign ));
    }
    if ( mensur->m_dot ) {
        meiMensur->m_MensurLog.setDot("true");
    }
    if ( mensur->m_slash ) {
        meiMensur->m_Slashcount.setSlash("1"); // only one slash for now
    }
    if ( mensur->m_reversed ) {
        meiMensur->m_MensurVis.setOrient("reversed"); // only orientation
    }
    if ( mensur->m_num ) {
        meiMensur->m_DurationRatio.setNum( wxString::Format("%d", mensur->m_num ).c_str() );
    }
    if ( mensur->m_numBase ) {
        meiMensur->m_DurationRatio.setNumbase( wxString::Format("%d", mensur->m_numBase ).c_str() );
    }
    // missing m_meterSymb
    
    return;
}

void MusMeiOutput::WriteMeiNote( Note *meiNote, MusNote *note )
{
    meiNote->m_Pitch.setPname( PitchToStr( note->m_pname ));
    meiNote->m_Octave.setOct( OctToStr( note->m_oct ));
    meiNote->m_DurationMusical.setDur( DurToStr( note->m_dur ));
    if ( note->m_dots ) {
        meiNote->m_Augmentdots.setDots( wxString::Format("%d", note->m_dots).c_str() );
    }
    if ( note->m_accid ) {
        meiNote->m_Accidental.setAccid( AccidToStr( note->m_accid ));
    }
    // missing m_artic, m_chord, m_colored, m_lig, m_headShape, m_ligObliqua, m_slur, m_stemDir, m_stemLen
    return;
}

void MusMeiOutput::WriteMeiRest( Rest *meiRest, MusRest *rest )
{    
    meiRest->m_DurationMusical.setDur( DurToStr( rest->m_dur ));
    if ( rest->m_dots ) {
        meiRest->m_Augmentdots.setDots( wxString::Format("%d", rest->m_dots).c_str() );
    }
    // missing position
    return;
}

MeiElement *MusMeiOutput::WriteMeiSymbol( MusSymbol *symbol )
{
    MeiElement *meiElement = NULL;
    if (symbol->m_type==SYMBOL_ACCID) {
        Accid *accid = new Accid();
        accid->m_Accidental.setAccid( AccidToStr( symbol->m_accid ));
        // missing position
        meiElement = accid;
    }
    else if (symbol->m_type==SYMBOL_CUSTOS) {
        Custos *custos = new Custos();
        custos->m_Pitch.setPname( PitchToStr( symbol->m_pname ) );
        custos->m_Octave.setOct( OctToStr( symbol->m_oct ) );
        meiElement = custos;
    }
    else if (symbol->m_type==SYMBOL_DOT) {
        Dot *dot = new Dot();
        // missing m_dots
        // missing position
        meiElement = dot;
    }
    return meiElement;
}


bool MusMeiOutput::WriteLayout( MusLayout *layout )
{
    wxASSERT( m_layouts );
    m_layout = new Layout();
    m_layout->m_Typed.setType(LayoutTypeToStr( layout->GetType() ));
    m_layout->setId( UuidToMeiStr( layout ));
    m_layouts->addChild( m_layout );
    return true;
}

bool MusMeiOutput::WritePage( MusPage *page )
{
    wxASSERT( m_layout );
    m_page = new Page();
    m_page->setId( UuidToMeiStr( page ));
    // size and margins but only if any - we rely on pageHeight only to check this
    if ( page->m_pageHeight != -1 ) {
        m_page->m_ScoreDefVis.setPageWidth( wxString::Format( "%d", page->m_pageWidth ).c_str() );
        m_page->m_ScoreDefVis.setPageHeight( wxString::Format( "%d", page->m_pageHeight ).c_str() );
        m_page->m_ScoreDefVis.setPageLeftmar( wxString::Format( "%d", page->m_pageLeftMar ).c_str() );
        m_page->m_ScoreDefVis.setPageRightmar( wxString::Format( "%d", page->m_pageRightMar ).c_str() );
    }
    //
    MeiCommentNode *comment = new MeiCommentNode();
    comment->setValue("Coordinates in MEI axis direction");
    m_layout->addChild( comment );
    m_layout->addChild( m_page );
    return true;
}

bool MusMeiOutput::WriteSystem( MusSystem *system )
{
    wxASSERT( m_page );
    m_system = new System();
    m_system->setId( UuidToMeiStr( system ));
    // margins
    m_system->m_ScoreDefVis.setSystemLeftmar( wxString::Format( "%d", system->m_systemLeftMar ).c_str() );
    m_system->m_ScoreDefVis.setSystemRightmar( wxString::Format( "%d", system->m_systemRightMar ).c_str() );
    // y positions
    m_system->m_Coordinated.setUly( wxString::Format( "%d", system->m_y_abs ).c_str() );
    m_page->addChild( m_system );
    return true;
}

bool MusMeiOutput::WriteLaidOutStaff( MusLaidOutStaff *laidOutStaff )
{
    wxASSERT( m_system );
    m_laidOutStaff = new LaidOutStaff();
    m_laidOutStaff->setId( UuidToMeiStr( laidOutStaff ));
    // y position
    if ( laidOutStaff->notAnc ) {
        m_laidOutStaff->m_Typed.setType("mensural");
    }
    m_laidOutStaff->m_Coordinated.setUly( wxString::Format( "%d", laidOutStaff->m_y_abs ).c_str() );
    m_laidOutStaff->m_Staffident.setStaff( wxString::Format( "%d", laidOutStaff->m_logStaffNb ).c_str() );
    m_system->addChild( m_laidOutStaff );
    return true;
}

bool MusMeiOutput::WriteLaidOutLayer( MusLaidOutLayer *laidOutLayer )
{    
    wxASSERT( m_laidOutStaff );
    m_laidOutLayer = new LaidOutLayer();
    m_laidOutLayer->setId( UuidToMeiStr( laidOutLayer ));
    m_laidOutLayer->m_Layerident.setLayer( wxString::Format( "%d", laidOutLayer->m_logLayerNb ).c_str() );
    m_laidOutLayer->m_Staffident.setStaff( wxString::Format( "%d", laidOutLayer->m_logLayerNb ).c_str() );
    if ( laidOutLayer->GetSection() ) {
        // unmeasured music
        m_laidOutLayer->m_Pointing.setTarget( UuidToMeiStr( laidOutLayer->GetSection() ) );
    }
    else if ( laidOutLayer->GetMeasure() ) {
        // measured music
        m_laidOutLayer->m_Pointing.setTarget( UuidToMeiStr( laidOutLayer->GetMeasure() ) );        
    }
    else {
        wxLogWarning( "Attempt to write a <laidOutLayer> without @target" );
    }
    m_laidOutStaff->addChild( m_laidOutLayer );
    return true;
}

bool MusMeiOutput::WriteLaidOutLayerElement( MusLaidOutLayerElement *laidOutLayerElement )
{
    wxASSERT( m_laidOutLayer );
    LaidOutElement *element = new LaidOutElement();
    
    element->setId( UuidToMeiStr( laidOutLayerElement ));
    // y position
    element->m_Coordinated.setUlx( wxString::Format( "%d", laidOutLayerElement->m_x_abs ).c_str() );
    // pointer to the logical element
    element->m_Pointing.setTarget(UuidToMeiStr( laidOutLayerElement->m_layerElement ) );
    m_laidOutLayer->addChild( element );
    return true;
}


bool MusMeiOutput::WriteLayerApp( MusLayerApp *app )
{    
    wxASSERT( m_currentLayer );
    m_app = new App();
    m_currentLayer->addChild( m_app ); 
    return true;
}

bool MusMeiOutput::WriteLayerRdg( MusLayerRdg *rdg )
{   
    wxASSERT( m_app );
    m_rdgLayer = new Rdg();
    // now swith the m_currentLayer pointer
    m_currentLayer = m_rdgLayer;
    m_rdgLayer->m_Source.setSource( rdg->m_srcId.c_str() );
    m_app->addChild( m_rdgLayer ); 
    return true;
}


bool MusMeiOutput::EndLayerRdg( MusLayerRdg *rgd )
{
    // swith back the m_currentLayer
    m_currentLayer = m_layer;
    return true;
}



std::string MusMeiOutput::OctToStr(int oct)
{
	char buf[3];
	snprintf(buf, 2, "%d", oct);
	return std::string(buf);
	
	// For some reason, #include <sstream> does not work with xcode 3.2
	//std::ostringstream oss;
	//oss << oct;
	//return oss.str();
}


std::string MusMeiOutput::PitchToStr(int pitch)
{
    string value;
    switch (pitch) {
        case 7:
        case 0: value = "b"; break;
        case 1: value = "c"; break;
        case 2: value = "d"; break;
        case 3: value = "e"; break;
        case 4: value = "f"; break;
        case 5: value = "g"; break;
        case 6: value = "a"; break;
        default: 
            wxLogWarning("Unknown pitch '%d'", pitch);
            value = "";
            break;
    }
	return value;
}

std::string MusMeiOutput::AccidToStr(unsigned char accid)
{
    string value;
    switch (accid) {
        case ACCID_SHARP: value = "s"; break;
        case ACCID_FLAT: value = "f"; break;
        case ACCID_NATURAL: value = "n"; break;
        case ACCID_DOUBLE_SHARP: value = "x"; break;
        case ACCID_DOUBLE_FLAT: value = "ff"; break;
        case ACCID_QUARTER_SHARP: value = "ns"; break;
        case ACCID_QUARTER_FLAT: value = "nf"; break;
        default: 
            wxLogWarning("Unknown accid '%d'", accid);
            value = "";
            break;
    }
	return value;
}

std::string MusMeiOutput::ClefLineToStr( ClefId clefId )
{	
	string value; 
	switch(clefId)
	{	
        case SOL2 : value = "2"; break;
		case SOL1 : value = "1"; break; 
		case SOLva : value = "2"; break;
		case FA5 : value = "5"; break;
		case FA4 : value = "4"; break;
		case FA3 : value = "3"; break;
		case UT1 : value = "1"; break;
        case UT2 : value = "2"; break;
		case UT3 : value = "3"; break;
		case UT4 : value = "4"; break;
		case UT5 : value = "5"; break;
        default: 
            wxLogWarning("Unknown clef '%d'", clefId);
            value = "";
            break;
	}
	return value;
}

std::string MusMeiOutput::ClefShapeToStr( ClefId clefId )
{	
	string value; 
	switch(clefId)
	{	
        case SOL2 : 
		case SOL1 : 
		case SOLva : value = "G"; break;
		case FA5 : 
		case FA4 :
		case FA3 : value = "F"; break;
        case UT1 :
		case UT2 : 
		case UT3 : 
		case UT4 : 
		case UT5 : value = "C"; break;		
        default: 
            wxLogWarning("Unknown clef '%d'", clefId);
            value = "";
            break;
	}
	return value;
}

std::string MusMeiOutput::MensurSignToStr(MensurSign sign)
{
 	string value; 
	switch(sign)
	{	case MENSUR_SIGN_C : value = "C"; break;
		case MENSUR_SIGN_O : value = "O"; break;		
        default: 
            wxLogWarning("Unknown mensur sign '%d'", sign);
            value = "";
            break;
	}
	return value;   
}


std::string MusMeiOutput::DurToStr( int dur )
{
    string value;
    if (dur == DUR_LG) value = "longa";
    else if (dur == DUR_BR) value = "brevis";
    else if (dur == DUR_1) value = "semibrevis";
    else if (dur == DUR_2) value = "minima";
    else if (dur == DUR_4) value = "semiminima";
    else if (dur == DUR_8) value = "fusa";
    else if (dur == DUR_16) value = "semifusa";
    else if (dur == DUR_LG) value = "long";
    else if (dur == DUR_BR) value = "breve";
    else if (dur == DUR_1) value = "1";
    else if (dur == DUR_2) value = "2";
    else if (dur == DUR_4) value = "4";
    else if (dur == DUR_8) value = "8";
    else if (dur == DUR_16) value = "16";
	else {
		wxLogWarning("Unknown duration '%d'", dur);
        value = "4";
	}
    return value;
}

std::string MusMeiOutput::LayoutTypeToStr(LayoutType type)
{
 	string value; 
	switch(type)
	{	case Rendering : value = "rendering"; break;
		case Transcription : value = "transcription"; break;		
        default: 
            wxLogWarning("Unknown layout type '%d'", type);
            value = "";
            break;
	}
	return value;   
}

//----------------------------------------------------------------------------
// MusMeiInput
//----------------------------------------------------------------------------

MusMeiInput::MusMeiInput( MusDoc *doc, wxString filename ) :
    // This is pretty bad. We open a bad fileoinputstream as we don't use it
	MusFileInputStream( doc, -1 )
{
    m_filename = filename;
	// logical
	m_div = NULL;
	m_score = NULL;
	m_parts = NULL;
	m_part = NULL;
	m_section = NULL;
	m_measure = NULL;
	m_staff = NULL;
	m_layer = NULL;
    // layout
    m_layout = NULL;
    m_page = NULL;
    m_system = NULL;
    m_laidOutStaff = NULL;
    m_laidOutLayer = NULL;
}

MusMeiInput::~MusMeiInput()
{
}

bool MusMeiInput::ImportFile( )
{
    
    //printf("ROOD %s\n", m_filename.c_str());
    try {
        m_doc->Reset();
        mei::MeiDocument *doc = XmlImport::documentFromFile( *new string( m_filename.c_str()) );
        if ( !doc ) {
            return false;
        }
        MeiElement *root = doc->getRootElement();
        // header
        if ( root->hasChildren("meiHead") ) {
             vector<MeiElement*> children = root->getChildrenByName("meiHead");
             if ( dynamic_cast<MeiHead*> (children[0]) ) {
                ReadMeiHeader(dynamic_cast<MeiHead*> (children[0]));
             }
        }
        // music
        MeiElement *music = NULL;
        MeiElement *body = NULL;
        MeiElement *layouts = NULL;
        if ( root->hasChildren("music") ) {
            music = root->getChildrenByName("music")[0];
        }
        
        // reading the body first
        if ( music && music->hasChildren("body") ) {
            body = music->getChildrenByName("body")[0];
        }
		if ( body && body->hasChildren("mdiv") ) {
			vector<MeiElement*> children = body->getChildrenByName("mdiv");
			for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
				MeiElement *e = *iter;
				m_div = new MusDiv( );
                SetMeiUuid( e, m_div );
				if (ReadMeiDiv(dynamic_cast<Mdiv*>(e))) {
					m_doc->AddDiv( m_div );
				}
				else {
					delete m_div;
				}
				m_div = NULL;
			}
        }
        
        // reading the layouts
        if ( music && music->hasChildren("layouts") ) {
            layouts = music->getChildrenByName("layouts")[0];
        }
        if ( layouts && layouts->hasChildren("layout") ) {
			vector<MeiElement*> children = layouts->getChildrenByName("layout");
			for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
				MeiElement *e = *iter;
                ReadMeiLayout( dynamic_cast<Layout*>(e));
			}
        }
        return true;
    }
    catch( char * str ) {
        wxLogError("%s", str );
        return false;
   }
}

bool MusMeiInput::ReadMeiHeader( MeiHead *meiHead )
{
    return true;
}

bool MusMeiInput::ReadMeiDiv( Mdiv *mdiv )
{		
	if ( mdiv && mdiv->hasChildren("score") ) {
		vector<MeiElement*> children = mdiv->getChildrenByName("score");
		if ( dynamic_cast<Score*> (children[0]) ) {
			m_score = new MusScore( );
            SetMeiUuid( children[0], m_score );
			if (ReadMeiScore(dynamic_cast<Score*> (children[0]))) {
				m_div->AddScore(m_score);
				return true;
			}
			else {
				delete m_score;
				return false;
			}
		}
	} else if ( mdiv && mdiv->hasChildren("parts") ) {
		vector<MeiElement*> children = mdiv->getChildrenByName("parts");
		if ( dynamic_cast<Parts*> (children[0]) ) {
			m_parts = new MusPartSet( );
            SetMeiUuid( children[0], m_parts );
			if (ReadMeiParts(dynamic_cast<Parts*> (children[0]))) {
				m_div->AddPartSet( m_parts );
				return true;
			}
			else {
				delete m_parts;
				return false;
			}
		}
	}
    return false;
}

bool MusMeiInput::ReadMeiScore( Score *score )
{
	if ( score && score->hasChildren("section") ) {
		vector<MeiElement*> children = score->getChildrenByName("section");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_section = new MusSection( );
            SetMeiUuid( e, m_section );
			if (ReadMeiSection(dynamic_cast<Section*>(e))) {
				m_score->AddSection( m_section );
			}
			else {
				delete m_section;
			}
			m_section = NULL;
		}
		// success only if at least one section was added to the score
		return (m_score->m_sections.GetCount() > 0);
	}
	return false;
}

bool MusMeiInput::ReadMeiParts( Parts * parts )
{
	if ( parts && parts->hasChildren("part") ) {
		vector<MeiElement*> children = parts->getChildrenByName("part");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_part = new MusPart(  );
            SetMeiUuid( e, m_part );
			if (ReadMeiPart(dynamic_cast<Part*>(e))) {
				m_parts->AddPart( m_part );
			}
			else {
				delete m_part;
			}
			m_part = NULL;
		}
		// success only if at least one part was added to the parts
		return (m_parts->m_parts.GetCount() > 0);
	}
	return false;
}

bool MusMeiInput::ReadMeiPart( Part *part )
{
	if ( part && part->hasChildren("section") ) {
		vector<MeiElement*> children = part->getChildrenByName("section");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_section = new MusSection( );
            SetMeiUuid( e, m_section );
			if (ReadMeiSection(dynamic_cast<Section*>(e))) {
				m_part->AddSection( m_section );
			}
			else {
				delete m_section;
			}
			m_section = NULL;
		}
		// success only if at least one section was added to the score
		return (m_part->m_sections.GetCount() > 0);
	}
	return false;
}

bool MusMeiInput::ReadMeiSection( Section *section )
{
	if ( section && section->hasChildren("measure") ) {
		vector<MeiElement*> children = section->getChildrenByName("measure");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_measure = new MusMeasure( );
            SetMeiUuid( e, m_measure );
			if (ReadMeiMeasure(dynamic_cast<Measure*>(e))) {
				m_section->AddMeasure( m_measure );
			}
			else {
				delete m_measure;
			}
			m_measure = NULL;
		}
		// success only if at least one measure was added to the section
		return (m_section->m_measures.GetCount() > 0);
	} else if ( section && section->hasChildren("staff") ) {
		vector<MeiElement*> children = section->getChildrenByName("staff");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_staff= new MusStaff( );
            SetMeiUuid( e, m_staff );
			if (ReadMeiStaff(dynamic_cast<Staff*>(e))) {
				m_section->AddStaff( m_staff );
			}
			else {
				delete m_staff;
			}
			m_staff = NULL;
		}
		// success only if at least one staff was added to the section
		return (m_section->m_staves.GetCount() > 0);
	}
    return false;
}

bool MusMeiInput::ReadMeiMeasure( Measure *measure )
{
	if ( measure && measure->hasChildren("staff") ) {
		vector<MeiElement*> children = measure->getChildrenByName("staff");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_staff = new MusStaff( );
            SetMeiUuid( e, m_staff );
			if (ReadMeiStaff(dynamic_cast<Staff*>(e))) {
				m_measure->AddStaff( m_staff );
			}
			else {
				delete m_staff;
			}
			m_staff = NULL;
		}
		// success only if at least one staff was added to the measure
		return (m_measure->m_staves.GetCount() > 0);
	}
    return false;
}

bool MusMeiInput::ReadMeiStaff( Staff *staff )
{
	if ( staff && staff->hasChildren("layer") ) {
		vector<MeiElement*> children = staff->getChildrenByName("layer");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_layer = new MusLayer( );
            SetMeiUuid( e, m_layer );
			if (ReadMeiLayer(dynamic_cast<Layer*>(e))) {
				m_staff->AddLayer( m_layer );
			}
			else {
				delete m_layer;
			}
			m_layer = NULL;
		}
		// success only if at least one layer was added to the staff
		return (m_staff->m_layers.GetCount() > 0);
	}
    return false;
}

bool MusMeiInput::ReadMeiLayer( Layer *layer )
{
	if ( layer && layer->hasChildren() ) {
		vector<MeiElement*> children = layer->getChildren();
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
            if (e->getName()=="barLine") {
				if (!ReadMeiBarline(dynamic_cast<BarLine*>(e))) {
					return false;
				}
			}
            else if (e->getName()=="beam") {
				if (!ReadMeiBeam(dynamic_cast<Beam*>(e))) {
					return false;
				}
			}
            else if (e->getName()=="clef") {
				if (!ReadMeiClef(dynamic_cast<Clef*>(e))) {
					return false;
				}
			}
            else if (e->getName()=="mensur") {
				if (!ReadMeiMensur(dynamic_cast<Mensur*>(e))) {
					return false;
				}
			}
            else if (e->getName()=="note") {
				if (!ReadMeiNote(dynamic_cast<Note*>(e))) {
					return false;
				}
			}
            else if (e->getName()=="rest") {
				if (!ReadMeiRest(dynamic_cast<Rest*>(e))) {
					return false;
				}
			}
            // symbols
            else if (e->getName()=="accid") {
				if (!ReadMeiSymbol(dynamic_cast<Accid*>(e))) {
					return false;
				}
			}
            else if (e->getName()=="custos") {
				if (!ReadMeiSymbol(dynamic_cast<Custos*>(e))) {
					return false;
				}
			}
            else if (e->getName()=="dot") {
				if (!ReadMeiSymbol(dynamic_cast<Dot*>(e))) {
					return false;
				}
			}
            // unkown            
			else {
				wxLogDebug("LayerElement %s ignored", e->getName().c_str() );
			}

		}
		// success in any case
		return true;
	}
    return false;
}

bool MusMeiInput::ReadMeiBarline( BarLine *barline )
{
    MusBarline *musBarline = new MusBarline();
    SetMeiUuid( barline, musBarline );
    
    m_layer->AddLayerElement( musBarline );
    return true;
}

bool MusMeiInput::ReadMeiBeam( Beam *beam )
{
    MusBeam *musBeam = new MusBeam();
    SetMeiUuid( beam, musBeam );
    // we add it before the notes
    m_layer->AddLayerElement( musBeam );
    
	if ( beam && beam->hasChildren() ) {
		vector<MeiElement*> children = beam->getChildren();
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
            if (e->getName()=="note") {
				if (!ReadMeiNote(dynamic_cast<Note*>(e))) {
					return false;
				}
                musBeam->AddNote( &m_layer->m_elements.Last() );
			}
            else if (e->getName()=="rest") {
				if (!ReadMeiRest(dynamic_cast<Rest*>(e))) {
					return false;
				}
                musBeam->AddNote( &m_layer->m_elements.Last() );
			}
            // unkown            
			else {
				wxLogDebug("LayerElement %s ignored", e->getName().c_str() );
			}
        }
    }
    
    if ( musBeam->m_notes.Count() < 2 ) {
        // this does everything we need, that is:
        // - removing the MusBeam from the m_layer
        // - detaching the note
        // - changing the flag of the note
        // - delete the MusBeam
        delete musBeam;
        wxLogWarning("Beam element with only zero or one note");
    }
    
    return true;
}

bool MusMeiInput::ReadMeiClef( Clef *clef )
{ 
    MusClef *musClef = new MusClef();
    SetMeiUuid( clef, musClef );
    if ( clef->m_Clefshape.hasShape( ) && clef->m_Lineloc.hasLine( ) ) {
        musClef->m_clefId = StrToClef( clef->m_Clefshape.getShape()->getValue(), clef->m_Lineloc.getLine()->getValue() );
    }
    
    m_layer->AddLayerElement( musClef );
    return true;
}

bool MusMeiInput::ReadMeiMensur( Mensur *mensur )
{
    MusMensur *musMensur = new MusMensur();
    SetMeiUuid( mensur, musMensur );
    if ( mensur->m_MensurLog.hasSign( ) ) {
        musMensur->m_sign = StrToMensurSign( mensur->m_MensurLog.getSign()->getValue() );
    }
    if ( mensur->m_MensurLog.hasDot( ) ) {
        musMensur->m_dot = ( mensur->m_MensurLog.getDot()->getValue() == "true" );
    }
    if ( mensur->m_Slashcount.hasSlash( ) ) {
        musMensur->m_slash =  1; //atoi( mensur->m_Slashcount.getSlash()->getValue() );
    }
    if ( mensur->m_MensurVis.hasOrient( ) ) {
        musMensur->m_reversed = ( mensur->m_MensurVis.getOrient()->getValue() == "reversed" );
    }
    if ( mensur->m_DurationRatio.hasNum( ) ) {
        musMensur->m_num = atoi ( mensur->m_DurationRatio.getNum()->getValue().c_str() );
    }
    if ( mensur->m_DurationRatio.hasNumbase( ) ) {
        musMensur->m_numBase = atoi ( mensur->m_DurationRatio.getNumbase()->getValue().c_str() );
    }
    // missing m_meterSymb
    
    m_layer->AddLayerElement( musMensur );
    return true;
}

bool MusMeiInput::ReadMeiNote( Note *note )
{
	MusNote *musNote = new MusNote();
    SetMeiUuid( note, musNote );
	// pitch
	if ( note->m_Pitch.hasPname() ) {
		musNote->m_pname = StrToPitch( note->m_Pitch.getPname()->getValue() );
	}
	// oct
	if ( note->m_Octave.hasOct() ) {
		musNote->m_oct = StrToOct( note->m_Octave.getOct()->getValue() );
	}
	// duration
	if ( note->m_DurationMusical.hasDur() ) {
		musNote->m_dur = StrToDur( note->m_DurationMusical.getDur()->getValue() );
	}
    // dots
    if ( note->m_Augmentdots.hasDots() ) {
		musNote->m_dots = atoi( note->m_Augmentdots.getDots()->getValue().c_str() );
	}
    // accid
    if ( note->m_Accidental.hasAccid() ) {
		musNote->m_accid = StrToAccid( note->m_Accidental.getAccid()->getValue() );
	}
	
	m_layer->AddLayerElement( musNote );
    return true;
}

bool MusMeiInput::ReadMeiRest( Rest *rest )
{
    MusRest *musRest = new MusRest();
    SetMeiUuid( rest, musRest );
	// duration
	if ( rest->m_DurationMusical.hasDur() ) {
		musRest->m_dur = StrToDur( rest->m_DurationMusical.getDur()->getValue() );
	}
    if ( rest->m_Augmentdots.hasDots() ) {
		musRest->m_dots = atoi( rest->m_Augmentdots.getDots()->getValue().c_str() );
	}
    // missing position
	
	m_layer->AddLayerElement( musRest );
    return true;
}

bool MusMeiInput::ReadMeiSymbol( Accid *accid )
{
    MusSymbol *musAccid = new MusSymbol( SYMBOL_ACCID );
    SetMeiUuid( accid, musAccid );
    if ( accid->m_Accidental.hasAccid() ) {
        musAccid->m_accid = StrToAccid( accid->m_Accidental.getAccid()->getValue() );
    }
    // missing position
	
	m_layer->AddLayerElement( musAccid );
    return true;
}

bool MusMeiInput::ReadMeiSymbol( Custos *custos )
{
    MusSymbol *musCustos = new MusSymbol( SYMBOL_CUSTOS );
    SetMeiUuid( custos, musCustos );
	// position (pitch)
	if ( custos->m_Pitch.hasPname() ) {
		musCustos->m_pname = StrToPitch( custos->m_Pitch.getPname()->getValue() );
	}
	// oct
	if ( custos->m_Octave.hasOct() ) {
		musCustos->m_oct = StrToOct( custos->m_Octave.getOct()->getValue() );
	}
	
	m_layer->AddLayerElement( musCustos );    
    return true;
}

bool MusMeiInput::ReadMeiSymbol( Dot *dot )
{
    MusSymbol *musDot = new MusSymbol( SYMBOL_DOT );
    SetMeiUuid( dot, musDot );
    musDot->m_dot = 0;
    // missing m_dots
    // missing position
	
	m_layer->AddLayerElement( musDot );
    return true;
}


bool MusMeiInput::ReadMeiLayout( Layout *layout )
{
    LayoutType type;
    if ( layout->m_Typed.hasType() ) {
        type = StrToLayoutType( layout->m_Typed.getType()->getValue().c_str() );
    }
    else {
        wxLogWarning( "@type missing in layout element" );
        return false;
    }

    m_layout = new MusLayout( type );
    SetMeiUuid( layout, m_layout );
    
	if ( layout && layout->hasChildren("page") ) {
		vector<MeiElement*> children = layout->getChildrenByName("page");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_page = new MusPage( );
            SetMeiUuid( e, m_page );
			if (ReadMeiPage(dynamic_cast<Page*>(e))) {
				m_layout->AddPage( m_page );
			}
			else {
				delete m_page;
			}
			m_page = NULL;
		}
	}
    
    if (m_layout->GetPageCount() > 0) {
        m_doc->AddLayout( m_layout );
    }
    else {
        delete m_layout;
    }
    m_layout = NULL;
    return true;
}


bool MusMeiInput::ReadMeiPage( Page *page )
{
    if ( page->m_ScoreDefVis.hasPageHeight() ) {
        m_page->m_pageHeight = atoi ( page->m_ScoreDefVis.getPageHeight()->getValue().c_str() );
    }
    if ( page->m_ScoreDefVis.hasPageWidth() ) {
        m_page->m_pageWidth = atoi ( page->m_ScoreDefVis.getPageWidth()->getValue().c_str() );
    }
    if ( page->m_ScoreDefVis.hasPageLeftmar() ) {
        m_page->m_pageLeftMar = atoi ( page->m_ScoreDefVis.getPageLeftmar()->getValue().c_str() );
    }
    if ( page->m_ScoreDefVis.hasPageRightmar() ) {
        m_page->m_pageRightMar = atoi ( page->m_ScoreDefVis.getPageRightmar()->getValue().c_str() );
    }
    
	if ( page && page->hasChildren("system") ) {
		vector<MeiElement*> children = page->getChildrenByName("system");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
			m_system = new MusSystem( );
            SetMeiUuid( e, m_system );
			if (ReadMeiSystem(dynamic_cast<System*>(e))) {
				m_page->AddSystem( m_system );
			}
			else {
				delete m_system;
			}
			m_system = NULL;
		}
		// success only if at least one system was added to the page
		return (m_page->GetSystemCount() > 0);
	}
    return false;
}

bool MusMeiInput::ReadMeiSystem( System *system )
{
    if ( system->m_ScoreDefVis.hasSystemLeftmar() ) {
        m_system->m_systemLeftMar = atoi ( system->m_ScoreDefVis.getSystemLeftmar()->getValue().c_str() );
    }
    if ( system->m_ScoreDefVis.hasSystemRightmar() ) {
        m_system->m_systemRightMar = atoi ( system->m_ScoreDefVis.getSystemRightmar()->getValue().c_str() );
    }
    if ( system->m_Coordinated.hasUly() ) {
        m_system->m_y_abs = atoi ( system->m_Coordinated.getUly()->getValue().c_str() );
    }
    
	if ( system && system->hasChildren("laidOutStaff") ) {
		vector<MeiElement*> children = system->getChildrenByName("laidOutStaff");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
            ReadMeiLaidOutStaff(dynamic_cast<LaidOutStaff*>(e));
		}
		// success only if at least one staff was added to the system
		return (m_system->GetStaffCount() > 0);
	}
    return false;
}

bool MusMeiInput::ReadMeiLaidOutStaff( LaidOutStaff *staff  )
{
    int logStaffNb = -1;
    if ( staff->m_Staffident.hasStaff() ) {
        logStaffNb = atoi ( staff->m_Staffident.getStaff()->getValue().c_str() );
    }
    else {
        // no idea what will happen if this is missing...
        wxLogWarning( "@staff missing in laidOutStaff element" );
        return false;
    }
    
    m_laidOutStaff = new MusLaidOutStaff( logStaffNb );
    SetMeiUuid( staff, m_laidOutStaff );
    
    if ( staff->m_Coordinated.hasUly() ) {
        m_laidOutStaff->m_y_abs = atoi ( staff->m_Coordinated.getUly()->getValue().c_str() );
    }
    if ( staff->m_Typed.hasType() ) {
        // we use type only for typing mensural notation
        m_laidOutStaff->notAnc = true;
    }
    
	if ( staff && staff->hasChildren("laidOutLayer") ) {
		vector<MeiElement*> children = staff->getChildrenByName("laidOutLayer");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
            ReadMeiLaidOutLayer(dynamic_cast<LaidOutLayer*>(e));
		}
	}
    
    m_system->AddStaff( m_laidOutStaff );
    m_laidOutStaff = NULL;
    return true;
}


bool MusMeiInput::ReadMeiLaidOutLayer( LaidOutLayer *layer )
{
    int logLayerNb = -1;
    int logStaffNb = -1;
    MusSection *section = NULL;
    MusMeasure *measure = NULL;
    if ( layer->m_Layerident.hasLayer() ) {
        logLayerNb = atoi ( layer->m_Layerident.getLayer()->getValue().c_str() );
    }
    else {
        // no idea what will happen if this is missing...
        wxLogWarning( "@layer missing in laidOutLayer element" );
        return false;
    }
    if ( layer->m_Staffident.hasStaff() ) {
        logStaffNb = atoi ( layer->m_Staffident.getStaff()->getValue().c_str() );
    }
    else {
        // no idea what will happen if this is missing...
        wxLogWarning( "@staff missing in laidOutLayer element" );
        return false;
    }
    if ( layer->m_Pointing.hasTarget() ) {
        uuid_t uuid;
        StrToUuid( layer->m_Pointing.getTarget()->getValue(), uuid );
        MusFunctor findTargetUuid( &MusObject::FindWithUuid );
        MusObject *target = m_doc->FindLogicalObject( &findTargetUuid, uuid );
        if ( dynamic_cast<MusSection*>(target) ) {
            section =  dynamic_cast<MusSection*>(target);
        }
        else if ( dynamic_cast<MusMeasure*>(target) ) {
            measure = dynamic_cast<MusMeasure*>(target);
        }
        else {
            // no idea what will happen if this is missing...
            wxLogWarning( "The laidOutLayer target could not be found" ); 
            return false;
        }
    }
    else {
        // no idea what will happen if this is missing...
        wxLogWarning( "@target missing in laidOutLayer element" );
        return false;
    }
        
    m_laidOutLayer = new MusLaidOutLayer( logLayerNb, logStaffNb, section, measure );
    SetMeiUuid( layer, m_laidOutLayer );
    
	if ( layer && layer->hasChildren("laidOutElement") ) {
		vector<MeiElement*> children = layer->getChildrenByName("laidOutElement");
		for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
			MeiElement *e = *iter;
            ReadMeiLaidOutElement(dynamic_cast<LaidOutElement*>(e));
        }
	}
    
    m_laidOutStaff->AddLayer( m_laidOutLayer );
    m_laidOutLayer = NULL;
    return true;
}

bool MusMeiInput::ReadMeiLaidOutElement( LaidOutElement *laidOutElement )
{
    
    MusLayerElement *element = NULL;
    if ( laidOutElement->m_Pointing.hasTarget() ) {
        uuid_t uuid;
        StrToUuid( laidOutElement->m_Pointing.getTarget()->getValue(), uuid );
        MusFunctor findTargetUuid( &MusObject::FindWithUuid );
        MusObject *target = m_doc->FindLogicalObject( &findTargetUuid, uuid );
        if ( dynamic_cast<MusLayerElement*>(target) ) {
            element =  dynamic_cast<MusLayerElement*>(target);
        }
        else {
            // no idea what will happen if this is missing...
            wxLogWarning( "The laidOutElement target could not be found" ); 
            return false;
        }
    }
    else {
        // no idea what will happen if this is missing...
        wxLogWarning( "@target missing in laidOutElement element" );
        return false;
    }
    
    MusLaidOutLayerElement *laidOutLayerElement = new MusLaidOutLayerElement( element );
    SetMeiUuid( laidOutElement, laidOutLayerElement );
    
    if ( laidOutElement->m_Coordinated.hasUlx() ) {
        laidOutLayerElement->m_x_abs = atoi ( laidOutElement->m_Coordinated.getUlx()->getValue().c_str() );
    }
    
    m_laidOutLayer->AddElement( laidOutLayerElement );
    return true;
}


void MusMeiInput::SetMeiUuid( MeiElement *element, MusObject *object )
{
    if ( !element->hasId() ) {
        return;
    }
    
    uuid_t uuid;
    StrToUuid( element->getId(), uuid );
    object->SetUuid( uuid );
}

void MusMeiInput::StrToUuid(std::string uuid, uuid_t dest)
{
    uuid_clear( dest );
    if ( uuid.length() != 38 ) {
        return;
    }
    if ( uuid.compare( 0, 2, "m-" ) != 0 ) {
        return;
    }
    uuid.erase( 0, 2 );
    uuid_parse( uuid.c_str(), dest );
}

int MusMeiInput::StrToDur(std::string dur)
{
    int value;
    if (dur == "longa") value = DUR_LG;
    else if (dur == "brevis") value = DUR_BR;
    else if (dur == "semibrevis") value = DUR_1;
    else if (dur == "minima") value = DUR_2;
    else if (dur == "semiminima") value = DUR_4;
    else if (dur == "fusa") value = DUR_8;
    else if (dur == "semifusa") value = DUR_16;
    else if (dur == "long") value = DUR_LG;
    else if (dur == "breve") value = DUR_BR;
    else if (dur == "1") value = DUR_1;
    else if (dur == "2") value = DUR_2;
    else if (dur == "4") value = DUR_4;
    else if (dur == "8") value = DUR_8;
    else if (dur == "16") value = DUR_16;
	else {
		wxLogWarning("Unknow duration '%s'", dur.c_str());
        value = DUR_4;
	}
    return value;
}

int MusMeiInput::StrToOct(std::string oct)
{
	return atoi(oct.c_str());
}

int MusMeiInput::StrToPitch(std::string pitch)
{
    int value;
    if (pitch == "c") value = PITCH_C;
    else if (pitch == "d") value = PITCH_D;
    else if (pitch == "e") value = PITCH_E;
    else if (pitch == "f") value = PITCH_F;
    else if (pitch == "g") value = PITCH_G;
    else if (pitch == "a") value = PITCH_A;
    else if (pitch == "b") value = PITCH_B;
    else {
		wxLogWarning("Unknow pname '%s'", pitch.c_str());
        value = PITCH_C;
    }
    return value;
}


unsigned char MusMeiInput::StrToAccid(std::string accid)
{
    unsigned char value;
    if ( accid == "s" ) value = ACCID_SHARP;
    else if ( accid == "f" ) value = ACCID_FLAT;
    else if ( accid == "n" ) value = ACCID_NATURAL;
    else if ( accid == "x" ) value = ACCID_DOUBLE_SHARP;
    else if ( accid == "ff" ) value = ACCID_DOUBLE_FLAT;
    else if ( accid == "ns" ) value = ACCID_QUARTER_SHARP;
    else if ( accid == "nf" ) value = ACCID_QUARTER_FLAT;
    else {
        wxLogWarning("Unknown accid '%s'", accid.c_str() );
    }
	return value;
}


ClefId MusMeiInput::StrToClef( std::string shape, std::string line )
{
    ClefId clefId = SOL2;
    std::string clef = shape + line;
    if ( clef == "G2" ) clefId = SOL2;
    else if ( clef == "G1" ) clefId = SOL1; 
    else if ( clef == "F5" ) clefId = FA5;
    else if ( clef == "F4" ) clefId = FA4; 
    else if ( clef == "F3" ) clefId = FA3; 
    else if ( clef == "C1" ) clefId = UT1; 
    else if ( clef == "C2" ) clefId = UT2; 
    else if ( clef == "C3" ) clefId = UT3; 
    else if ( clef == "C4" ) clefId = UT4; 
    else if ( clef == "C5" ) clefId = UT5; 
    else 
    {
        wxLogWarning("Unknown clef shape '%s' line '%s'", shape.c_str(), line.c_str() );
    }
    return clefId;
}

MensurSign MusMeiInput::StrToMensurSign(std::string sign)
{
    if (sign == "C") return MENSUR_SIGN_C;
    else if (sign == "O") return MENSUR_SIGN_O;
    else {
        wxLogWarning("Unknown mensur sign '%s'", sign.c_str() );
	}
    // default
	return MENSUR_SIGN_C;
}

LayoutType MusMeiInput::StrToLayoutType(std::string type)
{
    if (type == "rendering") return Rendering;
    else if (type == "transcription") return Transcription;
    else {
        wxLogWarning("Unknown layout type '%s'", type.c_str() );
	}
    // default
	return Transcription;
}



