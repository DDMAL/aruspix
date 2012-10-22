/////////////////////////////////////////////////////////////////////////////
// Name:        musdoc.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musdoc.h"
#include "musdiv.h"

#include "muslayout.h"
#include "muspage.h"
#include "mussystem.h"
#include "muslaidoutlayer.h"
#include "muslaidoutlayerelement.h"


#ifndef HEADLESS
#include "app/axapp.h"
#endif

//----------------------------------------------------------------------------
// MusDoc
//----------------------------------------------------------------------------

// Initialize static respource path
wxString MusDoc::m_respath = "/usr/local/share/aruspix";

MusDoc::MusDoc()
{
    Reset();
}

MusDoc::~MusDoc()
{
    // We do this because we want the layout to be deleted first.
    // This avoid all MusLayerElement to be searched in the layouts
    // when deleted in the logical tree.
    this->Reset();
}

void MusDoc::Reset()
{
    m_pageWidth = 2100;
    m_pageHeight = 2970;
    m_pageRightMar = 0;
    m_pageLeftMar = 0;
    m_pageTopMar = 0; 
    m_layouts.Clear();
    m_divs.Clear();
}

void MusDoc::AddDiv( MusDiv *div )
{
	div->SetDoc( this );
	m_divs.Add( div );
}


void MusDoc::AddLayout( MusLayout *layout )
{
	layout->SetDoc( this );
	m_layouts.Add( layout );
}

bool MusDoc::Save( MusFileOutputStream *output )
{
    output->WriteDoc( this );
    
    wxArrayPtrVoid params;
	params.Add( output );
    
    // logical
    MusDivFunctor div( &MusDiv::Save );
    this->ProcessLogical( &div, params );
    
    // layout
    MusLayoutFunctor layout( &MusLayout::Save );
    this->ProcessLayout( &layout, params );    
    
    return true;
}

void MusDoc::Check()
{
    wxArrayPtrVoid params;
    MusFunctor checkObjects( &MusObject::CheckFunctor );
    this->ProcessLogical( &checkObjects, params );
    this->ProcessLayout( &checkObjects, params );
}


void MusDoc::ResetAndCheckLayouts()
{
    wxArrayPtrVoid params;
    params.Add( this );
    MusLaidOutLayerElementFunctor checkLaidOutLayerElements( &MusLaidOutLayerElement::CheckAndResetLayerElement );
    // because we are going to delete MusLaidOutLayerElements, we need to process it from the end
    checkLaidOutLayerElements.m_reverse = true;
    this->ProcessLayout( &checkLaidOutLayerElements, params );
    
    MusLaidOutLayerFunctor checkLaidOutLayers( &MusLaidOutLayer::CheckAndResetSectionOrMeasure );
    // because we are going to delete MusLaidOutLayers, we need to process it from the end
    checkLaidOutLayers.m_reverse = true;
    this->ProcessLayout( &checkLaidOutLayers, params );
}

void MusDoc::GetNumberOfVoices( int *min_voice, int *max_voice )
{
	wxArrayPtrVoid params; // tableau de pointeurs pour parametres

    (*max_voice) = 0;
    (*min_voice) = 100000; // never more than 10000 voices ?

	params.Add( min_voice );
    params.Add( max_voice );
    MusPageFunctor countVoices( &MusPage::CountVoices );
    //this->Process( &countVoices, params ); // ax2
}
        
    
MusLaidOutStaff *MusDoc::GetVoice( int i )
{
    /*
	wxArrayPtrVoid params; // tableau de pointeurs pour parametres
    
    MusLaidOutLayerFunctor copyElements( &MusLaidOutLayer::CopyElements );
    wxArrayPtrVoid staffParams; // idem for staff functor
    MusLaidOutStaff *staff = new MusLaidOutStaff();
    staffParams.Add( staff );
    
    params.Add(&copyElements);
    params.Add(&staffParams);
    params.Add(&i);

    MusPageFunctor processVoices( &MusPage::ProcessVoices );
    this->Process( &processVoices, params ); 
    return staff;
    */ // ax2
    return NULL;
}

MusObject *MusDoc::FindLogicalObject( MusFunctor *functor, uuid_t uuid )
{
    if ( uuid_is_null( uuid ) ) {
        return NULL;
    }
    
    MusObject *element = NULL;
    wxArrayPtrVoid params;
	params.Add( uuid );
    params.Add( &element );
    this->ProcessLogical( functor, params );
    
    if (!element) {
        // RZ uuid_string_t does not exist on freebsd   
        //but typedef       char    __darwin_uuid_string_t[37];
        //and typedef __darwin_uuid_string_t        uuid_string_t;
        char uuidStr[37]; // bad fix
        uuid_unparse( uuid, uuidStr ); 
        //wxLogDebug( "%s not found in the logical tree", uuidStr );
    }
    return element;
    
}

// functors for MusLayout

void MusDoc::ProcessLayout(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusLayoutFunctor *layoutFunctor = dynamic_cast<MusLayoutFunctor*>(functor);
    MusLayout *layout;
	int i;
    for (i = 0; i < (int)m_layouts.GetCount(); i++) 
	{
        layout = &m_layouts[i];
        functor->Call( layout, params );
        if (layoutFunctor) { // is is a MusSystemFunctor, call it
            layoutFunctor->Call( layout, params );
        }
        else { // process it further
            layout->Process( functor, params );
        }
	}
}

void MusDoc::ProcessLogical(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusDivFunctor *divFunctor = dynamic_cast<MusDivFunctor*>(functor);
    MusDiv *div;
	int i;
    for (i = 0; i < (int)m_divs.GetCount(); i++) 
	{
        div = &m_divs[i];
        functor->Call( div, params );
        if (divFunctor) { // is is a MusSystemFunctor, call it
            divFunctor->Call( div, params );
        }
        else { // process it further
            div->Process( functor, params );
        }
	}
}

wxString MusDoc::GetAxVersion() {
#ifdef HEADLESS
    return wxString("command line"); // we need to add versioning
#else
    return AxApp::s_version;
#endif
}

wxString MusDoc::GetResourcesPath() {
#ifdef HEADLESS
    //hardcode galore
    return m_respath;
#else
    return wxGetApp().m_resourcesPath;
#endif
}    
    
wxString MusDoc::GetMusicFontDescStr() {
#ifdef HEADLESS
    return wxString("0;13;70;90;90;0;Leipzig 4.7;33");
#else
    return wxGetApp().m_musicFontDesc;
#endif
}
    
wxString MusDoc::GetNeumeFontDescStr() {
#ifdef HEADLESS
    return wxString("0;53;70;90;90;0;Festa Dies A;0");
#else
    return wxGetApp().m_neumeFontDesc;
#endif
}
        
wxString MusDoc::GetLyricFontDescStr() {
#ifdef HEADLESS
    return wxString("0;12;70;93;90;0;Garamond;0");
#else
    return wxGetApp().m_lyricFontDesc;
#endif
}


int MusDoc::GetFontPosCorrection(){
#ifdef HEADLESS
    return 0;
#else
    return wxGetApp().m_fontPosCorrection;
#endif
}

wxString MusDoc::GetFileVersion(int vmaj, int vmin, int vrev) {
    return wxString::Format("%04d.%04d.%04d", vmaj, vmin, vrev );
}
