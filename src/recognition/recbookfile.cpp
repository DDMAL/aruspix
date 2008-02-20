/////////////////////////////////////////////////////////////////////////////
// Name:        recbookfile.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION

#if defined(__GNUG__) && ! defined(__APPLE__)
#pragma implementation "recbookfile.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "recbookfile.h"
#include "rec.h"
#include "recfile.h"
#include "recmodels.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfBookFiles );

int SortBookFiles( RecBookFileItem **first, RecBookFileItem **second )
{
    if ( (*first)->m_filename < (*second)->m_filename )
        return -1;
    else if ( (*first)->m_filename > (*second)->m_filename )
        return 1;
    else
        return 0;
}

// WDR: class implementations

//----------------------------------------------------------------------------
// RecBookFile
//----------------------------------------------------------------------------

RecBookFile::RecBookFile( wxString name, RecEnv *env )
: AxFile( name, AX_FILE_PROJECT, AX_FILE_RECOGNITION )
{
    m_envPtr = env;
}

RecBookFile::~RecBookFile()
{
}


void RecBookFile::NewContent( )
{
    m_RISM = "";
    m_Composer = "";
    m_Title = "";
    m_Printer = "";
    m_Year = "";
    m_Library = "";
    // files
    m_axFileDir  = "";
    m_imgFileDir = "";
    m_axFiles.Clear();
    m_imgFiles.Clear();
	// adaptation
	m_fullOptimized = false;
	m_nbFilesOptimization = 0;
	m_optFiles.Clear();
}


