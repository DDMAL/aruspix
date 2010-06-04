/*
 *  MusKeyboardEditor.cpp
 *  aruspix
 *
 *  Created by Chris Niven on 03/06/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifdef __GNUG__
#pragma implementation "muskeyboardeditor.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "muskeyboardeditor.h"
#include "musstaff.h"
#include "muswindow.h"
#include "muselement.h"


/** MusKeyboardEditor implementation */

void MusKeyboardEditor::handleKeyEvent(wxKeyEvent &event)
{
	int key = event.GetKeyCode();
	bool oct_switch = false;
	switch (key)
	{
		case 'Z':
			m_keyEntryOctave--;
			oct_switch = true;
			break;
		case 'X':
			m_keyEntryOctave++;
			oct_switch = true;
			break;
		case WXK_RETURN:
		case WXK_SPACE:
			handleMetaKey(key);
			break;
	}
	
	int pitch, octave, hauteur;
	
	MusElement *element = w_ptr->m_currentElement;
	MusStaff *staff = w_ptr->m_currentStaff;
	
	for (int i = 0; i < 17; i++) {
		
		if (event.GetKeyCode() == keyPitches[i] || oct_switch) {
			pitch = 60 + i + (12 * m_keyEntryOctave);
			octave = pitch / 12;
			hauteur = pitch - (octave * 12);
			
			if ( element && 
				(element->IsNote() || element->IsNeume())) {
				element->SetPitch( die[hauteur], octave, staff );
			}
			
			break;
		}
		
	}
	
	lastKeyEntered = event.GetKeyCode();
} 

MusKeyboardEditor::~MusKeyboardEditor()
{}

/** NeumeKeyEditor implementation */

NeumeKeyEditor::NeumeKeyEditor(MusWindow *pwindow)
{
	lastKeyEntered = NULL;
	m_keyEntryOctave = 0;	
	w_ptr = pwindow;
}

void NeumeKeyEditor::handleMetaKey(int key) {
	switch (key)
	{
		case WXK_RETURN:
			break;
		case WXK_SPACE:
			break;
	}
}

/** MensuralKeyEditor implementation */

MensuralKeyEditor::MensuralKeyEditor(MusWindow *pwindow)
{
	lastKeyEntered = NULL;
	m_keyEntryOctave = 0;
	w_ptr = pwindow;
}

void MensuralKeyEditor::handleMetaKey(int key) {
	switch (key)
	{
		case WXK_RETURN:
			// change note style?
			break;
		case WXK_SPACE:
			// not sure what to do here
			break;
	}
}

