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

#include "musbarline.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musneumesymbol.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"

#include "muslaidoutlayerelement.h"

#include "app/axapp.h"

#include <vector>
using std::vector;

#include <uuid/uuid.h>

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
        mei::MeiDocument *meiDoc = new mei::MeiDocument();
        
        m_mei = new MeiElement("mei");
        // this starts the call of all the functors
        
        m_doc->Save( this );

        meiDoc->setRootElement(m_mei);
        XmlExport::meiDocumentToFile( meiDoc, m_filename.c_str() );
    }
    catch( char * str ) {
        wxLogError("%s", str );
        return false;
    }
	return true;    
}

std::string MusMeiOutput::GetMeiUuid( MusObject *element )
{
    uuid_string_t uuidStr;
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
    
    p1->setValue( wxString::Format( "Encoded with Aruspix version %s",  AxApp::s_version.c_str() ).c_str() );
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
    m_div->setId( GetMeiUuid( div ));
    m_body->addChild( m_div );
    return true;
}

bool MusMeiOutput::WriteScore( MusScore *score )
//bool MusMeiOutput::WriteMeiScore( Score *meiScore, MusScore *score )
{
    wxASSERT( m_div );
    m_score = new Score();
    m_score->setId( GetMeiUuid( score ));
    m_div->addChild( m_score );
    return true;
}

bool MusMeiOutput::WritePartSet( MusPartSet *partSet )
//bool MusMeiOutput::WriteMeiParts( Parts * meiParts, MusPartSet *partSet )
{
    wxASSERT( m_div );
    m_parts = new Parts();
    m_parts->setId( GetMeiUuid( partSet ));
    m_div->addChild( m_parts );
    return true;
}

bool MusMeiOutput::WritePart( MusPart *part )
//bool MusMeiOutput::WriteMeiPart( Part *meiPart, MusPart *part )
{
    wxASSERT( m_parts );
    m_part = new Part();
    m_part->setId( GetMeiUuid( part ));
    m_parts->addChild( m_part );
    return true;
}

bool MusMeiOutput::WriteSection( MusSection *section )
//bool MusMeiOutput::WriteMeiSection( Section *meiSection, MusSection *section )
{
    wxASSERT( m_score || m_part );
    m_section = new Section();
    m_section->setId( GetMeiUuid( section ));
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
    m_measure->setId( GetMeiUuid( measure ));
    m_section->addChild( m_measure );
    return true;
}

bool MusMeiOutput::WriteStaff( MusStaff *staff )
//bool MusMeiOutput::WriteMeiStaff( Staff *meiStaff, MusStaff *staff )
{
    wxASSERT( m_section || m_measure );
    m_staff = new Staff();
    m_staff->setId( GetMeiUuid( staff ));
    if ( m_section ) {
        m_section->addChild( m_staff );
    }
    else {
        m_measure->addChild( m_staff );
    }
    return true;
}

bool MusMeiOutput::WriteLayer( MusLayer *layer )
//bool MusMeiOutput::WriteMeiLayer( Layer *meiLayer, MusLayer *layer )
{
    wxASSERT( m_staff );
    m_layer = new Layer();
    m_layer->setId( GetMeiUuid( layer ));
    m_staff->addChild( m_layer );
    return true;
}

