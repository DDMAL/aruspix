/////////////////////////////////////////////////////////////////////////////
// Name:        wgfile.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_WG

#ifdef __GNUG__
    #pragma implementation "wgfile.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "wgfile.h"

#include "wgpage.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfWgPages );

WX_DEFINE_OBJARRAY( ArrayOfWgFonts );


char *WgFile::sep = "#ώτυώ#";

// WDR: class implementations

//----------------------------------------------------------------------------
// WgFile
//----------------------------------------------------------------------------

WgFile::WgFile()
{
    m_pages.Clear();
	m_masqueFixe.Clear();
	m_masqueVariable.Clear();

	m_fonts.Clear();
	WgFont font;

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

WgFile::~WgFile()
{
}

void WgFile::CheckIntegrity()
{
	this->m_fheader.nbpage = (int)this->m_pages.GetCount();

	WgPage *page;
	int i;
    for (i = 0; i < m_fheader.nbpage; i++) 
	{
		page = &m_pages[i];
		page->npage = i + 1;
		page->CheckIntegrity();
	}
}


// WDR: handler implementations for WgFile


//----------------------------------------------------------------------------
// WgFileOutputStream
//----------------------------------------------------------------------------

WgFileOutputStream::WgFileOutputStream( WgFile *file, wxString filename ) :
	wxFileOutputStream( filename )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_file = file;
}

WgFileOutputStream::WgFileOutputStream( WgFile *file, int fd ) :
	wxFileOutputStream( fd )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_file = file;
}

/*
WgFileOutputStream::WgFileOutputStream( WgFile *file, wxFile *wxfile ) :
	wxFileOutputStream( *wxfile )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_file = file;
}
*/

WgFileOutputStream::~WgFileOutputStream()
{
}

// WDR: handler implementations for WgFileOutputStream


//----------------------------------------------------------------------------
// WgFileInputStream
//----------------------------------------------------------------------------

WgFileInputStream::WgFileInputStream( WgFile *file, wxString filename  ) :
	wxFileInputStream( filename )
{
	//wxASSERT_MSG( file, "File cannot be NULL" );
	m_file = file;
}

WgFileInputStream::~WgFileInputStream()
{
}

// WDR: handler implementations for WgFileInputStream



#endif // AX_WG
