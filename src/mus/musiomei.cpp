/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei.cpp
// Author:      Laurent Pugin
// Created:     2008
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musiomei.h"

//----------------------------------------------------------------------------

#include <algorithm>
#include <ctime>
#include <sstream>

//----------------------------------------------------------------------------

#include "mus.h"
#include "musapp.h"
#include "musbarline.h"
#include "musbeam.h"
#include "musclef.h"
#include "muskeysig.h"
#include "muslayer.h"
#include "muslayerelement.h"
#include "musmeasure.h"
#include "musmensur.h"
#include "musmultirest.h"
#include "musnote.h"
#include "muspage.h"
#include "musrest.h"
#include "musscoredef.h"
#include "musstaff.h"
#include "mussymbol.h"
#include "mussystem.h"
#include "mustuplet.h"

//----------------------------------------------------------------------------
// MusMeiOutput
//----------------------------------------------------------------------------

MusMeiOutput::MusMeiOutput( MusDoc *doc, std::string filename ) :
	MusFileOutputStream( doc )
{
    m_filename = filename;
    m_mei = NULL;
    m_pages = NULL;
    m_page = NULL;
    m_scoreDef = NULL;
    m_system = NULL;
    m_staffGrp = NULL;
    m_staffDef = NULL;
    m_measure = NULL;
    m_staff = NULL;
    m_layer = NULL;
    m_rdgLayer = NULL;
    m_beam = NULL;
}

MusMeiOutput::~MusMeiOutput()
{
}

bool MusMeiOutput::ExportFile( )
{
    try {
        TiXmlDocument *meiDoc = new TiXmlDocument();
        
        m_mei = new TiXmlElement("mei");
        m_mei->SetAttribute( "xmlns", "http://www.music-encoding.org/ns/mei" );
        m_mei->SetAttribute( "meiversion", "2013" );

        // element to place the pages
        m_pages = new TiXmlElement("pages");
        m_pages->SetAttribute( "type",  DocTypeToStr( m_doc->GetType() ).c_str() );
        
        
        // this starts the call of all the functors
        m_doc->Save( this );
        
        // after the functor has run, we have the header in m_mei and the score in m_score
        TiXmlElement *mdiv = new TiXmlElement("mdiv");
        mdiv->LinkEndChild( m_pages );
        TiXmlElement *body = new TiXmlElement("body");
        body->LinkEndChild( mdiv );
        TiXmlElement *music = new TiXmlElement("music");
        music->LinkEndChild( body );
        m_mei->LinkEndChild( music );
        
        TiXmlUnknown *schema = new TiXmlUnknown();
        schema->SetValue("?xml-model href=\"http://www.aruspix.net/mei-page-based-2013-08-29.rng\" type=\"application/xml\" schematypens=\"http://relaxng.org/ns/structure/1.0\"?");
        
        meiDoc->LinkEndChild( new TiXmlDeclaration( "1.0", "UTF-8", "" ) );
        meiDoc->LinkEndChild(schema);
        meiDoc->LinkEndChild(m_mei);
        meiDoc->SaveFile( m_filename.c_str() );
    }
    catch( char * str ) {
        Mus::LogError("%s", str );
        return false;
    }
	return true;    
}

std::string MusMeiOutput::UuidToMeiStr( MusObject *element )
{
    std::string out = element->GetUuid();
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    Mus::LogDebug("uuid: %s\n", out.c_str());
    return out;
}

bool MusMeiOutput::WriteDoc( MusDoc *doc )
{
    assert( m_mei );
    
    // ---- header ----
    TiXmlElement *meiHead = new TiXmlElement("meiHead");
    
    TiXmlElement *fileDesc = new TiXmlElement("fileDesc");
    TiXmlElement *titleStmt = new TiXmlElement("titleStmt");
    fileDesc->LinkEndChild(titleStmt);
    TiXmlElement *title = new TiXmlElement("title");
    titleStmt->LinkEndChild(title);
    TiXmlElement *pubStmt = new TiXmlElement("pubStmt");
    fileDesc->LinkEndChild(pubStmt);
    TiXmlElement *date = new TiXmlElement("date");
    pubStmt->LinkEndChild(date);
    
    TiXmlElement *encodingDesc = new TiXmlElement("encodingDesc");
    TiXmlElement *projectDesc = new TiXmlElement("projectDesc");
    encodingDesc->LinkEndChild(projectDesc);
    TiXmlElement *p1 = new TiXmlElement("p");
    projectDesc->LinkEndChild(p1);
    
    p1->LinkEndChild( new TiXmlText( Mus::StringFormat( "Encoded with Aruspix version %s",  Mus::GetAxVersion().c_str() ).c_str() ) );
    
    // date
    time_t now = time(0);
    date->LinkEndChild( new TiXmlText( ctime( &now ) ) );
    
    meiHead->LinkEndChild(fileDesc);
    meiHead->LinkEndChild(encodingDesc);
    m_mei->LinkEndChild( meiHead );
    
    return true;
}


bool MusMeiOutput::WritePage( MusPage *page )
{
    assert( m_pages );
    m_page = new TiXmlElement("page");
    m_page->SetAttribute( "xml:id",  UuidToMeiStr( page ).c_str() );
    // size and margins but only if any - we rely on page.height only to check this
    if ( page->m_pageHeight != -1 ) {
        m_page->SetAttribute( "page.width", Mus::StringFormat( "%d", page->m_pageWidth ).c_str() );
        m_page->SetAttribute( "page.height", Mus::StringFormat( "%d", page->m_pageHeight ).c_str() );
        m_page->SetAttribute( "page.leftmar", Mus::StringFormat( "%d", page->m_pageLeftMar ).c_str() );
        m_page->SetAttribute( "page.rightmar", Mus::StringFormat( "%d", page->m_pageRightMar ).c_str() );
        m_page->SetAttribute( "page.rightmar", Mus::StringFormat( "%d", page->m_pageRightMar ).c_str() );
    }
    if ( !page->m_surface.empty() ) {
        m_page->SetAttribute( "surface", page->m_surface.c_str() );
    }
    //
    TiXmlComment *comment = new TiXmlComment();
    comment->SetValue( "Coordinates in MEI axis direction" );
    m_pages->LinkEndChild( comment );
    m_pages->LinkEndChild( m_page );
    return true;
}

bool MusMeiOutput::WriteSystem( MusSystem *system )
{
    assert( m_page );
    m_system = new TiXmlElement("system");
    m_system->SetAttribute( "xml:id",  UuidToMeiStr( system ).c_str() );
    // margins
    m_system->SetAttribute( "system.leftmar", Mus::StringFormat( "%d", system->m_systemLeftMar ).c_str() );
    m_system->SetAttribute( "system.rightmar", Mus::StringFormat( "%d", system->m_systemRightMar ).c_str() );
    // y positions
    m_system->SetAttribute( "uly", Mus::StringFormat( "%d", system->m_y_abs ).c_str() );
    m_page->LinkEndChild( m_system );
    return true;
}