bool MusMeiOutput::WriteLayerElement( MusLayerElement *element )
{
    wxASSERT( m_layer );
    
    MeiElement *meiElement = NULL;
    if (dynamic_cast<MusBarline*>(element)) {
        BarLine *barline = new BarLine();
        WriteMeiBarline( barline, dynamic_cast<MusBarline*>(element) );
        meiElement = barline;
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
        Note *note = new Note();
        WriteMeiNote( note, dynamic_cast<MusNote*>(element) );
        meiElement = note;
    }
    else if (dynamic_cast<MusRest*>(element)) {
        Rest *rest = new Rest();
        WriteMeiRest( rest, dynamic_cast<MusRest*>(element) );
        meiElement = rest;
    }
    else if (dynamic_cast<MusSymbol*>(element)) {        
        meiElement = WriteMeiSymbol( dynamic_cast<MusSymbol*>(element) );
    }   
    
    // we have it, set the uuid we read
    if ( meiElement ) {
        meiElement->setId( GetMeiUuid( element ));
        m_layer->addChild( meiElement );
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


void MusMeiOutput::WriteMeiClef( Clef *meiClef, MusClef *clef )
{
    meiClef->m_Lineloc.setLine( ClefLineToStr( clef->m_clefId ) );
    meiClef->m_Clefshape.setShape( ClefShapeToStr( clef->m_clefId ) );
    return;
}


void MusMeiOutput::WriteMeiMensur( Mensur *meiMensur, MusMensur *mensur )
{
    if ( mensur->m_sign ) {
        meiMensur->m_MensurVis.setSign( MensurSignToStr( mensur->m_sign ));
    }
    if ( mensur->m_dot ) {
        meiMensur->m_MensurVis.setDot("true");
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
    // missing m_artic, m_chord, m_colored, m_lig, m_headShape, m_ligObliqua, m_slur, m_stemDir, m_stemLen
    return;
}

void MusMeiOutput::WriteMeiRest( Rest *meiRest, MusRest *rest )
{    
    meiRest->m_DurationMusical.setDur( DurToStr( rest->m_dur ));
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
    m_layout->setId( GetMeiUuid( layout ));
    m_layouts->addChild( m_layout );
    return true;
}

bool MusMeiOutput::WritePage( MusPage *page )
{
    wxASSERT( m_layout );
    m_page = new Page();
    m_page->setId( GetMeiUuid( page ));
    MeiCommentNode *comment = new MeiCommentNode();
    comment->setValue("Coordinates needs to be corrected");
    m_layout->addChild( comment );
    m_layout->addChild( m_page );
    return true;
}

bool MusMeiOutput::WriteSystem( MusSystem *system )
{
    wxASSERT( m_page );
    m_system = new System();
    m_system->setId( GetMeiUuid( system ));
    // x - y positions (to be corrected)
    m_system->m_Coordinated.setUlx( wxString::Format( "%d", system->m_x_abs ).c_str() );
    m_system->m_Coordinated.setUly( wxString::Format( "%d", system->m_y_abs ).c_str() );
    m_page->addChild( m_system );
    return true;
}

bool MusMeiOutput::WriteLaidOutStaff( MusLaidOutStaff *laidOutStaff )
{
    wxASSERT( m_system );
    m_laidOutStaff = new LaidOutStaff();
    m_laidOutStaff->setId( GetMeiUuid( laidOutStaff ));
    // x - y positions (to be corrected)
    m_laidOutStaff->m_Coordinated.setUlx( wxString::Format( "%d", laidOutStaff->m_x_abs ).c_str() );
    m_laidOutStaff->m_Coordinated.setUly( wxString::Format( "%d", laidOutStaff->m_y_abs ).c_str() );
    m_system->addChild( m_laidOutStaff );
    return true;
}

bool MusMeiOutput::WriteLaidOutLayer( MusLaidOutLayer *laidOutLayer )
{    
    wxASSERT( m_laidOutStaff );
    m_laidOutLayer = new LaidOutLayer();
    m_laidOutLayer->setId( GetMeiUuid( laidOutLayer ));
    m_laidOutStaff->addChild( m_laidOutLayer );
    return true;
}

bool MusMeiOutput::WriteLaidOutLayerElement( MusLaidOutLayerElement *laidOutLayerElement )
{
    wxASSERT( m_laidOutLayer );
    LaidOutElement *element = new LaidOutElement();
    element->setId( GetMeiUuid( laidOutLayerElement ));
    // x - y position (to be corrected)
    element->m_Coordinated.setUlx( wxString::Format( "%d", laidOutLayerElement->m_x_abs ).c_str() );
    //element->m_Coordinated.setUly( wxString::Format( "%d", laidOutLayerElement->m_y_abs ).c_str() );
    // pointer to the logical element
    element->m_Pointing.setTarget(GetMeiUuid( laidOutLayerElement->m_layerElement ) );
    m_laidOutLayer->addChild( element );
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
	{	case SOL2 : value = "2"; break;
		case SOL1 : value = "1"; break; 
		case SOLva : value = "2"; break;
		case FA5 : value = "5"; break;
		case FA4 : value = "4"; break;
		case FA3 : value = "3"; break;
		case UT2 : value = "2"; break;
		case UT3 : value = "3"; break;
		case UT5 : value = "5"; break;
		case UT4 : value = "4"; break;		
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
	{	case SOL2 : 
		case SOL1 : 
		case SOLva : value = "G"; break;
		case FA5 : 
		case FA4 :
		case FA3 : value = "F"; break;
		case UT2 : 
		case UT3 : 
		case UT5 : 
		case UT4 : value = "C"; break;		
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

//----------------------------------------------------------------------------
// MusMeiInput
//----------------------------------------------------------------------------

MusMeiInput::MusMeiInput( MusDoc *doc, wxString filename ) :
    // This is pretty bad. We open a bad fileoinputstream as we don't use it
	MusFileInputStream( doc, -1 )
{
    m_filename = filename;
	
	m_div = NULL;
	m_score = NULL;
	m_parts = NULL;
	m_part = NULL;
	m_section = NULL;
	m_measure = NULL;
	m_staff = NULL;
	m_layer = NULL;
}

MusMeiInput::~MusMeiInput()
{
}

bool MusMeiInput::ImportFile( )
{
    try {
        mei::MeiDocument *doc = XmlImport::documentFromFile( m_filename.c_str() );
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
        if ( root->hasChildren("music") ) {
            music = root->getChildrenByName("music")[0];
        }
        if ( music && music->hasChildren("body") ) {
            body = music->getChildrenByName("body")[0];
        }
		if ( body && body->hasChildren("mdiv") ) {
			vector<MeiElement*> children = body->getChildrenByName("mdiv");
			for (vector<MeiElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
				MeiElement *e = *iter;
				m_div = new MusDiv( );
				if (ReadMeiDiv(dynamic_cast<Mdiv*>(e))) {
					m_doc->AddDiv( m_div );
				}
				else {
					delete m_div;
				}
				m_div = NULL;
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
			if (e->getName()=="note") {
				if (!ReadMeiNote(dynamic_cast<Note*>(e))) {
					return false;
				}
			}
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
    return true;
}

bool MusMeiInput::ReadMeiClef( Clef *clef )
{
    return true;
}

bool MusMeiInput::ReadMeiMensur( Mensur *mensur )
{
    return true;
}

bool MusMeiInput::ReadMeiNote( Note *note )
{
	MusNote *musNote = new MusNote();
	// pitch
	if ( note->m_Pitch.hasPname() ) {
		musNote->m_pname = StrToPitch( note->m_Pitch.getPname()->getValue() );
	}
	// oct
	if ( note->m_Octave.hasOct() ) {
		musNote->m_oct = StrToOct( note->m_Octave.getOct()->getValue() );
	}
	// duration
	
	m_layer->AddLayerElement( musNote );
    return true;
}

bool MusMeiInput::ReadMeiRest( Rest *rest )
{
    return true;
}

bool MusMeiInput::ReadMeiSymbol( MeiElement *symbol )
{
    return true;
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



