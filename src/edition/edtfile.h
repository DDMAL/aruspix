/////////////////////////////////////////////////////////////////////////////
// Name:        edtfile.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __edtfile_H__
#define __edtfile_H__

#ifdef AX_EDT

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "edtfile.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axfile.h"

class EdtEnv;
class MusFile;

// WDR: class declarations


//----------------------------------------------------------------------------
// EdtFile
//----------------------------------------------------------------------------

class EdtFile: public AxFile 
{
public:
    // constructors and destructors
    EdtFile::EdtFile( wxString name, EdtEnv *env = NULL );
    virtual EdtFile::~EdtFile();
    
    // WDR: method declarations for EdtFile
	virtual void EdtFile::NewContent(); // Create content for a new file
	virtual void EdtFile::OpenContent( ); // Open content after archive extraction
	virtual void EdtFile::SaveContent( ); // Save content before archive creation
	virtual void EdtFile::CloseContent( ); // Desactivate content before deletion
	
private:
    
public:
    // WDR: member variable declarations for EdtFile
	EdtEnv *m_envPtr;
    MusFile *m_musFilePtr;

protected:
    // WDR: member variable declarations for EdtFile
   
private:
    // WDR: handler declarations for EdtFile

};

#endif //AX_EDT

#endif
