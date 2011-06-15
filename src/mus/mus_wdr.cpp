/*
 *  mus_wdr.cpp
 *  aruspix
 *
 *  Created by Jamie Klassen on 11-06-14.
 *  Copyright 2011 com.aruspix.www. All rights reserved.
 *
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "mus_wdr.h"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// Include private header
#include "mus_wdr.h"

#include <wx/intl.h>

// Euro sign hack of the year
#if wxUSE_UNICODE
#define __WDR_EURO__ wxT("\u20ac")
#else
#if defined(__WXMAC__)
#define __WDR_EURO__ wxT("\xdb")
#elif defined(__WXMSW__)
#define __WDR_EURO__ wxT("\x80")
#else
#define __WDR_EURO__ wxT("\xa4")
#endif
#endif

// Custom source
#include "muswindow.h"

// Implement window functions

wxSizer *NewNeumeDlgFunc( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
	
    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );
	
    wxFlexGridSizer *item2 = new wxFlexGridSizer( 1, 0, 0 );
	
	wxString strs7[] =
	{
		_("Ancus"),
		_("Cavum"),
		_("Cephalicus"),
		_("Clivis"),
		_("Compound"),
		_("Custos"),
		_("Epiphonus"),
		_("Podatus"),
		_("Porrectus"),
		_("Punctum"),
		_("Salicus"),
		_("Scandicus"),
		_("Torculus"),
		_("Virga")
	};
	
	wxStaticText *item8 = new wxStaticText( parent, ID7_TEXT, _("Neume Type"), wxDefaultPosition, wxDefaultSize, 0 );
	item2->Add( item8, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );
	
	wxComboBox *item7 = new wxComboBox( parent, ID7_NEUME_TYPE, _("Neume Type"), wxDefaultPosition, wxDefaultSize, 14, strs7, wxCB_READONLY );
	item2->Add( item7, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );
	
    wxString strs3[] = 
    {
        _("None"), 
        _("Flexus"), 
        _("Resupinus")
    };
	
    wxRadioBox *item3 = new wxRadioBox( parent, ID7_NEUME_VARIANT, _("Variant"), wxDefaultPosition, wxDefaultSize, 3, strs3, 1, wxRA_SPECIFY_COLS );
    item2->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );
	
    wxBoxSizer *item4 = new wxBoxSizer( wxHORIZONTAL );
	
    wxStaticText *item5 = new wxStaticText( parent, ID7_TEXT, _("Number of Pitches"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );
	
    wxSpinCtrl *item6 = new wxSpinCtrl( parent, ID7_NB_PITCHES, wxT("1"), wxDefaultPosition, wxDefaultSize, 0, 1, 8, 1 );
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );
	
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );
	
    item1->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
    wxStaticBox *item12 = new wxStaticBox( parent, -1, _("Other") );
    wxStaticBoxSizer *item11 = new wxStaticBoxSizer( item12, wxVERTICAL );
	
	wxCheckBox *item13 = new wxCheckBox( parent, ID7_INCLINATUM, _("Inclinatum"), wxDefaultPosition, wxDefaultSize );
    item11->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxCheckBox *item14 = new wxCheckBox( parent, ID7_QUILISMA, _("Quilisma"), wxDefaultPosition, wxDefaultSize );
	item11->Add( item14, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );
	
    item1->Add( item11, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP, 5 );
	
    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );
	
    wxBoxSizer *item20 = new wxBoxSizer( wxHORIZONTAL );
	
    wxButton *item21 = new wxButton( parent, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item20->Add( item21, 0, wxALIGN_CENTER|wxALL, 5 );
	
    wxButton *item22 = new wxButton( parent, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item20->Add( item22, 0, wxALIGN_CENTER|wxALL, 5 );
	
    item0->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );
	
    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
