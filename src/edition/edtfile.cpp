/////////////////////////////////////////////////////////////////////////////
// Name:        edtfile.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_EDT

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "edtfile.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/file.h"
#include "wx/filename.h"

#include "edtfile.h"
#include "edt.h"

#include "app/axapp.h"

#include "mus/musfile.h"
#include "mus/musiobin.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// EdtFile
//----------------------------------------------------------------------------

EdtFile::EdtFile( wxString name, EdtEnv *env )
	: AxFile( name, AX_FILE_DEFAULT, AX_FILE_EDITION )
{
	m_envPtr = env;
    m_musFilePtr = NULL;
}



EdtFile::~EdtFile()
{
	if ( m_musFilePtr )
		delete m_musFilePtr;
}


void EdtFile::NewContent( )
{
	wxASSERT_MSG( !m_musFilePtr, "MusFile should be NULL" );

	// new MusFile
    m_musFilePtr = new MusFile();
    m_musFilePtr->m_fname = m_basename + "page.bin"; 
}


void EdtFile::OpenContent( )
{
	this->NewContent();
	
	if ( !wxFileExists( m_basename + "page.bin") ) {
        return;
    }
    
    MusBinInput *bin_input = new MusBinInput( m_musFilePtr, m_musFilePtr->m_fname );
    bin_input->ImportFile();
    delete bin_input;
	
	return;       
}


void EdtFile::SaveContent( )
{
	wxASSERT_MSG( m_musFilePtr, "MusFile should not be NULL" );
	wxASSERT( m_xml_root );
	
    // save
    MusBinOutput *bin_output = new MusBinOutput( m_musFilePtr, m_musFilePtr->m_fname );
    bin_output->ExportFile();
    delete bin_output;

}

void EdtFile::CloseContent( )
{
	// nouveau fichier ?
    if ( m_musFilePtr )
    {
        delete m_musFilePtr;
        m_musFilePtr = NULL;
    }
}

// WDR: handler implementations for EdtFile

#endif //AX_EDT

