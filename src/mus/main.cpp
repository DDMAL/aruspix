/////////////////////////////////////////////////////////////////////////////
// Name:        headless_main.cpp
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

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

using namespace std;

string m_infile;
string m_svgdir;
string m_outfile;
string m_outformat = "svg";
int m_scale = 100;
int m_boder = 10;

bool m_pae = false;
bool m_darms = false;
bool m_mei = false;
bool m_no_mei_hdr = false;

const char *cmdlineopts = "ndmpr:o:t:s:hb:";

// Some handy string split functions
/*
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    wxStringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}
*/

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void display_usage() {
    cerr << "Aruspix headless usage:" << endl;
    cerr << "aruspix [-d -p -m] [-s -t mei, svg] [-o outfile -r resources -h] infile" << endl << endl;

    cerr << "-d read DARMS file [default if no option is given]" << endl;
    cerr << "-p read PAE file." << endl;
    cerr << "-m read MEI file." << endl;
    cerr << "-t select output format: mei, svg (default)"<< endl;
    cerr << "-s scale percent (100 default, max 1000)" << endl<< endl;
    cerr << "-b add border (10 px default, max 1000)" << endl;
    
    cerr << "Resources default dir: " << Mus::GetResourcesPath() << endl;
}

int main(int argc, char** argv) {
    
    int opt;
    
    if (argc < 2) {
        cerr << "Expecting one input file." << endl << endl;
        display_usage();
        exit(1);
    }
    
    opt = getopt( argc, argv, cmdlineopts );
    while( opt != -1 ) {
        switch( opt ) {
            case 'p': m_pae = true; break;
            case 'd': m_darms = true; break;
            case 'm': m_mei = true; break;
            case 'n': m_no_mei_hdr = true; break;
                
            case 'r':
                Mus::SetResourcesPath(optarg);
                break;
                
            case 'o':
                m_outfile = *new string(optarg);
                break;
            
            case 't':
                m_outformat = *new string(optarg);
                break;
                
            case 's':
                m_scale = atoi(optarg);
                break;
                
            case 'b':
                m_boder = atoi(optarg);
                break;
                
            case 'h':
                display_usage();
                exit(0);
                break;
                
            default:
                break;
        }
        
        opt = getopt( argc, argv, cmdlineopts );
    }
    
    if (optind <= argc - 1) {
        m_infile = *new string(argv[optind]);
    }
    else {
        cerr << "Incorrect number of options: expecting one input file." << endl << endl;
        display_usage();
        exit(1);
    }
    
    if ((m_pae && m_mei) || (m_pae && m_darms) || (m_darms && m_mei) || (m_pae && m_darms && m_mei)) {
        cerr << "Please select only one output option: -d -m -p." << endl;
        exit(1);
    }
    
    // If no input type is specified, set darms as default
    if (!m_pae && !m_darms && !m_mei)
        m_darms = true;
    
    
    if (m_outformat != "svg" && m_outformat != "mei") {
        cerr << "Output format can only be: mei svg" << endl;
        exit(1);
    }
    
    if (m_outformat == "mei" && m_mei) {
        cerr << "Cannot convert from mei to mei (do not specify -m and -t mei)." << endl;
        exit(1);
    }
    
    if (m_scale > 1000) {
        cerr << "Scale cannot be > 1000." << endl;
        exit(1);
    }
    
    cerr << "Reading " << m_infile << "..." << endl;
    
    MusDoc *doc =  new MusDoc();

    MusFileInputStream import;
    
    if (m_pae) {
        MusPaeInput mpae( doc, m_infile.c_str() );
        if ( !mpae.ImportFile()) {
            cerr << "Error importing " << m_outfile << "." << endl;
            exit(1);
        }
    } else if (m_darms) {
        MusDarmsInput mdarms( doc, m_infile.c_str() );
        if ( !mdarms.ImportFile()) {
           cerr << "Error importing " << m_outfile << "." << endl;
            exit(1);
        }
    } else if (m_mei){
        MusMeiInput meiinput( doc, m_infile.c_str() );
        if ( !meiinput.ImportFile()) {
            cerr << "Error importing " << m_outfile << "." << endl;
            exit(1);
        }
    } else {
        cerr << "No input type selected! Should never get here, default is DARMS" << endl;
        exit(1); 
    }
    
    // create outfile
    if (m_outfile.length() == 0) {
        std::vector<string> v = split(m_infile, '/');
        m_outfile = v[v.capacity() - 1] + "." + m_outformat; // can be only mei or svg
    }
    
    // Create SVG or mei
    if (m_outformat == "svg") {
        
        // Create a new visual layout and spave the music
        //doc->Realize( );
        doc->SpaceMusic();
        
        // Get the current system for the SVG clipping size    
        MusPage *page = dynamic_cast<MusPage*>(doc->m_children[0]);
        MusSystem *system = dynamic_cast<MusSystem*>(page->m_children[0]);
        
        // creare a new local RC and set the above created layout
        MusRC rc;
        rc.SetDoc(doc);
        // no left margin
        //layout->m_leftMargin = 0; // good done here?
        
        // Create the SVG object, h & w come from the system
        // we add border*2 so it is centered into the image
        MusSvgDC *svg = new MusSvgDC(m_outfile.c_str(), system->m_contentBB_x2 - system->m_contentBB_x1 + m_boder*2, (system->m_contentBB_y2 - system->m_contentBB_y1) + m_boder*2);
        
        // set scale and border from user options
        svg->SetUserScale((double)m_scale / 100, (double)m_scale / 100);
        svg->SetLogicalOrigin(m_boder, m_boder);
        
        // render the page
        rc.DrawPage(svg, page , false);
        
        delete svg;
    } else {
        MusXMLOutput meioutput(doc, m_outfile.c_str());
        if (!meioutput.ExportFile()) {
            cerr << "Unable to write MEI to " << m_outfile << "." << endl;
            exit(1);
        }
        
    }
    cerr << "Output written to " << m_outfile << endl;
    
    return 0;
}
