/////////////////////////////////////////////////////////////////////////////
// Name:        musiobin.cpp
// Author:      Laurent Pugin
// Created:     2008
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musiobin.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"

#include "musiobin.h"

#include "app/axapp.h"
#include "app/axfile.h"

//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

MusBinOutput::MusBinOutput( MusFile *file, wxString filename, int flag ) :
	MusFileOutputStream( file, filename )
{
	m_flag = flag;
	m_filename = filename;
}

MusBinOutput::~MusBinOutput()
{
}

bool MusBinOutput::ExportFile( )
{
	int i;

	if ( !IsOk() )
	{
		wxLogMessage(_("Cannot write file '%s'"), m_filename.c_str() );
		return false;
	}

    WriteFileHeader( &m_file->m_fheader ); // fileheader

	MusPage *page = NULL;
    for (i = 0; i < m_file->m_fheader.nbpage; i++ )
	{
		page = &m_file->m_pages.Item(i);
		WritePage( page );
    }
    if (!WriteSeparator() ) 
		return false;
	if ( m_file->m_fheader.param.entetePied & PAGINATION )
		WritePagination( &m_file->m_pagination );
	if ( m_file->m_fheader.param.entetePied & ENTETE )
		WriteHeaderFooter( &m_file->m_header );
	if ( m_file->m_fheader.param.entetePied & PIEDDEPAGE )
		WriteHeaderFooter( &m_file->m_footer );

	//wxMessageBox("stop");
	//wxLogMessage("OK %d", m_file->m_pages.GetCount() );

	return true;
}

bool MusBinOutput::WriteFileHeader( const MusFileHeader *header )
{
	int32 = wxINT32_SWAP_ON_BE( m_flag );
	Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_BE( AxApp::s_version_major );
	Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_BE( AxApp::s_version_minor );
	Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_BE( AxApp::s_version_revision );
	Write( &int32, 4 );
	uint16 = wxUINT16_SWAP_ON_BE( header->nbpage ); // nbpage
	Write( &uint16, 2 ); 
	uint16 = wxUINT16_SWAP_ON_BE( header->nopage ); // nopage
	Write( &uint16, 2 ); 
	uint16 = wxUINT16_SWAP_ON_BE( header->noligne ); // noligne
	Write( &uint16, 2 ); 
	uint32 = wxUINT32_SWAP_ON_BE( header->xpos ); // xpso
	Write( &uint32, 4 );
	Write( &header->param.orientation, 1 ); // param - orientation
	Write( &header->param.EpLignesPortee, 1 ); // param - epLignesPortee
	Write( &header->param.EpQueueNote, 1 ); // param - epQueueNotes
	Write( &header->param.EpBarreMesure, 1 ); // param - epBarreMesure
	Write( &header->param.EpBarreValeur, 1 ); // param - epBarreValeur
	Write( &header->param.EpBlancBarreValeur, 1 ); // param - epBlancBarreValeur
	int32 = wxINT32_SWAP_ON_BE( header->param.pageFormatHor ); // param - pageFormatHor
	Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_BE( header->param.pageFormatVer ); // param - pageFormatVer
	Write( &int32, 4 );
	int16 = wxINT16_SWAP_ON_BE( header->param.MargeSOMMET ); // param - margeSommet
	Write( &int16, 2 );
	int16 = wxINT16_SWAP_ON_BE( header->param.MargeGAUCHEIMPAIRE ); // param - margeGaucheImpaire
	Write( &int16, 2 );
	int16 = wxINT16_SWAP_ON_BE( header->param.MargeGAUCHEPAIRE ); // param - margeGauchePaire
	Write( &int16, 2 );
    
	Write( &header->param.rapportPorteesNum, 1 ); // rpPorteesNum
	Write( &header->param.rapportPorteesDen, 1 ); // rpPorteesDen
	Write( &header->param.rapportDiminNum, 1 ); // rpDiminNum
	Write( &header->param.rapportDiminDen, 1 ); // rpDiminDen
	Write( &header->param.hampesCorr, 1 ); // hampesCorr
    
	int32 = wxINT32_SWAP_ON_BE( header->param.notationMode ); // param - pageFormatVer
	Write( &int32, 4 );    
    
	return true;
}

