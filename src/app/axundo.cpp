/////////////////////////////////////////////////////////////////////////////
// Name:        axundo.cpp
// Author:      Laurent Pugin
// Created:     04/05/25
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "axundo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/file.h"

#include "axundo.h"
#include "axapp.h"

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(AxUndoList);

//#include "wx/ptr_scpd.h"
//wxDEFINE_SCOPED_PTR_TYPE(wxZipEntry);

//statics
wxString AxUndo::s_directory = "";
int AxUndo::s_index = 0;
bool AxUndo::s_isOk = false;


// WDR: class implementations

//----------------------------------------------------------------------------
// AxUndoFile
//----------------------------------------------------------------------------

AxUndoFile::AxUndoFile( wxString filename, int type )
{
	m_filename = filename;
	m_type = type;
}

AxUndoFile::~AxUndoFile()
{
	wxLogDebug("Delete undo file %s", m_filename.c_str() );
	wxLogNull *logNo = new wxLogNull();
    if ( wxFileExists( m_filename ) )
	{
		bool ok = wxRemoveFile( m_filename );
		wxASSERT_MSG( ok , "Error erasing undo file" );
	}
	delete logNo;
}

//----------------------------------------------------------------------------
// AxUndoRow
//----------------------------------------------------------------------------

AxUndoRow::AxUndoRow( )
{
	m_undoFile = NULL;
	m_redoFile = NULL;
}

AxUndoRow::~AxUndoRow()
{
	DeleteRedoFile();
	DeleteUndoFile();
}

void AxUndoRow::DeleteUndoFile()
{
	if ( m_undoFile )
		delete m_undoFile;
	m_undoFile = NULL;
}

void AxUndoRow::DeleteRedoFile()
{
	if ( m_redoFile )
		delete m_redoFile;
	m_redoFile = NULL;
}

//----------------------------------------------------------------------------
// AxUndo
//----------------------------------------------------------------------------

AxUndo::AxUndo( int undoLevels )
{
	m_undoLevels = undoLevels;
	m_lastRow = new AxUndoRow();
}


AxUndo::~AxUndo()
{
	m_undolist.DeleteContents( true );
	m_undolist.Clear();
	m_redolist.DeleteContents( true );
	m_redolist.Clear();
	if ( m_lastRow )
		delete m_lastRow;
}


void AxUndo::ClearRedoList()
{
	m_redolist.DeleteContents( true );
	m_redolist.Clear();
	m_redolist.DeleteContents( false );
}


bool AxUndo::CanUndo() 
{
    return (m_undolist.GetCount() > 0);
}


bool AxUndo::CanRedo() 
{
    return (m_redolist.GetCount() > 0);
}

// Adds state to the beginning of undo list
void AxUndo::AddUndo( AxUndoRow *undoPtr )
{
    // Remove old state if there are more than max allowed
	m_undolist.DeleteContents( true );
    if ( (int)m_undolist.GetCount() > m_undoLevels ) {
        AxUndoList::Node *node = m_undolist.GetLast();
		m_undolist.DeleteNode( node );
    }
	m_undolist.DeleteContents( false );
    // Add new state to head of undo list
	//wxLogDebug("Add %s to undo list", undoPtr->GetFilename().c_str() );
    m_undolist.Insert(undoPtr);
}

// Save current object state to undoFile of currentRow if previous (redo) was partial
void AxUndo::PrepareCheckPoint( int type, int flags ) 
{
	wxASSERT_MSG( m_lastRow , "PrepareCheckPoint: UndoRow cannot be NULL");
	//if ( !m_lastRow )
	//	return; // PrepareCheckPoint has already been called
	
	m_lastRow->DeleteUndoFile();
    this->ClearRedoList();
			
	if ( (!m_lastRow->m_redoFile) || (m_lastRow->m_redoFile->GetType()!=UNDO_ALL) )
	// We don'thave what we need
	{	
		m_lastRow->m_undoFile = AxUndo::GetNext( type, flags );
		this->Store( m_lastRow->m_undoFile );
	}
	this->AddUndo( m_lastRow );
	m_lastRow = NULL;
}

