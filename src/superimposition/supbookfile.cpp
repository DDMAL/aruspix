/////////////////////////////////////////////////////////////////////////////
// Name:        supbookfile.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_SUPERIMPOSITION

#if defined(__GNUG__) && ! defined(__APPLE__)
#pragma implementation "supbookfile.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "supbookfile.h"
#include "sup.h"
#include "supfile.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// SupBookFile
//----------------------------------------------------------------------------

SupBookFile::SupBookFile( wxString name, SupEnv *env )
: AxFile( name, AX_FILE_PROJECT, AX_FILE_RECOGNITION )
{
    m_envPtr = env;
}

SupBookFile::~SupBookFile()
{
}


void SupBookFile::NewContent( )
{
    m_RISM = "";
    m_Composer = "";
    m_Title = "";
    m_Printer = "";
    m_Year = "";
    m_Library1 = "";
	m_Library2 = "";
    // files
    m_axFileDir  = "";
    m_imgFileDir1 = "";
	m_imgFileDir2 = "";
    m_axFiles.Clear();
    m_imgFiles1.Clear();
	m_imgFiles2.Clear();
}


void SupBookFile::OpenContent( )
{
    this->NewContent();
    
    TiXmlElement *root = NULL;
    TiXmlNode *node = NULL;
    TiXmlElement *elem = NULL;
    
    wxASSERT( m_xml_root );
    
    // book information
    node = m_xml_root->FirstChild( "book" );
    if ( !node ) return;        
    root = node->ToElement();
    if ( !root ) return;

    if ( root->Attribute("RISM"))
        m_RISM = root->Attribute("RISM");
    if ( root->Attribute("Composer"))
        m_Composer = root->Attribute("Composer");
    if ( root->Attribute("Title"))
        m_Title = root->Attribute("Title");
    if ( root->Attribute("Printer"))
        m_Printer = root->Attribute("Printer");
    if ( root->Attribute("Year"))
        m_Year = root->Attribute("Year");
    if ( root->Attribute("Library1"))
        m_Library1 = root->Attribute("Library1");
    if ( root->Attribute("Library2"))
        m_Library2 = root->Attribute("Library2");
        
    // images1
    node = m_xml_root->FirstChild( "images1" );
    if ( !node ) return;
    root = node->ToElement();
    if ( !root ) return;
    
    wxString path1;
    if ( root->Attribute("Path") )
        path1 = root->Attribute("Path");
    wxFileName dirname1 = wxFileName::DirName( path1 );
    dirname1.MakeAbsolute( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( dirname1.GetPath() );
    m_imgFileDir1 = dirname1.GetPath();
    for( node = root->FirstChild( "image" ); node; node = node->NextSibling( "image" ) )
    {
        elem = node->ToElement();
        if (!elem || !elem->Attribute("filename") || !elem->Attribute( "flags" ) ) 
            continue;
        m_imgFiles1.Add( AxBookFileItem( elem->Attribute("filename"), atoi(elem->Attribute( "flags" )) ) );
    }
    
    // images2
    node = m_xml_root->FirstChild( "images2" );
    if ( !node ) return;
    root = node->ToElement();
    if ( !root ) return;
    
	wxString path2;
    if ( root->Attribute("Path") )
        path2 = root->Attribute("Path");
    wxFileName dirname2 = wxFileName::DirName( path2 );
    dirname2.MakeAbsolute( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( dirname1.GetPath() );
    m_imgFileDir2 = dirname2.GetPath();
    for( node = root->FirstChild( "image" ); node; node = node->NextSibling( "image" ) )
    {
        elem = node->ToElement();
        if (!elem || !elem->Attribute("filename") || !elem->Attribute( "flags" ) ) 
            continue;
        m_imgFiles2.Add( AxBookFileItem( elem->Attribute("filename"), atoi(elem->Attribute( "flags" )) ) );
    }

	
    // axfiles
    node = m_xml_root->FirstChild( "axfiles" );
    if ( !node ) return;
    root = node->ToElement();
    if ( !root ) return;
    
    wxString path3;
    if ( root->Attribute("Path") )
        path3 = root->Attribute("Path");
    wxFileName dirname3 = wxFileName::DirName( path3 );
    dirname3.MakeAbsolute( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( dirname2.GetPath() );
    m_axFileDir = dirname3.GetPath();
    for( node = root->FirstChild( "axfile" ); node; node = node->NextSibling( "axfile" ) )
    {
        elem = node->ToElement();
        if (!elem || !elem->Attribute("filename") || !elem->Attribute( "flags" ) ) 
            continue;
        m_axFiles.Add( AxBookFileItem( elem->Attribute("filename"), atoi(elem->Attribute( "flags" )) ) );
    }
}


void SupBookFile::SaveContent( )
{
    wxASSERT( m_xml_root );

    TiXmlElement book("book");
    
    book.SetAttribute("RISM",  m_RISM.c_str() );
    book.SetAttribute("Composer", m_Composer.c_str() );    
    book.SetAttribute("Title", m_Title.c_str() );
    book.SetAttribute("Printer", m_Printer.c_str() );
    book.SetAttribute("Year", m_Year.c_str() );
    book.SetAttribute("Library1", m_Library1.c_str() );
	book.SetAttribute("Library2", m_Library2.c_str() );
    m_xml_root->InsertEndChild( book );
    
    TiXmlElement images1("images1");
    wxFileName dirname1 = wxFileName::DirName( m_imgFileDir1 );
    dirname1.MakeRelativeTo( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( dirname1.GetPath() );
    images1.SetAttribute("Path", dirname1.GetPath().c_str() );
    for ( int i = 0; i < (int)m_imgFiles1.GetCount(); i++)
    {
        TiXmlElement image("image");
        image.SetAttribute("filename", m_imgFiles1[i].m_filename.c_str() );
        image.SetAttribute("flags", wxString::Format("%d", m_imgFiles1[i].m_flags ).c_str() );
        images1.InsertEndChild( image );
    }   
    m_xml_root->InsertEndChild( images1 );
	
    TiXmlElement images2("images");
    wxFileName dirname2 = wxFileName::DirName( m_imgFileDir2 );
    dirname2.MakeRelativeTo( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( dirname1.GetPath() );
    images2.SetAttribute("Path", dirname2.GetPath().c_str() );
    for ( int i = 0; i < (int)m_imgFiles2.GetCount(); i++)
    {
        TiXmlElement image("image");
        image.SetAttribute("filename", m_imgFiles2[i].m_filename.c_str() );
        image.SetAttribute("flags", wxString::Format("%d", m_imgFiles2[i].m_flags ).c_str() );
        images2.InsertEndChild( image );
    }   
    m_xml_root->InsertEndChild( images2 );
    
    TiXmlElement axfiles("axfiles");
    wxFileName dirname3 = wxFileName::DirName( m_axFileDir );
    dirname3.MakeRelativeTo( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( dirname2.GetPath() );
    axfiles.SetAttribute("Path", dirname3.GetPath().c_str() );
    for ( int i = 0; i < (int)m_axFiles.GetCount(); i++)
    {
        TiXmlElement axfile("axfile");
        axfile.SetAttribute("filename", m_axFiles[i].m_filename.c_str() );
        axfile.SetAttribute("flags", wxString::Format("%d", m_axFiles[i].m_flags ).c_str() );
        axfiles.InsertEndChild( axfile );
    }   
    m_xml_root->InsertEndChild( axfiles );
}

void SupBookFile::CloseContent( )
{
}

// probably to be modified to return two lists...
int SupBookFile::FilesToSuperimpose( wxArrayString *filenames, wxArrayString *paths, bool add_axfiles )
{
	int index;
	
	filenames->Clear();
	paths->Clear();

	for( int i = 0; i < (int)m_imgFiles1.GetCount(); i++)
	{
		if ( m_imgFiles1[i].m_flags & FILE_DESACTIVATED )
			continue;
		wxFileName filename( m_imgFiles1[i].m_filename );
		filename.SetExt("axz");
		//wxLogDebug( filename.GetFullName() );
		if ( AxBookFileItem::FindFile( &m_axFiles, filename.GetFullName() , &index ) )
			continue;
		
		if ( add_axfiles )
			m_axFiles.Add( AxBookFileItem( filename.GetFullName() ) );
		paths->Add( m_imgFileDir1 + wxFileName::GetPathSeparator() +  m_imgFiles1[i].m_filename );
		filenames->Add( m_imgFiles1[i].m_filename );
	}
	m_axFiles.Sort( SortBookFileItems );
	return (int)filenames->GetCount();
}


bool SupBookFile::HasToBeSuperimposed( wxString imagefile )
{
	wxArrayString paths, filenames;
	size_t nbOfFiles;
    
	nbOfFiles = FilesToSuperimpose( &filenames, &paths, false );
	if ( paths.Index( imagefile ) == wxNOT_FOUND )
	{
		wxLogMessage( _("Nothing to do! Check if the file is desactivated or already preprocessed") );
		return false;
	}
	else
	{
		wxFileName filename( imagefile );
		filename.SetExt("axz");
		m_axFiles.Add( AxBookFileItem( filename.GetFullName() ) );
		m_axFiles.Sort( SortBookFileItems );
		return true;
	}
}


bool SupBookFile::LoadAxfiles( )
{
    wxArrayString paths;
    int index, nbfiles, i;

    if ( wxDirExists( m_axFileDir ) )
    {
        nbfiles = wxDir::GetAllFiles( m_axFileDir, &paths, "*.axz" );
    
        for ( i = 0; i < (int)nbfiles; i++ )
        {
            wxFileName name( paths[i] );
            if ( !AxBookFileItem::FindFile( &m_axFiles, name.GetFullName(), &index ) )
                m_axFiles.Add( AxBookFileItem( name.GetFullName() ) );
        }
        m_axFiles.Sort( SortBookFileItems );
    }
    
    return true;
}


bool SupBookFile::LoadImages( )
{
    wxArrayString paths;
    int index, nbfiles, i;

    if ( wxDirExists( m_imgFileDir1 ) )
    {
        nbfiles = wxDir::GetAllFiles( m_imgFileDir1, &paths, "*.tif", wxDIR_FILES  );
    
        for ( i = 0; i < (int)nbfiles; i++ )
        {
            wxFileName name( paths[i] );
            if ( !AxBookFileItem::FindFile( &m_imgFiles1, name.GetFullName(), &index ) )
                m_imgFiles1.Add( AxBookFileItem( name.GetFullName() ) );
        }
        m_imgFiles1.Sort( SortBookFileItems );
    }

    if ( wxDirExists( m_imgFileDir2 ) )
    {
        nbfiles = wxDir::GetAllFiles( m_imgFileDir2, &paths, "*.tif", wxDIR_FILES  );
    
        for ( i = 0; i < (int)nbfiles; i++ )
        {
            wxFileName name( paths[i] );
            if ( !AxBookFileItem::FindFile( &m_imgFiles2, name.GetFullName(), &index ) )
                m_imgFiles2.Add( AxBookFileItem( name.GetFullName() ) );
        }
        m_imgFiles1.Sort( SortBookFileItems );
    }
    
    return true;
}

bool SupBookFile::RemoveImage( wxString filename )
{
    int index;
    const AxBookFileItem *book = NULL;
	book = AxBookFileItem::FindFile( &m_imgFiles1, filename, &index );
    if ( book )
        m_imgFiles1.RemoveAt( index );
    book = AxBookFileItem::FindFile( &m_imgFiles2, filename, &index );
    if ( book )
        m_imgFiles2.RemoveAt( index );
    return true;
}

bool SupBookFile::DesactivateImage( wxString filename )
{
    int index;
    AxBookFileItem *book = NULL;
	book = AxBookFileItem::FindFile( &m_imgFiles1, filename, &index );
    if ( book )
    {
        if (book->m_flags & FILE_DESACTIVATED)
            book->m_flags &= ~FILE_DESACTIVATED;
        else
            book->m_flags |= FILE_DESACTIVATED;
    }
	book = AxBookFileItem::FindFile( &m_imgFiles2, filename, &index );
    if ( book )
    {
        if (book->m_flags & FILE_DESACTIVATED)
            book->m_flags &= ~FILE_DESACTIVATED;
        else
            book->m_flags |= FILE_DESACTIVATED;
    }
    return true;
}

bool SupBookFile::DesactivateAxfile( wxString filename )
{
    int index;
    AxBookFileItem *book = AxBookFileItem::FindFile( &m_axFiles, filename, &index );
    if ( book )
    {
        if (book->m_flags & FILE_DESACTIVATED)
            book->m_flags &= ~FILE_DESACTIVATED;
        else
            book->m_flags |= FILE_DESACTIVATED;
    }
    return true;
}

bool SupBookFile::RemoveAxfile( wxString filename )
{
    int index;
    const AxBookFileItem *book = AxBookFileItem::FindFile( &m_axFiles, filename, &index );
    if ( book )
        m_axFiles.RemoveAt( index );
    return true;
}

bool SupBookFile::DeleteAxfile( wxString filename )
{
    RemoveAxfile( filename );
    wxString fullname = m_axFileDir + wxFileName::GetPathSeparator() +  filename;
    if ( wxFileExists( fullname ) )
        wxRemoveFile( fullname );
	return true;
}

// WDR: handler implementations for RecFile

#endif //AX_SUPERIMPOSITION
