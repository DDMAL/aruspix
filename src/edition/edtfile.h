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



//----------------------------------------------------------------------------
// EdtFile
//----------------------------------------------------------------------------

class EdtFile: public AxFile 
{
public:
    // constructors and destructors
    EdtFile( wxString name, EdtEnv *env = NULL );
    virtual ~EdtFile();
    
    virtual void NewContent(); // Create content for a new file
	virtual void OpenContent( ); // Open content after archive extraction
	virtual void SaveContent( ); // Save content before archive creation
	virtual void CloseContent( ); // Desactivate content before deletion
	
private:
    
public:
    EdtEnv *m_envPtr;
    MusFile *m_musFilePtr;

protected:
       
private:
    
};

#endif //AX_EDT

#endif
