/*
 *  mus_wdr.h
 *  aruspix
 *
 *  Created by Jamie Klassen on 11-06-14.
 *  Copyright 2011 com.aruspix.www. All rights reserved.
 *
 */

#ifndef __WDR_mus_H__
#define __WDR_mus_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "mus_wdr.h"
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
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/grid.h>
#include <wx/toolbar.h>
#include <wx/tglbtn.h>

// Declare window functions

const int ID7_NEUME_TYPE = 17000;
const int ID7_NEUME_VARIANT = 17001;
const int ID7_NB_PITCHES = 17002;
const int ID7_INCLINATUM = 17003;
const int ID7_QUILISMA = 17004;
const int ID7_TEXT = 17005;
wxSizer *NewNeumeDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

#endif