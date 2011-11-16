/////////////////////////////////////////////////////////////////////////////
// Name:       axenv.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
//#include "wx/wx.h"

#include "axenv.h"
#include "axapp.h"
#include "axframe.h"


//----------------------------------------------------------------------------
// AxEnv
//----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(AxEnv,wxEvtHandler)
	//EVT_UPDATE_UI_RANGE( wxID_LOWEST, ENV_IDS_MAX , AxEnv::OnUpdateUI )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(AxEnv,wxEvtHandler)

AxEnv::AxEnv() :
    wxEvtHandler()
{
    m_framePtr = NULL;
    m_envWindowPtr = NULL;
    m_isLoaded = false;
    m_isShown = false;

	m_envMenuBarFuncPtr = NULL;
    m_envMenuCount = 0;
    //m_envToolBarFuncPtr = NULL;
    m_envToolBarPtr = NULL;
}

AxEnv::~AxEnv()
{
}

void AxEnv::Load(AxFrame *parent)
{
    if (m_isLoaded) return;
    
    wxASSERT_MSG(parent,"Parent frame window cannot be NULL");
    m_framePtr = parent;

    // this->LoadConfig(); called from axframe
	this->LoadWindow();
	//if (this->m_envWindowPtr)
	//this->m_envWindowPtr->SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    
    m_isLoaded = true;
}
    
void AxEnv::Unload()
{
    if (!m_isLoaded) return;
    
    //this->SaveConfig(); called from axframe
    
    m_isLoaded = false;
}
    
void AxEnv::Show()
{
    wxASSERT_MSG(m_framePtr,"Parent frame window cannot be NULL");
	wxASSERT_MSG(m_envWindowPtr,"Environment window window cannot be NULL");

    if (m_isShown) 
		return;
    
	this->RealizeToolbar( );

	int i;
    m_envMenuCount = 0;
    wxMenuBar *frameMenuBar = m_framePtr->GetMenuBar();
    if (m_envMenuBarFuncPtr && frameMenuBar)
    {
		wxMenuBar *menuBar = m_envMenuBarFuncPtr();

		// menu 1 (Fichier) insertion des autres items
		m_commonMenuItems1.Clear();
		wxMenu *menu1 = menuBar->GetMenu(0);
		wxMenu *frameMenu1 = frameMenuBar->GetMenu(0);
		int frameMenu1Count = frameMenu1->GetMenuItemCount();
		wxMenuItemList *list1 = &menu1->GetMenuItems();
		for ( wxMenuItemList::Node* node1 = list1->GetLast(); node1; node1 = node1->GetPrevious() )
		{
			wxMenuItem *item = node1->GetData();
			if (!item)
				continue;
			wxString text = item->GetText(); // necessaire sous GTK, sinon remplace les racourcis par _
			text.Replace("_","&");
			frameMenu1->Insert(frameMenu1Count - AX_MENUS_1_ITEMS,item->GetId(), text,item->GetHelp(),item->GetKind());			
			m_commonMenuItems1.Add(item->GetId()); // tableau des ids a supprimer quand decharge
		}

		// menu 2 (Edition) insertion des autres items
		m_commonMenuItems2.Clear();
		wxMenu *menu2 = menuBar->GetMenu(1);
		wxMenu *frameMenu2 = frameMenuBar->GetMenu(1);
		int frameMenu2Count = frameMenu2->GetMenuItemCount();
		wxMenuItemList *list2 = &menu2->GetMenuItems();
		for ( wxMenuItemList::Node* node2 = list2->GetLast(); node2; node2 = node2->GetPrevious() )
		{
			wxMenuItem *item = node2->GetData();
			if (!item)
				continue;
			wxString text = item->GetText(); // necessaire sous GTK, sinon remplace les racourcis par _
			text.Replace("_","&");
			frameMenu2->Insert(frameMenu2Count - AX_MENUS_2_ITEMS, item->GetId(), text, item->GetHelp(),item->GetKind());			
			m_commonMenuItems2.Add(item->GetId()); // tableau des ids a supprimer quand decharge
		}

		// insertion des autres menus (entre AX_MENUS_COMMON et AX_MENUS_END)
		int position = frameMenuBar->GetMenuCount() - 1 - AX_MENUS_END;
        int env_count = menuBar->GetMenuCount();
        for (i = env_count; i > AX_MENUS_COMMON; i--)
        {
            wxString title = menuBar->GetLabelTop(i-1); 
            // ne devrait pas marcher d'apres la doc wx (GetLabelTop)
            // appeler uniquement si dejà associe à une frame ???
            wxMenu *menu = menuBar->Remove(i-1);
            frameMenuBar->Insert(position ,menu , title);
            m_envMenuCount++;
        }

		menuBar->Destroy();
    }

	this->Resize();
	this->RestoreToolBar();
	this->UpdateTitle();
	m_envWindowPtr->Show();
	m_framePtr->SetFocus();
    m_isShown = true;
}
    
