/////////////////////////////////////////////////////////////////////////////
// Name:        supfile.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_SUPERIMPOSITION

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "supfile.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/file.h"
#include "wx/filename.h"

#include "supfile.h"
#include "sup.h"
#include "im/impage.h"
//#include "im/imstaff.h"
//#include "im/imstaffsegment.h"

#include "app/axapp.h"
#include "app/axprocess.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// SupFile
//----------------------------------------------------------------------------

SupFile::SupFile( wxString name, SupEnv *env )
	: AxFile( name, AX_FILE_DEFAULT, AX_FILE_RECOGNITION )
{
	m_envPtr = env;

	m_imPagePtr1 = NULL;
	m_imPagePtr2 = NULL;
	
	m_isSuperimposed = false;
}



SupFile::~SupFile()
{
	if ( m_imPagePtr1 )
		delete m_imPagePtr2;
	if ( m_imPagePtr1 )
		delete m_imPagePtr2;
}


void SupFile::NewContent( )
{
	wxASSERT_MSG( !m_imPagePtr1, "ImPage should be NULL" );
	wxASSERT_MSG( !m_imPagePtr2, "ImPage should be NULL" );
        
	// new ImPage and Load
    m_imPagePtr1 = new ImPage( m_basename, &m_isModified );
	m_imPagePtr2 = new ImPage( m_basename, &m_isModified );
	
	m_isSuperimposed = false;
}


void SupFile::OpenContent( )
{
	this->NewContent();
	
	// start
    bool failed = false;
	
	failed = !m_imPagePtr1->Load( m_xml_root );
	if ( failed )
		return;

	failed = !m_imPagePtr2->Load( m_xml_root );
	if ( failed )
		return;
		
	// if both ImPage loaded, superimposed is true???	
	m_isSuperimposed = true;
	
	return;       
}


void SupFile::SaveContent( )
{
	wxASSERT_MSG( m_imPagePtr1, "ImPage1 should not be NULL" );
	wxASSERT_MSG( m_imPagePtr2, "ImPage2 should not be NULL" );
	wxASSERT( m_xml_root );
		
	if ( !m_isSuperimposed )
		return;
		
	m_imPagePtr1->Save( m_xml_root ); // save in the SupFile directory
	m_imPagePtr2->Save( m_xml_root ); // save in the SupFile directory
}

void SupFile::CloseContent( )
{
	// nouveau fichier ?
    if ( m_imPagePtr1 )
    {
        delete m_imPagePtr1;
        m_imPagePtr1 = NULL;
    }

	// old ImPage
    if ( m_imPagePtr2 )
    {
        delete m_imPagePtr2;
        m_imPagePtr2 = NULL;
    }
	
	m_isSuperimposed = false;
}

// static
bool SupFile::IsSuperimposed( wxString filename )
{
	// don't know which file to check....
	//return !AxFile::GetPreview( filename, "page.wwg"  ).IsEmpty();
	return true;
}

// functors

bool SupFile::Superimpose( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
    // params 0: wxString: output_dir
	wxString image_file = *(wxString*)params[0];
	
	m_imPagePtr1->SetProgressDlg( dlg );
	m_imPagePtr2->SetProgressDlg( dlg );

    wxString ext, shortname;
    wxFileName::SplitPath( image_file, NULL, &shortname, &ext );

    dlg->SetMaxJobBar( 17 );
    dlg->SetJob( shortname );
    wxYield();
		
    bool failed = false;
	/*
    if ( RecEnv::s_check && !failed ) 
        failed = !m_imPagePtr->Check( image_file, 2500 ); // 2 operations max
	
	//// TEST
	//m_error = m_imPagePtr->GetError();
	//this->m_isPreprocessed = true;
	//return true;
		
    if ( RecEnv::s_deskew && !failed ) 
        failed = !m_imPagePtr->Deskew( 2.0 ); // 2 operations max
    //op.m_inputfile = output + "/deskew." + shortname + ".tif";
        
    if ( RecEnv::s_staves_position && !failed ) 
        failed = !m_imPagePtr->FindStaves(  3, 50 );  // 4 operations max
    //op.m_inputfile = output + "/resize." + shortname + ".tif";

    if ( RecEnv::s_binarize_and_clean  && !failed ) 
        failed = !m_imPagePtr->BinarizeAndClean( );  // 3 operations max

    if ( RecEnv::s_find_borders && !failed ) 
        failed = !m_imPagePtr->FindBorders( );  // 1 operation max
    //op.m_inputfile = output + "/border." + shortname + ".tif";
        
    if ( RecEnv::s_find_ornate_letters && !failed ) 
        failed = !m_imPagePtr->FindOrnateLetters( ); // 1 operation max

    if ( RecEnv::s_find_text_in_staves && !failed ) 
        failed = !m_imPagePtr->FindTextInStaves( ); // 1 operation max

    if ( RecEnv::s_find_text && !failed ) 
        failed = !m_imPagePtr->FindText( ); // 1 operation max
		
	if (!failed)
		this->m_isPreprocessed = true;
			
	m_error = m_imPagePtr->GetError();
	*/
	return !failed;
}

// WDR: handler implementations for SupFile

#endif //AX_SUPERIMPOSITION

