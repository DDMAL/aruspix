/////////////////////////////////////////////////////////////////////////////
// Name:        muscontroller.cpp
// Author:      Laurent Pugin
// Created:     17/10/2013
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "muscontroller.h"

//----------------------------------------------------------------------------

#include <iostream>

//----------------------------------------------------------------------------

#include "musbboxdc.h"
#include "musdoc.h"
#include "musiodarms.h"
#include "musiomei.h"
#include "musiomusxml.h"
#include "musiopae.h"
#include "muspage.h"
#include "musrc.h"
#include "mussvgdc.h"
#include "mussystem.h"

#ifdef USE_EMSCRIPTEN
#include "jsonxx.h"
using namespace jsonxx;
#endif

//----------------------------------------------------------------------------
// MusController
//----------------------------------------------------------------------------

MusController::MusController()
{
    m_border = 10;
    m_scale = 100;
    m_format = pae_file;
}


MusController::~MusController()
{
}

bool MusController::LoadFile( std::string filename )
{
    MusFileInputStream *input = NULL;
    if (m_format == pae_file) {
        input = new MusPaeInput( &m_doc, filename.c_str() );
    } else if (m_format == darms_file) {
        input = new MusDarmsInput( &m_doc, filename.c_str() );
    } else if (m_format == mei_file) {
        input = new MusMeiInput( &m_doc, filename.c_str() );
    }
    else {
        Mus::LogError( "Unknown format" );
        return false;
    }
    
    // something went wrong
    if ( !input ) {
        Mus::LogError( "Unknown error" );
        return false;
    }

    // load the file
    if ( !input->ImportFile()) {
        Mus::LogError( "Error importing file '%s'", filename.c_str() );
        delete input;
        return false;
    }
    
    m_rc.SetDoc( &m_doc );
    delete input;
    return true;
}

bool MusController::LoadString( std::string data )
{
    MusFileInputStream *input = NULL;
    if (m_format == pae_file) {
        input = new MusPaeInput( &m_doc, "" );
    } else if (m_format == mei_file) {
        input = new MusMeiInput( &m_doc, "" );
    }
    else {
        Mus::LogError( "Unknown format" );
        return false;
    }
    
    // something went wrong
    if ( !input ) {
        Mus::LogError( "Unknown error" );
        return false;
    }
    
    // load the file
    if ( !input->ImportString( data )) {
        Mus::LogError( "Error importing data" );
        delete input;
        return false;
    }
    
    m_rc.SetDoc( &m_doc );
    delete input;
    return true;
}

bool MusController::ParseOptions( std::string json_options ) {
#ifdef USE_EMSCRIPTEN
    std::string in_format = "pae";
    
    int scale = m_scale;
    int border = m_border;
    
    Object json;
        
    // Read JSON options
    if (!json.parse(json_options)) {
        Mus::LogError( "Can not parse JSON string." );
        return false;
    }
    
    if (json.has<String>("InputFormat"))
        in_format = json.get<String>("InputFormat");
    
    if (json.has<Number>("Scale"))
        scale = json.get<Number>("Scale");
    
    if (json.has<Number>("Border"))
        border = json.get<Number>("Border");
    
    if (in_format == "pae") 
        SetFormat(pae_file);
    else if (in_format == "mei")
        SetFormat(mei_file);
    else if (in_format == "darms")
        SetFormat(darms_file);
    else { // fail if format in invalid
        Mus::LogError( "InputFormat is invalid: %s\n", in_format.c_str() );
        return false;
    }
    
    // Check boundaries for scale and border
    
    if (border < 0 || border > 1000) {
        Mus::LogError( "Border out of bounds, use 10 (default)." );
        border = 10;
    }
        
    if (scale < 0 || scale > 1000) {
        Mus::LogError( "Scale out of bounds, use 10 (default)." );
        scale = 100;
    }
    
    SetScale(scale);
    SetBorder(border);
    
    return true;
    
#else
    // The non js version of the app should not use this function.
    return false;
#endif
    
}

std::string MusController::RenderToSvg( int pageNo, bool xml_tag )
{
    m_doc.Layout();
    
    // Get the current system for the SVG clipping size
    MusPage *page = dynamic_cast<MusPage*>(m_doc.m_children[0]);
    //MusSystem *system = dynamic_cast<MusSystem*>(page->m_children[0]);
    
    // Create the SVG object, h & w come from the system
    // we add border*2 so it is centered into the image
    //MusSvgDC svg(system->m_contentBB_x2 - system->m_contentBB_x1 + m_border * 2, (system->m_contentBB_y2 - system->m_contentBB_y1) + m_border * 2);
    MusSvgDC svg( m_doc.m_pageWidth, m_doc.m_pageHeight );
    
    // set scale and border from user options
    svg.SetUserScale((double)m_scale / 100, (double)m_scale / 100);
    svg.SetLogicalOrigin(m_border, m_border);
    
    // render the page
    m_rc.DrawPage( &svg, page , false);
    
    std::string out_str = svg.GetStringSVG( xml_tag );
    return out_str;
}


bool MusController::RenderToSvgFile( std::string filename, int pageNo )
{
    std::string output = RenderToSvg( pageNo, true );
    
    std::ofstream outfile;
    outfile.open ( filename.c_str() );
    
    if ( !outfile.is_open() ) {
        // add message?
        return false;
    }
    
    outfile << output;
    outfile.close();
    return true;
}