bool MusBinOutput::WriteSeparator( )
{
	Write( MusFile::sep, 7 );
	return true;
}

bool MusBinOutput::WritePage( const MusPage *page )
{
	int j;

    if ( !WriteSeparator() )
		return false;
	int32 = wxINT32_SWAP_ON_BE( page->npage );
	Write( &int32, 4 );
	int16 = wxINT16_SWAP_ON_BE( page->nbrePortees );
	Write( &int16, 2 );
    Write( &page->noMasqueFixe, 1 );    
	Write( &page->noMasqueVar, 1 );
	Write( &page->reserve, 1 );
	Write( &page->defin, 1 );
	int32 = wxINT32_SWAP_ON_BE( page->indent );
	Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_BE( page->indentDroite );
	Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_BE( page->lrg_lign );
	Write( &int32, 4 );
	MusStaff *staff = NULL;
    for (j = 0; j < page->nbrePortees; j++) 
	{
		staff = &page->m_staves[j];
		WriteStaff( staff );
    }

	return true;

}
bool MusBinOutput::WriteStaff( const MusStaff *staff )
{
	unsigned int k;

	uint32 = wxUINT32_SWAP_ON_BE( staff->nblement );
	Write( &uint32, 4 );
	uint16 = wxUINT16_SWAP_ON_BE( staff->voix );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( staff->noGrp );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( staff->totGrp );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( staff->noLigne );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( staff->no );
	Write( &uint16, 2 );
	Write( &staff->armTyp, 1 );
	Write( &staff->armNbr, 1 );
	Write( &staff->notAnc, 1 );
	Write( &staff->grise, 1 );
	Write( &staff->invisible, 1 );
	uint16 = wxUINT16_SWAP_ON_BE( staff->ecart );
	Write( &uint16, 2 );
	Write( &staff->vertBarre, 1 );
	Write( &staff->brace, 1 );
	Write( &staff->pTaille, 1 );
    int32 = wxINT32_SWAP_ON_BE( staff->indent );
    Write( &int32, 4 );
	Write( &staff->indentDroite, 1 );
	Write( &staff->portNbLine, 1 );
	Write( &staff->accol, 1 );
	Write( &staff->accessoire, 1 );
	uint16 = wxUINT16_SWAP_ON_BE( staff->reserve );
	Write( &uint16, 2 );
	for (k = 0;k < staff->nblement ; k++ )
	{
		if ( staff->m_elements[k].IsNote() )
		{
			WriteNote( (MusNote*)&staff->m_elements[k] );
		}
		else if ( staff->m_elements[k].IsSymbol() )
		{
			WriteSymbol( (MusSymbol*)&staff->m_elements[k] );
		}
        else if ( staff->m_elements[k].IsNeume() )
		{
			WriteNeume( (MusNeume*)&staff->m_elements[k] );
		}
		if ( m_flag == MUS_BIN_ARUSPIX_CMP )
		{		
			MusElement *elem = &staff->m_elements[k];
			int32 = wxINT32_SWAP_ON_BE( (int)elem->m_im_filename.Length() );
			Write( &int32, 4 );
			Write( elem->m_im_filename.c_str(), (int)elem->m_im_filename.Length() + 1 );
			int32 = wxINT32_SWAP_ON_BE( elem->m_im_staff );
			Write( &int32, 4 );
			int32 = wxINT32_SWAP_ON_BE( elem->m_im_pos );
			Write( &int32, 4 );
			int32 = wxINT32_SWAP_ON_BE( elem->m_cmp_flag );
			Write( &int32, 4 );
		}
	}

	return true;
}

