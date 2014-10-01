/////////////////////////////////////////////////////////////////////////////
// Name:        edtfile.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_EDITION

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/file.h"
#include "wx/filename.h"
#include "wx/valgen.h"

#include "edtfile.h"
#include "edt.h"

#include "app/axapp.h"

#include "musdoc.h"
#include "muspage.h"
#include "mussystem.h"
#include "musstaff.h"
#include "musiomei.h"
#include "musiobin.h"

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
int EdtNewDlg::s_m_notationMode = MUS_MENSURAL_MODE;
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
    m_musDocPtr = NULL;
}



EdtFile::~EdtFile()
{
	if ( m_musDocPtr )
		delete m_musDocPtr;
}


void EdtFile::NewContent( )
{
	wxASSERT_MSG( !m_musDocPtr, "MusDoc should be NULL" );

	// new MusDoc
    m_musDocPtr = new MusDoc();
    m_musDocPtr->m_fname = m_basename + "page.bin";
}


void EdtFile::OpenContent( )
{
	this->NewContent();
	
	if ( !wxFileExists( m_basename + "page.bin") ) {
        return;
    }
    
    MusBinInput_1_X *bin_input = new MusBinInput_1_X( m_musDocPtr, m_musDocPtr->m_fname );
    bin_input->ImportFile();
    delete bin_input;
	
	return;       
}


void EdtFile::SaveContent( )
{
	wxASSERT_MSG( m_musDocPtr, "MusDoc should not be NULL" );
	wxASSERT( m_xml_root );
	
    // save
    MusMeiOutput *mei_output = new MusMeiOutput( m_musDocPtr, m_musDocPtr->m_fname );
    mei_output->ExportFile();
    delete mei_output; 
}

void EdtFile::CloseContent( )
{
	// nouveau fichier ?
    if ( m_musDocPtr )
    {
        delete m_musDocPtr;
        m_musDocPtr = NULL;
    }
}

bool EdtFile::Create( )
{
    EdtNewDlg dlg( m_envPtr->GetWindow(), -1, "Parameters for the new file" );
    dlg.Center(wxBOTH);
    if (dlg.ShowModal() != wxID_OK) 
        return false;
        
    if (EdtNewDlg::s_m_notationMode == MUS_CMN_MODE) {
        wxLogMessage("This is not implemented");
        return false;
    }
    m_musDocPtr->m_env.m_notationMode = EdtNewDlg::s_m_notationMode;
        
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
    
    // provisory util we change the dialog box
    int nb_systems;
    int nb_staves_per_system = 1;
    if ((EdtNewDlg::s_nbStaves % 4)==0) {
        nb_staves_per_system = 4;
    } 
    else if ((EdtNewDlg::s_nbStaves % 3)==0) {
        nb_staves_per_system = 3;
    }
    else if ((EdtNewDlg::s_nbStaves % 2)==0) {
        nb_staves_per_system = 2;
    }
    nb_systems = EdtNewDlg::s_nbStaves / nb_staves_per_system;
    
    int i, j;
    
    // the way the doc is created need to be rethought
    // especially handling pageBreaks and systemBreaks
    
    // reset the MusDoc and create the logical tree
    m_musDocPtr->Reset( Transcription);	
    MusPage *page = new MusPage();
    
	m_musDocPtr->m_pageWidth = width * 10;
	m_musDocPtr->m_pageHeight = height * 10;
  
    for (i = 0; i < nb_systems; i++) {
        MusSystem *system = new MusSystem();
        //system->lrg_lign = width - 20; // we have now m_systemRightMar  
        for (j = 0; j < nb_staves_per_system; j++)
        {
            MusStaff *staff = new MusStaff( j + 1 );
            staff->portNbLine = EdtNewDlg::s_staffLines;
            if (EdtNewDlg::s_m_notationMode == MUS_MENSURAL_MODE)
                staff->notAnc = true;
            if (nb_staves_per_system == 1) {
                staff->vertBarre = START_END;
            }
            else if (j == 0) {
                staff->vertBarre = START;
                staff->brace = START;
            }
            else if (j == nb_staves_per_system - 1) {
                staff->vertBarre = END;
                staff->brace = END;
            }
            // We also need to create the layers!
            // ...
            // We also need to add pageBreaks and systemBreak to the layers
            system->AddStaff( staff );
        }
        page->AddSystem( system );
    }
    m_musDocPtr->AddPage( page );

    return true;
}


//----------------------------------------------------------------------------
// EdtNewDlg
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(EdtNewDlg,wxDialog)
EVT_RADIOBOX(ID5_NOTATION, EdtNewDlg::OnNotationChange)
END_EVENT_TABLE()

EdtNewDlg::EdtNewDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    EdtNewDlgFunc( this, true );
    
    this->GetScStaffLines()->SetValidator(
        wxGenericValidator(&EdtNewDlg::s_staffLines));
    this->GetScNbStaves()->SetValidator(
        wxGenericValidator(&EdtNewDlg::s_nbStaves));
    this->GetRbNotationMode()->SetValidator(
        wxGenericValidator(&EdtNewDlg::s_m_notationMode));
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

void EdtNewDlg::OnNotationChange(wxCommandEvent& event) {
	int id = event.GetInt();
	wxSpinCtrl* spin = (wxSpinCtrl*) FindWindow( ID5_STAFF_LINES );
	if (id == MUS_MENSURAL_MODE) {
		spin->SetValue(5);
	} else if (id == MUS_NEUMATIC_MODE) {
		spin->SetValue(4);
	} else if (id == MUS_CMN_MODE) {
		spin->SetValue(5);
	}
}

#endif //AX_EDITION

