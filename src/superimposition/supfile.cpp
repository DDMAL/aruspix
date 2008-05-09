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
#include "im/imregister.h"

#include "app/axapp.h"
#include "app/axprocess.h"

// IMLIB
#include <im.h>
#include <im_counter.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_util.h>
#include <im_binfile.h>
#include <im_math_op.h>


// WDR: class implementations

//----------------------------------------------------------------------------
// SupFile
//----------------------------------------------------------------------------

SupFile::SupFile( wxString name, SupEnv *env )
	: AxFile( name, AX_FILE_DEFAULT, AX_FILE_SUPERIMPOSITION )
{
	m_envPtr = env;

	m_imRegisterPtr = NULL;
	
	m_isSuperimposed = false;
	m_hasPoints1 = false;
	m_hasPoints2 = false;
	
}



SupFile::~SupFile()
{
	if ( m_imRegisterPtr )
		delete m_imRegisterPtr;
}


void SupFile::NewContent( )
{
	wxASSERT_MSG( !m_imRegisterPtr, "ImRegister should be NULL" );
	        
	// new ImPage and Load
    m_imRegisterPtr = new ImRegister( m_basename, &m_isModified );
	
	m_isSuperimposed = false;
	m_hasPoints1 = false;
	m_hasPoints2 = false;
}


