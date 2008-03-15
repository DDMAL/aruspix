/////////////////////////////////////////////////////////////////////////////
// Name:        supfile.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __supfile_H__
#define __supfile_H__

#ifdef AX_SUPERIMPOSITION

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "supfile.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axfile.h"

class ImPage;
class SupEnv;
class AxImage;

// WDR: class declarations


//----------------------------------------------------------------------------
// SupFile
//----------------------------------------------------------------------------

class SupFile: public AxFile 
{
public:
    // constructors and destructors
    SupFile::SupFile( wxString name, SupEnv *env = NULL );
    virtual SupFile::~SupFile();
    
    // WDR: method declarations for SupFile
	virtual void SupFile::NewContent(); // Create content for a new file
	virtual void SupFile::OpenContent( ); // Open content after archive extraction
	virtual void SupFile::SaveContent( ); // Save content before archive creation
	virtual void SupFile::CloseContent( ); // Desactivate content before deletion
	
	// functor
	bool SupFile::Superimpose( wxArrayPtrVoid params, AxProgressDlg *dlg );
	// getters
	ImPage *SupFile::GetImPage1() { return m_imPagePtr1; }
	ImPage *SupFile::GetImPage2() { return m_imPagePtr2; }
	
	// status
	bool SupFile::IsSuperimposed() { return m_isSuperimposed; }
	
	// status
	static bool SupFile::IsSuperimposed( wxString filename );
	
    
public:
    // WDR: member variable declarations for SupFile
	SupEnv *m_envPtr;
	ImPage *m_imPagePtr1, *m_imPagePtr2;

protected:
    // WDR: member variable declarations for SupFile
	bool m_isSuperimposed;
   
private:
    // WDR: handler declarations for SupFile

};

#endif //AX_SUPERIMPOSITION

#endif
