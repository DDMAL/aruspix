/*
 *  MusKeyboardEditor.h
 *  aruspix
 *
 *  Created by Chris Niven on 03/06/10.
 *
 */

#ifndef __MUS_KEYBOARD_EDITOR_H__
#define __MUS_KEYBOARD_EDITOR_H__

#ifdef __GNUG__
#pragma implementation "muskeyboardeditor.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

class MusWindow;
class MensuralKeyEditor;
class NeumeKeyEditor;

const char keyPitches[] = 
{ 'A', 'W', 'S', 'E', 'D', 'F','T', 'G', 'Y', 'H', 'U', 'J',
'K', 'O', 'L', 'P', ';', '\'' };


class MusKeyboardEditor 
{
protected:
	int lastKeyEntered;
	int m_keyEntryOctave;	
	MusWindow *w_ptr;
	bool handleMetaKey(int key);
public:
	MusKeyboardEditor(MusWindow *pwindow);
	virtual ~MusKeyboardEditor();
	
	bool handleKeyEvent(wxKeyEvent &event);
};

#endif