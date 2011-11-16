/////////////////////////////////////////////////////////////////////////////
// Name:        supfile.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __supfile_H__
#define __supfile_H__

#ifdef AX_SUPERIMPOSITION

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axfile.h"

class ImRegister;
class SupEnv;
class AxImage;



//----------------------------------------------------------------------------
// SupFile
//----------------------------------------------------------------------------

class SupFile: public AxFile 
{
public:
    // constructors and destructors
    SupFile( wxString name, SupEnv *env = NULL );
    virtual ~SupFile();
    
    virtual void NewContent(); // Create content for a new file
	virtual void OpenContent( ); // Open content after archive extraction
	virtual void SaveContent( ); // Save content before archive creation
	virtual void CloseContent( ); // Desactivate content before deletion
	
	// functor
	bool Superimpose( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool DetectPoints( wxArrayPtrVoid params, AxProgressDlg *dlg );
	bool RegisterImages( wxArrayPtrVoid params, AxProgressDlg *dlg ); 
	// getters
	void GetSrc1( AxImage *image );
	void GetSrc2( AxImage *image );
	void GetResult( AxImage *image );
	
	// status
	bool IsSuperimposed() { return m_isSuperimposed; }
	
	// status
	static bool IsSuperimposed( wxString filename );
	
private:
    
public:
    SupEnv *m_envPtr;
	ImRegister *m_imRegisterPtr;
	wxString m_original1, m_original2;
	bool m_hasNewPoints1, m_hasNewPoints2;
	wxPoint m_points1[4];
    wxPoint m_points2[4];
	bool m_hasPoints1;
	bool m_hasPoints2;

protected:
    bool m_isSuperimposed;
	//AxImage 
   
private:
    
};

#endif //AX_SUPERIMPOSITION

#endif