void AxEnv::Hide()
{
    wxASSERT_MSG(m_framePtr,"Parent frame window cannot be NULL");
	wxASSERT_MSG(m_envWindowPtr,"Environment window window cannot be NULL");
    
	if (!m_isShown)
		return;

    wxMenuBar *frameMenuBar = m_framePtr->GetMenuBar();
    if (frameMenuBar)
    {
		int i;
		// suppression des autres menus (entre AX_MENUS_COMMON et AX_MENUS_END)
        int position = frameMenuBar->GetMenuCount() - m_envMenuCount - AX_MENUS_END;
        for (i = 0; i < m_envMenuCount; i++)
        {
            wxMenu* menu = frameMenuBar->Remove(position - 1);
            delete menu;
        }

		// menu 1 (Fichier) suppressions des autres items (dans m_commonMenuItems1)
		wxMenu *frameMenu1 = frameMenuBar->GetMenu(0);
		for(i = 0; i < (int)m_commonMenuItems1.GetCount(); i++)
		{
			frameMenu1->Destroy(frameMenu1->FindItem(m_commonMenuItems1[i]));
		}
		m_commonMenuItems1.Clear();
		
		// menu 2 (Edition) suppressions des autres items (dans m_commonMenuItems2)
		wxMenu *frameMenu2 = frameMenuBar->GetMenu(1);
		for(i = 0; i < (int)m_commonMenuItems2.GetCount(); i++)
		{
			frameMenu2->Destroy(frameMenu2->FindItem(m_commonMenuItems2[i]));
		}
		m_commonMenuItems2.Clear();

    }


    //if (m_envToolBarFuncPtr && this->m_envWindowPtr)
    {
		/*m_framePtr->GetToolBar()->AddTool( 0, "", wxBitmap() ); // first add dummy tool to get last position...
		int last = m_framePtr->GetToolBar()->GetToolPos( 0 );
		for ( ; last >= 0; last--) 
			m_framePtr->GetToolBar()->DeleteToolByPos(last);*/

		/*m_framePtr->GetToolBar()->Destroy();
		m_framePtr->m_toolBarPtr = m_framePtr->CreateToolBar( wxTB_HORIZONTAL | wxNO_BORDER  , -1);
		MainToolBarFunc( m_framePtr->GetToolBar() );*/
		m_framePtr->RealizeToolbar();	
    }

    m_envWindowPtr->Hide();
	m_isShown = false;
    m_framePtr->SendSizeEvent();
}

void AxEnv::Resize()
{
	wxASSERT_MSG(m_framePtr,"Parent frame window cannot be NULL");
	
#if defined(__WXMSW__) || defined(__APPLE__)	

	/*
		not needed on mac, has to be checked on windoze
	wxToolBar *tb = m_framePtr->GetToolBar();
	wxASSERT_MSG(tb,"Frame toolbar cannot be NULL");

	wxSize bsize = tb->GetBestSize();
	wxSize size = tb->GetSize();
	wxPoint pos = tb->GetPosition();
	
	if ( m_envToolBarPtr )
    {
 		tb->SetSize(bsize.x,bsize.y);
        m_envToolBarPtr->Move(pos.x + bsize.x, pos.y);
        m_envToolBarPtr->SetSize(size.x - bsize.x, bsize.y);
    }
	*/

	wxSize csize = m_framePtr->GetClientSize();
	if (this->m_envWindowPtr)
		this->m_envWindowPtr->SetSize(0, 0, csize.x, csize.y);

#elif defined( __WXGTK__)
//#elif defined( __WXGTK__)  || defined(__APPLE__)	

	wxSize size = m_framePtr->GetClientSize();

    int offset;
    if ( m_envToolBarPtr )
    {
        m_envToolBarPtr->SetSize(size.x, -1);
        m_envToolBarPtr->Move(0, 0);
        offset = m_envToolBarPtr->GetSize().y;
    }
    else
    {
        offset = 0;
    }
	
	if (this->m_envWindowPtr)
		this->m_envWindowPtr->SetSize(0, offset, size.x, size.y - offset);

#endif
}

void AxEnv::UpdateTitle( )
{
	this->SetTitle(_T(""));
}


void AxEnv::SetTitle( const wxChar *pszFormat, ...)
{
	wxWindow *frame = wxGetApp().GetTopWindow();
	if ( !frame || !frame->IsKindOf( CLASSINFO( AxFrame ) ) )
		return;

    va_list argptr;
    va_start( argptr, pszFormat );

    wxString tit;
    tit.PrintfV( pszFormat, argptr );

    va_end(argptr);

	wxString env = ((AxFrame*)frame)->GetEnvironmentName( );
	if ( env.IsEmpty() )
		env = wxGetApp().GetAppName();
	else
		env.Prepend( wxGetApp().GetAppName() + " - ");
	if ( tit.IsEmpty() )
		tit = env;
	else
		tit.Prepend( env + " - " );

	((AxFrame*)frame)->SetTitle( tit );
	
}





void AxEnv::OnUpdateUI( wxUpdateUIEvent &event )
{
	event.Enable(true);
}


//----------------------------------------------------------------------------
// EnvironmentRow
//----------------------------------------------------------------------------

AxEnvRow::AxEnvRow()
{
    m_envPtr = NULL;
    m_menuId = 0;
}

AxEnvRow::AxEnvRow( wxString className, wxString menuItem, int menuId, wxString helpStr )
{
    m_envPtr = NULL;
    m_className = className;
    m_menuItem = menuItem;
    m_menuId = menuId;
	m_helpStr = helpStr;
}

AxEnvRow::~AxEnvRow()
{
    if (m_envPtr)
        delete m_envPtr;
}

