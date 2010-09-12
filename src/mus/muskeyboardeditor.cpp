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

#define MAX_VALUES 6


/** MusKeyboardEditor implementation */

MusKeyboardEditor::MusKeyboardEditor(MusWindow *pwindow) {
	lastKeyEntered = NULL;
	m_keyEntryOctave = 0;
	w_ptr = pwindow;
}

MusKeyboardEditor::~MusKeyboardEditor()
{}

// TODO: I should separate this method for neumes and mensural
// 08/28/2010 getting rid of redundant keybinds


bool MusKeyboardEditor::handleMetaKey(int key) {
	//different handling for mensural vs. neumes

	// check for other meta keys
	if (key == 'O') {
		if (w_ptr->m_currentElement)
		{
			if (w_ptr->m_currentElement->IsNeume()) {
				MusNeume *temp = (MusNeume*)w_ptr->m_currentElement;
				temp->SetClosed(!temp->closed);

				
				printf("Setting closed: %d\n", temp->IsClosed());
				
				
				return true;	
			} else return false;
		}
	}
	
	// left/right movement 
	
	if (key == WXK_RIGHT) {
		if (w_ptr->m_currentElement->IsNeume()) {
			MusNeume *temp = (MusNeume*)w_ptr->m_currentElement;
			if (temp->IsClosed()) {
//				w_ptr->m_currentElement = w_ptr->m_currentStaff->
//				GetNext( w_ptr->m_currentElement);
//				return true;
				return false;	//use current keybind
			} else {
				temp->GetNextPunctum();
				return true;
			}
		} else return false;
	}
	
	if (key == WXK_LEFT) {
		if (w_ptr->m_currentElement->IsNeume()) {
			MusNeume *temp = (MusNeume*)w_ptr->m_currentElement;
			if (temp->IsClosed()) {
//				w_ptr->m_currentElement = w_ptr->m_currentStaff->
//				GetPrevious( w_ptr->m_currentElement);
//				return true;
				return false;	//use current keybind
			} else {
				temp->GetPreviousPunctum();
				return true;
			}
		} else return false;
	}
	
	// up/down for easy pitch changing

	if (key == WXK_DOWN || key == WXK_UP)
	{
		int direction;
		key == WXK_DOWN ? direction = -1 : direction = 1; 
		
		
		int newoctave;
		int	newcode = w_ptr->m_insertcode = 
			w_ptr->m_currentElement->filtrcod(w_ptr->m_currentElement->code + direction, 
											  &newoctave);
			
		printf("Setting pitch to this; %d, %d\n", newcode, newoctave);
		w_ptr->m_currentElement->SetPitch(newcode, newoctave);
		return true;
	}
	
	// M key changes note value (note 'Mode')
	if (key == 'M') {
		printf("mode change\n");
		if (w_ptr->m_currentElement->IsNeume()) {
			// closed vs. open editing handling
			MusNeume *temp = (MusNeume*)w_ptr->m_currentElement;
			int value;
			if (temp->IsClosed()) {
				//do stuff
			} else {
				temp->SetValue((temp->GetValue() + 1) % 
							   MAX_VALUES, w_ptr->m_currentStaff, 0);
			}
			//int value = (((MusNeume*)w_ptr->m_currentElement)->val + 1) % 5;
			//w_ptr->m_currentElement->SetValue(value, w_ptr->m_currentStaff, 0);
		}
		return true;
	}
	
	if (key == 'N') {
		if (w_ptr->m_currentElement->IsNeume())
		{
			((MusNeume *)w_ptr->m_currentElement)->InsertPitchAfterSelected();
			return true;
		} 
	}
	
	if (key == WXK_SPACE) {
		if (w_ptr->m_currentElement->IsNeume())
		{
			((MusNeume *)w_ptr->m_currentElement)->
			Copy();
			return true;
		}
	}
	
	if (key == WXK_DELETE || WXK_BACK) {
		if (w_ptr->m_currentElement->IsNeume())
		{
			MusNeume *p = (MusNeume*)w_ptr->m_currentElement;
			if (p->IsClosed()) {
				return false;
			} else {
				((MusNeume *)w_ptr->m_currentElement)->RemoveSelectedPitch();
				return true;
			}
		}
	}
	
	return false;
}

bool MusKeyboardEditor::handleKeyEvent(wxKeyEvent &event)
{
//	printf("Handling a key event~!\n");
	//no point in being in keyboard edit mode if there's no element to edit 
	if (!w_ptr->m_currentElement) return false;
	
	bool event_handled = false;
	
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
		case WXK_SPACE: // break neumes
		case WXK_DELETE:
		case WXK_BACK:
		case WXK_LEFT:
		case WXK_RIGHT:
		case WXK_DOWN:
		case WXK_UP:
		case 'M': // mode change: toggle value of neume
		case 'N': // append punctum in open mode
		case 'O': // open-closed mode toggle
//		case '1':
//		case '2':
//		case '3':
//		case '4':
//		case '5':
//		case '6':
//		case '7':
//		case '8':
//		case '9':
//		case '0':
			return handleMetaKey(key); //hmm
	}
	
	int pitch, octave, hauteur;
	
	MusElement *element = w_ptr->m_currentElement;
	MusStaff *staff = w_ptr->m_currentStaff;
	int i = 0;
	for (i; i < 17; i++) {
		
		if (event.GetKeyCode() == keyPitches[i] || oct_switch) {
			//this keybind is supported by MusKeyboardEditor
			event_handled = true;
			
//			printf("key found: %d\n", i);
			pitch = 60 + i + (12 * m_keyEntryOctave);
			octave = pitch / 12;
			hauteur = pitch - (octave * 12);
			
			if ( w_ptr->IsNoteSelected() ) 
			{
//				printf("changing pitch: %d\n", i);
//				element->SetPitch( die[hauteur], octave, staff );
				element->SetPitch( die[hauteur], octave );
			}
			
			break;
		}
		
	} 
	
	lastKeyEntered = event.GetKeyCode();
	
	if (i == 17) event_handled = false;
	return event_handled;
	//printf("i: %d\n", i);
	
	
} 