bool MusMeiOutput::WriteScoreDef( MusScoreDef *scoreDef )
{
    assert( m_system );
    m_scoreDef = new TiXmlElement("scoreDef");
    m_scoreDef->SetAttribute( "xml:id",  UuidToMeiStr( scoreDef ).c_str() );
    if (scoreDef->GetClefAttr()) {
        m_scoreDef->SetAttribute( "clef.line", ClefLineToStr( scoreDef->GetClefAttr()->m_clefId ).c_str() );
        m_scoreDef->SetAttribute( "clef.shape", ClefShapeToStr( scoreDef->GetClefAttr()->m_clefId ).c_str() );
        // we should add 8va attr
    }
    if (scoreDef->GetKeySigAttr()) {
        m_scoreDef->SetAttribute( "key.sig", KeySigToStr( scoreDef->GetKeySigAttr()->m_num_alter,
                                                         scoreDef->GetKeySigAttr()->m_alteration ).c_str() );
    }
    // this needs to be fixed
    m_page->LinkEndChild( m_system );
    return true;
    
}

bool MusMeiOutput::WriteStaffGrp( MusStaffGrp *staffGrp )
{
    // for now only as part of a system - this needs to be fixed
    assert( m_system );
    m_staffGrp = new TiXmlElement("staffGrp");
    m_staffGrp->SetAttribute( "xml:id",  UuidToMeiStr( staffGrp ).c_str() );
    m_system->LinkEndChild( m_staffGrp );
    return true;
}

bool MusMeiOutput::WriteStaffDef( MusStaffDef *staffDef )
{
    assert( m_staffGrp );
    m_staffDef = new TiXmlElement("staffDef");
    m_staffDef->SetAttribute( "xml:id",  UuidToMeiStr( staffDef ).c_str() );
    m_staffDef->SetAttribute( "n", Mus::StringFormat( "%d", staffDef->GetStaffNo() ).c_str() );
    if (staffDef->GetClefAttr()) {
        m_staffDef->SetAttribute( "clef.line", ClefLineToStr( staffDef->GetClefAttr()->m_clefId ).c_str() );
        m_staffDef->SetAttribute( "clef.shape", ClefShapeToStr( staffDef->GetClefAttr()->m_clefId ).c_str() );
        // we should add 8va attr
    }
    if (staffDef->GetKeySigAttr()) {
        m_staffDef->SetAttribute( "key.sig", KeySigToStr( staffDef->GetKeySigAttr()->m_num_alter,
                                                         staffDef->GetKeySigAttr()->m_alteration ).c_str() );
    }

    m_staffGrp->LinkEndChild( m_staffDef );
    return true;
}

bool MusMeiOutput::WriteStaff( MusStaff *staff )
{
    assert( m_system );
    m_staff = new TiXmlElement("staff");
    m_staff->SetAttribute( "xml:id",  UuidToMeiStr( staff ).c_str() );
    // y position
    if ( staff->notAnc ) {
        m_staff->SetAttribute( "label", "mensural" );
    }
    m_staff->SetAttribute( "uly", Mus::StringFormat( "%d", staff->m_y_abs ).c_str() );
    m_staff->SetAttribute( "n", Mus::StringFormat( "%d", staff->GetStaffNo() ).c_str() );

    m_system->LinkEndChild( m_staff );
    return true;
}

bool MusMeiOutput::WriteMeasure( MusMeasure *measure )
{
    assert( m_staff );
    m_measure = new TiXmlElement("measure");
    m_measure->SetAttribute( "xml:id",  UuidToMeiStr( measure ).c_str() );
    m_measure->SetAttribute( "n", Mus::StringFormat( "%d", measure->m_logMeasureNb ).c_str() );
    m_staff->LinkEndChild( m_measure );
    return true;
}

bool MusMeiOutput::WriteLayer( MusLayer *layer )
{
    assert( m_staff );
    m_layer = new TiXmlElement("layer");
    m_layer->SetAttribute( "xml:id",  UuidToMeiStr( layer ).c_str() );
    m_layer->SetAttribute( "n", Mus::StringFormat( "%d", layer->GetLayerNo() ).c_str() );
    if ( m_measure ) {
        m_measure->LinkEndChild( m_layer );
    }
    else {
        m_staff->LinkEndChild( m_layer );
    }
    return true;
}

bool MusMeiOutput::WriteLayerElement( MusLayerElement *element )
{
    assert( m_layer );
    
    // Here we look at what is the parent.
    // For example, if we are in a beam, we must attach it to the beam xml element (m_beam)
    // By default, we attach it to m_layer
    TiXmlElement *currentParent = m_layer;
    if ( dynamic_cast<MusLayerRdg*>(element->m_parent) ) {
        assert( m_rdgLayer );
        currentParent = m_rdgLayer;
    }
    else if ( dynamic_cast<MusBeam*>(element->m_parent) ) {
        assert( m_beam );
        currentParent = m_beam;
    }
    else if ( dynamic_cast<MusTuplet*>(element->m_parent) ) {
        assert( m_tuplet );
        currentParent = m_tuplet;
    }
    // we should do the same for any MusLayerElement container (slur, tuplet, etc. )
    
    TiXmlElement *xmlElement = NULL;
    if (dynamic_cast<MusBarline*>(element)) {
        xmlElement = new TiXmlElement( "barline" );
        WriteMeiBarline( xmlElement, dynamic_cast<MusBarline*>(element) );
    }
    else if (dynamic_cast<MusBeam*>(element)) {
        xmlElement = new TiXmlElement("beam");
        m_beam = xmlElement;
        WriteMeiBeam( xmlElement, dynamic_cast<MusBeam*>(element) );
    }
    else if (dynamic_cast<MusClef*>(element)) {
        xmlElement = new TiXmlElement("clef");
        WriteMeiClef( xmlElement, dynamic_cast<MusClef*>(element) );
    }
    else if (dynamic_cast<MusMensur*>(element)) {
        xmlElement = new TiXmlElement("mensur");
        WriteMeiMensur( xmlElement, dynamic_cast<MusMensur*>(element) );
    }
    else if (dynamic_cast<MusMultiRest*>(element)) {
        xmlElement = new TiXmlElement("multiRest");
        WriteMeiMultiRest( xmlElement, dynamic_cast<MusMultiRest*>(element) );
    }
    else if (dynamic_cast<MusNote*>(element)) {
        xmlElement = new TiXmlElement("note");
        WriteMeiNote( xmlElement, dynamic_cast<MusNote*>(element) );
    }
    else if (dynamic_cast<MusRest*>(element)) {
        xmlElement = new TiXmlElement("rest");
        WriteMeiRest( xmlElement, dynamic_cast<MusRest*>(element) );
    }
    else if (dynamic_cast<MusTuplet*>(element)) {
        xmlElement = new TiXmlElement("tuplet");
        m_tuplet = xmlElement;
        WriteMeiTuplet( xmlElement, dynamic_cast<MusTuplet*>(element) );
    }
    else if (dynamic_cast<MusSymbol*>(element)) {        
        xmlElement = WriteMeiSymbol( dynamic_cast<MusSymbol*>(element) );
    }
    
    // we have it, set the uuid we read
    if ( xmlElement ) {
        this->WriteSameAsAttr( xmlElement, element );
        xmlElement->SetAttribute( "xml:id",  UuidToMeiStr( element ).c_str() );
        if ( element->m_x_abs != AX_UNSET) {
            xmlElement->SetAttribute( "ulx", Mus::StringFormat( "%d", element->m_x_abs ).c_str() );
        }
        currentParent->LinkEndChild( xmlElement );
        return true;
    }
    else {
        Mus::LogWarning( "Element class %s could not be saved", element->MusClassName().c_str() );
        return false;
    }    
}

