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

/*
void MusDoc::Process(MusPageFunctor *functor, wxArrayPtrVoid params )
{
    
    MusPage *page;
	int i;
    for (i = 0; i < m_parameters.nbpage; i++) 
	{
		page = &m_pages[i];
        functor->Call( page, params );
	}
    
    wxLogDebug( "MusDoc::Process missing in ax2");
}
*/
// ax2

MeiDocument *MusDoc::GetMeiDocument() {
    return m_meidoc;
}

void MusDoc::SetMeiDocument(MeiDocument *doc) {
    m_meidoc = doc;
}


//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

MusFileOutputStream::MusFileOutputStream( MusDoc *doc, wxString filename ) :
	wxFileOutputStream( filename )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_doc = doc;
}

MusFileOutputStream::MusFileOutputStream( MusDoc *doc, int fd ) :
	wxFileOutputStream( fd )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_doc = doc;
}

/*
MusFileOutputStream::MusFileOutputStream( MusDoc *doc, wxFile *wxfile ) :
	wxFileOutputStream( *wxfile )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_doc = doc;
}
*/

MusFileOutputStream::~MusFileOutputStream()
{
}



//----------------------------------------------------------------------------
// MusFileInputStream
//----------------------------------------------------------------------------

MusFileInputStream::MusFileInputStream( MusDoc *doc, wxString filename  ) :
	wxFileInputStream( filename )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_doc = doc;
}

MusFileInputStream::MusFileInputStream( MusDoc *doc, int fd ) :
	wxFileInputStream( fd )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_doc = doc;
}

MusFileInputStream::~MusFileInputStream()
{
}





