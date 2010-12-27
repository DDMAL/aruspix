/////////////////////////////////////////////////////////////////////////////
// Name:        axfile.cpp
// Author:      Laurent Pugin
// Created:     04/05/25
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "axfile.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/file.h"
#include "wx/filedlg.h"

#include "axfile.h"
#include "axapp.h"

#include "wx/ptr_scpd.h"
wxDEFINE_SCOPED_PTR_TYPE(wxZipEntry);

const char *extensions[] = { 
	"axz", 
	"axtyp", 
	"axmus", 
	"axproj" ,
	"zip",
};

const char *filters[] = { 
	"Aruspix files|*.axz", 
	"Typographic models|*.axtyp",
	"Music models|*.axmus",
	"Aruspix projects|*.axproj",
	"Zip archive|*.zip"
};

const char *types[] = { 
	"Aruspix file", 
	"Typographic model",
	"Music model",
	"Project",
	"Zip archive"
};


// don't forget to check MAX_ENV_TYPES in axfile.h
const char *envtypes[] = { 
	"Rec", 
	"Sup",
	"Cmp",
    "Edt"
};


#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfBookFileItems );

int SortBookFileItems( AxBookFileItem **first, AxBookFileItem **second )
{
    if ( (*first)->m_filename < (*second)->m_filename )
        return -1;
    else if ( (*first)->m_filename > (*second)->m_filename )
        return 1;
    else
        return 0;
}





//----------------------------------------------------------------------------
// AxBookFileItem
//----------------------------------------------------------------------------

// static
AxBookFileItem *AxBookFileItem::FindFile( ArrayOfBookFileItems *array, wxString filename, int* index )
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

//----------------------------------------------------------------------------
// AxFile
//----------------------------------------------------------------------------

AxFile::AxFile( wxString name, int type, int envtype )
{
	m_type = type;
	m_envtype = envtype;
	m_isOpened = false;
	m_isModified = false;
	m_isNew = false;
	
	m_filename = "";
	m_shortname = _("untitled");
	
	m_error = ERR_NONE;
	
	m_xml_root = NULL;
	
	m_basename = wxGetApp().m_workingDir + "/" + name + "/";
	
	if ( wxDirExists( m_basename ) )
		AxDirTraverser clean( m_basename );
	else
		wxMkdir( m_basename );
}


AxFile::~AxFile()
{
	if ( wxDirExists( m_basename ) )
		AxDirTraverser clean( m_basename );	
}


// static method
wxString AxFile::GetEnvName( int envtype )
{
	if ( (envtype < 0) || (envtype >= MAX_ENV_TYPES) )
		return "";
	else
		return envtypes[ envtype ];
}

// static method
wxString AxFile::Open( int file_type )
{
	wxString *defaultFile = NULL;
	if ( file_type == AX_FILE_DEFAULT )
		defaultFile = &wxGetApp().m_lastDirAX0_in;
	else if ( file_type == AX_FILE_TYP_MODEL )
		defaultFile = &wxGetApp().m_lastDirAX1_in;
	else if ( file_type == AX_FILE_MUS_MODEL )
		defaultFile = &wxGetApp().m_lastDirAX2_in;
	else if ( file_type == AX_FILE_PROJECT )
		defaultFile = &wxGetApp().m_lastDirAX3_in;
	else if ( file_type == AX_FILE_ZIP )
		defaultFile = &wxGetApp().m_lastDirAX4_in;
	else
		return "";
	
	wxString filename = wxFileSelector( _("Open Aruspix file"), *defaultFile, _T(""), extensions[file_type], filters[file_type], wxFD_OPEN);
	if ( filename.IsEmpty() )
		return ""; // canceled
	
	(*defaultFile) = wxPathOnly( filename );
	
	return filename;
}