void MusMeiOutput::WriteMeiBarline( TiXmlElement *meiBarline, MusBarline *barline )
{
    return;
}


void MusMeiOutput::WriteMeiBeam( TiXmlElement *meiBeam, MusBeam *beam )
{
    return;
}


void MusMeiOutput::WriteMeiClef( TiXmlElement *meiClef, MusClef *clef )
{
    meiClef->SetAttribute( "line", ClefLineToStr( clef->m_clefId ).c_str() );
    meiClef->SetAttribute( "shape", ClefShapeToStr( clef->m_clefId ).c_str() );
    // we should add 8va attr
    return;
}


void MusMeiOutput::WriteMeiMensur( TiXmlElement *meiMensur, MusMensur *mensur )
{
    if ( mensur->m_sign ) {
        meiMensur->SetAttribute( "sign", MensurSignToStr( mensur->m_sign ).c_str() );
    }
    if ( mensur->m_dot ) {
        meiMensur->SetAttribute( "dot", "true" );
    }
    if ( mensur->m_slash ) {
        meiMensur->SetAttribute( "slash", "1" ); // only one slash for now
    }
    if ( mensur->m_reversed ) {
        meiMensur->SetAttribute( "orient", "reversed" ); // only orientation
    }
    if ( mensur->m_num ) {
        meiMensur->SetAttribute( "num", Mus::StringFormat("%d", mensur->m_num ).c_str() );
    }
    if ( mensur->m_numBase ) {
        meiMensur->SetAttribute( "numbase", Mus::StringFormat("%d", mensur->m_numBase ).c_str() );
    }
    // missing m_meterSymb
    
    return;
}

void MusMeiOutput::WriteMeiMultiRest( TiXmlElement *meiMultiRest, MusMultiRest *multiRest )
{
    meiMultiRest->SetAttribute( "num", Mus::StringFormat("%d", multiRest->GetNumber()).c_str() );

    return;
}

void MusMeiOutput::WriteMeiNote( TiXmlElement *meiNote, MusNote *note )
{
    meiNote->SetAttribute( "pname", PitchToStr( note->m_pname ).c_str() );
    meiNote->SetAttribute( "oct", OctToStr( note->m_oct ).c_str() );
    meiNote->SetAttribute( "dur", DurToStr( note->m_dur ).c_str() );
    if ( note->m_dots ) {
        meiNote->SetAttribute( "dots", Mus::StringFormat("%d", note->m_dots).c_str() );
    }
    if ( note->m_accid ) {
        meiNote->SetAttribute( "accid", AccidToStr( note->m_accid ).c_str() );
    }
    if ( note->m_lig ) {
        if ( note->m_ligObliqua ) {
            meiNote->SetAttribute( "lig", "obliqua" );
        }
        else {
            meiNote->SetAttribute( "lig", "recta" );
        }
    }
    if ( note->m_stemDir ) {
        // this is not really correct because MusNote::m_stemDir indicates that it is opposite the normal position
        meiNote->SetAttribute( "stem.dir", "up" );
    }
    if ( note->m_colored ) {
        meiNote->SetAttribute( "colored", "true" );
    }
    // missing m_artic, m_chord, m_headShape, m_slur, m_stemLen
    return;
}

void MusMeiOutput::WriteMeiRest( TiXmlElement *meiRest, MusRest *rest )
{    
    meiRest->SetAttribute( "dur", DurToStr( rest->m_dur ).c_str() );
    if ( rest->m_dots ) {
        meiRest->SetAttribute( "dots", Mus::StringFormat("%d", rest->m_dots).c_str() );
    }
    // missing position
    meiRest->SetAttribute( "ploc", PitchToStr( rest->m_pname ).c_str() );
    meiRest->SetAttribute( "oloc", OctToStr( rest->m_oct ).c_str() );
    return;
}

TiXmlElement *MusMeiOutput::WriteMeiSymbol( MusSymbol *symbol )
{
    TiXmlElement *xmlElement = NULL;
    if (symbol->m_type==SYMBOL_ACCID) {
        TiXmlElement *accid = new TiXmlElement("accid");
        accid->SetAttribute( "accid", AccidToStr( symbol->m_accid ).c_str() );
        // position
        accid->SetAttribute( "ploc", PitchToStr( symbol->m_pname ).c_str() );
        accid->SetAttribute( "oloc", OctToStr( symbol->m_oct ).c_str() );
        xmlElement = accid;
    }
    else if (symbol->m_type==SYMBOL_CUSTOS) {
        TiXmlElement *custos = new TiXmlElement("custos");
        custos->SetAttribute( "pname", PitchToStr( symbol->m_pname ).c_str() );
        custos->SetAttribute( "oct", OctToStr( symbol->m_oct ).c_str() );
        xmlElement = custos;
    }
    else if (symbol->m_type==SYMBOL_DOT) {
        TiXmlElement *dot = new TiXmlElement("dot");
        // missing m_dots
        // position
        dot->SetAttribute( "ploc", PitchToStr( symbol->m_pname ).c_str() );
        dot->SetAttribute( "oloc", OctToStr( symbol->m_oct ).c_str() );
        xmlElement = dot;
    }
    return xmlElement;
}


void MusMeiOutput::WriteMeiTuplet( TiXmlElement *meiTuplet, MusTuplet *tuplet )
{
    return;
}

bool MusMeiOutput::WriteLayerApp( MusLayerApp *app )
{    
    assert( m_layer );
    m_app = new TiXmlElement("app");
    m_layer->LinkEndChild( m_app ); 
    return true;
}

bool MusMeiOutput::WriteLayerRdg( MusLayerRdg *rdg )
{   
    assert( m_app );
    m_rdgLayer = new TiXmlElement("rdg");
    m_rdgLayer->SetAttribute( "source", rdg->m_source.c_str() );
    m_app->LinkEndChild( m_rdgLayer ); 
    return true;
}


