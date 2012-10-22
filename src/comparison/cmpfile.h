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

class MusLayer;
class MusLayerElement;


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
	bool IsCollationLoaded( CmpCollationPart *part );
    MusDoc *GetMusDoc() { return m_musDocPtr; };
	
	
protected:
    /**
     * Align the two layer using a dynamic progamming approach.
     * The alignement is performed using the edit distance.
     * For each match, the uuid is added to the uuid_refs and uuid_vars and uuid_count is incremented.
     * This is done in order to then change the uuid in the variant layout for element synchronization.
     */
	MusLayer *Align( MusLayer *layer_ref, MusLayer *layer_var, uuid_t *uuid_refs, uuid_t *uuid_vars, int *uuid_count );
	
    /**
     * Create a <app> element in the layer_aligned MusLayer.
     * The appType can be CMP_APP_DEL, CMP_APP_INS ou CMP_APP_SUBST.
     * A deletion means that the element position i in not in layer_var.
     * A insertion means that the element position j in layer_var is missing in layer_aligned after i.
     * A substitution is element position j replacing element position i.
     */ 
    void CreateApp( MusLayer *layer_aligned, int i, MusLayer *layer_var, int j, int appType );  
	

public:
    wxString m_id;
	wxString m_name;
	ArrayOfCmpCollationParts m_collationParts; // to be modified to an extended object with position for refinement
	int m_length; // x_abs of the last element of the reference staff + 50 (used to build the file)
	// for dislpay in the tree
	wxTreeItemId m_colId;
	
	static int s_index;
	
private:
    /** The basename of the CmpFile, used to read/write file from this class. */
	wxString m_basename;
    /** The name of the reference source */
    wxString m_refSource;
    /** The name of the variant source */
    wxString m_varSource;
    /** The MusDoc of the final collation */
    MusDoc *m_musDocPtr;

	bool m_isColLoaded;
};

//----------------------------------------------------------------------------
// CmpPartPage
//----------------------------------------------------------------------------

class CmpPartPage: public wxObject
{
public:
    // constructors and destructors
    CmpPartPage();
    CmpPartPage(  wxString axfile );
    ~CmpPartPage() {};

public:
    wxString m_axfile;
	uuid_t m_start;
    uuid_t m_end;
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
    
    /** @name File methods
     * Standard file operation (new, open, save, close) methods for this AxFile child class.
     */
    ///@{
    virtual void NewContent(); // Create content for a new file
    virtual void OpenContent( ); // Open content after archive extraction
    virtual void SaveContent( ); // Save content before archive creation
    virtual void CloseContent( ); // Desactivate content before deletion
    ///@}
    
    /**
     * This method load the content from the Recognition books (or files).
     * For each book, it create one MEI file for each part in the book.
     * The part in the MEI file is represented in one layer.
     * We several pages (or page parts) can be concatenated.
     * The MEI file also contain the corresonding layout pages.
     * Also see MusDoc::ResetAndCheckLayouts().
     */
    bool LoadBooks( wxArrayPtrVoid params, AxProgressDlg *dlg );
    
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

    bool Collate( wxArrayPtrVoid params, AxProgressDlg *dlg );
    // static on arrays
    static CmpBookItem *FindFile( ArrayOfCmpBookItems *array, wxString filename, int* index  );
   
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