bool AxFile::New()
{
	// save file if needed
	if ( !this->Save( true ) )
		return false;
		
	// close file if needed
	if ( !this->Close() )
		return false;
		
	m_filename = "";
	m_shortname = _("untitled");
	m_vmaj = m_vmin = m_vrev = 10000; // arbitrary version, we assume we will never reach version 10000...
	
	// open content
	this->NewContent( );
		
	m_isNew = true;
	m_isModified = false;
	m_isOpened = true;
	
	return true;
}

bool AxFile::Open( wxString filename )
{
	// save file if needed
	if ( !this->Save( true ) )
		return false;
	
	// this check is redundant when opening from AxFrame
	if ( !this->Check( filename ) )
		return false;
		
	// close file if needed
	if ( !this->Close() )
		return false;
		
	m_filename = filename;
	wxFileName::SplitPath( filename, NULL, &m_shortname, NULL );

	wxZipEntryPtr entry;
    wxFFileInputStream in( m_filename );
    wxZipInputStream zip(in);

    while (entry.reset( zip.GetNextEntry()), entry.get() != NULL)
    {
        wxString name = entry->GetName();
		wxFFileOutputStream out( m_basename + name );
		out.Write( zip );
    }
	
	wxASSERT( !m_xml_root );
    TiXmlDocument dom( (m_basename + "index.xml").c_str() );
	if ( dom.LoadFile() )
	{
		m_xml_root = dom.RootElement();
		AxFile::GetVersion( m_xml_root, &m_vmaj, &m_vmin, &m_vrev );
	}
	// add warning if failed ???
	
	// open content
	this->OpenContent( );
	
	m_xml_root = NULL; // not needed anymore, and allocated by dom variable
	
	m_isNew = false;
	m_isModified = false;
	m_isOpened = true;
	
	return true;
}
	
bool AxFile::Save(  bool askUser )
{
	if ( !m_isOpened || !m_isModified )
		return true;
		
	if ( askUser )
	{	
		wxString msg = wxString::Format(_("Do you want to save changes made to Aruspix file '%s'?"), m_shortname.c_str() );
		int res = wxMessageBox( msg, wxGetApp().GetAppName() , wxYES_NO | wxCANCEL | wxICON_QUESTION );
		if ( res == wxNO )
		{
			m_isModified = false;
			return true;
		}
		else if ( res == wxCANCEL )
			return false;
	}
		
	if ( m_isNew )
		return this->SaveAs();

	// write xml file
	wxASSERT( !m_xml_root );
	m_xml_root = new TiXmlElement("file"); // new root
    m_xml_root->SetAttribute( "version_major",  wxString::Format("%d", AxApp::s_version_major ).c_str() );
	m_xml_root->SetAttribute( "version_minor",  wxString::Format("%d", AxApp::s_version_minor ).c_str() );
	m_xml_root->SetAttribute( "version_revision",  wxString::Format("%d", AxApp::s_version_revision ).c_str() );
	m_xml_root->SetAttribute( "type_id",  wxString::Format("%d", m_type ).c_str() );
	m_xml_root->SetAttribute( "env_type_id",  wxString::Format("%d", m_envtype ).c_str() );
	
	TiXmlElement infos("infos");
    infos.SetAttribute( "modified",  wxNow().c_str() );
	infos.SetAttribute( "user",  wxGetUserName().c_str() );
	infos.SetAttribute( "os",  wxGetOsDescription().c_str() );
	infos.SetAttribute( "type",  types[m_type] );
	infos.SetAttribute( "environment",  envtypes[m_envtype] );
	m_xml_root->InsertEndChild( infos );
		
	// save content
	this->SaveContent( );

	TiXmlDocument dom( (m_basename + "index.xml").c_str() );
    dom.InsertEndChild( *m_xml_root );
    if ( !dom.SaveFile() )
		return false;
	
	delete m_xml_root;
	m_xml_root = NULL;
		
	wxFFileOutputStream out( m_filename );
	wxZipOutputStream zip(out);
		
	wxArrayString files;
	wxDir::GetAllFiles( m_basename, &files, wxEmptyString, wxDIR_DEFAULT );
	for( int i = files.GetCount() - 1; i >=0; i-- )
	{
		wxFileName name( files[i] );
		name.MakeRelativeTo( m_basename );
		zip.PutNextEntry( name.GetFullPath() );
		//wxLogMessage( name.GetFullPath() );
		//wxLogMessage( files[i] );
		wxFFileInputStream in( files[i] );
		in.Read( zip );	
		zip.CloseEntry();	
	}
	zip.Close();
	
	m_isModified = false;
	
	return true;
}

