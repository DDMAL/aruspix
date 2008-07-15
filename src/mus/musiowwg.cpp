/////////////////////////////////////////////////////////////////////////////
// Name:        musiowwg.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musiowwg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"

#include "musiowwg.h"
#include "musiomlf.h"

//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

MusWWGOutput::MusWWGOutput( MusFile *file, wxString filename, int flag ) :
	MusFileOutputStream( file, filename )
{
	m_flag = flag;
	m_filename = filename;
}

MusWWGOutput::~MusWWGOutput()
{
}

bool MusWWGOutput::ExportFile( )
{
	int i;

	if ( !IsOk() )
	{
		wxLogMessage(_("Cannot write file '%s'"), m_filename.c_str() );
		return false;
	}

    wxString shortname;
    wxFileName::SplitPath( m_filename, NULL, &shortname, NULL );
	m_file->m_fheader.name = shortname + ".wwg";

    WriteFileHeader( &m_file->m_fheader ); // fileheader
    WriteParametersMidi( &m_file->m_midi ); // parametres midi
    WriteParameters2( &m_file->m_param2 ); // param2
	WriteFonts( &m_file->m_fonts );
	MusPage *page = NULL;
    for (i = 0; i < m_file->m_fheader.nbpage; i++ )
	{
		page = &m_file->m_pages.Item(i);
		WritePage( page );
    }
    if (!WriteSeparator() ) 
		return false;
	if ( m_file->m_param2.entetePied & PAGINATION )
		WritePagination( &m_file->m_pagination );
	if ( m_file->m_param2.entetePied & ENTETE )
		WriteHeaderFooter( &m_file->m_header );
	if ( m_file->m_param2.entetePied & PIEDDEPAGE )
		WriteHeaderFooter( &m_file->m_footer );
	if ( m_file->m_param2.entetePied & MASQUEFIXE )
		WritePage( &m_file->m_masqueFixe );
	if ( m_file->m_param2.entetePied & MASQUEVARIABLE )
		WritePage( &m_file->m_masqueVariable );

	//wxMessageBox("stop");
	//wxLogMessage("OK %d", m_file->m_pages.GetCount() );

	return true;
}

bool MusWWGOutput::WriteFileHeader( const MusFileHeader *header )
{
	char buffer[WIN_MAX_FNAME + WIN_MAX_EXT + 1];
	memset( buffer, 0, WIN_MAX_FNAME + WIN_MAX_EXT + 1 );

	if ( m_flag == WWG_ARUSPIX )
		Write( "Aruspix ", 9); //
	else if ( m_flag == WWG_ARUSPIX_CMP ) // To be changed..., not safe or elegant
		Write( "AruspixC", 9); //
	else
		Write( "Wolfgang", 9); // (version 6)
	uint16 = wxUINT16_SWAP_ON_BE( header->maj_ver);
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( header->min_ver );
	Write( &uint16, 2 );
	if ( header->name.Length() > WIN_MAX_FNAME + WIN_MAX_EXT )
		return false;
	memcpy( buffer, header->name.c_str(), header->name.Length() );
	Write( buffer, WIN_MAX_FNAME + WIN_MAX_EXT );
    uint32 = wxUINT32_SWAP_ON_BE( header->filesize ); // filesize
	Write( &uint32, 4 );
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
	Write( &header->param.beamPenteMax, 1 ); // param - beamPenteMax
	Write( &header->param.beamPenteMin, 1 ); // param - beamPenteMin
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
	Write( &header->param.Epais1, 1 ); // param - epais1
	Write( &header->param.Epais2, 1 ); // param - epais2
	Write( &header->param.Epais3, 1 ); // param - epais3
	Write( &header->reserve[0], 1 ); // reserve_0
	Write( &header->reserve[1], 1 ); // reserve_1
	return true;
}

bool MusWWGOutput::WriteParametersMidi( const MusParametersMidi *midi )
{
    int i;

	int32 = wxINT32_SWAP_ON_BE( midi->tempo ); // tempo
	Write( &int32, 4 );
	Write( &midi->minVeloc, 1 ); // minVeloc
	Write( &midi->maxVeloc, 1 );  // maxVeloc
	Write( &midi->collerBeamLiai, 1 ); // collerBeamLs
	Write( &midi->pedale, 1 ); // pedale
	Write( &midi->xResolution1, 1 ); // xResolution1
	Write( &midi->xResolution2, 1 ); // xResolution2
	Write( &midi->appogg, 1 );  // appogg
	Write( &midi->mes_proportion, 1 ); // mesProportion
    for (i = 0; i < MAXMIDICANAL; i++) // canal2patch
		Write( &midi->canal2patch[i], 1 );
	for (i = 0; i < MAXMIDICANAL; i++) // volume
		Write( &midi->volume[i], 1 );
    for (i = 0; i < MAXPORTNBRE + 1; i++) // piste2canal
		Write( &midi->piste2canal[i], 1 ); 
    return true;
}

