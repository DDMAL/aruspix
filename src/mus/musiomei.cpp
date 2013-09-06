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

#include "musbarline.h"
#include "musbeam.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musneumesymbol.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"

#include "musstaff.h"
#include "muslayer.h"
#include "muslayerelement.h"

//#include "app/axapp.h"

//----------------------------------------------------------------------------
// MusMeiOutput
//----------------------------------------------------------------------------

MusMeiOutput::MusMeiOutput( MusDoc *doc, wxString filename ) :
    // This is pretty bad. We open a bad fileoutputstream as we don't use it
	MusFileOutputStream( doc, -1 )
{
    m_filename = filename;
    m_mei = NULL;
    m_score = NULL;
    m_page = NULL;
    m_system = NULL;
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
        m_score = new TiXmlElement("score");
        m_score->SetAttribute( "type",  DocTypeToStr( m_doc->GetType() ).c_str() );
        
        
        // this starts the call of all the functors
        m_doc->Save( this );
        
        // after the functor has run, we have the header in m_mei and the score in m_score
        TiXmlElement *mdiv = new TiXmlElement("mdiv");
        mdiv->LinkEndChild( m_score );
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
        wxLogError("%s", str );
        return false;
    }
	return true;    
}

wxString MusMeiOutput::UuidToMeiStr( MusObject *element )
{
    // RZ uuid_string_t does not exist on freebsd
    char uuidStr[37];
    uuid_unparse( *element->GetUuid(), uuidStr );
    wxString out;    
    // xml IDs can't start with a number, so we prepend "m-" to every ID.
    out = "m-" + wxString(uuidStr);
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

bool MusMeiOutput::WriteDoc( MusDoc *doc )
{
    wxASSERT( m_mei );
    
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
    
    p1->LinkEndChild( new TiXmlText( wxString::Format( "Encoded with Aruspix version %s",  MusDoc::GetAxVersion().c_str() ).c_str() ) );
    date->LinkEndChild( new TiXmlText( wxNow().c_str() ) );
    
    meiHead->LinkEndChild(fileDesc);
    meiHead->LinkEndChild(encodingDesc);
    m_mei->LinkEndChild( meiHead );
    
    return true;
}


bool MusMeiOutput::WritePage( MusPage *page )
{
    wxASSERT( m_score );
    m_page = new TiXmlElement("page");
    m_page->SetAttribute( "xml:id",  UuidToMeiStr( page ).c_str() );
    // size and margins but only if any - we rely on page.height only to check this
    if ( page->m_pageHeight != -1 ) {
        m_page->SetAttribute( "page.width", wxString::Format( "%d", page->m_pageWidth ).c_str() );
        m_page->SetAttribute( "page.height", wxString::Format( "%d", page->m_pageHeight ).c_str() );
        m_page->SetAttribute( "page.leftmar", wxString::Format( "%d", page->m_pageLeftMar ).c_str() );
        m_page->SetAttribute( "page.rightmar", wxString::Format( "%d", page->m_pageRightMar ).c_str() );
    }
    if ( !page->m_surface.IsEmpty() ) {
        m_page->SetAttribute( "surface", page->m_surface.c_str() );
    }
    //
    TiXmlComment *comment = new TiXmlComment();
    comment->SetValue( "Coordinates in MEI axis direction" );
    m_score->LinkEndChild( comment );
    m_score->LinkEndChild( m_page );
    return true;
}

bool MusMeiOutput::WriteSystem( MusSystem *system )
{
    wxASSERT( m_page );
    m_system = new TiXmlElement("system");
    m_system->SetAttribute( "xml:id",  UuidToMeiStr( system ).c_str() );
    // margins
    m_system->SetAttribute( "system.leftmar", wxString::Format( "%d", system->m_systemLeftMar ).c_str() );
    m_system->SetAttribute( "system.rightmar", wxString::Format( "%d", system->m_systemRightMar ).c_str() );
    // y positions
    m_system->SetAttribute( "uly", wxString::Format( "%d", system->m_y_abs ).c_str() );
    m_page->LinkEndChild( m_system );
    return true;
}

bool MusMeiOutput::WriteStaff( MusStaff *staff )
{
    wxASSERT( m_system );
    m_staff = new TiXmlElement("staff");
    m_staff->SetAttribute( "xml:id",  UuidToMeiStr( staff ).c_str() );
    // y position
    if ( staff->notAnc ) {
        m_staff->SetAttribute( "label", "mensural" );
    }
    m_staff->SetAttribute( "uly", wxString::Format( "%d", staff->m_y_abs ).c_str() );
    m_staff->SetAttribute( "n", wxString::Format( "%d", staff->m_logStaffNb ).c_str() );    

    m_system->LinkEndChild( m_staff );
    return true;
}

bool MusMeiOutput::WriteLayer( MusLayer *layer )
{
    wxASSERT( m_staff );
    m_layer = new TiXmlElement("layer");
    m_layer->SetAttribute( "xml:id",  UuidToMeiStr( layer ).c_str() );
    m_layer->SetAttribute( "n", wxString::Format( "%d", layer->m_logLayerNb ).c_str() );
    m_staff->LinkEndChild( m_layer );
    return true;
}

bool MusMeiOutput::WriteLayerElement( MusLayerElement *element )
{
    wxASSERT( m_layer );
    
    // Here we look at what is the parent.
    // For example, if we are in a beam, we must attach it to the beam xml element (m_beam)
    // By default, we attach it to m_layer
    TiXmlElement *currentParent = m_layer;
    if ( dynamic_cast<MusLayerRdg*>(element->m_parent) ) {
        wxASSERT( m_rdgLayer );
        currentParent = m_rdgLayer;
    }
    else if ( dynamic_cast<MusBeam*>(element->m_parent) ) {
        wxASSERT( m_beam );
        currentParent = m_beam;
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
    else if (dynamic_cast<MusNeume*>(element)) {
        wxLogWarning( "Neume are not saved in MEI files" );
    }
    else if (dynamic_cast<MusNeumeSymbol*>(element)) {
        wxLogWarning( "NeumeSymbol are not saved in MEI files" );
    }
    else if (dynamic_cast<MusNote*>(element)) {
        xmlElement = new TiXmlElement("note");
        WriteMeiNote( xmlElement, dynamic_cast<MusNote*>(element) );
    }
    else if (dynamic_cast<MusRest*>(element)) {
        xmlElement = new TiXmlElement("rest");
        WriteMeiRest( xmlElement, dynamic_cast<MusRest*>(element) );
    }
    else if (dynamic_cast<MusSymbol*>(element)) {        
        xmlElement = WriteMeiSymbol( dynamic_cast<MusSymbol*>(element) );
    }
    
    // we have it, set the uuid we read
    if ( xmlElement ) {
        this->WriteSameAsAttr( xmlElement, element );
        xmlElement->SetAttribute( "xml:id",  UuidToMeiStr( element ).c_str() );
        xmlElement->SetAttribute( "ulx", wxString::Format( "%d", element->m_x_abs ).c_str() );
        currentParent->LinkEndChild( xmlElement );
        return true;
    }
    else {
        wxLogWarning( "Element class %s could not be saved", element->MusClassName().c_str() );
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
        meiMensur->SetAttribute( "num", wxString::Format("%d", mensur->m_num ).c_str() );
    }
    if ( mensur->m_numBase ) {
        meiMensur->SetAttribute( "numbase", wxString::Format("%d", mensur->m_numBase ).c_str() );
    }
    // missing m_meterSymb
    
    return;
}

void MusMeiOutput::WriteMeiNote( TiXmlElement *meiNote, MusNote *note )
{
    meiNote->SetAttribute( "pname", PitchToStr( note->m_pname ).c_str() );
    meiNote->SetAttribute( "oct", OctToStr( note->m_oct ).c_str() );
    meiNote->SetAttribute( "dur", DurToStr( note->m_dur ).c_str() );
    if ( note->m_dots ) {
        meiNote->SetAttribute( "dots", wxString::Format("%d", note->m_dots).c_str() );
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
        meiRest->SetAttribute( "dots", wxString::Format("%d", rest->m_dots).c_str() );
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

bool MusMeiOutput::WriteLayerApp( MusLayerApp *app )
{    
    wxASSERT( m_layer );
    m_app = new TiXmlElement("app");
    m_layer->LinkEndChild( m_app ); 
    return true;
}

bool MusMeiOutput::WriteLayerRdg( MusLayerRdg *rdg )
{   
    wxASSERT( m_app );
    m_rdgLayer = new TiXmlElement("rdg");
    m_rdgLayer->SetAttribute( "source", rdg->m_source.c_str() );
    m_app->LinkEndChild( m_rdgLayer ); 
    return true;
}


void MusMeiOutput::WriteSameAsAttr( TiXmlElement *meiElement, MusObject *element )
{
    if ( !element->m_sameAs.IsEmpty() ) {
        meiElement->SetAttribute( "sameas", element->m_sameAs.c_str() );
    }
}


wxString MusMeiOutput::OctToStr(int oct)
{
	char buf[3];
	snprintf(buf, 2, "%d", oct);
	return wxString(buf);
	
	// For some reason, #include <sstream> does not work with xcode 3.2
	//std::ostringstream oss;
	//oss << oct;
	//return oss.str();
}


wxString MusMeiOutput::PitchToStr(int pitch)
{
    wxString value;
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

wxString MusMeiOutput::AccidToStr(unsigned char accid)
{
    wxString value;
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

wxString MusMeiOutput::ClefLineToStr( ClefId clefId )
{	
	wxString value; 
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

wxString MusMeiOutput::ClefShapeToStr( ClefId clefId )
{	
	wxString value; 
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

wxString MusMeiOutput::MensurSignToStr(MensurSign sign)
{
 	wxString value; 
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


wxString MusMeiOutput::DurToStr( int dur )
{
    wxString value;
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

wxString MusMeiOutput::DocTypeToStr(DocType type)
{
 	wxString value; 
	switch(type)
	{	
        case Raw : value = "raw"; break;
        case Rendering : value = "rendering"; break;
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
    wxFileName::SplitPath( m_filename, NULL, &doc->m_fname, NULL );
    m_page = NULL;
    m_system = NULL;
	m_staff = NULL;
	m_layer = NULL;
    m_layerApp = NULL;
    m_layerRdg = NULL;
    m_beam = NULL;
    //
    m_currentLayer = NULL; 
}

MusMeiInput::~MusMeiInput()
{
}

bool MusMeiInput::ImportFile( )
{
    try {
        TiXmlElement *current;
        
        m_doc->Reset( Raw );

        TiXmlDocument doc( m_filename.c_str() );
        bool loadOkay = doc.LoadFile();
        if (!loadOkay)
        {
            return false;
        }
        TiXmlElement *root = doc.RootElement();
        if ( root && (current = root->FirstChildElement( "meiHead" ) ) ) 
        {
            ReadMeiHeader( current );
        }
        // music
        TiXmlElement *music = NULL;
        TiXmlElement *body = NULL;
        TiXmlElement *mdiv = NULL;
        TiXmlElement *score = NULL;
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
            score = mdiv->FirstChildElement("score");
        }
        if ( score ) {
            
            // check if there is a type attribute for the score
            DocType type;
            if ( score->Attribute( "type" ) ) {
                type = StrToDocType( score->Attribute( "type" ) );
                m_doc->Reset( type );
            }
            
            for( current = score->FirstChildElement( "page" ); current; current = current->NextSiblingElement( "page" ) ) {
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
        return true;
    }
    catch( char * str ) {
        wxLogError("%s", str );
        return false;
   }
}

bool MusMeiInput::ReadMeiHeader( TiXmlElement *meiHead )
{
    return true;
}


bool MusMeiInput::ReadMeiPage( TiXmlElement *page )
{
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
    for( current = system->FirstChildElement( "staff" ); current; current = current->NextSiblingElement( "staff" ) ) {
        m_staff = new MusStaff( );
        SetMeiUuid( current, m_staff );
        if (ReadMeiStaff( current )) {
            m_system->AddStaff( m_staff );
        }
        else {
            delete m_staff;
        }
        m_staff = NULL;
    }
    // success only if at least one staff was added to the measure
    return (m_system->GetStaffCount() > 0);
}

bool MusMeiInput::ReadMeiStaff( TiXmlElement *staff )
{
    if ( staff->Attribute( "n" ) ) {
        m_staff->m_logStaffNb = atoi ( staff->Attribute( "n" ) );
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
    // success only if at least one layer was added to the staff
    return (m_staff->GetLayerCount() > 0);
}

bool MusMeiInput::ReadMeiLayer( TiXmlElement *layer )
{
    if ( layer->Attribute( "n" ) ) {
        m_layer->m_logLayerNb = atoi ( layer->Attribute( "n" ) );
    }
    
    TiXmlElement *current = NULL;
    for( current = layer->FirstChildElement( ); current; current = current->NextSiblingElement( ) ) {
        if ( wxString( current->Value() )  == "barLine" ) {
            if (!ReadMeiBarline( current )) {
                return false;
            }
        }
        else if ( wxString( current->Value() ) == "beam" ) {
            if (!ReadMeiBeam( current )) {
                return false;
            }
        }
        else if ( wxString( current->Value() ) == "clef" ) {
            if (!ReadMeiClef( current )) {
                return false;
            }
        }
        else if ( wxString( current->Value() ) == "mensur" ) {
            if (!ReadMeiMensur( current )) {
                return false;
            }
        }
        else if ( wxString( current->Value() ) == "note" ) {
            if (!ReadMeiNote( current )) {
                return false;
            }
        }
        else if ( wxString( current->Value() ) == "rest" ) {
            if (!ReadMeiRest( current )) {
                return false;
            }
        }
        // symbols
        else if ( wxString( current->Value() ) == "accid" ) {
            if (!ReadMeiAccid( current )) {
                return false;
            }
        }
        else if ( wxString( current->Value() ) == "custos" ) {
            if (!ReadMeiCustos( current )) {
                return false;
            }
        }
        else if ( wxString( current->Value() ) == "dot" ) {
            if (!ReadMeiDot( current )) {
                return false;
            }
        }
        // app
        else if ( wxString( current->Value() ) == "app" ) {
            if (!ReadMeiApp( current )) {
                return false;
            }
        }
        // unkown            
        else {
            wxLogDebug("Element %s ignored", current->Value() );
        }

    }
    // success in any case
    return true;
}

bool MusMeiInput::ReadMeiLayerElement( TiXmlElement *xmlElement, MusLayerElement *musElement )
{
    if ( xmlElement->Attribute( "ulx" ) ) {
        musElement->m_x_abs = atoi ( xmlElement->Attribute( "ulx" ) );
    }
    ReadSameAsAttr( xmlElement, musElement );
    return true;
}

bool MusMeiInput::ReadMeiBarline( TiXmlElement *barline )
{
    MusBarline *musBarline = new MusBarline();
    SetMeiUuid( barline, musBarline );
    ReadMeiLayerElement( barline, musBarline );
    
    AddLayerElement( musBarline );
    return true;
}

bool MusMeiInput::ReadMeiBeam( TiXmlElement *beam )
{
    wxASSERT ( !m_beam );
    
    m_beam = new MusBeam();
    SetMeiUuid( beam, m_beam );
    ReadMeiLayerElement( beam, m_beam );
    
    MusObject *previousLayer = m_currentLayer;
    m_currentLayer = m_beam;
    
    TiXmlElement *current = NULL;
    for( current = beam->FirstChildElement( ); current; current = current->NextSiblingElement( ) ) {
        if ( wxString( current->Value() ) == "note" ) {
            if (!ReadMeiNote( current )) {
                return false;
            }
        }
        else if ( wxString( current->Value() ) == "rest" ) {
            if (!ReadMeiRest( current )) {
                return false;
            }
        }
        // unkown            
        else {
            wxLogDebug("LayerElement %s ignored", current->Value() );
        }
    }
    
    if ( m_beam->GetNoteCount() == 1 ) {
        wxLogWarning("Beam element with only one note");
    }
    // switch back to the previous one
    m_currentLayer = previousLayer;
    if ( m_beam->GetNoteCount() < 1 ) {
        delete m_beam;
    } 
    else {
        AddLayerElement( m_beam );
    }
    m_beam = NULL;
    
    return true;
}

bool MusMeiInput::ReadMeiClef( TiXmlElement *clef )
{ 
    MusClef *musClef = new MusClef();
    SetMeiUuid( clef, musClef );
    ReadMeiLayerElement( clef, musClef );
    
    if ( clef->Attribute( "shape" ) && clef->Attribute( "line" ) ) {
        musClef->m_clefId = StrToClef( clef->Attribute( "shape" ) , clef->Attribute( "line" ) );
    }
    
    AddLayerElement( musClef );
    return true;
}

bool MusMeiInput::ReadMeiMensur( TiXmlElement *mensur )
{
    MusMensur *musMensur = new MusMensur();
    SetMeiUuid( mensur, musMensur );
    ReadMeiLayerElement( mensur, musMensur );
    
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
    
    AddLayerElement( musMensur );
    return true;
}

bool MusMeiInput::ReadMeiNote( TiXmlElement *note )
{
	MusNote *musNote = new MusNote();
    SetMeiUuid( note, musNote );
    ReadMeiLayerElement( note, musNote );
    
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
	
	AddLayerElement( musNote );
    return true;
}

bool MusMeiInput::ReadMeiRest( TiXmlElement *rest )
{
    MusRest *musRest = new MusRest();
    SetMeiUuid( rest, musRest );
    ReadMeiLayerElement( rest, musRest );
    
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
	
	AddLayerElement( musRest );
    return true;
}

bool MusMeiInput::ReadMeiAccid( TiXmlElement *accid )
{
    MusSymbol *musAccid = new MusSymbol( SYMBOL_ACCID );
    SetMeiUuid( accid, musAccid );
    ReadMeiLayerElement( accid, musAccid );
    
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
	
	AddLayerElement( musAccid );
    return true;
}

bool MusMeiInput::ReadMeiCustos( TiXmlElement *custos )
{
    MusSymbol *musCustos = new MusSymbol( SYMBOL_CUSTOS );
    SetMeiUuid( custos, musCustos );
    ReadMeiLayerElement( custos, musCustos );
    
	// position (pitch)
	if ( custos->Attribute( "pname" ) ) {
		musCustos->m_pname = StrToPitch( custos->Attribute( "pname" ) );
	}
	// oct
	if ( custos->Attribute( "oct" ) ) {
		musCustos->m_oct = StrToOct( custos->Attribute( "oct" ) );
	}
	
	AddLayerElement( musCustos );    
    return true;
}

bool MusMeiInput::ReadMeiDot( TiXmlElement *dot )
{
    MusSymbol *musDot = new MusSymbol( SYMBOL_DOT );
    SetMeiUuid( dot, musDot );
    ReadMeiLayerElement( dot, musDot );
    
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
	
	AddLayerElement( musDot );
    return true;
}

bool MusMeiInput::ReadMeiApp( TiXmlElement *app )
{
    m_layerApp = new MusLayerApp( );
    SetMeiUuid( app, m_layerApp );
   
    TiXmlElement *current = NULL;
    for( current = app->FirstChildElement( "rdg" ); current; current = current->NextSiblingElement( "rdg" ) ) {
        ReadMeiRdg( current );
	}
	
	AddLayerElement( m_layerApp );
    m_layerApp = NULL;
    return true;
}

bool MusMeiInput::ReadMeiRdg( TiXmlElement *rdg )
{
    wxASSERT ( !m_layerRdg );
    wxASSERT( m_layerApp );
    
    m_layerRdg = new MusLayerRdg( );
    SetMeiUuid( rdg, m_layerRdg );
    
    if ( rdg->Attribute( "source" ) ) {
        m_layerRdg->m_source = rdg->Attribute( "source" );
    }
    
    // switch to the rdg
    MusObject *previousLayer = m_currentLayer;
    m_currentLayer = m_layerRdg;
    
    bool success = ReadMeiLayer( rdg );
    
    // switch back to the previous one
    m_layerApp->AddLayerRdg( m_layerRdg );
    m_currentLayer = previousLayer;
    m_layerRdg = NULL;
    
    return success;
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
    wxASSERT( m_currentLayer );
    if ( dynamic_cast<MusLayer*>( m_currentLayer ) ) {
        ((MusLayer*)m_currentLayer)->AddElement( element );
    }
    else if ( dynamic_cast<MusLayerRdg*>( m_currentLayer ) ) {
        ((MusLayerRdg*)m_currentLayer)->AddElement( element );
    }
    else if ( dynamic_cast<MusBeam*>( m_currentLayer ) ) {
        ((MusBeam*)m_currentLayer)->AddNote( element );
    }
    
}

void MusMeiInput::SetMeiUuid( TiXmlElement *element, MusObject *object )
{
    if ( !element->Attribute( "xml:id" ) ) {
        return;
    }
    
    uuid_t uuid;
    StrToUuid( element->Attribute( "xml:id" ), uuid );
    object->SetUuid( uuid );
}

void MusMeiInput::StrToUuid(wxString uuid, uuid_t dest)
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

int MusMeiInput::StrToDur(wxString dur)
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
		//wxLogWarning("Unknown duration '%s'", dur.c_str());
        value = DUR_4;
	}
    return value;
}

int MusMeiInput::StrToOct(wxString oct)
{
	return atoi(oct.c_str());
}

int MusMeiInput::StrToPitch(wxString pitch)
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


unsigned char MusMeiInput::StrToAccid(wxString accid)
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


ClefId MusMeiInput::StrToClef( wxString shape, wxString line )
{
    ClefId clefId = SOL2;
    wxString clef = shape + line;
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

MensurSign MusMeiInput::StrToMensurSign(wxString sign)
{
    if (sign == "C") return MENSUR_SIGN_C;
    else if (sign == "O") return MENSUR_SIGN_O;
    else {
        wxLogWarning("Unknown mensur sign '%s'", sign.c_str() );
	}
    // default
	return MENSUR_SIGN_C;
}

DocType MusMeiInput::StrToDocType(wxString type)
{
    if (type == "raw") return Raw;
    else if (type == "rendering") return Rendering;
    else if (type == "transcription") return Transcription;
    else {
        wxLogWarning("Unknown layout type '%s'", type.c_str() );
	}
    // default
	return Raw;
}



