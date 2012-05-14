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


//----------------------------------------------------------------------------
// MusDoc
//----------------------------------------------------------------------------

MusDoc::MusDoc()
{
    Reset();
    m_meidoc = NULL;
}

MusDoc::~MusDoc()
{
}

void MusDoc::Reset()
{
    m_layouts.Clear();
    m_divs.Clear();
    // what about m_meidoc ?
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

bool MusDoc::Load( MusFileInputStream *input )
{
    Reset();
    input->ReadDoc( this );
    
    wxArrayPtrVoid params;
	params.Add( input );
    
    // logical
    MusDiv *div;
    while( (div = input->ReadDiv()) ) {
        div->Load( params );
        this->AddDiv( div );
    }
    
    // layout
    MusLayout *layout;
    while( (layout = input->ReadLayout()) ) {
        layout->Load( params );
        this->AddLayout( layout );
    }   
    
    return true;
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

MeiDocument *MusDoc::GetMeiDocument() {
    return m_meidoc;
}

void MusDoc::SetMeiDocument(MeiDocument *doc) {
    m_meidoc = doc;
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
        if (divFunctor) { // is is a MusSystemFunctor, call it
            divFunctor->Call( div, params );
        }
        else { // process it further
            div->Process( functor, params );
        }
	}
}
