/////////////////////////////////////////////////////////////////////////////
// Name:        musdoc.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_DOC_H__
#define __MUS_DOC_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

class MusPageFunctor;
class MeiDocument;
class MusDiv;

#include "musobject.h"
#include "musdef.h"
#include "musio.h"
#include "muswwg.h"

class MusFileOutputStream;


//----------------------------------------------------------------------------
// MusDoc
//----------------------------------------------------------------------------

/** 
 * This class is a hold the data and corresponds to the model of a MVC design pattern.
 */
class MusDoc
{
	//friend class MusFileOutputStream;
	friend class MusFileInputStream;

public:
    // constructors and destructors
    MusDoc();
    virtual ~MusDoc();
	
	void AddDiv( MusDiv *div );
	
	void AddLayout( MusLayout *layout );
    
    /*
     * Clear the content of the document.
     */ 
    void Reset();
    
    /**
     * Check the validity of the document.
     * Verify that all element have a valid pointer to their parent.
     * This is more of a debugging method.
     */ 
    void Check();
    
    bool Save( MusFileOutputStream *output );
    
    bool Load( MusFileInputStream *input );
    
    // moulinette
    void GetNumberOfVoices( int *min_voice, int *max_voice );
    MusLaidOutStaff *GetVoice( int i );

    MeiDocument *GetMeiDocument();
    void SetMeiDocument(MeiDocument *doc);
    
    MusObject *FindLogicalObject( MusFunctor *functor, uuid_t uuid );
    
    // moulinette
    virtual void ProcessLayout(MusFunctor *functor, wxArrayPtrVoid params );
    virtual void ProcessLogical(MusFunctor *functor, wxArrayPtrVoid params );
    // functors    
    
    // Some statis utiliry functions
    static wxString GetAxVersion();
    static wxString GetResourcesPath();
    static void SetResourcesPath(wxString p) {m_respath = p;}
    static wxString GetMusicFontDescStr();
    static wxString GetNeumeFontDescStr();
    static wxString GetLyricFontDescStr();
    static int GetFontPosCorrection();
    static wxString GetFileVersion(int vmaj, int vmin, int vrev);
    
private:
    static wxString m_respath;
    
public:
    /** nom complet du fichier */
    wxString m_fname;
    
    /** The layouts */
    ArrayOfMusLayouts m_layouts;
    ArrayOfMusDivs m_divs;
    
    MusEnv m_env;
	
    /** Data loaded from the Wolfgang files but unused **/
	MusWWGData m_wwgData;
    


private:
    MeiDocument *m_meidoc;
	
};


#endif