bool MusBinOutput::WriteSymbol( const MusSymbol *symbol )
{
	Write( &symbol->TYPE, 1 );
	WriteElementAttr( symbol );
	Write( &symbol->flag , 1 );
	Write( &symbol->calte , 1 );
	Write( &symbol->carStyle , 1 );
	Write( &symbol->carOrient , 1 );
	Write( &symbol->fonte , 1 );
	Write( &symbol->s_lie_l , 1 );
	Write( &symbol->point , 1 );
	uint16 = wxUINT16_SWAP_ON_BE( symbol->code );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( symbol->l_ptch );
	Write( &uint16, 2 );	
	int32 = wxINT32_SWAP_ON_BE( symbol->dec_y );
	Write( &int32, 4 );
    // if ( symbol->IsLyric() ) // To be fixed ??
    if ( (symbol->flag == CHAINE) && (symbol->fonte == LYRIC) )
		WriteLyric( symbol );
	
	return true;
}

bool MusBinOutput::WriteNote( const MusNote *note )
{
	int i;
	
	Write( &note->TYPE, 1 );
	WriteElementAttr( note );
	Write( &note->sil, 1 ); 
	Write( &note->val, 1 );
	Write( &note->inv_val, 1 );
	Write( &note->point, 1 );
	Write( &note->stop_rel, 1 );
	Write( &note->acc, 1 );
	Write( &note->accInvis, 1 );
	Write( &note->q_auto, 1 );
	Write( &note->queue, 1 );
	Write( &note->stacc, 1 );
	Write( &note->oblique, 1 );
	Write( &note->queue_lig, 1 );
	Write( &note->chord, 1 );
	Write( &note->fchord, 1 );
	Write( &note->lat, 1 );
	Write( &note->haste, 1 );
	//Write( &note->code, sizeof( note->code ) );
	unsigned char code = (unsigned char)note->code;
	Write( &code, 1 ); // deplace dans element, mais unsigned short
	Write( &note->tetenot, 1 );
	Write( &note->typStac, 1 );
	
	// lyric
	char count = 0;
	for ( i = 0; i < (int)note->m_lyrics.GetCount(); i++ ){
		MusSymbol *lyric = &note->m_lyrics[i];
		if ( lyric )
			count++;
	}
	Write( &count, 1 );
	
	for ( i = 0; i < (int)note->m_lyrics.GetCount(); i++ ){
		MusSymbol *lyric = &note->m_lyrics[i];
		if ( lyric )
			WriteSymbol( lyric );
	}
	
	return true;
}

bool MusBinOutput::WriteNeume( const MusNeume *neume )
{
	// Write member values
    // ...
	Write( &neume->TYPE, 1 );
	WriteElementAttr( neume );
	Write( &neume->closed, 1 );
	Write( &neume->code, 1);
	
	Write( &neume->xrel_right, 1);
	
	// for mei (eventually) (these fields are all char)
	Write( &neume->name, 1);
	Write( &neume->n_type, 1);
	Write( &neume->form, 1);
    // if ( symbol->IsLyric() ) // no lyrics in MusNeume yet
	Write( &neume->p_max, 1);
	Write( &neume->p_min, 1);
	Write( &neume->p_range, 1);
	unsigned char code = (unsigned char)neume->code;
	Write( &code, 1 ); // placed in element, but as unsigned short
	
	Write( &neume->n_selected, 1);
	
	//provided neumes never exceed 255 elements
	unsigned char size = (unsigned char) neume->n_pitches.size();
	Write( &size, 1);
	
	/** MusNeumePitch list (n_pitches) **/
	// XXX: Write neumes properly
	MusNeumePitch *temp;
	/*
	for (unsigned int i = 0; i < neume->n_pitches.size(); i++)
	{ 
		temp = neume->n_pitches.at(i);
		WriteElementAttr( temp );
		Write( &temp->GetValue(), 1);
		unsigned char size = (unsigned char) temp->GetFestaString().size() + 1; // Laurent: added the 0
		Write( &size, 1);
		Write( temp->GetFestaString(), size );
	}
	 */
	
	return true;
}

