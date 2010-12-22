/////////////////////////////////////////////////////////////////////////////
// Name:        edtfile.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_EDT

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "edtfile.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/file.h"
#include "wx/filename.h"
#include "wx/valgen.h"

#include "edtfile.h"
#include "edt.h"

#include "app/axapp.h"

#include "mus/musfile.h"
#include "mus/musiobin.h"

enum
{
    MUS_PAPER_A4 = 0,
    MUS_PAPER_A5,
    MUS_PAPER_OTHER
};

enum
{
    MUS_PAPER_PORTRAIT = 0,
    MUS_PAPER_LANDSCAPE
};

int EdtNewDlg::s_staffLines = 5;
int EdtNewDlg::s_nbStaves = 12;
int EdtNewDlg::s_notationMode = MUS_MENSURAL_MODE;
int EdtNewDlg::s_paperSize = MUS_PAPER_A4;
wxString EdtNewDlg::s_paperHeight = "297";
wxString EdtNewDlg::s_paperWidth = "210";
int EdtNewDlg::s_paperOrientation = MUS_PAPER_PORTRAIT; 


//----------------------------------------------------------------------------
// EdtFile
//----------------------------------------------------------------------------

EdtFile::EdtFile( wxString name, EdtEnv *env )
	: AxFile( name, AX_FILE_DEFAULT, AX_FILE_EDITION )
{
	m_envPtr = env;
    m_musFilePtr = NULL;
}



EdtFile::~EdtFile()
{
	if ( m_musFilePtr )
		delete m_musFilePtr;
}


void EdtFile::NewContent( )
{
	wxASSERT_MSG( !m_musFilePtr, "MusFile should be NULL" );

	// new MusFile
    m_musFilePtr = new MusFile();
    m_musFilePtr->m_fname = m_basename + "page.bin";
}


void EdtFile::OpenContent( )
{
	this->NewContent();
	
	if ( !wxFileExists( m_basename + "page.bin") ) {
        return;
    }
    
    MusBinInput *bin_input = new MusBinInput( m_musFilePtr, m_musFilePtr->m_fname );
    bin_input->ImportFile();
    delete bin_input;
	
	return;       
}


void EdtFile::SaveContent( )
{
	wxASSERT_MSG( m_musFilePtr, "MusFile should not be NULL" );
	wxASSERT( m_xml_root );
	
    // save
    MusBinOutput *bin_output = new MusBinOutput( m_musFilePtr, m_musFilePtr->m_fname );
    bin_output->ExportFile();
    delete bin_output;

}

void EdtFile::CloseContent( )
{
	// nouveau fichier ?
    if ( m_musFilePtr )
    {
        delete m_musFilePtr;
        m_musFilePtr = NULL;
    }
}

bool EdtFile::Create( )
{
    EdtNewDlg dlg( m_envPtr->GetWindow(), -1, "Parameters for the new file" );
    dlg.Center(wxBOTH);
    if (dlg.ShowModal() != wxID_OK) 
        return false;
        
    if (EdtNewDlg::s_notationMode == MUS_CMN_MODE) {
        wxLogMessage("This is not implemented");
        return false;
    }
    m_musFilePtr->m_fheader.param.notationMode = EdtNewDlg::s_notationMode;
        
    int height, width;
    switch (EdtNewDlg::s_paperSize) {
    case (MUS_PAPER_A4):
        height = 297;
        width = 210;
        break;
    case (MUS_PAPER_A5):
        height = 210;
        width = 148;
        break;
    case (MUS_PAPER_OTHER):
        height = atoi(EdtNewDlg::s_paperHeight);
        width = atoi(EdtNewDlg::s_paperWidth);
    }
    
    if (EdtNewDlg::s_paperOrientation == MUS_PAPER_LANDSCAPE) {
        int tmp = height;
        height = width;
        width = tmp;
    }

    MusPage *musPage = new MusPage();
	m_musFilePtr->m_pages.Clear();
	m_musFilePtr->m_fheader.param.pageFormatHor = width;
	m_musFilePtr->m_fheader.param.pageFormatVer = height;
    
    /*
    int x1 = 5, x2 = 195;
    m_imPagePtr->CalcLeftRight( &x1, &x2 ); 
	x1 = 0; // force it, indentation will be calculated staff by staff
    m_musFilePtr->m_fheader.param.MargeGAUCHEIMPAIRE = x1 / 10;
    m_musFilePtr->m_fheader.param.MargeGAUCHEPAIRE = x1 / 10;
    */
    musPage->lrg_lign = width - 20;
  
    for (int i = 0; i < EdtNewDlg::s_nbStaves; i++)
    {
        MusStaff *musStaff = new MusStaff();
        musStaff->no = i;
        musStaff->portNbLine = EdtNewDlg::s_staffLines;
        if (EdtNewDlg::s_notationMode == MUS_MENSURAL_MODE)
            musStaff->notAnc = true;
        musStaff->vertBarre = DEB_FIN;
        musPage->m_staves.Add( musStaff );
    }   
    m_musFilePtr->m_pages.Add( musPage );
    m_musFilePtr->CheckIntegrity();

    return true;
}


//----------------------------------------------------------------------------
// EdtNewDlg
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(EdtNewDlg,wxDialog)
END_EVENT_TABLE()

EdtNewDlg::EdtNewDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    EdtNewDlgFunc( this, TRUE );
    
    this->GetScStaffLines()->SetValidator(
        wxGenericValidator(&EdtNewDlg::s_staffLines));
    this->GetScNbStaves()->SetValidator(
        wxGenericValidator(&EdtNewDlg::s_nbStaves));
    this->GetRbNotationMode()->SetValidator(
        wxGenericValidator(&EdtNewDlg::s_notationMode));
    // paper
    this->GetRbPaperSize()->SetValidator(
        wxGenericValidator(&EdtNewDlg::s_paperSize)); 
    this->GetRbPaperOrientation()->SetValidator(
        wxGenericValidator(&EdtNewDlg::s_paperOrientation)); 
    this->GetTxPaperHeight()->SetValidator(
        wxTextValidator(wxFILTER_NUMERIC, &EdtNewDlg::s_paperHeight)); 
    this->GetTxPaperWidth()->SetValidator(
        wxTextValidator(wxFILTER_NUMERIC, &EdtNewDlg::s_paperWidth)); 
        
}

#endif //AX_EDT