bool AxFile::SaveAs( wxString filename )
{
	if ( !m_isOpened )
		return true;
		
	if ( filename.IsEmpty() )
	{
		wxString *defaultFile = NULL;
		if ( m_type == AX_FILE_DEFAULT )
			defaultFile = &wxGetApp().m_lastDirAX0_out;
		else if ( m_type == AX_FILE_TYP_MODEL )
			defaultFile = &wxGetApp().m_lastDirAX1_out;
		else if ( m_type == AX_FILE_MUS_MODEL )
			defaultFile = &wxGetApp().m_lastDirAX2_out;
		else if ( m_type == AX_FILE_PROJECT )
			defaultFile = &wxGetApp().m_lastDirAX3_out;
		else if ( m_type == AX_FILE_ZIP )
			defaultFile = &wxGetApp().m_lastDirAX4_out;
		else
			return false;
	
		filename = wxFileSelector( _("Save Aruspix file"), *defaultFile, m_shortname + "." + extensions[m_type], extensions[m_type], filters[m_type], wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
		if ( filename.IsEmpty() ) // cancel
			return false;
		
		(*defaultFile) = wxPathOnly( filename );
	}
    
	m_filename = filename;
	wxFileName::SplitPath( filename, NULL, &m_shortname, NULL );
	
	m_isNew = false;
	m_isModified = true; // to allow save

	return this->Save();
}

	
bool AxFile::Close( bool askUser )
{
	// save file if needed
	if ( !this->Save( askUser ) )
		return false;
		
	// desactivate content
	this->CloseContent();

	if ( wxDirExists( m_basename ) )
		AxDirTraverser clean( m_basename );
	else
		wxMkdir( m_basename );
		
	wxASSERT( !m_xml_root );
		
	m_isOpened = false;
	m_isModified = false;
	m_isNew = false;
	
	m_filename = "";
	m_shortname = _("untitled");
	
	return true;
}

bool AxFile::Check( wxString filename )
{
	if ( filename.IsEmpty() )
		return false;
	
	int type, envtype;
	//check version
	if ( !AxFile::Check( filename, &type, &envtype ) )
		return false;
		
	// temporaire...
	// return true;
	
	// check type and envtype
	if ( (m_type != type) || (m_envtype != envtype ) )
	{
		wxLogError( _("File type and environment type are not valid for '%s'"), filename.c_str() );
		return false;
	}
			
	return true;
}

// static method
void AxFile::GetVersion( TiXmlElement *root, int *vmaj, int *vmin, int *vrev )
{
    if ( root->Attribute("version_major"))
        *vmaj = atoi(root->Attribute("version_major"));
    if ( root->Attribute("version_minor"))
        *vmin = atoi(root->Attribute("version_minor"));
    if ( root->Attribute("version_revision"))
        *vrev = atoi(root->Attribute("version_revision"));
}

// static method
wxString AxFile::FormatVersion( int vmaj, int vmin, int vrev )
{
	return wxString::Format("%04d.%04d.%04d", vmaj, vmin, vrev );
}

// static method
bool AxFile::Check( wxString filename, int *type, int *envtype )
{
	wxASSERT( type );
	wxASSERT( envtype );
	
	*type = -1;
	*envtype = -1;
	
	if ( !wxFileExists( filename ) )
		return false;
		
	// temporaire, for files without xml
	// *type = 0;
	// *envtype = 0;
	// return true;

	wxString basename = wxGetApp().m_workingDir + "/axfile/";
	
	if ( wxDirExists( basename ) )
		AxDirTraverser clean( basename );
	else
		wxMkdir( basename );
		
	//wxFileName::SplitPath( filename, NULL, &m_shortname, NULL );

	wxZipEntryPtr entry;
    wxFFileInputStream in( filename );
    wxZipInputStream zip(in);

    while (entry.reset( zip.GetNextEntry()), entry.get() != NULL)
    {
        wxString name = entry->GetName();
		//wxLogMessage( name );
		if ( name == "index.xml" )
		{
			wxFFileOutputStream out( basename + name );
			out.Write( zip );
			break;
		}
    }

    TiXmlDocument dom( ( basename + "index.xml" ).c_str() );

	if ( !dom.LoadFile() )
	{
		wxLogDebug( "Failed opening file, probably index.xml is missing" );
		return false; // probably missing index.xml ...
	}
	
    TiXmlElement *root = NULL;

    root = dom.RootElement();
    if ( !root ) 
		return false;

	int vmaj, vmin, vrev;
	vmaj = vmin = vrev = 10000; // arbitrary version, we assume we will never reach version 10000...
	AxFile::GetVersion( root, &vmaj, &vmin, &vrev );
	if ( AxFile::FormatVersion( vmaj, vmin, vrev ) > 
		AxFile::FormatVersion( AxApp::s_version_major, AxApp::s_version_minor, AxApp::s_version_revision ) )
	//if ((AxApp::s_version_major <= vmaj) && (AxApp::s_version_minor <= vmin) || (AxApp::s_version_revision < vrev))
	{	
		wxLogError(_("Aruspix version is anterior to file version (%d.%d.%d)"), vmaj, vmin, vrev );
		return false;
	}

	if ( root->Attribute("type_id"))
        *type = atof(root->Attribute("type_id"));

	if ( root->Attribute("env_type_id"))
        *envtype = atof(root->Attribute("env_type_id"));
	
	return true;
}

// static method
wxString AxFile::GetPreview( wxString filename, wxString preview )
{
	if ( !wxFileExists( filename ) )
		return "";
		
	wxString basename = wxGetApp().m_workingDir + "/axfile/";
	
	if ( wxDirExists( basename ) )
		AxDirTraverser clean( basename );
	else
		wxMkdir( basename );

	wxZipEntryPtr entry;
    wxFFileInputStream in( filename );
    wxZipInputStream zip(in);

    while (entry.reset( zip.GetNextEntry()), entry.get() != NULL)
    {
        wxString name = entry->GetName();
		if ( name == preview )
		{
			wxFFileOutputStream out( basename + name );
			out.Write( zip );
			return basename + preview;
		}
    }

	return "";	
}

// static method
bool AxFile::ContainsFile( wxString filename, wxString search_filename )
{
	if ( !wxFileExists( filename ) )
		return false;
		
	wxString basename = wxGetApp().m_workingDir + "/axfile/";
	
	if ( wxDirExists( basename ) )
		AxDirTraverser clean( basename );
	else
		wxMkdir( basename );

	wxZipEntryPtr entry;
    wxFFileInputStream in( filename );
    wxZipInputStream zip(in);

    while (entry.reset( zip.GetNextEntry()), entry.get() != NULL)
    {
        wxString name = entry->GetName();
		if ( name == search_filename )
			return true;
    }

	return false;	
}

bool AxFile::Terminate( int code, ... )
{
    m_error = code;

    if ( code == ERR_NONE )
        return true; // normal ending

    // operation canceled
    if ( code == ERR_CANCELED )
    {
        wxLogMessage( _("Operation canceled") );
        return false;
    }

    // error
    va_list argptr;
    va_start( argptr, code );
    wxString msg;

    if ( code == ERR_UNKNOWN )
        msg = _("Unknown error");
    else if ( code == ERR_MEMORY )
        msg = _("Insufficient memory");
    else if ( code == ERR_FILE )
        msg.PrintfV( _("Error opening file '%s'"), argptr );
    else if ( code == ERR_READING )
        msg.PrintfV( _("Error reading image %d in file '%s'") , argptr );
    else if ( code == ERR_WRITING )
        msg.PrintfV( _("Error writing image in file '%s'") , argptr );
    
    va_end(argptr);
    wxLogError( msg );
    return false;
}

//----------------------------------------------------------------------------
// File selector function
//----------------------------------------------------------------------------

int AxFileSelector( int type, wxArrayString *filenames, wxArrayString *paths, wxWindow* parent )
{
	int i, nbOfFiles2;
	int nbOfFiles = 0;
	
	wxString file_wildcards;
	wxString dir_wildcard;
	wxString *defaultDir = NULL;
	wxString *defaultFile = NULL;
	wxString file_message;
	wxString dir_message;
	
	if ( type == AX_FILE_DEFAULT )
	{
		file_wildcards = "AXZ|*.axz";
		dir_wildcard = "*.axz";
		defaultDir = &wxGetApp().m_lastDirBatch_in;
		defaultFile = &wxGetApp().m_lastDirAX0_in;
		file_message = _("Select AXZ files");
		dir_message = _("Input folder");	
	}
	else if ( type == AX_FILE_TIFF )
	{
		file_wildcards = "TIFF|*.tif";
		dir_wildcard = "*.tif";
		defaultDir = &wxGetApp().m_lastDirBatch_in;
		defaultFile = &wxGetApp().m_lastDirTIFF_in;
		file_message = _("Select TIFF files");
		dir_message = _("Input folder");	
	}
	else
		return 0;
	
	wxArrayString paths2, filenames2;
	
	bool addmore;
	do {
		paths2.Clear();
		filenames2.Clear();
	
		// choose a directory of select muliple files
		if ( wxMessageBox("Select a directory rather than list of files?", wxGetApp().GetAppName() ,
                            wxYES | wxNO | wxICON_QUESTION ) == wxYES )
		{
			wxString input = wxDirSelector( dir_message, *defaultDir );
			if ( input.empty() )
				break;
				
			(*defaultDir) = input;
			nbOfFiles2 = wxDir::GetAllFiles( input, &paths2, dir_wildcard );
			if ( nbOfFiles2 == 0 )
			{
				addmore = ( wxMessageBox(_("Add more files?"), wxGetApp().GetAppName() ,
					wxYES | wxNO | wxICON_QUESTION ) == wxYES );
				continue;
			}
			for ( i = 0; i < (int)nbOfFiles2; i++ )
			{
				wxFileName name( paths2[i] );
				filenames2.Add( name.GetFullName() );
			}
			WX_APPEND_ARRAY( *paths, paths2 );
			WX_APPEND_ARRAY( *filenames, filenames2 );
		}
		else
		{
			// Get the WWG files
			wxFileDialog dialog( parent, file_message,
				*defaultFile, "", file_wildcards, wxFD_OPEN | wxFD_MULTIPLE);

			if (dialog.ShowModal() != wxID_OK)
				break;
				
			dialog.GetPaths(paths2);
			dialog.GetFilenames(filenames2);
			
			if (paths2.GetCount()==0)
				break;
			
			(*defaultFile) = paths2[0];
			WX_APPEND_ARRAY( *paths, paths2 );
			WX_APPEND_ARRAY( *filenames, filenames2 );
		}
		addmore = ( wxMessageBox(_("Add more files?"), wxGetApp().GetAppName() ,
                        wxYES | wxNO | wxICON_QUESTION ) == wxYES );
	} while ( addmore );

	nbOfFiles = paths->GetCount();
	
	wxASSERT( paths->GetCount() == filenames->GetCount() );
	
	return nbOfFiles;

}