void MusMeiOutput::WriteSameAsAttr( TiXmlElement *meiElement, MusObject *element )
{
    if ( !element->m_sameAs.empty() ) {
        meiElement->SetAttribute( "sameas", element->m_sameAs.c_str() );
    }
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
    std::string value;
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
            Mus::LogWarning("Unknown pitch '%d'", pitch);
            value = "";
            break;
    }
	return value;
}

std::string MusMeiOutput::AccidToStr(unsigned char accid)
{
    std::string value;
    switch (accid) {
        case ACCID_SHARP: value = "s"; break;
        case ACCID_FLAT: value = "f"; break;
        case ACCID_NATURAL: value = "n"; break;
        case ACCID_DOUBLE_SHARP: value = "x"; break;
        case ACCID_DOUBLE_FLAT: value = "ff"; break;
        case ACCID_QUARTER_SHARP: value = "ns"; break;
        case ACCID_QUARTER_FLAT: value = "nf"; break;
        default: 
            Mus::LogWarning("Unknown accid '%d'", accid);
            value = "";
            break;
    }
	return value;
}

std::string MusMeiOutput::ClefLineToStr( ClefId clefId )
{	
	std::string value; 
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
            Mus::LogWarning("Unknown clef '%d'", clefId);
            value = "";
            break;
	}
	return value;
}

std::string MusMeiOutput::ClefShapeToStr( ClefId clefId )
{	
	std::string value; 
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
            Mus::LogWarning("Unknown clef '%d'", clefId);
            value = "";
            break;
	}
	return value;
}

std::string MusMeiOutput::MensurSignToStr(MensurSign sign)
{
 	std::string value; 
	switch(sign)
	{	case MENSUR_SIGN_C : value = "C"; break;
		case MENSUR_SIGN_O : value = "O"; break;		
        default: 
            Mus::LogWarning("Unknown mensur sign '%d'", sign);
            value = "";
            break;
	}
	return value;   
}


std::string MusMeiOutput::DurToStr( int dur )
{
    std::string value;
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
    else if (dur == DUR_32) value = "32";
    else if (dur == DUR_64) value = "64";
    else if (dur == DUR_128) value = "128";
	else {
		Mus::LogWarning("Unknown duration '%d'", dur);
        value = "4";
	}
    return value;
}

std::string MusMeiOutput::DocTypeToStr(DocType type)
{
 	std::string value; 
	switch(type)
	{	
        case Raw : value = "raw"; break;
        case Rendering : value = "rendering"; break;
		case Transcription : value = "transcription"; break;		
        default: 
            Mus::LogWarning("Unknown layout type '%d'", type);
            value = "";
            break;
	}
	return value;   
}


std::string MusMeiOutput::KeySigToStr(int num, char alter_type )
{
 	std::string value;
    if (num == 0) {
        return "0";
    }
	switch(alter_type)
	{	case ACCID_FLAT : value = Mus::StringFormat("%df", num); break;
		case ACCID_SHARP : value = Mus::StringFormat("%ds", num); break;
        default:
            Mus::LogWarning("Unknown key signature values '%d' and '%d", num, alter_type);
            value = "0";
            break;
	}
	return value;
}


//----------------------------------------------------------------------------
// MusMeiInput
//----------------------------------------------------------------------------

MusMeiInput::MusMeiInput( MusDoc *doc, std::string filename ) :
	MusFileInputStream( doc )
{
    m_filename = filename;
    m_doc->m_fname = Mus::GetFilename( filename );
    m_page = NULL;
    m_scoreDef = NULL;
    m_staffDef = NULL;
    m_system = NULL;
	m_staff = NULL;
    m_measure = NULL;
	m_layer = NULL;
    m_layerApp = NULL;
    m_layerRdg = NULL;
    m_beam = NULL;
    m_tuplet = NULL;
    //
    m_currentLayer = NULL;
    //
    m_hasScoreDef = false;
}

MusMeiInput::~MusMeiInput()
{
}

bool MusMeiInput::ImportFile( )
{
    try {
        m_doc->Reset( Raw );
        TiXmlDocument doc( m_filename.c_str() );
        bool loadOkay = doc.LoadFile();
        if (!loadOkay)
        {
            return false;
        }
        TiXmlElement *root = doc.RootElement();
        return ReadMei( root );
        }
    catch( char * str ) {
        Mus::LogError("%s", str );
        return false;
    }
}

bool MusMeiInput::ImportString( const std::string mei )
{
    try {
        m_doc->Reset( Raw );
        TiXmlDocument doc;
        doc.Parse( mei.c_str() );
        TiXmlElement *root = doc.RootElement();
        return ReadMei( root );
    }
    catch( char * str ) {
        Mus::LogError("%s", str );
        return false;
    }
}



bool MusMeiInput::ReadMei( TiXmlElement *root )
{
    TiXmlElement *current;
    
    if ( root && (current = root->FirstChildElement( "meiHead" ) ) )
    {
        ReadMeiHeader( current );
    }
    // music
    TiXmlElement *music = NULL;
    TiXmlElement *body = NULL;
    TiXmlElement *mdiv = NULL;
    TiXmlElement *pages = NULL;
    if ( root ) {
        music = root->FirstChildElement("music");
    }
    if ( music) {
        body = music->FirstChildElement("body");
    }
    if ( body ) {
        mdiv = body->FirstChildElement("mdiv");
    }
    if ( mdiv ) {
        pages = mdiv->FirstChildElement("pages");
    }
    if ( pages ) {
        
        // check if there is a type attribute for the score
        DocType type;
        if ( pages->Attribute( "type" ) ) {
            type = StrToDocType( pages->Attribute( "type" ) );
            m_doc->Reset( type );
        }
        
        // this is a page-based mei file, we just loop trough the pages
        if ( pages->FirstChildElement( "page" ) ) {
            for( current = pages->FirstChildElement( "page" ); current; current = current->NextSiblingElement( "page" ) ) {
                m_page = new MusPage( );
                SetMeiUuid( current, m_page );
                if (ReadMeiPage( current )) {
                    m_doc->AddPage( m_page );
                }
                else {
                    delete m_page;
                }
                m_page = NULL;
            }
        }
    }
    else {
        m_page = new MusPage( );
        m_system = new MusSystem( );
        m_page->AddSystem( m_system );
        m_doc->AddPage( m_page );
        TiXmlElement *current = NULL;
        for( current = mdiv->FirstChildElement( ); current; current = current->NextSiblingElement( ) ) {
            ReadUnsupported( current );
        }
    }
    return true;
}

bool MusMeiInput::ReadMeiHeader( TiXmlElement *meiHead )
{
    return true;
}


