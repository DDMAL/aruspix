/////////////////////////////////////////////////////////////////////////////
// Name:        axenv.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __axenv_H__
#define __axenv_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/event.h"
#include "wx/cmdline.h"  // command line support

class AxEnvRow;
class AxFrame;

#define AX_MENUS_END 0 // nombre de menus à la fin de la barre (aide, ...)
#define AX_MENUS_COMMON 2 // nombre de menus commun au debut (fichier, edition, ...)
#define AX_MENUS_1_ITEMS 6 // nombre d'entrees de menus a la fin du menu 1
#define AX_MENUS_2_ITEMS 0 // nombre d'entrees de menus a la fin du menu 2

//typedef void EnvToolBarFunc_t ( wxToolBar *parent );
typedef wxMenuBar *EnvMenuBarFunc_t ( );



//----------------------------------------------------------------------------
// AxEnv
//----------------------------------------------------------------------------

class AxEnv: public wxEvtHandler
{
public:
    // constructors and destructors
    AxEnv();
    virtual ~AxEnv();
    
    void Load(AxFrame *parent);
    void Unload();
    void Show();
    void Hide();
	void Resize();
    wxWindow *GetWindow() { return m_envWindowPtr; };
	virtual void ParseCmd( wxCmdLineParser *parser ) {};
	virtual void UpdateTitle( );
	virtual void Open( wxString filename, int type ) {};
	virtual bool CloseAll( ) { return true; };
	static void LoadConfig() {};
    static void SaveConfig() {};

protected:
    virtual void LoadWindow() {};
	virtual void RestoreToolBar() {}; // restore values
	virtual void RealizeToolbar( ) {}; // build the toolbar
	void SetTitle( const wxChar *pszFormat, ... );
    
private:
    
protected:
    AxFrame *m_framePtr; // pointer to parent frame
    wxWindow *m_envWindowPtr; // pointer to environment window
	EnvMenuBarFunc_t *m_envMenuBarFuncPtr;
    int m_envMenuCount;
	wxArrayInt m_commonMenuItems1, m_commonMenuItems2; // ID des entrees dans le menus communs
	// toolbars
    wxToolBar *m_envToolBarPtr;
    //EnvToolBarFunc_t *m_envToolBarFuncPtr;

public:
    bool m_isLoaded; // flag true if environemnent is loaded
    bool m_isShown;
    
private:
    virtual void OnUpdateUI( wxUpdateUIEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(AxEnv)

	DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// AxEnvRow
//----------------------------------------------------------------------------

class AxEnvRow: public wxObject
{
public:
    // constructors and destructors
    AxEnvRow();
    AxEnvRow(wxString className, wxString menuItem, int menuId, wxString helpStr = "" );
    ~AxEnvRow();
    
        
private:
    
public:
    AxEnv *m_envPtr;
    wxString m_menuItem;
    wxString m_className;
	wxString m_helpStr;
    int m_menuId;

private:
    
};

#endif