bool MusBinOutput::WriteElementAttr( const MusElement *element )
{
	Write( &element->liaison , 1 );
	Write( &element->dliai , 1 );
	Write( &element->fliai , 1 );
	Write( &element->lie_up , 1 );
	Write( &element->rel , 1 );
	Write( &element->drel , 1 );
	Write( &element->frel , 1 );
	Write( &element->oct , 1 );
	Write( &element->dimin , 1 );
	Write( &element->grp , 1 );
	Write( &element->_shport , 1 );
	Write( &element->ligat , 1 );
	Write( &element->ElemInvisible , 1 );
	Write( &element->pointInvisible , 1 );
	Write( &element->existDebord , 1 );
	Write( &element->fligat , 1 );
	Write( &element->notschowgrp , 1 );
	Write( &element->cone , 1 );
	Write( &element->liaisonPointil , 1 );
	Write( &element->reserve1 , 1 );
	Write( &element->reserve2 , 1 );
	Write( &element->ottava , 1 );
	uint16 = wxUINT16_SWAP_ON_BE( element->durNum );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( element->durDen );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( element->offset );
	Write( &uint16, 2 );
	int32 = wxINT32_SWAP_ON_BE( element->xrel );
	Write( &int32, 4 );
	
	return true;
}

bool MusBinOutput::WriteLyric( const MusElement *element )
{
	int32 = wxINT32_SWAP_ON_BE( (int)element->m_debord_str.Length() );
	Write( &int32, 4 );
	
	Write( element->m_debord_str.c_str(), (int)element->m_debord_str.Length() + 1 );
	return true;
}

bool MusBinOutput::WritePagination( const MusPagination *pagination )
{
	int16 = wxINT16_SWAP_ON_BE( pagination->numeroInitial );
	Write( &int16, 2 );
	Write( &pagination->aussiPremierPage, 1 );
	Write( &pagination->position, 1 );
	Write( &pagination->numeroFonte, 1 );
	Write( &pagination->carStyle, 1 );
	Write( &pagination->taille, 1 );
	Write( &pagination->offsetDuBord, 1 );
	return true;
}

bool MusBinOutput::WriteHeaderFooter( const MusHeaderFooter *headerfooter)
{
	char buffer[HEADER_FOOTER_TEXT + 1];
	memset( buffer, 0, HEADER_FOOTER_TEXT + 1);
	if ( headerfooter->texte.Length() > HEADER_FOOTER_TEXT + 1 )
		return false;
	memcpy( buffer, headerfooter->texte.c_str(), headerfooter->texte.Length() );
	Write( buffer, HEADER_FOOTER_TEXT );
	Write( &headerfooter->aussiPremierPage, 1 );
	Write( &headerfooter->position, 1 );
	Write( &headerfooter->numeroFonte, 1 );
	Write( &headerfooter->carStyle, 1 );
	Write( &headerfooter->taille, 1 );
	Write( &headerfooter->offsetDuBord, 1 );
	return true;
}





//----------------------------------------------------------------------------
// MusBinInput
//----------------------------------------------------------------------------

MusBinInput::MusBinInput( MusFile *file, wxString filename, int flag ) :
	MusFileInputStream( file, filename )
{
	m_flag = flag;
	m_vmaj = m_vmin = m_vrev = 10000; // arbitrary version, we assume we will never reach version 10000...
}

MusBinInput::~MusBinInput()
{
}