bool MusWWGOutput::WriteParameters2( const MusParameters2 *param2 )
{
	int i;

	Write( &param2->transp_sil, 1 );// transpSil
	Write( &param2->rapportPorteesNum, 1 ); // rpPorteesNum
	Write( &param2->rapportPorteesDen, 1 ); // rpPorteesDen
	Write( &param2->rapportDiminNum, 1 ); // rpDiminNum
	Write( &param2->rapportDiminDen, 1 ); // rpDiminDen
	Write( &param2->autoval_sil, 1 ); // autoValSil
	Write( &param2->nbPagesEncontinu, 1 ); // nbPagesEnContinu
	Write( &param2->vertCorrEcr, 1 ); // vertCorrEcr
	Write( &param2->vertCorrPrn, 1 ); // vertCorrPrn
	Write( &param2->hampesCorr, 1 ); // hampesCorr
	Write( &param2->epaisBezier, 1 ); // epaisBezier
	uint32 = wxUINT32_SWAP_ON_BE( param2->entetePied ); // entetePied
	Write( &uint32, 4 );
    for (i = 0; i < MAXPORTNBRE+1; i++) // transposition
		Write( &param2->transposition[i], 1 );

	return true;
}

bool MusWWGOutput::WriteFonts( const ArrayOfWgFonts *fonts )
{
	int i;
	char buffer[MAXPOLICENAME + 1];
	
	MusFont font;
    for (i = 0; i < MAXPOLICES; i++) // lecture des polices
	{
		memset( buffer, 0, MAXPOLICENAME + 1 );
		font = fonts->Item( i );
		Write( &font.fonteJeu, 1 );
		if ( font.fonteNom.Length() > MAXPOLICENAME )
			return false;
		memcpy( buffer, font.fonteNom.c_str(), font.fonteNom.Length() );
		Write( buffer, MAXPOLICENAME );
	}

	return true;
}

bool MusWWGOutput::WriteSeparator( )
{
	Write( MusFile::sep, 7 );
	return true;
}

