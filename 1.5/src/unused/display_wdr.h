//------------------------------------------------------------------------------
// Header generated by wxDesigner from file: display.wdr
// Do not modify this file, all changes will be lost!
//------------------------------------------------------------------------------

#ifndef __WDR_display_H__
#define __WDR_display_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "display_wdr.h"
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

const int ID3_SPLITTER1 = 13000;
wxSizer *WindowFunc3( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

// Declare menubar functions

const int ID3_MENU_FILE = 13001;
const int ID3_MENU0 = 13002;
const int ID3_OPEN1 = 13003;
const int ID3_OPEN2 = 13004;
const int ID3_MENU_EDIT = 13005;
const int ID3_TOOL = 13006;
wxMenuBar *MenuBarFunc3();

// Declare toolbar functions

void ToolBarFunc3( wxToolBar *parent );

// Declare bitmap functions

wxBitmap BitmapsFunc3( size_t index );

#endif

// End of generated file