bool MusBinInput::ImportFile( )
{
	int i;

	if ( !IsOk() )
	{
		wxLogMessage(_("Cannot read file '%s'"), m_file->m_fname.c_str() );
		return false;
	}

    ReadFileHeader( &m_file->m_fheader ); // fileheader
    
	m_file->m_pages.Clear();		
    for (i = 0; i < m_file->m_fheader.nbpage; i++ )
	{
		MusPage *page = new MusPage();
		ReadPage( page );
		m_file->m_pages.Add( page );
    }
    if ( !ReadSeparator() ) 
		return false;
	if ( m_file->m_fheader.param.entetePied & PAGINATION )
		ReadPagination( &m_file->m_pagination );
	if ( m_file->m_fheader.param.entetePied & ENTETE )
		ReadHeaderFooter( &m_file->m_header );
	if ( m_file->m_fheader.param.entetePied & PIEDDEPAGE )
		ReadHeaderFooter( &m_file->m_footer );

	//wxLogMessage("OK %d", m_file->m_pages.GetCount() );
    //m_file->CheckIntegrity();

	return true;
}

bool MusBinInput::ReadFileHeader( MusFileHeader *header )
{
	Read( &int32, 4 ); 
	m_flag = wxINT32_SWAP_ON_BE( int32 ); 
	Read( &int32, 4 ); 
	m_vmaj = wxINT32_SWAP_ON_BE( int32 ); 
    Read( &int32, 4 ); 
	m_vmin = wxINT32_SWAP_ON_BE( int32 ); 
    Read( &int32, 4 ); 
	m_vrev = wxINT32_SWAP_ON_BE( int32 ); 
	Read( &uint16, 2 ); 
	header->nbpage = wxUINT16_SWAP_ON_BE( uint16 ); // nbpage
	Read( &uint16, 2 ); 
	header->nopage = wxUINT16_SWAP_ON_BE( uint16 ); // nopage
	Read( &uint16, 2 ); 
	header->noligne = wxUINT16_SWAP_ON_BE( uint16 ); // noligne
	Read( &uint32, 4 );
	header->xpos = wxUINT32_SWAP_ON_BE( uint32 );  // xpso
	Read( &header->param.orientation, 1 ); // param - orientation
	Read( &header->param.EpLignesPortee, 1 ); // param - epLignesPortee
    header->param.EpLignesPortee = 1;	
	Read( &header->param.EpQueueNote, 1 ); // param - epQueueNotes
    header->param.EpQueueNote = 2;
	Read( &header->param.EpBarreMesure, 1 ); // param - epBarreMesure
	Read( &header->param.EpBarreValeur, 1 ); // param - epBarreValeur
	Read( &header->param.EpBlancBarreValeur, 1 ); // param - epBlancBarreValeur
	Read( &int32, 4 );
	header->param.pageFormatHor = wxINT32_SWAP_ON_BE( int32 ); // param - pageFormatHor
	Read( &int32, 4 );
	header->param.pageFormatVer = wxINT32_SWAP_ON_BE( int32 ); // param - pageFormatVer
	Read( &int16, 2 );
	header->param.MargeSOMMET = wxINT16_SWAP_ON_BE( int16 ); // param - margeSommet
	Read( &int16, 2 );
	header->param.MargeGAUCHEIMPAIRE = wxINT16_SWAP_ON_BE( int16 ); // param - margeGaucheImpaire
	Read( &int16, 2 );
	header->param.MargeGAUCHEPAIRE = wxINT16_SWAP_ON_BE( int16 ); // param - margeGauchePaire
    
	Read( &header->param.rapportPorteesNum, 1 ); // rpPorteesNum
	Read( &header->param.rapportPorteesDen, 1 ); // rpPorteesDen
	Read( &header->param.rapportDiminNum, 1 ); // rpDiminNum
	Read( &header->param.rapportDiminDen, 1 ); // rpDiminDen	
	Read( &header->param.hampesCorr, 1 ); // hampesCorr
    header->param.hampesCorr = 1;	 
    
	if ( AxFile::FormatVersion(m_vmaj, m_vmin, m_vrev) < AxFile::FormatVersion(1, 6, 1) )
		return true; // following values where added in 1.6.1
    // 1.6.1
    Read( &int32, 4 );
    header->param.notationMode = wxINT32_SWAP_ON_BE( int32 );

	return true;
}