// Save current object state to Undo list
void AxUndo::CheckPoint( int type, int flags ) 
{
	wxASSERT_MSG( !m_lastRow , "CheckPoint: UndoRow should be NULL");
	//if ( m_lastRow )
	//	return; // CheckPoint has already been called

	m_lastRow = new AxUndoRow();
	m_lastRow->m_redoFile = AxUndo::GetNext( type, flags );
	this->Store( m_lastRow->m_redoFile );
}

void AxUndo::AddRedo( AxUndoRow *redoPtr )
{
    // Move state to head of redo list
	//wxLogDebug("Add %s to redo list", redoPtr->GetFilename().c_str() );
    m_redolist.Insert(redoPtr);
}

// Perform an Undo command
void AxUndo::Undo() 
{
    if ( !CanUndo() )
		return;
		
	wxASSERT_MSG( m_lastRow , "Undo: UndoRow cannot be NULL");
	
	// first we put last row in the redolist
	this->AddRedo( m_lastRow );
	m_lastRow = NULL;

	// first row in UndoList become last row
	AxUndoList::Node *node;
	node = m_undolist.GetFirst();
	wxASSERT_MSG( node , "Undo: node cannot be NULL");
	m_lastRow = node->GetData();
	//wxLogDebug("Remove %s from undo list", redo->GetFilename().c_str() );
	m_undolist.Erase( node );
	wxASSERT_MSG( m_lastRow , "Undo: m_lastRow cannot be NULL");

	// load
	if ( m_lastRow->m_undoFile == NULL ) // this is the case when redoFile is UNDO_ALL
	{
		wxASSERT_MSG( m_lastRow->m_redoFile , "Undo: redoFile cannot be NULL");
		Load( m_lastRow->m_redoFile );
	}
	else
	{
		Load( m_lastRow->m_undoFile );
	}
}

//Perform a Redo Command
void AxUndo::Redo() 
{
    if ( !CanRedo() ) 
		return;
		
	wxASSERT_MSG( m_lastRow , "Redo: UndoRow cannot be NULL");
	
	// first we put last row in the undolist
	this->AddUndo( m_lastRow );
	m_lastRow = NULL;
	
	// first row in RedoList become last row
	AxUndoList::Node *node;
	node = m_redolist.GetFirst();
	wxASSERT_MSG( node , "Redo: node cannot be NULL");
	m_lastRow = node->GetData();
	//wxLogDebug("Remove %s from undo list", redo->GetFilename().c_str() );
	m_redolist.Erase( node );
	wxASSERT_MSG( m_lastRow , "Redo: m_lastRow cannot be NULL");
	
	// load
	wxASSERT_MSG( m_lastRow->m_redoFile , "Undo: redoFile cannot be NULL");
	Load( m_lastRow->m_redoFile );
}

// statics

void AxUndo::InitUndos()
{
	wxString dir = wxGetApp().m_workingDir + "/undo/";

	if ( wxDirExists( dir ) )
		AxDirTraverser clean( dir );
	else
		wxMkdir( dir );

	if ( !AxApp::CheckDir( dir, 0755 ) )
		return;
	
	s_directory = dir;
	s_isOk = true;
}

AxUndoFile * AxUndo::GetNext( int type, int flags )
{
	wxASSERT_MSG( s_isOk, "AxUndo::GetNext: AxUndo has not been initialized"  );
			
	wxString filename = wxString::Format("%s_%d", s_directory.c_str(), AxUndo::s_index++ );
	AxUndoFile *undo = new AxUndoFile( filename, type );
	undo->m_flags = flags;
	return undo;
}

// WDR: method declarations for AxUndo



// WDR: handler implementations for AxUndo
