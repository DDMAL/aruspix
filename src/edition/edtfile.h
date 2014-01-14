/////////////////////////////////////////////////////////////////////////////
// Name:        edtfile.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __edtfile_H__
#define __edtfile_H__

#ifdef AX_EDITION

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axfile.h"

#include "verovio/doc.h"

#include "edt_wdr.h"

class EdtEnv;
class vrv::Doc;


//----------------------------------------------------------------------------
// EdtFile
//----------------------------------------------------------------------------

class EdtFile: public AxFile 
{
public:
    // constructors and destructors
    EdtFile( wxString name, EdtEnv *env = NULL );
    virtual ~EdtFile();
    
    /** @name File methods
     * Standard file operation (new, open, save, close) methods for this AxFile child class.
     */
    ///@{
    virtual void NewContent(); // Create content for a new file
    virtual void OpenContent( ); // Open content after archive extraction
    virtual void SaveContent( ); // Save content before archive creation
    virtual void CloseContent( ); // Desactivate content before deletion
    ///@}
    
    bool Create( ); // Create a file with user parameters (dialog)
	
private:
    
public:
    EdtEnv *m_envPtr;
    vrv::Doc *m_musDocPtr;

protected:
       
private:
    
};


//----------------------------------------------------------------------------
// EdtNewDlg
//----------------------------------------------------------------------------

class EdtNewDlg: public wxDialog
{
public:
    // constructors and destructors
    EdtNewDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );

    wxSpinCtrl* GetScStaffLines()  { return (wxSpinCtrl*) FindWindow( ID5_STAFF_LINES ); }
    wxSpinCtrl* GetScNbStaves()  { return (wxSpinCtrl*) FindWindow( ID5_NB_STAVES ); }
    wxRadioBox* GetRbNotationMode()  { return (wxRadioBox*) FindWindow( ID5_NOTATION ); }
    
    // paper
    wxRadioBox* GetRbPaperSize()  { return (wxRadioBox*) FindWindow( ID5_RB_SIZE ); }
    wxRadioBox* GetRbPaperOrientation()  { return (wxRadioBox*) FindWindow( ID5_RB_ORIENTATION ); }
    wxTextCtrl* GetTxPaperHeight()  { return (wxTextCtrl*) FindWindow( ID5_TX_HEIGHT ); }
    wxTextCtrl* GetTxPaperWidth()  { return (wxTextCtrl*) FindWindow( ID5_TX_WIDTH ); }
	
	void OnNotationChange(wxCommandEvent& event);
    
public:
    static int s_staffLines;
    static int s_nbStaves;
    static int s_m_notationMode;
    static int s_paperSize;
    static wxString s_paperWidth;
    static wxString s_paperHeight;
    static int s_paperOrientation;
    
private:


private:
    DECLARE_EVENT_TABLE()
};

#endif //AX_EDITION

#endif
