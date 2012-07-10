//
//  headless_main.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 22/06/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#include <iostream>
#include <string>
#include <sstream>

#include <wx/string.h>
#include "wx/wx.h"

#include "musdoc.h"
#include "musiopae.h"
#include "muslayout.h"
#include "musrc.h"
#include "mussvgdc.h"
#include "musbboxdc.h"
#include "muspage.h"
#include "mussystem.h"
#include "musiomei.h"
#include "musiodarms.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

string m_infile;
string m_svgdir;
string m_outfile;

const char *cmdlineopts = "r:o:h";

// Some handy string split functions
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
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


void display_usage() {
    cout << "Aruspix headless usage:" << endl;
    cout << "aruspix [-o outfile -r resources -h] infile" << endl << endl;

    cout << "Resources default dir: " << MusDoc::GetResourcesPath() << endl;
}

int main(int argc, char** argv) {
    
    int opt;
    
    if (argc < 2) {
        cout << "Expecting one input file." << endl << endl;
        display_usage();
        exit(1);
    }
    
    opt = getopt( argc, argv, cmdlineopts );
    while( opt != -1 ) {
        switch( opt ) {
            case 'r':
                MusDoc::SetResourcesPath(optarg);
                break;
                
            case 'o':
                m_outfile = *new string(optarg);
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
        cout << "Incorrect number of options: expecting one input file." << endl << endl;
        display_usage();
	exit(1);
    }
    
    // create outfile
    if (m_outfile.length() == 0) {
        std::vector<std::string> v = split(m_infile, '/');
        m_outfile = v[v.capacity() - 1] + ".svg";
    }
    
    cout << "Reading " << m_infile << "..." << endl;
    
    MusDoc *doc =  new MusDoc();

    MusDarmsInput meiinput( doc, m_infile.c_str() );
	if ( !meiinput.ImportFile() )
		return -1;
    
	MusLayout *layout = new MusLayout( Raw );
	layout->Realize(doc->m_divs[0].m_score);
	doc->AddLayout( layout );
    layout->SpaceMusic();
        
    MusPage *page = &layout->m_pages[0];
    MusSystem *system = &page->m_systems[0];
    
    MusRC rc;
    rc.SetLayout(layout);
    layout->m_leftMargin = 0; // good done here?
    MusSvgDC *svg = new MusSvgDC(m_outfile.c_str(), system->m_contentBB_x2 - system->m_contentBB_x1, (system->m_contentBB_y2 - system->m_contentBB_y1));
    rc.DrawPage(svg, &layout->m_pages[0] , false);
    
    delete svg;
    
    cout << "Output written to " << m_outfile << endl;
    
    return 0;
}
