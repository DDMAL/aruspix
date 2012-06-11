/////////////////////////////////////////////////////////////////////////////
// Name:        cmpfile.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __cmpfile_H__
#define __cmpfile_H__

#ifdef AX_RECOGNITION
	#ifdef AX_COMPARISON

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dynarray.h"

#include "app/axfile.h"
#include "recognition/recbookfile.h"
#include "mus/musiomlf.h"

#include "cmp_wdr.h"

class CmpEnv;
class CmpFile;
class CmpCtrl;

class CmpBookItem;
WX_DECLARE_OBJARRAY( CmpBookItem, ArrayOfCmpBookItems);

class CmpCollation;
WX_DECLARE_OBJARRAY( CmpCollation, ArrayOfCmpCollations);

class CmpBookPart;
WX_DECLARE_OBJARRAY( CmpBookPart, ArrayOfCmpBookParts);

class CmpPartPage;
WX_DECLARE_OBJARRAY( CmpPartPage, ArrayOfCmpPartPages);

class CmpCollationPart;
WX_DECLARE_OBJARRAY( CmpCollationPart, ArrayOfCmpCollationParts);

class MusLaidOutStaff;


//----------------------------------------------------------------------------
// CmpCollationPart
//----------------------------------------------------------------------------

class CmpCollationPart: public wxObject
{
public:
    // constructors and destructors
    CmpCollationPart() { m_bookPart = NULL; }
    CmpCollationPart(  CmpBookPart *bookPart, int flags = 0  );
    ~CmpCollationPart();

public:
    CmpBookPart *m_bookPart;
	int m_flags;
	int m_del, m_ins, m_subst; // alignment results
	double m_recall, m_precision;
	int m_seq; // length of the sequence
	
};

//----------------------------------------------------------------------------
// CmpCollation
//----------------------------------------------------------------------------

class CmpCollation: public wxObject
{
public:
    // constructors and destructors
    CmpCollation() { }
    CmpCollation( wxString id, wxString name, wxString basename  );
    ~CmpCollation();
	
	bool Collate( );
	bool Realize( );
	bool IsCollationLoaded();
	MusDoc *GetMusFile() { return m_musDocPtr; }
	
	
protected:
	bool Align( MusLaidOutStaff *staff_ref, MusLaidOutStaff *staff_var, CmpCollationPart *part_var );
	void EndInsertion( CmpCollationPart *part_var  );
	void AddInsertion( MusElement *elem, MusLaidOutStaff *aligned_staff, int i ); // Add elem into an insertion Staff
			// If this staff has to be created, a symbol is added into the aligned staff before element i 
	void SetCmpValues( MusElement *dest, MusElement *src, int flag );
	

public:
    wxString m_id;
	wxString m_name;
	ArrayOfCmpCollationParts m_collationParts; // to be modified to an extended object with position for refinement
	int m_length; // x_abs of the last element of the reference staff + 50 (used to build the file)
	// for dislpay in the tree
	wxTreeItemId m_colId;
	
	static int s_index;
	
private:
	wxString m_basename; // the basename of the CmpFile, used to read/write file from this class
	MusDoc *m_musDocPtr;
	bool m_isColLoaded;
	
	MusLaidOutStaff *m_insStaff;
};

//----------------------------------------------------------------------------
// CmpPartPage
//----------------------------------------------------------------------------

class CmpPartPage: public wxObject
{
public:
    // constructors and destructors
    CmpPartPage() { }
    CmpPartPage(  wxString axfile ) { m_axfile = axfile; }
    ~CmpPartPage() {};

public:
    wxString m_axfile;
	wxArrayInt m_staves; // all staves if the array is empty
};

//----------------------------------------------------------------------------
// CmpBookPart
//----------------------------------------------------------------------------
// a part is a sub set of pages, with subset of staves


class CmpBookPart: public wxObject
{
public:
    // constructors and destructors
    CmpBookPart() { m_book = NULL; }
    CmpBookPart( wxString id, wxString name, wxString bookname, CmpBookItem *book );
    ~CmpBookPart();

public:
    wxString m_id;
	wxString m_name;
	wxString m_bookname;
	ArrayOfCmpPartPages m_partpages; // to be modified to an extended object with position for refinement
	CmpBookItem *m_book;
	
	static int s_index;
};

//----------------------------------------------------------------------------
// CmpBookItem
//----------------------------------------------------------------------------

class CmpBookItem: public wxObject
{
public:
    // constructors and destructors
    CmpBookItem() { m_recBookFilePtr = NULL; }
    CmpBookItem( wxString filename, int flags = 0 );
    ~CmpBookItem();
	
	// operations
	bool AssembleParts( );

public:
    wxString m_filename;
	wxString m_shortname;
	int m_flags;
	RecBookFile *m_recBookFilePtr;
    wxTreeItemId m_bookId;
    wxTreeItemId m_axFilesId;
    wxTreeItemId m_imgFilesId;
	wxTreeItemId m_partsId;
	// parts
	ArrayOfCmpBookParts m_bookParts;
	
	static int s_index;
};

//----------------------------------------------------------------------------
// CmpFile
//----------------------------------------------------------------------------

class CmpFile: public AxFile 
{
public:
    // constructors and destructors
    CmpFile( wxString name, CmpEnv *env = NULL );
    virtual ~CmpFile();
    
        virtual void NewContent(); // Create content for a new file
    virtual void OpenContent( ); // Open content after archive extraction
    virtual void SaveContent( ); // Save content before archive creation
    virtual void CloseContent( ); // Desactivate content before deletion
    
    // operations
    //bool LoadImages( );
    //bool LoadAxfiles( );
    //bool RemoveImage( wxString filename );
    //bool DesactivateImage( wxString filename );
    //bool RemoveAxfile( wxString filename );
    //bool DeleteAxfile( wxString filename );
	bool DesactivateAxfile( wxString filename );
	// files
	//int FilesToPreprocess( wxArrayString *filenames, wxArrayString *paths, bool add_axfiles = true );
	//int FilesToRecognize( wxArrayString *filenames, wxArrayString *paths );
	//int FilesForAdaptation( wxArrayString *filenames, wxArrayString *paths, bool *isCacheOk );
	//bool HasToBePreprocessed( wxString imagefile );
	CmpBookPart *FindBookPart( wxString id );
	bool LoadBooks( wxArrayPtrVoid params, AxProgressDlg *dlg );
    bool Collate( wxArrayPtrVoid params, AxProgressDlg *dlg );
    // static on arrays
    static CmpBookItem *FindFile( ArrayOfCmpBookItems *array, wxString filename, int* index  );
	// distance
	void Align( ArrayOfMLFSymbols *obtained, ArrayOfMLFSymbols *desired );
    
public:
        // files
    ArrayOfCmpBookItems m_bookFiles;
	ArrayOfCmpCollations m_collations;

protected:
        CmpEnv *m_envPtr;


private:
    
};

	#endif //AX_COMPARISON
#endif //AX_RECOGNITION

#endif