bool MusMeiInput::ReadMeiPage( TiXmlElement *page )
{
    assert( m_page );
    
    if ( page->Attribute( "page.height" ) ) {
        m_page->m_pageHeight = atoi ( page->Attribute( "page.height" ) );
    }
    if ( page->Attribute( "page.width" ) ) {
        m_page->m_pageWidth = atoi ( page->Attribute( "page.width" ) );
    }
    if ( page->Attribute( "page.leftmar" ) ) {
        m_page->m_pageLeftMar = atoi ( page->Attribute( "page.leftmar" ) );
    }
    if ( page->Attribute( "page.rightmar" ) ) {
        m_page->m_pageRightMar = atoi ( page->Attribute( "page.rightmar" ) );
    }
    if ( page->Attribute( "page.topmar" ) ) {
        m_page->m_pageTopMar = atoi ( page->Attribute( "page.topmar" ) );
    }
    if ( page->Attribute( "surface" ) ) {
        m_page->m_surface = page->Attribute( "surface" );
    }
    
    TiXmlElement *current = NULL;
    for( current = page->FirstChildElement( "system" ); current; current = current->NextSiblingElement( "system" ) ) {
        m_system = new MusSystem( );
        SetMeiUuid( current, m_system );
        if (ReadMeiSystem( current )) {
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

bool MusMeiInput::ReadMeiSystem( TiXmlElement *system )
{
    assert( m_system );
    assert( !m_measure );
    assert( !m_staff );
    
    if ( system->Attribute( "system.leftmar") ) {
        m_system->m_systemLeftMar = atoi ( system->Attribute( "system.leftmar" ) );
    }
    if ( system->Attribute( "system.rightmar" ) ) {
        m_system->m_systemRightMar = atoi ( system->Attribute( "system.rightmar" ) );
    }
    if ( system->Attribute( "uly" ) ) {
        m_system->m_y_abs = atoi ( system->Attribute( "uly" ) );
    }
    
    TiXmlElement *current = NULL;
    // unmeasured music
    if ( system->FirstChildElement( "staff" ) ) {
        // this is the trick for un-measured music: we add one measure ( false )
        if ( !m_measure ) {
            m_measure = new MusMeasure( false );
        }
        for( current = system->FirstChildElement( "staff" ); current; current = current->NextSiblingElement( "staff" ) ) {
            m_staff = new MusStaff( );
            SetMeiUuid( current , m_staff );
            if ( ReadMeiStaff( current )) {
                m_measure->AddStaff( m_staff );
            }
            else {
                delete m_staff;
            }
            m_staff = NULL;
        }
        if ( m_measure->GetStaffCount() > 0) {
            m_system->AddMeasure( m_measure );
        }
        else {
            delete m_measure;
        }
        m_measure = NULL;
    }
    else {
        // measured
        for( current = system->FirstChildElement( "measure" ); current; current = current->NextSiblingElement( "measure" ) ) {
            m_measure = new MusMeasure( );
            SetMeiUuid( current, m_measure );
            if (ReadMeiMeasure( current )) {
                m_system->AddMeasure( m_measure );
            }
            else {
                delete m_measure;
            }
            m_measure = NULL;
        }
    }
    
    // success only if at least one measure was added to the system
    return (m_system->GetMeasureCount() > 0);
}

bool MusMeiInput::ReadMeiScoreDef( TiXmlElement *scoreDef )
{
    assert( m_scoreDef );
    assert( m_staffGrps.empty() );
    
    if ( scoreDef->Attribute( "key.sig" ) ) {
        MusKeySig keysig(
                StrToKeySigNum( scoreDef->Attribute( "key.sig" ) ),
                StrToKeySigType( scoreDef->Attribute( "key.sig" ) ) );
        m_scoreDef->ReplaceKeySig( &keysig );
    }
    if ( scoreDef->Attribute( "clef.line" ) && scoreDef->Attribute( "clef.shape" ) ) {
        MusClef clef;
        clef.m_clefId = StrToClef( scoreDef->Attribute( "clef.shape" ) , scoreDef->Attribute( "clef.line" ) );
        m_scoreDef->ReplaceClef( &clef );
        // add other attributes for SOLva
    }
    
    TiXmlElement *current = NULL;
    for( current = scoreDef->FirstChildElement( "staffGrp" ); current; current = current->NextSiblingElement( "staffGrp" ) ) {
        MusStaffGrp *staffGrp = new MusStaffGrp( );
        m_staffGrps.push_back( staffGrp );
        SetMeiUuid( current , staffGrp );
        if (ReadMeiStaffGrp( current )) {
            m_scoreDef->AddStaffGrp( staffGrp );
        }
        else {
            delete staffGrp;
        }
        m_staffGrps.pop_back();
    }
    
    return true;
}

bool MusMeiInput::ReadMeiStaffGrp( TiXmlElement *staffGrp )
{
    assert( !m_staffGrps.empty() );
    assert( !m_staffDef );
    
    MusStaffGrp *currentStaffGrp = m_staffGrps.back();
    
    TiXmlElement *current = NULL;
    for( current = staffGrp->FirstChildElement( ); current; current = current->NextSiblingElement( ) ) {
        if ( std::string( current->Value() ) == "staffGrp" ) {
            MusStaffGrp *staffGrp = new MusStaffGrp( );
            m_staffGrps.push_back( staffGrp );
            SetMeiUuid( current , staffGrp );
            if (ReadMeiStaffGrp( current )) {
                currentStaffGrp->AddStaffGrp( staffGrp );
            }
            else {
                delete staffGrp;
            }
            m_staffGrps.pop_back();            
        }
        else if ( std::string( current->Value() ) == "staffDef" ) {
            m_staffDef = new MusStaffDef( );
            SetMeiUuid( current , m_staffDef );
            if (ReadMeiStaffDef( current )) {
                currentStaffGrp->AddStaffDef( m_staffDef );
            }
            else {
                delete m_staffDef;
            }
            m_staffDef = NULL;
        }        
    }
    
    return true;
}

bool MusMeiInput::ReadMeiStaffDef( TiXmlElement *staffDef )
{
    assert( m_staffDef );
    
    if ( staffDef->Attribute( "n" ) ) {
        m_staffDef->SetStaffNo( atoi ( staffDef->Attribute( "n" ) ) );
    }
    else {
        Mus::LogWarning("No @n on staffDef");
    }
    if ( staffDef->Attribute( "key.sig" ) ) {
        MusKeySig keysig(
                         StrToKeySigNum( staffDef->Attribute( "key.sig" ) ),
                         StrToKeySigType( staffDef->Attribute( "key.sig" ) ) );
        m_staffDef->ReplaceKeySig( &keysig );
    }
    if ( staffDef->Attribute( "clef.line" ) && staffDef->Attribute( "clef.shape" ) ) {
        MusClef clef;
        clef.m_clefId = StrToClef( staffDef->Attribute( "clef.shape" ) , staffDef->Attribute( "clef.line" ) );
        // this is obviously a short cut - assuming @clef.dis being SOLva
        if ( staffDef->Attribute( "clef.dis" ) ) {
            clef.m_clefId = SOLva;
        }
        m_staffDef->ReplaceClef( &clef );
    }
    
    return true;
}

bool MusMeiInput::ReadMeiMeasure( TiXmlElement *measure )
{
    assert( m_measure );
    assert( !m_staff );
    
    TiXmlElement *current = NULL;
    for( current = measure->FirstChildElement( "staff" ); current; current = current->NextSiblingElement( "staff" ) ) {
        m_staff = new MusStaff( );
        SetMeiUuid( current , m_staff );
        if ( ReadMeiStaff( current )) {
            m_measure->AddStaff( m_staff );
        }
        else {
            delete m_staff;
        }
        m_staff = NULL;
    }
    // success only if at least one staff was added to the measure
    return (m_measure->GetStaffCount() > 0);
}

bool MusMeiInput::ReadMeiStaff( TiXmlElement *staff )
{
    assert( m_staff );
    assert( !m_layer );
    
    if ( staff->Attribute( "n" ) ) {
        m_staff->SetStaffNo( atoi ( staff->Attribute( "n" ) ) );
    }
    else {
        Mus::LogWarning("No @n on staff");
    }
    if ( staff->Attribute( "uly" ) ) {
        m_staff->m_y_abs = atoi ( staff->Attribute( "uly" ) );
    }
    if ( staff->Attribute( "label" ) ) {
        // we use type only for typing mensural notation
        m_staff->notAnc = true;
    }
    
    TiXmlElement *current = NULL;
    for( current = staff->FirstChildElement( "layer" ); current; current = current->NextSiblingElement( "layer" ) ) {
        m_layer = new MusLayer( 1 );
        m_currentLayer = m_layer;
        SetMeiUuid( current , m_layer );
        if (ReadMeiLayer( current )) {
            m_staff->AddLayer( m_layer );
        }
        else {
            delete m_layer;
        }
        m_layer = NULL;
    }
    
    // success only if at least one measure was added to the staff
    return (m_staff->GetLayerCount() > 0);
}

bool MusMeiInput::ReadMeiLayer( TiXmlElement *layer )
{
    assert( m_layer );
    
    if ( layer->Attribute( "n" ) ) {
        m_layer->SetLayerNo( atoi ( layer->Attribute( "n" ) ) );
    }
    else {
        Mus::LogWarning("No @n on layer");
    }
    
    TiXmlElement *current = NULL;
    for( current = layer->FirstChildElement( ); current; current = current->NextSiblingElement( ) ) {
        ReadMeiLayerElement( current );
    }
    // success in any case
    return true;
}

bool MusMeiInput::ReadMeiLayerElement( TiXmlElement *xmlElement )
{
    MusLayerElement *musElement = NULL;
    if ( std::string( xmlElement->Value() )  == "barLine" ) {
        musElement = ReadMeiBarline( xmlElement );
    }
    else if ( std::string( xmlElement->Value() ) == "beam" ) {
        musElement = ReadMeiBeam( xmlElement );
    }
    else if ( std::string( xmlElement->Value() ) == "clef" ) {
        musElement = ReadMeiClef( xmlElement );
    }
    else if ( std::string( xmlElement->Value() ) == "mensur" ) {
        musElement = ReadMeiMensur( xmlElement );
    }
    else if ( std::string( xmlElement->Value() ) == "note" ) {
        musElement = ReadMeiNote( xmlElement );
    }
    else if ( std::string( xmlElement->Value() ) == "rest" ) {
        musElement = ReadMeiRest( xmlElement );
    }
    else if ( std::string( xmlElement->Value() ) == "multiRest" ) {
        musElement = ReadMeiMultiRest( xmlElement );
    }
    else if ( std::string( xmlElement->Value() ) == "tuplet" ) {
        musElement = ReadMeiTuplet( xmlElement );
    }
    // symbols
    else if ( std::string( xmlElement->Value() ) == "accid" ) {
        musElement = ReadMeiAccid( xmlElement );
    }
    else if ( std::string( xmlElement->Value() ) == "custos" ) {
        musElement = ReadMeiCustos( xmlElement );
    }
    else if ( std::string( xmlElement->Value() ) == "dot" ) {
        musElement = ReadMeiDot( xmlElement );
    }
    // app
    else if ( std::string( xmlElement->Value() ) == "app" ) {
        musElement = ReadMeiApp( xmlElement );
    }
    // unkown            
    else {
        Mus::LogDebug("Element %s ignored", xmlElement->Value() );
    }
    
    if ( !musElement ) {
        return false;
    }
    
    if ( xmlElement->Attribute( "ulx" ) ) {
        musElement->m_x_abs = atoi ( xmlElement->Attribute( "ulx" ) );
    }
    ReadSameAsAttr( xmlElement, musElement );
    SetMeiUuid( xmlElement, musElement );
    
    AddLayerElement( musElement );
    return true;
}

MusLayerElement *MusMeiInput::ReadMeiBarline( TiXmlElement *barline )
{
    MusBarline *musBarline = new MusBarline();
    
    return musBarline;    
}

MusLayerElement *MusMeiInput::ReadMeiBeam( TiXmlElement *beam )
{
    assert ( !m_beam );
    
    // m_beam will be used for adding elements to the beam
    m_beam = new MusBeam();
    
    MusObject *previousLayer = m_currentLayer;
    m_currentLayer = m_beam;
    
    TiXmlElement *current = NULL;
    for( current = beam->FirstChildElement( ); current; current = current->NextSiblingElement( ) ) {
        ReadMeiLayerElement( current );
    }
    
    if ( m_beam->GetNoteCount() == 1 ) {
        Mus::LogWarning("Beam element with only one note");
    }
    // switch back to the previous one
    m_currentLayer = previousLayer;
    if ( m_beam->GetNoteCount() < 1 ) {
        delete m_beam;
        m_beam = NULL;
        return NULL;
    } 
    else {
        // set the member to NULL but keep a pointer to be returned        
        MusBeam *musBeam = m_beam;
        m_beam = NULL;
        return musBeam;
    }
}

MusLayerElement *MusMeiInput::ReadMeiClef( TiXmlElement *clef )
{ 
    MusClef *musClef = new MusClef(); 
    if ( clef->Attribute( "shape" ) && clef->Attribute( "line" ) ) {
        musClef->m_clefId = StrToClef( clef->Attribute( "shape" ) , clef->Attribute( "line" ) );
    }
    
    return musClef;
}


MusLayerElement *MusMeiInput::ReadMeiMensur( TiXmlElement *mensur )
{
    MusMensur *musMensur = new MusMensur();
    
    if ( mensur->Attribute( "sign" ) ) {
        musMensur->m_sign = StrToMensurSign( mensur->Attribute( "sign" ) );
    }
    if ( mensur->Attribute( "dot" ) ) {
        musMensur->m_dot = ( strcmp( mensur->Attribute( "dot" ), "true" ) == 0 );
    }
    if ( mensur->Attribute( "slash" ) ) {
        musMensur->m_slash =  1; //atoi( mensur->Attribute( "Slash" ) );
    }
    if ( mensur->Attribute( "orient" ) ) {
        musMensur->m_reversed = ( strcmp ( mensur->Attribute( "orient" ), "reversed" ) == 0 );
    }
    if ( mensur->Attribute( "num" ) ) {
        musMensur->m_num = atoi ( mensur->Attribute( "num" ) );
    }
    if ( mensur->Attribute( "numbase" ) ) {
        musMensur->m_numBase = atoi ( mensur->Attribute( "numbase" ) );
    }
    // missing m_meterSymb
    
    return musMensur;
}

MusLayerElement *MusMeiInput::ReadMeiMultiRest( TiXmlElement *multiRest )
{
	MusMultiRest *musMultiRest = new MusMultiRest();
    
	// pitch
    if ( multiRest->Attribute( "num" ) ) {
        musMultiRest->SetNumber( atoi ( multiRest->Attribute( "num" ) ) );
    }
	
	return musMultiRest;
}

MusLayerElement *MusMeiInput::ReadMeiNote( TiXmlElement *note )
{
	MusNote *musNote = new MusNote();
    
	// pitch
	if ( note->Attribute( "pname" ) ) {
		musNote->m_pname = StrToPitch( note->Attribute( "pname" ) );
	}
	// oct
	if ( note->Attribute( "oct" ) ) {
		musNote->m_oct = StrToOct( note->Attribute( "oct" ) );
	}
	// duration
	if ( note->Attribute( "dur" ) ) {
		musNote->m_dur = StrToDur( note->Attribute( "dur" ) );
	}
    // dots
    if ( note->Attribute( "dots" ) ) {
		musNote->m_dots = atoi( note->Attribute( "dots" ) );
	}
    // accid
    if ( note->Attribute( "accid" ) ) {
		musNote->m_accid = StrToAccid( note->Attribute( "accid" ) );
	}
    // ligature
    if ( note->Attribute( "lig" ) ) {
        musNote->m_lig = true; // this has to be double checked
        if ( strcmp( note->Attribute( "lig" ), "obliqua" ) == 0 ) {
            musNote->m_ligObliqua = true;
        }
    }
    // stem direction
    if ( note->Attribute( "stem.dir" ) ) {
        // we use it to indicate opposite direction
        musNote->m_stemDir = 1;
    }
    // coloration
    if ( note->Attribute( "colored" ) ) {
        musNote->m_colored = ( strcmp ( note->Attribute( "colored" ), "true" ) == 0 );
    }
	
	return musNote;
}


MusLayerElement *MusMeiInput::ReadMeiRest( TiXmlElement *rest )
{
    MusRest *musRest = new MusRest();
    
	// duration
	if ( rest->Attribute( "dur" ) ) {
		musRest->m_dur = StrToDur( rest->Attribute( "dur" ) );
	}
    if ( rest->Attribute( "dots" ) ) {
		musRest->m_dots = atoi( rest->Attribute( "dots" ) );
	}
    // position
	if ( rest->Attribute( "ploc" ) ) {
		musRest->m_pname = StrToPitch( rest->Attribute( "ploc" ) );
	}
	// oct
	if ( rest->Attribute( "oloc" ) ) {
		musRest->m_oct = StrToOct( rest->Attribute( "oloc" ) );
	}
	
    return musRest;
}


MusLayerElement *MusMeiInput::ReadMeiTuplet( TiXmlElement *tuplet )
{
    assert ( !m_tuplet );
    
    // m_tuplet will be used for adding elements to the beam
    m_tuplet = new MusTuplet();
    
    MusObject *previousLayer = m_currentLayer;
    m_currentLayer = m_tuplet;
    
    // Read in the numerator and denominator properties
    if ( tuplet->Attribute( "num" ) ) {
		m_tuplet->m_num = atoi( tuplet->Attribute( "num" ) );
	}
    if ( tuplet->Attribute( "numbase" ) ) {
		m_tuplet->m_numbase = atoi( tuplet->Attribute( "numbase" ) );
	}
    
    TiXmlElement *current = NULL;
    for( current = tuplet->FirstChildElement( ); current; current = current->NextSiblingElement( ) ) {
        ReadMeiLayerElement( current );
    }
    
    if ( m_tuplet->GetNoteCount() == 1 ) {
        Mus::LogWarning("Tuplet element with only one note");
    }
    // switch back to the previous one
    m_currentLayer = previousLayer;
    if ( m_tuplet->GetNoteCount() < 1 ) {
        delete m_tuplet;
        return NULL;
    }
    else {
        // set the member to NULL but keep a pointer to be returned
        MusTuplet *musTuplet = m_tuplet;
        m_tuplet = NULL;
        return musTuplet;
    }
}


MusLayerElement *MusMeiInput::ReadMeiAccid( TiXmlElement *accid )
{
    MusSymbol *musAccid = new MusSymbol( SYMBOL_ACCID );
    
    if ( accid->Attribute( "accid" ) ) {
        musAccid->m_accid = StrToAccid( accid->Attribute( "accid" ) );
    }
    // position
	if ( accid->Attribute( "ploc" ) ) {
		musAccid->m_pname = StrToPitch( accid->Attribute( "ploc" ) );
	}
	// oct
	if ( accid->Attribute( "oloc" ) ) {
		musAccid->m_oct = StrToOct( accid->Attribute( "oloc" ) );
	}
	
	return musAccid;
}

MusLayerElement *MusMeiInput::ReadMeiCustos( TiXmlElement *custos )
{
    MusSymbol *musCustos = new MusSymbol( SYMBOL_CUSTOS );
    
	// position (pitch)
	if ( custos->Attribute( "pname" ) ) {
		musCustos->m_pname = StrToPitch( custos->Attribute( "pname" ) );
	}
	// oct
	if ( custos->Attribute( "oct" ) ) {
		musCustos->m_oct = StrToOct( custos->Attribute( "oct" ) );
	}
	
	return musCustos;    
}

MusLayerElement *MusMeiInput::ReadMeiDot( TiXmlElement *dot )
{
    MusSymbol *musDot = new MusSymbol( SYMBOL_DOT );
    
    musDot->m_dot = 0;
    // missing m_dots
    // position
	if ( dot->Attribute( "ploc" ) ) {
		musDot->m_pname = StrToPitch( dot->Attribute( "ploc" ) );
	}
	// oct
	if ( dot->Attribute( "oloc" ) ) {
		musDot->m_oct = StrToOct( dot->Attribute( "oloc" ) );
	}
	
	return musDot;
}

MusLayerElement *MusMeiInput::ReadMeiApp( TiXmlElement *app )
{
    m_layerApp = new MusLayerApp( );
   
    TiXmlElement *current = NULL;
    for( current = app->FirstChildElement( "rdg" ); current; current = current->NextSiblingElement( "rdg" ) ) {
        ReadMeiRdg( current );
	}
	
    // set the member to NULL but keep a pointer to be returned
    MusLayerApp *layerApp = m_layerApp;
    m_layerApp = NULL;
    
    return layerApp;
}

bool MusMeiInput::ReadMeiRdg( TiXmlElement *rdg )
{
    assert ( !m_layerRdg );
    assert( m_layerApp );
    
    m_layerRdg = new MusLayerRdg( );
    
    if ( rdg->Attribute( "source" ) ) {
        m_layerRdg->m_source = rdg->Attribute( "source" );
    }
    
    // switch to the rdg
    MusObject *previousLayer = m_currentLayer;
    m_currentLayer = m_layerRdg;
 
    TiXmlElement *current = NULL;
    for( current = rdg->FirstChildElement( ); current; current = current->NextSiblingElement( ) ) {
        ReadMeiLayerElement( current );
    }
    
    // switch back to the previous one
    m_currentLayer = previousLayer;

    // set the member to NULL but keep a pointer to be returned
    MusLayerRdg *layerRdg = m_layerRdg;
    m_layerRdg = NULL;
    
    return layerRdg;
}


void MusMeiInput::ReadSameAsAttr( TiXmlElement *element, MusObject *object )
{
    if ( !element->Attribute( "sameas" ) ) {
        return;
    }
    
    object->m_sameAs = element->Attribute( "sameas" );
}


void MusMeiInput::AddLayerElement( MusLayerElement *element )
{
    assert( m_currentLayer );
    if ( dynamic_cast<MusLayer*>( m_currentLayer ) ) {
        ((MusLayer*)m_currentLayer)->AddElement( element );
    }
    else if ( dynamic_cast<MusLayerRdg*>( m_currentLayer ) ) {
        ((MusLayerRdg*)m_currentLayer)->AddElement( element );
    }
    else if ( dynamic_cast<MusBeam*>( m_currentLayer ) ) {
        ((MusBeam*)m_currentLayer)->AddElement( element );
    }
    else if ( dynamic_cast<MusTuplet*>( m_currentLayer ) ) {
        ((MusTuplet*)m_currentLayer)->AddElement( element );
    }
    
}


bool MusMeiInput::ReadUnsupported( TiXmlElement *element )
{
    if ( std::string( element->Value() ) == "score" ) {
        TiXmlElement *current = NULL;
        for( current = element->FirstChildElement( ); current; current = current->NextSiblingElement( ) ) {
            ReadUnsupported( current );
        }
    }
    if ( std::string( element->Value() ) == "section" ) {
        TiXmlElement *current = NULL;
        for( current = element->FirstChildElement( ); current; current = current->NextSiblingElement( ) ) {
            ReadUnsupported( current );
        }       
    }
    else if ( std::string( element->Value() ) == "measure" ) {
        Mus::LogDebug( "measure" );
        m_measure = new MusMeasure( );
        SetMeiUuid( element, m_measure );
        if (ReadMeiMeasure( element )) {
            m_system->AddMeasure( m_measure );
        }
        else {
            delete m_measure;
        }
        m_measure = NULL;
    }
    /*
    else if ( std::string( element->Value() ) == "staff" ) {
        Mus::LogDebug( "staff" );
        int n = 1;
        if ( element->Attribute( "n" ) ) {
            element->Attribute( "n", &n );
        }
        MusStaff *staff = m_system->GetStaff( n - 1 );
        if ( staff ) {
            m_staff = staff;
        }
        else
        {
            m_staff = new MusStaff( n );
            m_system->AddStaff( m_staff );
        }
        m_measure = new MusMeasure( *m_contentBasedMeasure );
        ReadMeiStaff( element );
    }
    */
    else if ( (std::string( element->Value() ) == "pb") && (m_system->GetMeasureCount() > 0 ) ) {
        Mus::LogDebug( "pb" );
        m_page = new MusPage( );
        m_system = new MusSystem( );
        m_page->AddSystem( m_system );
        m_doc->AddPage( m_page );
        
    }
    else if ( (std::string( element->Value() ) == "sb") && (m_page->GetSystemCount() > 0 ) ) {
        Mus::LogDebug( "sb" );
        m_system = new MusSystem( );
        m_page->AddSystem( m_system );
    }
    else if ( (std::string( element->Value() ) == "scoreDef") && ( !m_hasScoreDef ) ) {
        Mus::LogDebug( "scoreDef" );
        m_scoreDef = &m_doc->m_scoreDef;
        SetMeiUuid( element, m_scoreDef );
        if (ReadMeiScoreDef( element )) {
            m_hasScoreDef = true;
        }
        else {
            m_hasScoreDef = false;
        }
    }
    else {
        Mus::LogWarning( "Element %s ignored", element->Value() );
    }
    return true;
}

void MusMeiInput::SetMeiUuid( TiXmlElement *element, MusObject *object )
{
    if ( !element->Attribute( "xml:id" ) ) {
        return;
    }
    
    object->SetUuid( element->Attribute( "xml:id" ) );
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
    else if (dur == "32") value = DUR_32;
    else if (dur == "64") value = DUR_64;
    else if (dur == "128") value = DUR_128;
	else {
		//Mus::LogWarning("Unknown duration '%s'", dur.c_str());
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
		Mus::LogWarning("Unknow pname '%s'", pitch.c_str());
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
        Mus::LogWarning("Unknown accid '%s'", accid.c_str() );
        value = ACCID_NATURAL;
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
        Mus::LogWarning("Unknown clef shape '%s' line '%s'", shape.c_str(), line.c_str() );
    }
    return clefId;
}

MensurSign MusMeiInput::StrToMensurSign(std::string sign)
{
    if (sign == "C") return MENSUR_SIGN_C;
    else if (sign == "O") return MENSUR_SIGN_O;
    else {
        Mus::LogWarning("Unknown mensur sign '%s'", sign.c_str() );
	}
    // default
	return MENSUR_SIGN_C;
}

DocType MusMeiInput::StrToDocType(std::string type)
{
    if (type == "raw") return Raw;
    else if (type == "rendering") return Rendering;
    else if (type == "transcription") return Transcription;
    else {
        Mus::LogWarning("Unknown layout type '%s'", type.c_str() );
	}
    // default
	return Raw;
}

unsigned char MusMeiInput::StrToKeySigType(std::string accid)
{
    if ( accid == "0" ) return  ACCID_NATURAL;
    else if ( accid.at(1) == 'f' ) return ACCID_FLAT;
    else if ( accid.at(1) == 's' ) return ACCID_SHARP;
    else {
        Mus::LogWarning("Unknown keysig '%s'", accid.c_str() );
        return ACCID_NATURAL;
    }
}

int MusMeiInput::StrToKeySigNum(std::string accid)
{
    if ( accid == "0" ) return  0;
    else {
        // low level way, remove '0', which is 48
        return accid.at(0) - '0';
    }
}