bool MusBinInput::ReadSeparator( )
{
	char buffer[7];

	Read( buffer, 7 );
	wxString str1( buffer, 0, 6 );
	wxString str2( MusFile::sep );
	if ( str1 != str2 )
		return false;
	else
		return true;
}

bool MusBinInput::ReadPage( MusPage *page )
{
	int j;

    if ( !ReadSeparator() )
		return false;

	Read( &int32, 4 );
	page->npage = wxINT32_SWAP_ON_BE( int32 );
	Read( &int16, 2 );
	page->nbrePortees = wxINT16_SWAP_ON_BE( int16 );
    Read( &page->noMasqueFixe, 1 );	
	Read( &page->noMasqueVar, 1 );
	Read( &page->reserve, 1 );
	Read( &page->defin, 1 );
	Read( &int32, 4 );
	page->indent = wxINT32_SWAP_ON_BE( int32 );	
	Read( &int32, 4 );
	page->indentDroite = wxINT32_SWAP_ON_BE( int32 ); 
	Read( &int32, 4 );
	page->lrg_lign = wxINT32_SWAP_ON_BE( int32 );
    for (j = 0; j < page->nbrePortees; j++) 
	{
		MusStaff *staff = new MusStaff();
		ReadStaff( staff );
        staff->voix = (j % 2 == 0) ? 1 : 0; // add voices
		page->m_staves.Add( staff );
	}

	return true;

}
bool MusBinInput::ReadStaff( MusStaff *staff )
{
	unsigned int k;

	Read( &uint32, 4 );
	staff->nblement = wxUINT32_SWAP_ON_BE( uint32 );
	Read( &uint16, 2 );
	staff->voix = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	staff->noGrp = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	staff->totGrp = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	staff->noLigne = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	staff->no = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &staff->armTyp, 1 );
	Read( &staff->armNbr, 1 );
	Read( &staff->notAnc, 1 );
	//staff->notAnc = true;// force notation ancienne
	Read( &staff->grise, 1 );
	Read( &staff->invisible, 1 );
	Read( &uint16, 2 );
	staff->ecart = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &staff->vertBarre, 1 );
	Read( &staff->brace, 1 );
	Read( &staff->pTaille, 1 );
    Read( &int32, 4 );
    staff->indent = wxINT32_SWAP_ON_BE( int32 );
	Read( &staff->indentDroite, 1 );
	Read( &staff->portNbLine, 1 );
	Read( &staff->accol, 1 );
	Read( &staff->accessoire, 1 );
	Read( &uint16, 2 );
	staff->reserve = wxUINT16_SWAP_ON_BE( uint16 );
	unsigned char c;
	for ( k = 0; k < staff->nblement; k++ )
	{
		Read( &c, 1 );
		if ( c == NOTE )
		{
			MusNote *note = new MusNote();
			note->no = k;
			ReadNote( note );
            
            /*
			//Test code
			if ( (int)note->m_lyrics.GetCount() == 0 ){
				MusSymbol *lyric = new MusSymbol();
				lyric->flag = CHAINE;
				lyric->fonte = LYRIC;
				lyric->m_debord_str = "z";
				lyric->xrel = note->xrel - 10;
				lyric->dec_y = - STAFF_OFFSET;   //Add define for height
				lyric->offset = note->offset;
				lyric->m_note_ptr = note;		
				note->m_lyrics.Add( lyric );	
				
				MusSymbol *lyric2 = new MusSymbol();
				lyric2->flag = CHAINE;
				lyric2->fonte = LYRIC;
				lyric2->m_debord_str = "d";
				lyric2->xrel = note->xrel;
				lyric2->dec_y = - STAFF_OFFSET;   //Add define for height
				lyric2->offset = note->offset;
				lyric2->m_note_ptr = note;
				note->m_lyrics.Add( lyric2 );
			}
            */
				 
			staff->m_elements.Add( note );
		}
		else if ( c == SYMB )
		{
			MusSymbol *symbol = new MusSymbol();
			symbol->no = k;
			ReadSymbol( symbol );
			staff->m_elements.Add( symbol );
		}
        else if ( c == NEUME )
		{
			MusNeume *neume = new MusNeume();
			ReadNeume( neume );
			staff->m_elements.Add(  neume );
		}
		if ( m_flag == MUS_BIN_ARUSPIX_CMP )
		{		
			MusElement *elem = &staff->m_elements.Last();
			Read( &int32, 4 );
			Read( elem->m_im_filename.GetWriteBuf( wxINT32_SWAP_ON_BE( int32 ) + 1 ) , wxINT32_SWAP_ON_BE( int32 ) + 1 );
			elem->m_im_filename.UngetWriteBuf();
			Read( &int32, 4 );
			elem->m_im_staff = wxINT32_SWAP_ON_BE( int32 );
			Read( &int32, 4 );
			elem->m_im_pos = wxINT32_SWAP_ON_BE( int32 );
			Read( &int32, 4 );
			elem->m_cmp_flag = wxINT32_SWAP_ON_BE( int32 );
		}
	}
			
	return true;
}

