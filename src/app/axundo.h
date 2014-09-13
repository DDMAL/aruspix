/////////////////////////////////////////////////////////////////////////////
// Name:        axundo.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __axundo_H__
#define __axundo_H__

#ifdef AX_CMDLINE

//----------------------------------------------------------------------------
// Commandline replacement class
//----------------------------------------------------------------------------

class AxUndo
{
public:
    // constructors and destructors
    AxUndo( int i ) {};
    
};

#else

//----------------------------------------------------------------------------
// Gui
//----------------------------------------------------------------------------

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#define UNDO_ALL 0
#define UNDO_PART 1
#define UNDO_PART_INTERNAL 2  

class AxUndoRow;
WX_DECLARE_LIST(AxUndoRow, AxUndoList);

// WDR: class declarations

class AxUndoFile: public wxObject
{
public:
    // constructors and destructors
    AxUndoFile( wxString filename, int type );
    ~AxUndoFile();
	wxString GetFilename( ) { return m_filename; }
	int GetType( ) { return m_type; }
    
    // WDR: method declarations for AxUndoFile
    
private:
    // WDR: member variable declarations for AxUndoFile
	wxString m_filename;
	int m_type;

public:
    int m_flags;

private:
    // WDR: handler declarations for AxUndoFile

};

//----------------------------------------------------------------------------
// AxUndoRow
//----------------------------------------------------------------------------

class AxUndoRow: public wxObject
{
public:
    // constructors and destructors
    //AxUndoRow();
    AxUndoRow();
    ~AxUndoRow();
    
    // WDR: method declarations for AxUndoRow
	void DeleteUndoFile();
	void DeleteRedoFile();
    
private:
    // WDR: member variable declarations for AxUndoRow


public:
    int m_flags;
	AxUndoFile *m_undoFile;
	AxUndoFile *m_redoFile;

private:
    // WDR: handler declarations for AxUndoRow

};


//----------------------------------------------------------------------------
// AxUndo
//----------------------------------------------------------------------------

class AxUndo
{
public:
    // constructors and destructors
    AxUndo( int undoLevels );
	AxUndo( ) { m_undoLevels = 5; }
    virtual ~AxUndo();
	
    // WDR: method declarations for AxUndo
    bool CanUndo();      // Returns TRUE if can Undo
    bool CanRedo();      // Returns TRUE if can Redo
    void Undo();         // Restore next Undo state
    void Redo();         // Restore next Redo state
    void ResetUndos();        // Reset the Undo and Redo, for example when opening a new file
	void PrepareCheckPoint( int type, int flags );	// Save state before operation if previous undo is partial
    void CheckPoint( int type, int flags );   // Save current state
	// initialisation
	static void InitUndos( );
	static AxUndoFile *GetNext( int type, int flags = 0 );
	
private:
    AxUndoList m_undolist;    // Stores undo states
    AxUndoList m_redolist;    // Stores redo states
    long    m_undoLevels;  // Requested Undolevels 
	AxUndoRow *m_lastRow; // keep last row: if partial, m_redoFile will be complete in PrepareCheckPoint

    void AddUndo( AxUndoRow *undoPtr );
    void AddRedo( AxUndoRow *redoPtr );
    void ClearRedoList();
	
protected:
    virtual void Load( AxUndoFile *undoPtr ) {};
    virtual void Store( AxUndoFile *undoPtr ) {};

    
public:
    // WDR: member variable declarations for AxUndo
	wxString m_filename; 
	wxString m_shortname;
	wxString m_basename; // name of the directory where the file is extracted
   
protected:

private:
	static wxString s_directory;
	static int s_index;
	static bool s_isOk;

};

#endif // AX_CMDLINE

#endif
