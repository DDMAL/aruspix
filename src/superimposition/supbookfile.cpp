/////////////////////////////////////////////////////////////////////////////
// Name:        supbookfile.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_SUPERIMPOSITION

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "supbookfile.h"
#include "sup.h"
#include "supfile.h"



//----------------------------------------------------------------------------
// SupBookFile
//----------------------------------------------------------------------------

SupBookFile::SupBookFile( wxString name, SupEnv *env )
: AxFile( name, AX_FILE_PROJECT, AX_FILE_SUPERIMPOSITION )
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
    int i;
	for ( i = 0; i < (int)m_imgFiles1.GetCount(); i++)
    {
        TiXmlElement image("image");
        image.SetAttribute("filename", m_imgFiles1[i].m_filename.c_str() );
        image.SetAttribute("flags", wxString::Format("%d", m_imgFiles1[i].m_flags ).c_str() );
        images1.InsertEndChild( image );
    }   
    m_xml_root->InsertEndChild( images1 );
	
    TiXmlElement images2("images2");
    wxFileName dirname2 = wxFileName::DirName( m_imgFileDir2 );
    dirname2.MakeRelativeTo( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( dirname1.GetPath() );
    images2.SetAttribute("Path", dirname2.GetPath().c_str() );
    for ( i = 0; i < (int)m_imgFiles2.GetCount(); i++)
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
    for ( i = 0; i < (int)m_axFiles.GetCount(); i++)
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
bool SupBookFile::CreateFiles( bool ask_user )
{
	int i;

	if ( ask_user && !m_axFiles.IsEmpty() )
	{	
		wxString msg = wxString::Format(_("This will erase previously superimposed files for this book. Do you want to continue ?") );
		int res = wxMessageBox( msg, wxGetApp().GetAppName() , wxYES_NO | wxICON_QUESTION );
		if ( res != wxYES )
			return false;
	}
	
	for( i = 0; i < (int)m_axFiles.GetCount(); i++)
	{
		wxString fullname = m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename;
		if ( wxFileExists( fullname ) )
			wxRemoveFile( fullname );
	}
	
	// get active image files from both sets
	wxArrayString paths1, paths2, filenames;	
	for( i = 0; i < (int)m_imgFiles1.GetCount(); i++)
	{
		if ( m_imgFiles1[i].m_flags & FILE_DESACTIVATED )
			continue;
		paths1.Add( m_imgFileDir1 + wxFileName::GetPathSeparator() +  m_imgFiles1[i].m_filename );
        filenames.Add( m_imgFiles1[i].m_filename );
	}
	for( i = 0; i < (int)m_imgFiles2.GetCount(); i++)
	{
		if ( m_imgFiles2[i].m_flags & FILE_DESACTIVATED )
			continue;
		paths2.Add( m_imgFileDir2 + wxFileName::GetPathSeparator() +  m_imgFiles2[i].m_filename );
	}
	
	// create Aruspix files using active image files
	for( i = 0; (i < (int)paths1.GetCount()) && (i < (int)paths2.GetCount()); i++)
	{
        wxString basename;
        wxFileName::SplitPath( filenames[i], NULL, &basename, NULL );
		wxString axfile = wxString::Format("%s_%s.axz", basename.c_str(), m_Library2.c_str() );
		SupFile supfile( "sup_book_file_create" );
		supfile.New();
		supfile.m_original1 = paths1[i];
		supfile.m_original2 = paths2[i];
		supfile.Modify();
		supfile.SaveAs( m_axFileDir + wxFileName::GetPathSeparator() + axfile );
	}
	LoadAxfiles();
	return true;
}

// probably to be modified to return two lists...
int SupBookFile::FilesToSuperimpose( wxArrayString *filenames, wxArrayString *paths )
{
	filenames->Clear();
	paths->Clear();

	for( int i = 0; i < (int)m_axFiles.GetCount(); i++)
	{
		if ( m_axFiles[i].m_flags & FILE_DESACTIVATED )
			continue;

		if ( SupFile::IsSuperimposed( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename ) )
			continue;
			
		paths->Add( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename );
		filenames->Add( m_axFiles[i].m_filename );
	}
	return (int)filenames->GetCount();
}

/*
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
*/

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


bool SupBookFile::LoadImages1( )
{
    wxArrayString paths;
    int index, nbfiles, i;

    if ( wxDirExists( m_imgFileDir1 ) )
    {
        nbfiles = AxFile::GetAllFiles( m_imgFileDir1, &paths, IMAGE_FILES  );
    
        for ( i = 0; i < (int)nbfiles; i++ )
        {
            wxFileName name( paths[i] );
            if ( !AxBookFileItem::FindFile( &m_imgFiles1, name.GetFullName(), &index ) )
                m_imgFiles1.Add( AxBookFileItem( name.GetFullName() ) );
        }
        m_imgFiles1.Sort( SortBookFileItems );
    }
    
    return true;
}

bool SupBookFile::LoadImages2( )
{
    wxArrayString paths;
    int index, nbfiles, i;

    if ( wxDirExists( m_imgFileDir2 ) )
    {
        nbfiles = AxFile::GetAllFiles( m_imgFileDir2, &paths, IMAGE_FILES  );
    
        for ( i = 0; i < (int)nbfiles; i++ )
        {
            wxFileName name( paths[i] );
            if ( !AxBookFileItem::FindFile( &m_imgFiles2, name.GetFullName(), &index ) )
                m_imgFiles2.Add( AxBookFileItem( name.GetFullName() ) );
        }
        m_imgFiles2.Sort( SortBookFileItems );
    }
    
    return true;
}

bool SupBookFile::RemoveImage( wxString filename, int book_no )
{
    int index;
    const AxBookFileItem *book = NULL;
	if ( book_no == 1 )
	{
		book = AxBookFileItem::FindFile( &m_imgFiles1, filename, &index );
		if ( book )
			m_imgFiles1.RemoveAt( index );
    }
	else if ( book_no == 2)
	{
		book = AxBookFileItem::FindFile( &m_imgFiles2, filename, &index );
		if ( book )
			m_imgFiles2.RemoveAt( index );
	}
    return true;
}

bool SupBookFile::DesactivateImage( wxString filename, int book_no )
{
    int index;
    AxBookFileItem *book = NULL;
	if ( book_no == 1 )
	{
		book = AxBookFileItem::FindFile( &m_imgFiles1, filename, &index );
		if ( book )
		{
			if (book->m_flags & FILE_DESACTIVATED)
				book->m_flags &= ~FILE_DESACTIVATED;
			else
				book->m_flags |= FILE_DESACTIVATED;
		}
	}
	else if ( book_no == 2)
	{
		book = AxBookFileItem::FindFile( &m_imgFiles2, filename, &index );
		if ( book )
		{
			if (book->m_flags & FILE_DESACTIVATED)
				book->m_flags &= ~FILE_DESACTIVATED;
			else
				book->m_flags |= FILE_DESACTIVATED;
		}
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


#endif //AX_SUPERIMPOSITION
