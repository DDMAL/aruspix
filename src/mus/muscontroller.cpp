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
    
    // default page size
    m_pageHeight = 2970;
    m_pageWidth = 2100;
    
    m_noLayout = false;
    m_ignoreLayout = false;
    m_adjustPageHeight = false;
    m_noJustification = false;
    m_showBoundingBoxes = false;
    
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
    
    int width = m_pageWidth;
    int height = m_pageHeight;
    
    Object json;
        
    // Read JSON options
    if (!json.parse(json_options)) {
        Mus::LogError( "Can not parse JSON string." );
        return false;
    }
    
    if (json.has<String>("inputFormat"))
        in_format = json.get<String>("inputFormat");
    
    if (json.has<Number>("scale"))
        scale = json.get<Number>("scale");
    
    if (json.has<Number>("border"))
        border = json.get<Number>("border");

    if (json.has<Number>("pageWidth"))
        width = json.get<Number>("pageWidth");
    
    if (json.has<Number>("pageHeight"))
        height = json.get<Number>("pageHeight");
    
    // Parse the various flags
    if (json.has<Boolean>("noLayout"))
        SetNoLayout(json.get<Boolean>("noLayout"));
    
    if (json.has<Boolean>("ignoreLayout"))
        SetIgnoreLayout(json.get<Boolean>("ignoreLayout"));

    if (json.has<Boolean>("adjustPageHeight"))
        SetAdjustPageHeight(json.get<Boolean>("adjustPageHeight"));

    if (json.has<Boolean>("noJustification"))
        SetNoJustification(json.get<Boolean>("noJustification"));

    if (json.has<Boolean>("showBoundingBoxes"))
        SetShowBoundingBoxes(json.get<Boolean>("showBoundingBoxes"));
    
    // set file type
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
    
    if (border < 0 || border > 1000)
        Mus::LogError( "Border out of bounds, use 10 (default)." );
    else
        SetBorder(border);
        
    if (scale < 0 || scale > 1000)
        Mus::LogError( "Scale out of bounds, use 10 (default)." );
    else
        SetScale(scale);
    
    if (width < 0 || width > 5000)
        Mus::LogError( "Page width out of bounds" );
    else
        SetPageWidth(width);
    
    if (height < 0 || height > 5000)
        Mus::LogError( "Page Height out of bounds." );
    else
        SetPageHeight(height);
    
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
    Page *page = dynamic_cast<Page*>(m_doc.m_children[0]);
    //System *system = dynamic_cast<System*>(page->m_children[0]);
    
    // Create the SVG object, h & w come from the system
    // we add border*2 so it is centered into the image
    //SvgDeviceContext svg(system->m_contentBB_x2 - system->m_contentBB_x1 + m_border * 2, (system->m_contentBB_y2 - system->m_contentBB_y1) + m_border * 2);
    SvgDeviceContext svg( m_pageWidth + m_border * 2, m_pageHeight + m_border * 2 );
    
    // set scale and border from user options
    svg.SetUserScale((double)m_scale / 100, (double)m_scale / 100);
    svg.SetLogicalOrigin(m_border, m_border);
    
    // debug BB?
    svg.SetDrawBoundingBoxes(m_showBoundingBoxes);
    
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