void RecBookFile::OpenContent( )
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
    if ( root->Attribute("Library"))
        m_Library = root->Attribute("Library");
        
    // images
    node = m_xml_root->FirstChild( "images" );
    if ( !node ) return;
    root = node->ToElement();
    if ( !root ) return;
    
    wxString path1;
    if ( root->Attribute("Path") )
        path1 = root->Attribute("Path");
    wxFileName dirname1 = wxFileName::DirName( path1 );
    dirname1.MakeAbsolute( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( dirname1.GetPath() );
    m_imgFileDir = dirname1.GetPath();
    for( node = root->FirstChild( "image" ); node; node = node->NextSibling( "image" ) )
    {
        elem = node->ToElement();
        if (!elem || !elem->Attribute("filename") || !elem->Attribute( "flags" ) ) 
            continue;
        m_imgFiles.Add( RecBookFileItem( elem->Attribute("filename"), atoi(elem->Attribute( "flags" )) ) );
    }
    
	
    // axfiles
    node = m_xml_root->FirstChild( "axfiles" );
    if ( !node ) return;
    root = node->ToElement();
    if ( !root ) return;
    
    wxString path2;
    if ( root->Attribute("Path") )
        path2 = root->Attribute("Path");
    wxFileName dirname2 = wxFileName::DirName( path2 );
    dirname2.MakeAbsolute( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( dirname2.GetPath() );
    m_axFileDir = dirname2.GetPath();
    for( node = root->FirstChild( "axfile" ); node; node = node->NextSibling( "axfile" ) )
    {
        elem = node->ToElement();
        if (!elem || !elem->Attribute("filename") || !elem->Attribute( "flags" ) ) 
            continue;
        m_axFiles.Add( RecBookFileItem( elem->Attribute("filename"), atoi(elem->Attribute( "flags" )) ) );
    }
	
	
	// optimization
    node = m_xml_root->FirstChild( "adaptation" );
    if ( !node ) return;
    root = node->ToElement();
    if ( !root ) return;
	
	if ( root->Attribute("full") )
        m_fullOptimized = (atoi(root->Attribute("full")) > 0);
    if ( root->Attribute("nb_files_for_full") )
        m_nbFilesOptimization = atoi(root->Attribute("nb_files_for_full"));
    for( node = root->FirstChild( "file" ); node; node = node->NextSibling( "file" ) )
    {
        elem = node->ToElement();
        if (!elem || !elem->Attribute("filename")) 
            continue;
        m_optFiles.Add( elem->Attribute("filename") );
    }	
}


void RecBookFile::SaveContent( )
{
    wxASSERT( m_xml_root );

    TiXmlElement book("book");
	int i;
    
    book.SetAttribute("RISM",  m_RISM.c_str() );
    book.SetAttribute("Composer", m_Composer.c_str() );    
    book.SetAttribute("Title", m_Title.c_str() );
    book.SetAttribute("Printer", m_Printer.c_str() );
    book.SetAttribute("Year", m_Year.c_str() );
    book.SetAttribute("Library", m_Library.c_str() );
    m_xml_root->InsertEndChild( book );
    
    TiXmlElement images("images");
    wxFileName dirname1 = wxFileName::DirName( m_imgFileDir );
    dirname1.MakeRelativeTo( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( dirname1.GetPath() );
    images.SetAttribute("Path", dirname1.GetPath().c_str() );
    for ( i = 0; i < (int)m_imgFiles.GetCount(); i++)
    {
        TiXmlElement image("image");
        image.SetAttribute("filename", m_imgFiles[i].m_filename.c_str() );
        image.SetAttribute("flags", wxString::Format("%d", m_imgFiles[i].m_flags ).c_str() );
        images.InsertEndChild( image );
    }   
    m_xml_root->InsertEndChild( images );
    
    TiXmlElement axfiles("axfiles");
    wxFileName dirname2 = wxFileName::DirName( m_axFileDir );
    dirname2.MakeRelativeTo( wxFileName( m_filename ).GetFullPath() );
    //wxLogDebug( dirname2.GetPath() );
    axfiles.SetAttribute("Path", dirname2.GetPath().c_str() );
    for ( i = 0; i < (int)m_axFiles.GetCount(); i++)
    {
        TiXmlElement axfile("axfile");
        axfile.SetAttribute("filename", m_axFiles[i].m_filename.c_str() );
        axfile.SetAttribute("flags", wxString::Format("%d", m_axFiles[i].m_flags ).c_str() );
        axfiles.InsertEndChild( axfile );
    }   
    m_xml_root->InsertEndChild( axfiles );
	
    TiXmlElement adapt("adaptation");
    adapt.SetAttribute("full", wxString::Format("%d", m_fullOptimized ).c_str() );
	adapt.SetAttribute("nb_files_for_full", wxString::Format("%d", m_nbFilesOptimization ).c_str() );
    for ( i = 0; i < (int)m_optFiles.GetCount(); i++)
    {
        TiXmlElement adapt_file("file");
        adapt_file.SetAttribute("filename", m_optFiles[i].c_str() );
        adapt.InsertEndChild( adapt_file );
    }   
    m_xml_root->InsertEndChild( adapt );
}

void RecBookFile::CloseContent( )
{
}

int RecBookFile::FilesToPreprocess( wxArrayString *filenames, wxArrayString *paths, bool add_axfiles )
{
	int index;
	
	filenames->Clear();
	paths->Clear();

	for( int i = 0; i < (int)m_imgFiles.GetCount(); i++)
	{
		if ( m_imgFiles[i].m_flags & FILE_DESACTIVATED )
			continue;
		wxFileName filename( m_imgFiles[i].m_filename );
		filename.SetExt("axz");
		//wxLogDebug( filename.GetFullName() );
		if ( RecBookFile::FindFile( &m_axFiles, filename.GetFullName() , &index ) )
			continue;
		
		if ( add_axfiles )
			m_axFiles.Add( RecBookFileItem( filename.GetFullName() ) );
		paths->Add( m_imgFileDir + wxFileName::GetPathSeparator() +  m_imgFiles[i].m_filename );
		filenames->Add( m_imgFiles[i].m_filename );
	}
	m_axFiles.Sort( SortBookFiles );
	return (int)filenames->GetCount();
}


int RecBookFile::RecognizedFiles( wxArrayString *filenames, wxArrayString *paths )
{
	filenames->Clear();
	paths->Clear();

	for( int i = 0; i < (int)m_axFiles.GetCount(); i++)
	{
		if ( m_axFiles[i].m_flags & FILE_DESACTIVATED )
			continue;

		if ( !RecFile::IsRecognized( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename ) )
			continue;
			
		paths->Add( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename );
		filenames->Add( m_axFiles[i].m_filename );
	}
	return (int)filenames->GetCount();
}

int RecBookFile::FilesToRecognize( wxArrayString *filenames, wxArrayString *paths )
{
	filenames->Clear();
	paths->Clear();

	for( int i = 0; i < (int)m_axFiles.GetCount(); i++)
	{
		if ( m_axFiles[i].m_flags & FILE_DESACTIVATED )
			continue;

		if ( RecFile::IsRecognized( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename ) )
			continue;
			
		paths->Add( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename );
		filenames->Add( m_axFiles[i].m_filename );
	}
	return (int)filenames->GetCount();
}

int RecBookFile::FilesForAdaptation( wxArrayString *filenames, wxArrayString *paths, bool *isCacheOk )
{
	wxASSERT( isCacheOk );
	int i;

	filenames->Clear();
	paths->Clear();
	*isCacheOk = false;

	for( i = 0; i < (int)m_axFiles.GetCount(); i++)
	{
		if ( m_axFiles[i].m_flags & FILE_DESACTIVATED )
			continue;
			
		if ( !RecFile::IsRecognized( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename ) )
			continue;
			
		paths->Add( m_axFileDir + wxFileName::GetPathSeparator() +  m_axFiles[i].m_filename );
		filenames->Add( m_axFiles[i].m_filename );
	}
	if ( (int)filenames->GetCount() == 0 )
		return 0;
		
	// cache is empty
	if ( (int)m_optFiles.GetCount() == 0 )
		return (int)filenames->GetCount() ;
		
	// check the cache. The idea is that if a file in the cache is not in the list, the cache is not valid.
	// otherwise, just keep the files that are not in the cache
	// cache is not optimized for path changes
	
	// first go through and check is the cache is valid
	for ( i = 0; i < (int)m_optFiles.GetCount(); i++ )
		if ( filenames->Index( m_optFiles[i] ) == wxNOT_FOUND )
			return (int)filenames->GetCount(); // cache is not valid, 
			
	// cache is valid, remove the files
	for ( i = 0; i < (int)m_optFiles.GetCount(); i++ )
	{
		int idx = filenames->Index( m_optFiles[i] );
		wxASSERT( idx != wxNOT_FOUND );
		paths->RemoveAt( idx );
		filenames->RemoveAt( idx );
	}
	*isCacheOk = true;
	return (int)filenames->GetCount();
}

bool RecBookFile::HasToBePreprocessed( wxString imagefile )
{
	wxArrayString paths, filenames;
	size_t nbOfFiles;
    
	nbOfFiles = FilesToPreprocess( &filenames, &paths, false );
	if ( paths.Index( imagefile ) == wxNOT_FOUND )
	{
		wxLogMessage( _("Nothing to do! Check if the file is desactivated or already preprocessed") );
		return false;
	}
	else
	{
		wxFileName filename( imagefile );
		filename.SetExt("axz");
		m_axFiles.Add( RecBookFileItem( filename.GetFullName() ) );
		m_axFiles.Sort( SortBookFiles );
		return true;
	}
}

bool RecBookFile::ResetAdaptation( bool ask_user )
{
	if ( ask_user )
	{	
		wxString msg = wxString::Format(_("This will erase fully optimized models for this book. Do you want to continue ?") );
		int res = wxMessageBox( msg, wxGetApp().GetAppName() , wxYES_NO | wxICON_QUESTION );
		if ( res != wxYES )
			return false;
	}

	wxRemoveFile( GetTypFilename() );
	wxRemoveFile( GetMusFilename() );
	wxRemoveFile( GetTypCacheFilename() );
	wxRemoveFile( GetMusCacheFilename() );
	
	m_fullOptimized = false;
	m_nbFilesOptimization = 0;
	m_optFiles.Clear( );
	return true;
}

bool RecBookFile::TypAdaptation( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	// params 0: nbfiles (unused)
	// params 1: paths (unused)
	// params 2: filenames 
	// params 3: is the cache ok ?
	// params 4: fast adaptation ? (unused)
	
	wxArrayString *filenames = (wxArrayString*)params[2];
	bool isCacheOk = *(bool*)params[3];

    // name of model to generate - temporary...
    RecTypModel model( "rec_typ_adapt" );
	if ( isCacheOk || wxFileExists( GetTypCacheFilename() ) )
		model.Open( GetTypCacheFilename() );
	else
		model.New();
    
    // name of adapted model to generate - temporary...
    RecTypModel outModel( "rec_typ_adapt_out" );
    outModel.New();
	params.Add( &outModel );
    
    bool failed = false;
    
    if ( !failed )
        failed = !model.AddFiles( params, dlg );

    if ( !failed )  
        failed = !model.Commit( dlg );
		
	if ( !failed )
		failed = !model.SaveAs( GetTypCacheFilename() );
        
    if ( !failed )  
        failed = !model.Adapt( params, dlg );
    
    if ( !failed )
        failed = !outModel.SaveAs( GetTypFilename() );
		
	if ( !failed )
	{
		if ( isCacheOk )
		{
			WX_APPEND_ARRAY( m_optFiles, *filenames );
		}
		else
		{
			m_optFiles = *filenames;
		}
	}

    return ( !failed );
}



bool RecBookFile::MusAdaptation( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	// params 0: nbfiles (unused)
	// params 1: paths (unused)
	// params 2: is the cache ok ?
	
	bool isCacheOk = *(bool*)params[2];

    // name of model to generate - temporary...
    RecMusModel model( "rec_mus_adapt" );	
	if ( isCacheOk || wxFileExists( GetMusCacheFilename() ) )
		model.Open( GetMusCacheFilename() );
	else
		model.New();
    
    bool failed = false;
    
    if ( !failed )
        failed = !model.AddFiles( params, dlg );
	
	if ( !failed )
		failed = !model.SaveAs( GetMusCacheFilename() );

    if ( !failed )  
        failed = !model.Commit( dlg );
        
    if ( !failed )  
        failed = !model.Adapt( params, dlg );
    
    if ( !failed )
        failed = !model.SaveAs( GetMusFilename() );

    return ( !failed );
}  

bool RecBookFile::FastAdaptation( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );

    wxArrayString paths, filenames;
    size_t nbOfFiles;
	bool isCacheOk;
	
	RecTypModel *typModelPtr = (RecTypModel*)params[0];
	RecMusModel *musModelPtr = (RecMusModel*)params[1];
	
    nbOfFiles = FilesForAdaptation( &filenames, &paths, &isCacheOk );

	// currently, mus model adaptation can be performed only if typ model adaptation is performed because we use 
	// the merged dictionnary generated by Adapt for Ngram generation
	// Ngram generation could be done alone, but dictionnary has to be merged ...
	// the following method was thought with mus adaptation performed alone as well
	// be careful because m_nbFilesOptimization is not handled, it is always 0
	// cache handling must be change as well because the same cache is used
	// MusAdaptation doesn't save its cache
	bool typ_optimize = ( !m_fullOptimized && (nbOfFiles > 0) ); // only if not fully adapted
	bool mus_optimize = ( (m_nbFilesOptimization < (int)nbOfFiles) & (nbOfFiles > 0) ); // only if more files
	// so for the moment, here we cancel mus_optimize if typ_optimize if false
	mus_optimize = typ_optimize;
	
	bool fast = true;
    wxArrayPtrVoid typ_params;
	typ_params.Add( &nbOfFiles );
	typ_params.Add( &paths );
	typ_params.Add( &filenames );
	typ_params.Add( &isCacheOk );
	typ_params.Add( &fast ); // bool, fast or not
	typ_params.Add( typModelPtr );
	
    wxArrayPtrVoid mus_params;
	mus_params.Add( &nbOfFiles );
	mus_params.Add( &paths );
	mus_params.Add( &isCacheOk );
	mus_params.Add( musModelPtr );
	mus_params.Add( typModelPtr ); // to get the merged dictionnary
	
	int batch = 0;
	if ( typ_optimize )
		batch += 3;
	if ( mus_optimize )
		batch += 3;
	dlg->AddMaxBatchBar( batch );
	
	bool failed = false;
	
    if ( !failed && typ_optimize )
        failed = !TypAdaptation( typ_params, dlg );
		
	if ( m_fullOptimized || (typ_optimize && !failed) ) // load optimized model if needed
		failed = (!typModelPtr->Close() || !typModelPtr->Open( GetTypFilename() ));
		
    if ( !failed && typ_optimize )
        failed = !MusAdaptation( mus_params, dlg );
		
	if ( m_fullOptimized || (mus_optimize && !failed) ) // load optimized model if needed
		failed = (!musModelPtr->Close() || !musModelPtr->Open( GetMusFilename() ));
		
	return !failed;
}
	
// static
RecBookFileItem *RecBookFile::FindFile( ArrayOfBookFiles *array, wxString filename, int* index )
{
    wxASSERT( array );
    wxASSERT( index );

    *index = -1;

    for( int i = 0; i < (int)array->GetCount(); i++ )
        if ( array->Item(i).m_filename == filename )
        {
            *index = i;
            return &array->Item(i);
        }
        
    return NULL;
}


bool RecBookFile::LoadAxfiles( )
{
    wxArrayString paths;
    int index, nbfiles, i;

    if ( wxDirExists( m_axFileDir ) )
    {
        nbfiles = wxDir::GetAllFiles( m_axFileDir, &paths, "*.axz" );
    
        for ( i = 0; i < (int)nbfiles; i++ )
        {
            wxFileName name( paths[i] );
            if ( !RecBookFile::FindFile( &m_axFiles, name.GetFullName(), &index ) )
                m_axFiles.Add( RecBookFileItem( name.GetFullName() ) );
        }
        m_axFiles.Sort( SortBookFiles );
    }
    
    return true;
}


bool RecBookFile::LoadImages( )
{
    wxArrayString paths;
    int index, nbfiles, i;

    if ( wxDirExists( m_imgFileDir ) )
    {
        nbfiles = wxDir::GetAllFiles( m_imgFileDir, &paths, "*.tif", wxDIR_FILES  );
    
        for ( i = 0; i < (int)nbfiles; i++ )
        {
            wxFileName name( paths[i] );
            if ( !RecBookFile::FindFile( &m_imgFiles, name.GetFullName(), &index ) )
                m_imgFiles.Add( RecBookFileItem( name.GetFullName() ) );
        }
        m_imgFiles.Sort( SortBookFiles );
    }
    
    return true;
}

bool RecBookFile::RemoveImage( wxString filename )
{
    int index;
    const RecBookFileItem *book = RecBookFile::FindFile( &m_imgFiles, filename, &index );
    if ( book )
        m_imgFiles.RemoveAt( index );
    return true;
}

bool RecBookFile::DesactivateImage( wxString filename )
{
    int index;
    RecBookFileItem *book = RecBookFile::FindFile( &m_imgFiles, filename, &index );
    if ( book )
    {
        if (book->m_flags & FILE_DESACTIVATED)
            book->m_flags &= ~FILE_DESACTIVATED;
        else
            book->m_flags |= FILE_DESACTIVATED;
    }
    return true;
}

bool RecBookFile::DesactivateAxfile( wxString filename )
{
    int index;
    RecBookFileItem *book = RecBookFile::FindFile( &m_axFiles, filename, &index );
    if ( book )
    {
        if (book->m_flags & FILE_DESACTIVATED)
            book->m_flags &= ~FILE_DESACTIVATED;
        else
            book->m_flags |= FILE_DESACTIVATED;
    }
    return true;
}

bool RecBookFile::RemoveAxfile( wxString filename )
{
    int index;
    const RecBookFileItem *book = RecBookFile::FindFile( &m_axFiles, filename, &index );
    if ( book )
        m_axFiles.RemoveAt( index );
    return true;
}

bool RecBookFile::DeleteAxfile( wxString filename )
{
    RemoveAxfile( filename );
    wxString fullname = m_axFileDir + wxFileName::GetPathSeparator() +  filename;
    if ( wxFileExists( fullname ) )
        wxRemoveFile( fullname );
	return true;
}

// WDR: handler implementations for RecFile

#endif //AX_RECOGNITION
