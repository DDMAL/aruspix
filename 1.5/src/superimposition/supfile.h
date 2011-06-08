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

class ImRegister;
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
	bool SupFile::DetectPoints( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool SupFile::RegisterImages( wxArrayPtrVoid params, AxProgressDlg *dlg ); 
	// getters
	void SupFile::GetSrc1( AxImage *image );
	void SupFile::GetSrc2( AxImage *image );
	void SupFile::GetResult( AxImage *image );
	
	// status
	bool SupFile::IsSuperimposed() { return m_isSuperimposed; }
	
	// status
	static bool SupFile::IsSuperimposed( wxString filename );
	
private:
    
public:
    // WDR: member variable declarations for SupFile
	SupEnv *m_envPtr;
	ImRegister *m_imRegisterPtr;
	wxString m_original1, m_original2;
	bool m_hasNewPoints1, m_hasNewPoints2;
	wxPoint m_points1[4];
    wxPoint m_points2[4];
	bool m_hasPoints1;
	bool m_hasPoints2;

protected:
    // WDR: member variable declarations for SupFile
	bool m_isSuperimposed;
	//AxImage 
   
private:
    // WDR: handler declarations for SupFile

};

#endif //AX_SUPERIMPOSITION

#endif