bool MusWWGOutput::WritePage( const MusPage *page )
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
	/*MusStaff *staff = NULL;
    for (j = 0; j < page->nbrePortees; j++) 
	{
		staff = &page->m_staves[j];
		WriteStaff( staff );
    }*/


    MusStaff *staff = NULL;
	for (j = 0; j < page->nbrePortees; j++) 
    {
        MusStaff *cstaff = &page->m_staves[j];
		staff = MusMLFOutput::SplitSymboles( cstaff );
        WriteStaff( staff );
		delete staff;
		staff = NULL;
    }

	return true;

}
bool MusWWGOutput::WriteStaff( const MusStaff *staff )
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
	if ( m_flag == WWG_ARUSPIX )
	{
		int32 = wxINT32_SWAP_ON_BE( staff->indent );
		Write( &int32, 4 );
	}
	else
	{
		char indent = ( staff->indent > 0);
		Write( &indent, 1 );
	}
	Write( &staff->indentDroite, 1 );
	Write( &staff->portNbLine, 1 );
	Write( &staff->accol, 1 );
	Write( &staff->accessoire, 1 );
	uint16 = wxUINT16_SWAP_ON_BE( staff->reserve );
	Write( &uint16, 2 );
	for (k = 0;k < staff->nblement ; k++ )
	{
		if ( staff->m_elements[k].TYPE == NOTE )
		{
			WriteNote( (MusNote*)&staff->m_elements[k] );
		}
		else
		{
			WriteSymbole( (MusSymbol*)&staff->m_elements[k] );
		}
		if ( m_flag == WWG_ARUSPIX_CMP )
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

bool MusWWGOutput::WriteNote( const MusNote *note )
{
	Write( &note->TYPE, 1 );
	WriteElementAttr( note );
	Write( &note->sil, 1 );
	unsigned char val = note->val; 
	if ( ( note->sil == _SIL ) && ( note->val == CUSTOS ) )
		val = Q6C;
	Write( &val, 1 );
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
	if ( note->existDebord ) 
		WriteDebord( note );
	
	char tmp;
	if ( note->m_hasAssociatedLyric ) tmp = 1;
	else tmp = 0;
	Write( &tmp, 1 );
	//Write( &note->m_hasAssociatedLyric, 1 );
	if ( note->m_hasAssociatedLyric )
		WriteSymbole( note->m_lyric_ptr );
	
	return true;
}

bool MusWWGOutput::WriteSymbole( const MusSymbol *symbole )
{
	Write( &symbole->TYPE, 1 );
	WriteElementAttr( symbole );
	Write( &symbole->flag , 1 );
	Write( &symbole->calte , 1 );
	Write( &symbole->carStyle , 1 );
	Write( &symbole->carOrient , 1 );
	Write( &symbole->fonte , 1 );
	Write( &symbole->s_lie_l , 1 );
	Write( &symbole->point , 1 );
	uint16 = wxUINT16_SWAP_ON_BE( symbole->code );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( symbole->l_ptch );
	Write( &uint16, 2 );	
	int32 = wxINT32_SWAP_ON_BE( symbole->dec_y );
	Write( &int32, 4 );
	if ( symbole->existDebord ) 
		WriteDebord( symbole );
    if ( symbole->flag == LYRIC )
		WriteLyric( symbole );
	
	return true;
}

bool MusWWGOutput::WriteElementAttr( const MusElement *element )
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

bool MusWWGOutput::WriteDebord( const MusElement *element )
{
	uint16 = wxUINT16_SWAP_ON_BE( element->debordSize );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( element->debordCode );
	Write( &uint16, 2 );

	int size = element->debordSize - 4; // - sizeof( element->debordSize ) - sizeof( element->debordCode );
	Write( element->pdebord, size );
	return true;
}

bool MusWWGOutput::WriteLyric( const MusElement *element )
{
	int32 = wxINT32_SWAP_ON_BE( (int)element->m_debord_str.Length() );
	Write( &int32, 4 );
	
	Write( element->m_debord_str.c_str(), (int)element->m_debord_str.Length() + 1 );
	return true;
}

bool MusWWGOutput::WritePagination( const MusPagination *pagination )
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

bool MusWWGOutput::WriteHeaderFooter( const MusHeaderFooter *headerfooter)
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



// WDR: handler implementations for MusWWGOutput


//----------------------------------------------------------------------------
// MusWWGInput
//----------------------------------------------------------------------------

MusWWGInput::MusWWGInput( MusFile *file, wxString filename, int flag ) :
	MusFileInputStream( file, filename )
{
	m_flag = flag;
}

MusWWGInput::~MusWWGInput()
{
}

bool MusWWGInput::ImportFile( )
{
	int i;

	if ( !IsOk() )
	{
		wxLogMessage(_("Cannot read file '%s'"), m_file->m_fname.c_str() );
		return false;
	}

    ReadFileHeader( &m_file->m_fheader ); // fileheader
    ReadParametersMidi( &m_file->m_midi ); // parametres midi
    ReadParameters2( &m_file->m_param2 ); // param2
	ReadFonts( &m_file->m_fonts );
	m_file->m_pages.Clear();		
    for (i = 0; i < m_file->m_fheader.nbpage; i++ )
	{
		MusPage *page = new MusPage();
		ReadPage( page );
		m_file->m_pages.Add( page );
    }
    if ( !ReadSeparator() ) 
		return false;
	if ( m_file->m_param2.entetePied & PAGINATION )
		ReadPagination( &m_file->m_pagination );
	if ( m_file->m_param2.entetePied & ENTETE )
		ReadHeaderFooter( &m_file->m_header );
	if ( m_file->m_param2.entetePied & PIEDDEPAGE )
		ReadHeaderFooter( &m_file->m_footer );
	if ( m_file->m_param2.entetePied & MASQUEFIXE )
		ReadPage( &m_file->m_masqueFixe );
	if ( m_file->m_param2.entetePied & MASQUEVARIABLE )
		ReadPage( &m_file->m_masqueVariable );

	//wxLogMessage("OK %d", m_file->m_pages.GetCount() );

	return true;
}

bool MusWWGInput::ReadFileHeader( MusFileHeader *header )
{


	char buffer[WIN_MAX_FNAME + WIN_MAX_EXT + 1];
	Read( buffer, 9 ); // (version 6)
	
	if ( strcmp( buffer, "Aruspix ") == 0 )
		m_flag = WWG_ARUSPIX;
	else if ( strcmp( buffer, "AruspixC") == 0 ) // To be changed..., not safe or elegant
		m_flag = WWG_ARUSPIX_CMP;
	else
		m_flag = WWG_WOLFGANG;

    unsigned short majVO; // maj_ver
    unsigned short minVO;
	Read( &uint16, 2 );
	majVO = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	minVO = wxUINT16_SWAP_ON_BE( uint16 );
    if ((majVO != 2) && (minVO < 1)) 
	{
		wxLogWarning(_("Invalid version") );
        return false;
	}
	header->maj_ver = majVO;
	header->min_ver = minVO;
    Read( buffer, WIN_MAX_FNAME + WIN_MAX_EXT); // (version 6)
	header->name = buffer;
	Read( &uint32, 4 );
    header->filesize = wxUINT32_SWAP_ON_BE( uint32 ); // filesize 
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
	Read( &header->param.EpQueueNote, 1 ); // param - epQueueNotes
	Read( &header->param.EpBarreMesure, 1 ); // param - epBarreMesure
	Read( &header->param.EpBarreValeur, 1 ); // param - epBarreValeur
	Read( &header->param.EpBlancBarreValeur, 1 ); // param - epBlancBarreValeur
	Read( &header->param.beamPenteMax, 1 ); // param - beamPenteMax
	Read( &header->param.beamPenteMin, 1 ); // param - beamPenteMin
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
	Read( &header->param.Epais1, 1 ); // param - epais1
	Read( &header->param.Epais2, 1 ); // param - epais2
	Read( &header->param.Epais3, 1 ); // param - epais3
	Read( &header->reserve[0], 1 ); // reserve_0
	Read( &header->reserve[1], 1 ); // reserve_1

	return true;
}

bool MusWWGInput::ReadParametersMidi( MusParametersMidi *midi )
{
    int i;

	Read( &int32, 4 );
	midi->tempo = wxINT32_SWAP_ON_BE( int32 ); // tempo
	Read( &midi->minVeloc, 1 ); // minVeloc
	Read( &midi->maxVeloc, 1 );  // maxVeloc
	Read( &midi->collerBeamLiai, 1 ); // collerBeamLs
	Read( &midi->pedale, 1 ); // pedale
	Read( &midi->xResolution1, 1 ); // xResolution1
	Read( &midi->xResolution2, 1 ); // xResolution2
	Read( &midi->appogg, 1 );  // appogg
	Read( &midi->mes_proportion, 1 ); // mesProportion
	for (i = 0; i < MAXMIDICANAL; i++) // canal2patch
		Read( &midi->canal2patch[i], 1 );
	for (i = 0; i < MAXMIDICANAL; i++) // volume
		Read( &midi->volume[i], 1 );
	for (i = 0; i < MAXPORTNBRE + 1; i++) // piste2canal
		Read( &midi->piste2canal[i], 1 ); 
    
	return true;
}

bool MusWWGInput::ReadParameters2( MusParameters2 *param2 )
{
	int i;

	Read( &param2->transp_sil, 1 );// transpSil
	Read( &param2->rapportPorteesNum, 1 ); // rpPorteesNum
	Read( &param2->rapportPorteesDen, 1 ); // rpPorteesDen
	Read( &param2->rapportDiminNum, 1 ); // rpDiminNum
	Read( &param2->rapportDiminDen, 1 ); // rpDiminDen
	Read( &param2->autoval_sil, 1 ); // autoValSil
	Read( &param2->nbPagesEncontinu, 1 ); // nbPagesEnContinu
	Read( &param2->vertCorrEcr, 1 ); // vertCorrEcr
	Read( &param2->vertCorrPrn, 1 ); // vertCorrPrn	
	Read( &param2->hampesCorr, 1 ); // hampesCorr	
	Read( &param2->epaisBezier, 1 ); // epaisBezier
	Read( &uint32, 4 );
	param2->entetePied = wxUINT32_SWAP_ON_BE( uint32 ); // entetePied
	param2->entetePied &= (~FILEINFO); // ignore FILEINFO  
	for (i = 0; i < MAXPORTNBRE+1; i++) // transposition
		Read( &param2->transposition[i], 1 );

	return true;
}

bool MusWWGInput::ReadFonts( ArrayOfWgFonts *fonts )
{
	int i;
	char buffer[MAXPOLICENAME + 1];

	fonts->Clear();
	MusFont font;
    for (i = 0; i < MAXPOLICES; i++) // lecture des polices
	{
		Read( &font.fonteJeu, 1 );
		Read( buffer, MAXPOLICENAME );
		font.fonteNom = buffer;
		fonts->Add( font );
	}

	return true;
}

bool MusWWGInput::ReadSeparator( )
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

bool MusWWGInput::ReadPage( MusPage *page )
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
		if ( (m_flag == WWG_WOLFGANG) && staff->indent )
			staff->indent = page->indent;
		page->m_staves.Add( staff );
	}

	return true;

}
bool MusWWGInput::ReadStaff( MusStaff *staff )
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
	staff->notAnc = true;// force notation ancienne
	Read( &staff->grise, 1 );
	Read( &staff->invisible, 1 );
	Read( &uint16, 2 );
	staff->ecart = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &staff->vertBarre, 1 );
	Read( &staff->brace, 1 );
	Read( &staff->pTaille, 1 );
	if ( m_flag == WWG_ARUSPIX )
	{
		Read( &int32, 4 );
		staff->indent = wxINT32_SWAP_ON_BE( int32 );
		
	}
	else
	{
		char indent;
		Read( &indent, 1 );
		if ( indent )
			staff->indent = 1;
	}
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
			  
			//Test code
			if ( note->m_hasAssociatedLyric == false ){
				note->m_lyric_ptr = new MusSymbol();
				note->m_lyric_ptr->TYPE = SYMB;
				note->m_lyric_ptr->flag = LYRIC;
				note->m_lyric_ptr->m_debord_str = "a";
				note->m_lyric_ptr->xrel = note->xrel;
				note->m_lyric_ptr->dec_y = note->dec_y;
				note->m_lyric_ptr->offset = note->offset;
				note->m_lyric_ptr->m_note_ptr = note;
				note->m_lyric_ptr->m_hasAssociatedNote = true;
				note->m_hasAssociatedLyric = true;				
			}
			
			staff->m_elements.Add( note );
		}
		else
		{
			MusSymbol *symbole = new MusSymbol();
			symbole->no = k;
			ReadSymbole( symbole );
			staff->m_elements.Add( symbole );
		}
		if ( m_flag == WWG_ARUSPIX_CMP )
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

