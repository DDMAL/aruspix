//------------------------------------------------------------------------------
// Header generated by wxDesigner from file: wgviewer.wdr
// Do not modify this file, all changes will be lost!
//------------------------------------------------------------------------------

#ifndef __WDR_wgviewer_H__
#define __WDR_wgviewer_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "wgviewer_wdr.h"
#endif

// Include wxWidgets' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/image.h>
#include <wx/statline.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/splitter.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/grid.h>
#include <wx/toolbar.h>
#include <wx/tglbtn.h>

// Declare window functions

const int ID5_LINE = 15000;
const int ID5_TOOLPANEL = 15001;
const int ID5_WGPANEL = 15002;
wxSizer *WindowFunc5( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

// Declare menubar functions

const int ID5_MENU_FILE = 15003;
const int ID5_MENU = 15004;
const int ID5_OPEN = 15005;
const int ID5_Save = 15006;
const int ID5_SAVE_MLF = 15007;
const int ID5_SAVE_MODEL = 15008;
const int ID5_OPEN_MLF = 15009;
const int ID5_MENU_EDIT = 15010;
const int ID5_PAGE = 15011;
const int ID5_PREVIOUS = 15012;
const int ID5_NEXT = 15013;
const int ID5_GOTO = 15014;
const int ID5_ZOOMOUT = 15015;
const int ID5_ZOOMIN = 15016;
const int ID5_TOOLS = 15017;
const int ID5_INSERT_MODE = 15018;
const int ID5_NOTES = 15019;
const int ID5_KEYS = 15020;
const int ID5_SIGNS = 15021;
const int ID5_SYMBOLES = 15022;
wxMenuBar *MenuBarFunc5();

// Declare toolbar functions

void ToolBarFunc5( wxToolBar *parent );

// Declare bitmap functions

wxBitmap BitmapsFunc5( size_t index );

#endif

// End of generated file
