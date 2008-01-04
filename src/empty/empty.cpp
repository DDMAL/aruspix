/////////////////////////////////////////////////////////////////////////////
// Name:        empty.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_EMPTY

#define VERSION 

#ifdef __GNUG__
    #pragma implementation "empty.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/dirctrl.h"
#include "wx/calctrl.h"

#include "empty.h"
#include "app/axframe.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// PanelEmpty
//----------------------------------------------------------------------------

// WDR: event table for PanelEmpty

BEGIN_EVENT_TABLE(PanelEmpty,wxPanel)
    //EVT_TREE_SEL_CHANGED( ID0_DIRECTORIES, PanelEmpty::OnTree )
END_EVENT_TABLE()

PanelEmpty::PanelEmpty( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    wxGenericDirCtrl *ctrl = new wxGenericDirCtrl(this,ID0_DIRECTORIES,"D:/",wxDefaultPosition, wxSize(200,600));
    //ctrl->Init();
    WindowFunc0( this, TRUE ); 
    
}

// WDR: handler implementations for PanelEmpty

void PanelEmpty::OnTree( wxTreeEvent &event )
{
    wxMessageBox("TREE");
}


//----------------------------------------------------------------------------
// EnvEmpty
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(EnvEmpty,AxEnv)
    EVT_MENU( ID0_MENUITEM, EnvEmpty::OnMenuItem )
    EVT_UPDATE_UI_RANGE( wxID_LOWEST, 14999 , EnvEmpty::OnUpdateUI )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(EnvEmpty,AxEnv)

EnvEmpty::EnvEmpty():
    AxEnv()
{
    this->m_envToolBarFuncPtr = ToolBarFunc0;
    this->m_envMenuBarFuncPtr = MenuBarFunc0;
}

EnvEmpty::~EnvEmpty()
{
    if (m_envWindowPtr) m_envWindowPtr->Destroy();
    m_envWindowPtr = NULL;
}

void EnvEmpty::LoadWindow()
{
    this->m_envWindowPtr = new PanelEmpty(m_framePtr,-1);
}


// WDR: handler implementations for EnvEmpty

void EnvEmpty::OnUpdateUI( wxUpdateUIEvent &event )
{
	event.Enable(true);
}

void EnvEmpty::OnMenuItem( wxCommandEvent &event )
{
    static const wxCmdLineEntryDesc cmdLineDesc[] = {
        {wxCMD_LINE_PARAM,  NULL, NULL, _("Input files"),
		 wxCMD_LINE_VAL_STRING,
         wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE},
        {wxCMD_LINE_NONE}
	};

	wxCmdLineParser parser( cmdLineDesc );
	parser.SetCmdLine("cmd D:/TIFF/db.jpeg D:/TIFF/allanota.jpg");
	((AxFrame*)m_framePtr)->SetEnvironment( "envsuperimposition" );
	((AxFrame*)m_framePtr)->ParseCmd( &parser );
}

#endif // AX_EMPTY