bool MusWWGInput::ReadNote( MusNote *note )
{
	ReadElementAttr( note );
	Read( &note->sil, 1 );
	Read( &note->val, 1 );
	if ( ( note->sil == _SIL ) && ( note->val == Q6C ) )
		note->val = CUSTOS;
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
	if ( note->existDebord ) 
		ReadDebord( note );
	
	char tmp;
	Read( &tmp, 1 );
	if ( tmp != 0 ) note->m_hasAssociatedLyric = true;
	else note->m_hasAssociatedLyric = false;
	
	if ( note->m_hasAssociatedLyric ){
		note->m_lyric_ptr = new MusSymbol();
		Read( &note->m_lyric_ptr->TYPE, 1 );
		ReadSymbole( note->m_lyric_ptr );
		
		note->m_lyric_ptr->m_hasAssociatedNote = true;
		note->m_lyric_ptr->m_note_ptr = note;
	}
	
	return true;
}

bool MusWWGInput::ReadSymbole( MusSymbol *symbole )
{
	ReadElementAttr( symbole );
	Read( &symbole->flag , 1 );
	Read( &symbole->calte , 1 );
	Read( &symbole->carStyle , 1 );
	Read( &symbole->carOrient , 1 );
	Read( &symbole->fonte , 1 );
	Read( &symbole->s_lie_l , 1 );
	Read( &symbole->point , 1 );
	Read( &uint16, 2 );
	symbole->code = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	symbole->l_ptch = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &int32, 4 );
	symbole->dec_y = wxINT32_SWAP_ON_BE( int32 );
	if ( symbole->existDebord ) 
		ReadDebord( symbole );
	if ( symbole->flag == LYRIC )
		ReadLyric( symbole );
     
	return true;
}

bool MusWWGInput::ReadElementAttr( MusElement *element )
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

bool MusWWGInput::ReadDebord( MusElement *element )
{
	Read( &uint16, 2 );
	element->debordSize = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	element->debordCode = wxUINT16_SWAP_ON_BE( uint16 );

	int size = element->debordSize - 4; // - sizeof( element->debordSize ) - sizeof( element->debordCode );
	element->pdebord = malloc( size );
	Read( element->pdebord, size );
	return true;
}

bool MusWWGInput::ReadLyric( MusElement *element )
{
	Read( &int32, 4 );
	Read( element->m_debord_str.GetWriteBuf( wxINT32_SWAP_ON_BE( int32 ) + 1 ) , wxINT32_SWAP_ON_BE( int32 ) + 1 );
	element->m_debord_str.UngetWriteBuf();
	return true;
}
bool MusWWGInput::ReadPagination( MusPagination *pagination )
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

bool MusWWGInput::ReadHeaderFooter( MusHeaderFooter *headerfooter)
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


// WDR: handler implementations for MusWWGInput