bool MusBinInput::ReadSymbol( MusSymbol *symbol )
{
	ReadElementAttr( symbol );
	Read( &symbol->flag , 1 );
	Read( &symbol->calte , 1 );
	Read( &symbol->carStyle , 1 );
	Read( &symbol->carOrient , 1 );
	Read( &symbol->fonte , 1 );
	Read( &symbol->s_lie_l , 1 );
	Read( &symbol->point , 1 );
	Read( &uint16, 2 );
	symbol->code = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	symbol->l_ptch = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &int32, 4 );
	symbol->dec_y = wxINT32_SWAP_ON_BE( int32 );
	if ( symbol->IsLyric() )
        ReadLyric( symbol );
     
	return true;
}

bool MusBinInput::ReadNote( MusNote *note )
{
	ReadElementAttr( note );
	Read( &note->sil, 1 );
	Read( &note->val, 1 );
	Read( &note->inv_val, 1 );
	Read( &note->point, 1 );
	Read( &note->stop_rel, 1 );
	Read( &note->acc, 1 );
	Read( &note->accInvis, 1 );
	Read( &note->q_auto, 1 );
	Read( &note->queue, 1 );
	Read( &note->stacc, 1 );
	Read( &note->oblique, 1 );
	Read( &note->queue_lig, 1 );
	Read( &note->chord, 1 );
	Read( &note->fchord, 1 );
	Read( &note->lat, 1 );
	Read( &note->haste, 1 );
	unsigned char c;
	Read( &c, 1 );
	note->code = (unsigned short)c; // code deplace dans element, unsigned SHORT !!
	Read( &note->tetenot, 1 );
	Read( &note->typStac, 1 );
	
	char count;
	Read( &count, 1 );
	
	for ( int i = 0; i < count; i++ ) {
		MusSymbol *lyric = new MusSymbol();
		Read( &lyric->TYPE, 1 );
		ReadSymbol( lyric );
		lyric->m_note_ptr = note;				 		
		note->m_lyrics.Add( lyric );
	}
	
	return true;
}