void SupFile::OpenContent( )
{
	this->NewContent();
	
	// start
	int i;
    bool failed = false;
	
    TiXmlElement *root = NULL;
    TiXmlNode *node = NULL;
    TiXmlElement *elem = NULL;
    
    wxASSERT( m_xml_root );
        
    // images
    node = m_xml_root->FirstChild( "image1" );
    if ( !node ) return;
    root = node->ToElement();
    if ( !root ) return;
	
    wxString file1;
    if ( root->Attribute("filename") )
        file1 = root->Attribute("filename");
    wxFileName img1( file1 );
    img1.MakeAbsolute( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( img1.GetFullPath() );
    m_original1 = img1.GetFullPath();
	
    node = m_xml_root->FirstChild( "image2" );
    if ( !node ) return;
    root = node->ToElement();
    if ( !root ) return;
	
    wxString file2;
    if ( root->Attribute("filename") )
        file2 = root->Attribute("filename");
    wxFileName img2(  file2 );
    img2.MakeAbsolute( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( img2.GetFullPath() );
    m_original2 = img2.GetFullPath();
	
	
    // points
	m_hasPoints1 = false;
	m_hasPoints2 = false;
    node = m_xml_root->FirstChild( "points" );
    if ( node )
	{
		root = node->ToElement();
		if ( !root ) 
			return;
    
		i = 0;
		for( node = root->FirstChild( "point1" ); node && i < 4; node = node->NextSibling( "point1" ), i++ )
		{
			elem = node->ToElement();
			if (!elem || !elem->Attribute("x") || !elem->Attribute( "y" ) ) 
				continue;
			m_points1[i] = wxPoint( atoi( elem->Attribute( "x" ) ), atoi( elem->Attribute( "y" ) ) );
		}
		i = 0;
		for( node = root->FirstChild( "point2" ); node && i < 4; node = node->NextSibling( "point2" ), i++ )
		{
			elem = node->ToElement();
			if (!elem || !elem->Attribute("x") || !elem->Attribute( "y" ) ) 
				continue;
			m_points2[i] = wxPoint( atoi( elem->Attribute( "x" ) ), atoi( elem->Attribute( "y" ) ) );
		}
		// we assume that, if there is a 'points' element all points are present
		m_hasPoints1 = true;
		m_hasPoints2 = true;
	}
	
	failed = !m_imRegisterPtr->Load( m_xml_root );
	if ( failed )
		return;
	else
		m_isSuperimposed = true;
	
	return;       
}


void SupFile::SaveContent( )
{
	wxASSERT_MSG( m_imRegisterPtr, "ImRegister should not be NULL" );
	wxASSERT( m_xml_root );
	
	int i;
	
    TiXmlElement image1("image1");
    wxFileName orig1( m_original1 );
    orig1.MakeRelativeTo( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( orig1.GetPath() );
	image1.SetAttribute( "filename" , orig1.GetFullPath().c_str() );
    m_xml_root->InsertEndChild( image1 );
	
    TiXmlElement image2("image2");
    wxFileName orig2( m_original2 );
    orig2.MakeRelativeTo( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( orig2.GetPath() );
	image2.SetAttribute( "filename" , orig2.GetFullPath().c_str() );
    m_xml_root->InsertEndChild( image2 );
	
	if ( m_hasPoints1 && m_hasPoints2 )
	{
		TiXmlElement points("points");
		for (i = 0; i < 4; i++ )
		{
			TiXmlElement point( "point1" );
			point.SetAttribute( "x", m_points1[i].x );
			point.SetAttribute( "y", m_points1[i].y );
			points.InsertEndChild( point );
		}
		for (i = 0; i < 4; i++ )
		{
			TiXmlElement point( "point2" );
			point.SetAttribute( "x", m_points2[i].x );
			point.SetAttribute( "y", m_points2[i].y );
			points.InsertEndChild( point );
		}
		m_xml_root->InsertEndChild( points );
	}
		
	if ( m_isSuperimposed )
		m_imRegisterPtr->Save( m_xml_root );
}

void SupFile::CloseContent( )
{
	// nouveau fichier ?
    if ( m_imRegisterPtr )
    {
        delete m_imRegisterPtr;
        m_imRegisterPtr = NULL;
    }
	
	m_isSuperimposed = false;
	m_hasPoints1 = false;
	m_hasPoints2 = false;
}


void SupFile::GetSrc1( AxImage *image )
{
	wxASSERT_MSG( image, "AxImage cannot be NULL" );
	wxASSERT_MSG( m_imRegisterPtr, "m_imRegisterPtr cannot be NULL" );
	wxASSERT_MSG( m_imRegisterPtr->m_src1, "Src1 cannot be NULL" );
	
	SetImImage( m_imRegisterPtr->m_src1, image );
}

void SupFile::GetSrc2( AxImage *image )
{
	wxASSERT_MSG( image, "AxImage cannot be NULL" );
	wxASSERT_MSG( m_imRegisterPtr, "m_imRegisterPtr cannot be NULL" );
	wxASSERT_MSG( m_imRegisterPtr->m_src2, "Src2 cannot be NULL" );
	
	SetImImage( m_imRegisterPtr->m_src2, image );
}

void SupFile::GetResult( AxImage *image )
{
	wxASSERT_MSG( image, "AxImage cannot be NULL" );
	wxASSERT_MSG( m_imRegisterPtr, "m_imRegisterPtr cannot be NULL" );
	wxASSERT_MSG( m_imRegisterPtr->m_result, "Result image cannot be NULL" );
	
	SetImImage( m_imRegisterPtr->m_result, image );
}

// static
bool SupFile::IsSuperimposed( wxString filename )
{
	// don't know which file to check....
	return !AxFile::GetPreview( filename, "result.tif"  ).IsEmpty();
}

// functors

bool SupFile::Superimpose( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
    // params 0: wxString: output_dir
	wxString image_file1 = *(wxString*)params[0];
	wxString image_file2 = *(wxString*)params[1];
	
	m_imRegisterPtr->SetProgressDlg( dlg );
	
    wxString ext, shortname1, shortname2;
    wxFileName::SplitPath( image_file1, NULL, &shortname1, &ext );
	wxFileName::SplitPath( image_file2, NULL, &shortname2, &ext );

    dlg->SetMaxJobBar( 17 );
    dlg->SetJob( shortname1 );
    wxYield();
		
    bool failed = false;
	
	/*
    if ( !failed ) 
        failed = !m_imPage1Ptr->Check( m_original1, 2500 ); // 2 operations max
		
    if ( !failed ) 
        failed = !m_imPage1Ptr->Deskew( 2.0 ); // 2 operations max
        
    if ( !failed ) 
        failed = !m_imPage1Ptr->FindStaves(  3, 50, false, false );  // 4 operations max

    if ( RecEnv::s_check && !failed ) 
        failed = !m_imPagePtr->Check( image_file, 2500 ); // 2 operations max
	*/
	
	if ( !failed )
		failed = !m_imRegisterPtr->Init( image_file1, image_file2 );

	if ( !failed && !m_hasPoints1 && !m_hasPoints2 )
		failed = !m_imRegisterPtr->DetectPoints( m_points1, m_points2 );

	if ( !failed )
		failed = !m_imRegisterPtr->Register( m_points1, m_points2 );
	
	if (!failed)
		this->m_isSuperimposed = true;
	
	m_error = m_imRegisterPtr->GetError();
	
	return true;
	
	/*	
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

