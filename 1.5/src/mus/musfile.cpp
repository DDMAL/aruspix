/////////////////////////////////////////////////////////////////////////////
// Name:        musfile.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musfile.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "musfile.h"

#include "muspage.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusPages );

char *MusFile::sep = "#ώτυώ#";

// WDR: class implementations

//----------------------------------------------------------------------------
// MusFile
//----------------------------------------------------------------------------

MusFile::MusFile()
{
    m_pages.Clear();
}

MusFile::~MusFile()
{
}

int MusFile::GetNumberOfVoices()
{
	wxArrayPtrVoid params; // tableau de pointeurs pour parametres
    
    int voice_number = 0;

	params.Add( &voice_number );
    MusPageFunctor countVoices( &MusPage::CountVoices );
    this->Process( &countVoices, params );
    return voice_number + 1;
}
    
    
MusStaff *MusFile::GetVoice( int i )
{
	wxArrayPtrVoid params; // tableau de pointeurs pour parametres
    
    MusStaffFunctor copyElements( &MusStaff::CopyElements );
    wxArrayPtrVoid staffParams; // idem for staff functor
    MusStaff *staff = new MusStaff();
    staffParams.Add( staff );
    
    params.Add(&copyElements);
    params.Add(&staffParams);
    params.Add(&i);

    MusPageFunctor processVoices( &MusPage::ProcessVoices );
    this->Process( &processVoices, params );
    return staff;
}

void MusFile::CheckIntegrity()
{
	this->m_fheader.nbpage = (int)this->m_pages.GetCount();

	MusPage *page;
	int i;
    for (i = 0; i < m_fheader.nbpage; i++) 
	{
		page = &m_pages[i];
		page->npage = i + 1;
		page->CheckIntegrity();
	}
}

void MusFile::Process(MusPageFunctor *functor, wxArrayPtrVoid params )
{
    MusPage *page;
	int i;
    for (i = 0; i < m_fheader.nbpage; i++) 
	{
		page = &m_pages[i];
        functor->Call( page, params );
	}
}

// WDR: handler implementations for MusFile


//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

MusFileOutputStream::MusFileOutputStream( MusFile *file, wxString filename ) :
	wxFileOutputStream( filename )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_file = file;
}

MusFileOutputStream::MusFileOutputStream( MusFile *file, int fd ) :
	wxFileOutputStream( fd )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_file = file;
}

/*
MusFileOutputStream::MusFileOutputStream( MusFile *file, wxFile *wxfile ) :
	wxFileOutputStream( *wxfile )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_file = file;
}
*/

MusFileOutputStream::~MusFileOutputStream()
{
}

// WDR: handler implementations for MusFileOutputStream


//----------------------------------------------------------------------------
// MusFileInputStream
//----------------------------------------------------------------------------

MusFileInputStream::MusFileInputStream( MusFile *file, wxString filename  ) :
	wxFileInputStream( filename )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_file = file;
}

MusFileInputStream::~MusFileInputStream()
{
}

// WDR: handler implementations for MusFileInputStream




