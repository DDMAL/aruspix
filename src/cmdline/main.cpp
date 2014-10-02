/////////////////////////////////////////////////////////////////////////////
// Name:        main.cpp
// Author:      Laurent Pugin
// Created:     12/09/2014
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <assert.h>
#include <cstdlib>
#include <ctime>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

#include "wx/string.h"

#include "app/axapp.h"
#include "app/axprogressdlg.h"

#include "im/impage.h"

#include "recognition/rec.h"
#include "recognition/recfile.h"
#include "recognition/recmodels.h"

using namespace std;

// Some redundant code to get basenames
// and remove extensions
// possible that it is not in std??
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '\\' || ch == '/';
    }
};

std::string basename( std::string const& pathname )
{
    return std::string( 
                       std::find_if( pathname.rbegin(), pathname.rend(),
                                    MatchPathSeparator() ).base(),
                       pathname.end() );
}

std::string removeExtension( std::string const& filename )
{
    std::string::const_reverse_iterator
    pivot
    = std::find( filename.rbegin(), filename.rend(), '.' );
    return pivot == filename.rend()
    ? filename
    : std::string( filename.begin(), pivot.base() - 1 );
}

std::string removePath( std::string const& filename )
{
    std::string::const_reverse_iterator
    pivot
    = std::find( filename.rbegin(), filename.rend(), '/' );
    return pivot == filename.rend()
    ? filename
    : std::string( pivot.base(), filename.end() );
}

bool dir_exists (string dir) {
    struct stat st;
    if ((stat(dir.c_str(), &st) == 0) && (((st.st_mode) & S_IFMT) == S_IFDIR)) {
        return true;
    } else {
        return false;
    }
}

void display_usage() {
    
    //cerr << "Aruspix " << GetVersion() << endl << "Usage:" << endl << endl;
    cerr << " aruspix-cmdline [-d outdir] [-o outfile] [-t tmpdir] infile" << endl << endl;

    // Options
    cerr << "Options" << endl;

    cerr << " -d, --outdir=OUTDIR       Output directory (Default is '.')" << endl;
    
    cerr << " -o, --outfile=FILENAME    Output file name (Default is input file name as .axz" << endl;
    
    cerr << " -t, --tmpdir=TMPDIR       Directory for temporary files (default is './tmp')" << endl;
}


int main(int argc, char** argv)
{
    string outdir;
    string infile;
    string outfile;
    
    // Init random number generator for uuids
    std::srand(std::time(0));
    
    int show_help = 0;
    
    if (argc < 2) {
        cerr << "Expecting one input file." << endl << endl;
        display_usage();
        exit(1);
    }
    int c;
        
    static struct option long_options[] =
    {
        {"outdir",              required_argument,  0, 'd'},
        {"help",                no_argument,        &show_help, 1},
        {"outfile",             required_argument,  0, 'o'},
        {"tmpdir",              required_argument,  0, 't'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    while ((c = getopt_long(argc, argv, "d:o:t:", long_options, &option_index)) != -1)
    {                
        switch (c)
        {
            case 'd':
                outdir = string(optarg);
                break;
                
            case 0:
                break;
                
            case 'o':
                outfile = string(optarg);
                break;
                
            case 't':
                AxApp::s_workingDir = string(optarg);
                break;
            
            case '?':
                display_usage();
                exit(0);
                break;
                
            default:
                break;
        }
    }
    
    if (show_help) {
        display_usage();
        exit(0);
    }
    
    if (optind <= argc - 1) {
        infile = string(argv[optind]);
    }
    else {
        cerr << "Incorrect number of arguments: expecting one input file." << endl << endl;
        display_usage();
        exit(1);
    }
    
    // Hardcode svg ext for now
    if (outfile.empty()) {
        outfile = removeExtension(infile);
        outfile += ".axz";
    }
    
    // Hardcode svg ext for now
    if (!outdir.empty()) {
        outfile = outdir + "/" + removePath(outfile);
    }
    
    cerr << "Reading " << infile << "..." << endl;
    
    if ( !AxApp::CheckDir( AxApp::GetWorkingDir(), 0755 ) )
        wxLogWarning( _("Unable to create or write in the working directory '%s'. Please verify the access permissions or change it in the preferences."),
                     AxApp::GetWorkingDir().c_str() );
    if ( !AxApp::CheckDir( AxApp::GetLogDir(), 0755 ) )
        wxLogDebug( _("Unable to create or write in the log directory '%s'. Please verify the access permissions."), AxApp::GetLogDir().c_str() );
    
    RecFile file( "test" );
    RecTypModel typ_model( "typ_model" );
    RecMusModel mus_model( "mus_model" );
    typ_model.Open( "./models/builtin.axtyp" );
    mus_model.Open( "./models/builtin.axmus" );
    
    // Create a fake progress dialog
    AxProgressDlg *dlg = new AxProgressDlg( );
    
    wxArrayPtrVoid pre_params;
    pre_params.Add( &infile );
    
    bool failed = false;
    
    file.New();
    
    failed = !file.Preprocess( pre_params, dlg );
    
    if (failed) {
        
    }
    
    // params 0: RecTypModel: typModelPtr
    // params 1: RecMusModel: musModelPtr
    // params 2: bool: rec_delayed
    // params 3: int: rec_lm_order
    // params 4: double: rec_lm_scaling
    // params 5: wxString: rec_wrdtrns
    wxArrayPtrVoid rec_params;
    rec_params.Add( &typ_model );
    rec_params.Add( &mus_model );
    rec_params.Add( &RecEnv::s_rec_delayed );
    rec_params.Add( &RecEnv::s_rec_lm_order );
    rec_params.Add( &RecEnv::s_rec_lm_scaling );
    wxString rec_wrdtrns = "";
    rec_params.Add( &rec_wrdtrns );
    
    failed = false;
    
    failed = !file.Recognize( rec_params, dlg );
    
    file.SaveAs( outfile );
    
    return 0;
}
