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
WX_DEFINE_OBJARRAY( ArrayOfWgPages );

WX_DEFINE_OBJARRAY( ArrayOfWgFonts );


char *MusFile::sep = "#ώτυώ#";

// WDR: class implementations

//----------------------------------------------------------------------------
// MusFile
//----------------------------------------------------------------------------

MusFile::MusFile()
{
    m_pages.Clear();
	m_masqueFixe.Clear();
	m_masqueVariable.Clear();

	m_fonts.Clear();
	MusFont font;

	font.fonteJeu = 1;
	font.fonteNom = "Leipzig 4.2";
	m_fonts.Add( font );

	int i;
    for (i = 0; i < MAXPOLICES - 1; i++) // chargement des polices vides
	{
		font.fonteJeu = 0;
		font.fonteNom = "";
		m_fonts.Add( font );
	}
}

MusFile::~MusFile()
{
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




