/////////////////////////////////////////////////////////////////////////////
// Name:        headless_main.cpp
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <ctime>

#include <sstream>
#include <string>

#include <getopt.h>

#include "mus.h"
#include "muscontroller.h"

using namespace std;

void display_usage() {
    cerr << "Aruspix command line usage:" << endl;
    cerr << "aruspix [-f pae | mei | darms] [-s scale] [-t mei | svg] [-r resources] [-o outfile] infile" << endl << endl;

    cerr << "-b, --border=BORDER" << endl;
    cerr << "\t\tAdd border (10 px default, max 1000)" << endl;

    cerr << "-f, --format=INPUT_FORMAT" << endl;
    cerr << "\t\tSelect input format: darms, mei, pae" << endl;
    cerr << "\t\t(optional, default is pae)" << endl;
    
    cerr << "-o, --output=FILE_NAME" << endl;
    cerr << "\t\tOutput file name" << endl;
    
    cerr << "-r, --recources=PATH" << endl;
    cerr << "\t\tSpecify path for SVG resoucces" << endl;
    cerr << "\t\t(default in " <<  Mus::GetResourcesPath() << ")" << endl;
    
    cerr << "-s, --scale=FACTOR" << endl;
    cerr << "\t\tscale percent (100 default, max 1000)" << endl;
    
    cerr << "-t, --type=OUTPUT_TYPE" << endl;
    cerr << "\t\tSelect output format: mei, svg" << endl;
    cerr << "\t\t(optional, default is svg)" << endl;

    
}


int main(int argc, char** argv)
{
    
    string m_infile;
    string m_svgdir;
    string m_outfile;
    string m_outformat = "svg";
    
    // Init random number generator for uuids
    std::srand(std::time(0));
    
    ConvertFileFormat m_type;
    int m_no_mei_hdr = 0;
      
    MusController controller;
    
    // read pae by default
    m_type = pae_file;
    
    if (argc < 2) {
        cerr << "Expecting one input file." << endl << endl;
        display_usage();
        exit(1);
    }
    int c;
    
    static struct option long_options[] =
    {
        {"no-mei-hdr",  no_argument,      &m_no_mei_hdr, 1},
        {"border",      required_argument, 0, 'b'},
        {"format",      required_argument, 0, 'f'},
        {"output",      required_argument, 0, 'o'},
        {"resources",   required_argument, 0, 'r'},
        {"scale",       required_argument, 0, 's'},
        {"type",        required_argument, 0, 't'},
        {0, 0, 0, 0}
    };
    
    while ((c = getopt_long(argc, argv, "b:f:o:r:s:t:", long_options, NULL)) != -1)
    {                
        switch (c)
        {
            case '0': break;

            case 'f': {
                string informat = string(optarg);
                if (informat == "pae")
                    m_type = pae_file;
                else if(informat == "darms")
                    m_type = darms_file;
                else if(informat == "mei")
                    m_type = mei_file;
                else {
                    cerr << "Input format can only be: pae mei darms" << endl;
                    display_usage();
                    exit(1);
                }
            }
                break;
                
            case 'r':
                Mus::SetResourcesPath(optarg);
                break;
                
            case 'o':
                m_outfile = string(optarg);
                break;
                
            case 't':
                m_outformat = string(optarg);
                break;
                
            case 's':
                controller.SetScale( atoi(optarg) );
                break;
                
            case 'b':
                controller.SetBorder( atoi(optarg) );
                break;
                
            case 'h':
                display_usage();
                exit(0);
                break;
            
            case '?':
                display_usage();
                exit(0);
                break;
                
            default:
                break;
        }
    }
    
    if (optind <= argc - 1) {
        m_infile = string(argv[optind]);
    }
    else {
        cerr << "Incorrect number of options: expecting one input file." << endl << endl;
        display_usage();
        exit(1);
    }
    
    if (m_outformat != "svg" && m_outformat != "mei") {
        cerr << "Output format can only be: mei svg" << endl;
        exit(1);
    }
    
    if (m_outformat == "mei" && m_type == mei_file) {
        cerr << "Cannot convert from mei to mei (do not specify -f mei and -t mei)." << endl;
        exit(1);
    }
    
    if (controller.GetScale() > 1000) {
        cerr << "Scale cannot be > 1000." << endl;
        exit(1);
    }
    
    cerr << "Reading " << m_infile << "..." << endl;

    controller.SetFormat(m_type);
    
    if ( !controller.LoadFile( m_infile ) ) {
        exit(1);
    }
    
    // Create SVG or mei
    if (m_outformat == "svg") {
        if ( !controller.RenderToSvgFile( m_outfile ) ) {
            cerr << "Unable to write SVG to " << m_outfile << "." << endl;
            exit(1);
        }
        // Write it to file
        
    } else {
        // To be implemented in MusController
        /*
        MusXMLOutput meioutput(doc, m_outfile.c_str());
        if (!meioutput.ExportFile()) {
            cerr << "Unable to write MEI to " << m_outfile << "." << endl;
            exit(1);
        }
        */
        
    }
    cerr << "Output written to " << m_outfile << endl;
    
    return 0;
}
