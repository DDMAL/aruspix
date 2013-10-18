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

//----------------------------------------------------------------------------
// MusController
//----------------------------------------------------------------------------

MusController::MusController()
{
    m_border = 10;
    m_scale = 100;
    
    m_cString = NULL;
}


MusController::~MusController()
{
    if (m_cString) {
        free( m_cString );
    }
}

bool MusController::LoadFile( std::string filename, ConvertFileType type )
{
    MusFileInputStream *input = NULL;
    if (type == pae_file) {
        input = new MusPaeInput( &m_doc, filename.c_str() );
    } else if (type == darms_file) {
        input = new MusDarmsInput( &m_doc, filename.c_str() );
    } else if (type == mei_file) {
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

bool MusController::LoadString( std::string data, ConvertFileType type )
{
    MusFileInputStream *input = NULL;
    if (type == pae_file) {
        input = new MusPaeInput( &m_doc, "" );
    } else if (type == mei_file) {
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

std::string MusController::RenderToSvg( int pageNo, bool xml_tag )
{
    m_doc.SpaceMusic();
    
    // Get the current system for the SVG clipping size
    MusPage *page = dynamic_cast<MusPage*>(m_doc.m_children[0]);
    MusSystem *system = dynamic_cast<MusSystem*>(page->m_children[0]);
    
    // Create the SVG object, h & w come from the system
    // we add border*2 so it is centered into the image
    MusSvgDC svg(system->m_contentBB_x2 - system->m_contentBB_x1 + m_border * 2, (system->m_contentBB_y2 - system->m_contentBB_y1) + m_border * 2);
    
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

void MusController::SetCString( std::string data )
{
    if (m_cString) {
        free(m_cString);
        m_cString = NULL;
    }
    
    m_cString = (char *)malloc(strlen(data.c_str()) + 1);
    
    // something went wrong
    if (!m_cString) {
        return;
    }
    strcpy(m_cString, data.c_str());
}

char *MusController::GetCString( )
{
    if (m_cString) {
        return m_cString;
    }
    else {
        return "[unspecified]";
    }
}