bool MusBinInput::ReadNeume( MusNeume *neume )
{
    // Read member values
    // ...
	ReadElementAttr( neume );
	Read( &neume->closed, 1 );
	Read( &neume->code, 1);
	
	Read( &neume->xrel_right, 1);
	
	// for mei (eventually) (these fields are all char)
	Read( &neume->name, 1);
	Read( &neume->n_type, 1);
	Read( &neume->form, 1);
    // if ( symbol->IsLyric() ) // no lyrics in MusNeume yet
	Read( &neume->p_max, 1);
	Read( &neume->p_min, 1);
	Read( &neume->p_range, 1);
	unsigned char code = (unsigned char)neume->code;
	Read( &code, 1 ); // placed in element, but as unsigned short
	
	Read( &neume->n_selected, 1);
	/** MusNeumePitch list (n_pitches) **/
	MusNeumePitch *temp;
	
	unsigned char size;	
	Read( &size, 1);
 
	
	for (unsigned int i = 0; i < size; i++)
	{ 
		// XXX: Read Neume Pitches and create objects
/*		temp = new MusNeumePitch();
		ReadElementAttr(temp);
		
		Read( &temp->val, 1);
		
		unsigned char strsize;
		Read( &strsize, 1);
        Read( temp->m_font_str.GetWriteBuf( strsize ) , strsize );
        temp->m_font_str.UngetWriteBuf();
		//char *str = (char*)malloc(strsize);
		//Read( str, strsize );
		//str[size] = '\0';
		//temp->m_font_str = std::string(str, strsize);
		neume->n_pitches.push_back(temp);
		*/
	}
	
	
	
     
	return true;
}

bool MusBinInput::ReadElementAttr( MusElement *element )
{
	Read( &element->liaison , 1 );
	Read( &element->dliai , 1 );
	Read( &element->fliai , 1 );
	Read( &element->lie_up , 1 );
	Read( &element->rel , 1 );
	Read( &element->drel , 1 );
	Read( &element->frel , 1 );
	Read( &element->oct , 1 );
	Read( &element->dimin , 1 );
	Read( &element->grp , 1 );
	Read( &element->_shport , 1 );
	Read( &element->ligat , 1 );
	Read( &element->ElemInvisible , 1 );
	Read( &element->pointInvisible , 1 );
	Read( &element->existDebord , 1 );
	Read( &element->fligat , 1 );
	Read( &element->notschowgrp , 1 );
	Read( &element->cone , 1 );
	Read( &element->liaisonPointil , 1 );
	Read( &element->reserve1 , 1 );
	Read( &element->reserve2 , 1 );
	Read( &element->ottava , 1 );
	Read( &uint16, 2 );
	element->durNum = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	element->durDen = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	element->offset = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &int32, 4 );
	element->xrel = wxINT32_SWAP_ON_BE( int32 );
	
	return true;
}

bool MusBinInput::ReadLyric( MusElement *element )
{
	Read( &int32, 4 );
	Read( element->m_debord_str.GetWriteBuf( wxINT32_SWAP_ON_BE( int32 ) + 1 ) , wxINT32_SWAP_ON_BE( int32 ) + 1 );
	element->m_debord_str.UngetWriteBuf();
	return true;
}
bool MusBinInput::ReadPagination( MusPagination *pagination )
{
	Read( &int16, 2 );
	pagination->numeroInitial = wxINT16_SWAP_ON_BE( int16 );
	Read( &pagination->aussiPremierPage, 1 );
	Read( &pagination->position, 1 );
	Read( &pagination->numeroFonte, 1 );
	Read( &pagination->carStyle, 1 );
	Read( &pagination->taille, 1 );
	Read( &pagination->offsetDuBord, 1 );
	return true;
}

bool MusBinInput::ReadHeaderFooter( MusHeaderFooter *headerfooter)
{
	char buffer[HEADER_FOOTER_TEXT + 1];
	Read( buffer, HEADER_FOOTER_TEXT );
	headerfooter->texte = buffer;
	Read( &headerfooter->aussiPremierPage, 1 );
	Read( &headerfooter->position, 1 );
	Read( &headerfooter->numeroFonte, 1 );
	Read( &headerfooter->carStyle, 1 );
	Read( &headerfooter->taille, 1 );
	Read(&headerfooter->offsetDuBord, 1 );
	return true;
}




