/////////////////////////////////////////////////////////////////////////////
// Name:        headless_main.cpp
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <sstream>
#include <string>

#include "mus.h"
#include "muscontroller.h"

using namespace std;

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

/*
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
*/

extern "C" {
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
}

int main(int argc, char** argv)
{
    
    string m_infile;
    string m_svgdir;
    string m_outfile;
    string m_outformat = "svg";
    
    ConvertFileType m_type;
    bool m_no_mei_hdr = false;
    
    const char *cmdlineopts = "ndmpr:o:t:s:hb:";
    
    int opt;
    
    MusController controller;
    
    // read pae by default
    m_type = pae_file;
    
    if (argc < 2) {
        cerr << "Expecting one input file." << endl << endl;
        display_usage();
        exit(1);
    }
    
    opt = getopt( argc, argv, cmdlineopts );
    while( opt != -1 ) {
        switch( opt ) {
            case 'p': m_type = pae_file; break;
            case 'd': m_type = darms_file; break;
            case 'm': m_type = mei_file; break;
            case 'n': m_no_mei_hdr = true; break;
                
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
                
            default:
                break;
        }
        
        opt = getopt( argc, argv, cmdlineopts );
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
    
    if (m_outformat == "mei" && (m_type = mei_file)) {
        cerr << "Cannot convert from mei to mei (do not specify -m and -t mei)." << endl;
        exit(1);
    }
    
    if (controller.GetScale() > 1000) {
        cerr << "Scale cannot be > 1000." << endl;
        exit(1);
    }
    
    cerr << "Reading " << m_infile << "..." << endl;

    if ( !controller.LoadFile( m_infile, m_type ) ) {
        exit(1);
    }
    
    // create outfile
    if (m_outfile.length() == 0) {
        /*
        std::vector<string> v = split(m_infile, '/');
        m_outfile = v[v.capacity() - 1] + "." + m_outformat; // can be only mei or svg
        */
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

extern "C" {
    char * convertMusic(ConvertFileType input_format, ConvertFileType output_format, const char * c_data) {
        
        string data(c_data);
        string out_str;
        char * leak_me;
        
        MusController controller;
        
        Mus::SetResourcesPath("/data");
        
        // default to mei if unset.
        if (input_format == pae_file) {
            controller.LoadString( data, pae_file );
        } else {
            controller.LoadString( data, mei_file );
        }
        
        // in the future we will be able to render to mei too
        out_str = controller.RenderToSvg();

        /* is this the correct way to output strings? */
        // LP What happen if you just return out_str.c_str() and set the return type to const char *
        leak_me = (char *)malloc(strlen(out_str.c_str()) + 1);
        strcpy(leak_me, out_str.c_str());
        return leak_me;
    }
}