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

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusFonts );

//----------------------------------------------------------------------------
// MusFont
//----------------------------------------------------------------------------

MusFont::MusFont()
{
}

MusFont::~MusFont()
{
}

// WDR: handler implementations for MusFont


//----------------------------------------------------------------------------
// MusParametersMidi
//----------------------------------------------------------------------------

MusParametersMidi::MusParametersMidi()
{
    tempo = 500000;
    minVeloc = 30;
    maxVeloc = 110;
    collerBeamLiai = 1;
    pedale = 1;
    xResolution1 = 20;
    xResolution2 = 15;
    appogg = 0;
    mes_proportion = 0;

	int i;
	for (i = 0; i < MAXMIDICANAL; i++) // canal2patch
		canal2patch[i] = 0;
	for (i = 0; i < MAXMIDICANAL; i++) // volume
		volume[i] = 64;
	for (i = 0; i < MAXPORTNBRE + 1; i++) // piste2canal
		piste2canal[i] = 0;
}

MusParametersMidi::~MusParametersMidi()
{
}

// WDR: handler implementations for MusParametersMidi


//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

MusWWGOutput::MusWWGOutput( MusFile *file, wxString filename ) :
	MusFileOutputStream( file, filename )
{
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

    WriteFileHeader( &m_file->m_fheader ); // fileheader
    WriteParametersMidi( ); // parametres midi
    WriteParameters2( &m_file->m_fheader.param ); // param2
	WriteFonts( );
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
	//if ( m_file->m_param2.entetePied & MASQUEFIXE )
	//	WritePage( &m_file->m_masqueFixe );
	//if ( m_file->m_param2.entetePied & MASQUEVARIABLE )
	//	WritePage( &m_file->m_masqueVariable );

	//wxMessageBox("stop");
	//wxLogMessage("OK %d", m_file->m_pages.GetCount() );

	return true;
}

bool MusWWGOutput::WriteFileHeader( const MusFileHeader *header )
{
    // unused
    unsigned short maj_ver = 2;
    unsigned short min_ver = 1;
    unsigned int filesize = 0;
    unsigned char beamPenteMax = 30;
    unsigned char beamPenteMin = 10;
    unsigned char Epais1 = 1;
    unsigned char Epais2 = 5;
    unsigned char Epais3 = 9;
    signed char reserve[2];
    reserve[0] = 0;
    reserve[1] = 0;
    
    wxString shortname;
    wxFileName::SplitPath( m_filename, NULL, &shortname, NULL );
    shortname += ".wwg";

	char buffer[WIN_MAX_FNAME + WIN_MAX_EXT + 1];
	memset( buffer, 0, WIN_MAX_FNAME + WIN_MAX_EXT + 1 );

    Write( "Wolfgang", 9); // (version 6)
	uint16 = wxUINT16_SWAP_ON_BE( maj_ver );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( min_ver );
	Write( &uint16, 2 );
	//if ( header->name.Length() > WIN_MAX_FNAME + WIN_MAX_EXT )
	//	return false;
	//memcpy( buffer, header->name.c_str(), header->name.Length() );
    memcpy( buffer, shortname.c_str(), shortname.Length() );
	Write( buffer, WIN_MAX_FNAME + WIN_MAX_EXT ); // name, unused
    uint32 = wxUINT32_SWAP_ON_BE( filesize ); // filesize
	Write( &uint32, 4 );
	uint16 = wxUINT16_SWAP_ON_BE( header->nbpage ); // nbpage
	Write( &uint16, 2 ); 
	uint16 = wxUINT16_SWAP_ON_BE( header->nopage ); // nopage
	Write( &uint16, 2 ); 
	uint16 = wxUINT16_SWAP_ON_BE( header->noligne ); // noligne
	Write( &uint16, 2 ); 
	uint32 = wxUINT32_SWAP_ON_BE( header->xpos ); // xpos
	Write( &uint32, 4 );
	Write( &header->param.orientation, 1 ); // param - orientation
	Write( &header->param.EpLignesPortee, 1 ); // param - epLignesPortee
	Write( &header->param.EpQueueNote, 1 ); // param - epQueueNotes
	Write( &header->param.EpBarreMesure, 1 ); // param - epBarreMesure
	Write( &header->param.EpBarreValeur, 1 ); // param - epBarreValeur
	Write( &header->param.EpBlancBarreValeur, 1 ); // param - epBlancBarreValeur
	Write( &beamPenteMax, 1 ); // param - beamPenteMax
	Write( &beamPenteMin, 1 ); // param - beamPenteMin
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
	Write( &Epais1, 1 ); // param - epais1
	Write( &Epais2, 1 ); // param - epais2
	Write( &Epais3, 1 ); // param - epais3
	Write( &reserve[0], 1 ); // reserve_0
	Write( &reserve[1], 1 ); // reserve_1
	return true;
}

bool MusWWGOutput::WriteParametersMidi( )
{
    int i;
    
    MusParametersMidi midi;

	int32 = wxINT32_SWAP_ON_BE( midi.tempo ); // tempo
	Write( &int32, 4 );
	Write( &midi.minVeloc, 1 ); // minVeloc
	Write( &midi.maxVeloc, 1 );  // maxVeloc
	Write( &midi.collerBeamLiai, 1 ); // collerBeamLs
	Write( &midi.pedale, 1 ); // pedale
	Write( &midi.xResolution1, 1 ); // xResolution1
	Write( &midi.xResolution2, 1 ); // xResolution2
	Write( &midi.appogg, 1 );  // appogg
	Write( &midi.mes_proportion, 1 ); // mesProportion
    for (i = 0; i < MAXMIDICANAL; i++) // canal2patch
		Write( &midi.canal2patch[i], 1 );
	for (i = 0; i < MAXMIDICANAL; i++) // volume
		Write( &midi.volume[i], 1 );
    for (i = 0; i < MAXPORTNBRE + 1; i++) // piste2canal
		Write( &midi.piste2canal[i], 1 ); 
    return true;
}

bool MusWWGOutput::WriteParameters2( const MusParameters *param )
{
    unsigned char transp_sil = 0;
    unsigned char autoval_sil = 1;
    unsigned char nbPagesEncontinu = 3;
    signed char vertCorrEcr = 2;
    signed char vertCorrPrn = 0;
    signed char epaisBezier = 0;
    signed char transposition[MAXPORTNBRE+1];
	int i;
	for (i = 0; i < MAXPORTNBRE+1; i++) // transposition
		transposition[i] = 0;

	Write( &transp_sil, 1 );// transpSil
	Write( &param->rapportPorteesNum, 1 ); // rpPorteesNum
	Write( &param->rapportPorteesDen, 1 ); // rpPorteesDen
	Write( &param->rapportDiminNum, 1 ); // rpDiminNum
	Write( &param->rapportDiminDen, 1 ); // rpDiminDen
	Write( &autoval_sil, 1 ); // autoValSil
	Write( &nbPagesEncontinu, 1 ); // nbPagesEnContinu
	Write( &vertCorrEcr, 1 ); // vertCorrEcr
	Write( &vertCorrPrn, 1 ); // vertCorrPrn
	Write( &param->hampesCorr, 1 ); // hampesCorr
	Write( &epaisBezier, 1 ); // epaisBezier
	uint32 = wxUINT32_SWAP_ON_BE( param->entetePied ); // entetePied
	Write( &uint32, 4 );
    for (i = 0; i < MAXPORTNBRE+1; i++) // transposition
		Write( &transposition[i], 1 );

	return true;
}

bool MusWWGOutput::WriteFonts(  )
{
	char buffer[MAXPOLICENAME + 1];
	
    ArrayOfMusFonts fonts;
	MusFont font;

	font.fonteJeu = 1;
	font.fonteNom = "Leipzig 4.2";
	fonts.Add( font );

	int i;
    for (i = 0; i < MAXPOLICES - 1; i++) // chargement des polices vides
	{
		font.fonteJeu = 0;
		font.fonteNom = "";
		fonts.Add( font );
	}
    
    for (i = 0; i < MAXPOLICES; i++) // ecriture des polices
	{
		memset( buffer, 0, MAXPOLICENAME + 1 );
		font = fonts.Item( i );
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
    char indent = ( staff->indent > 0);
    Write( &indent, 1 );
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
	
	return true;
}

bool MusWWGOutput::WriteSymbole( const MusSymbol *symbol )
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
	if ( symbol->existDebord ) 
		WriteDebord( symbol );
    // if ( symbol->IsLyric() ) // To be fixed ??
    if ( (symbol->flag == CHAINE) && (symbol->fonte == LYRIC) )
		WriteLyric( symbol );
	
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

MusWWGInput::MusWWGInput( MusFile *file, wxString filename ) :
	MusFileInputStream( file, filename )
{
    m_lyric = NULL;
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
    ReadParametersMidi( ); // parametres midi
    ReadParameters2( &m_file->m_fheader.param ); // param2
	ReadFonts( );
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
	//if ( &m_file->m_fheader.param.entetePied & MASQUEFIXE )
	//	ReadPage( &m_file->m_masqueFixe );
	//if ( &m_file->m_fheader.param.entetePied & MASQUEVARIABLE )
	//	ReadPage( &m_file->m_masqueVariable );

	//wxLogMessage("OK %d", m_file->m_pages.GetCount() );

	return true;
}

bool MusWWGInput::ReadFileHeader( MusFileHeader *header )
{

	char buffer[WIN_MAX_FNAME + WIN_MAX_EXT + 1];
	Read( buffer, 9 ); // (version 6)
    
    unsigned char unused_uc;
    signed char unused_sc;    

    unsigned short majVO; // maj_ver
    unsigned short minVO;
	Read( &uint16, 2 );
	majVO = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	minVO = wxUINT16_SWAP_ON_BE( uint16 );
    if ((majVO != 2) && (minVO < 1)) 
	{
		wxLogWarning(_("Invalid version (%d.%d"), majVO, minVO );
        return false;
	}
    Read( buffer, WIN_MAX_FNAME + WIN_MAX_EXT); // name (version 6) - unused
	Read( &uint32, 4 ); // filesize - unused
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
	Read( &unused_uc, 1 ); // param - beamPenteMax
	Read( &unused_uc, 1 ); // param - beamPenteMin
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
	Read( &unused_uc, 1 ); // param - epais1
	Read( &unused_uc, 1 ); // param - epais2
	Read( &unused_uc, 1 ); // param - epais3
	Read( &unused_sc, 1 ); // reserve_0
	Read( &unused_sc, 1 ); // reserve_1

	return true;
}

bool MusWWGInput::ReadParametersMidi( )
{
    int i;
    
    MusParametersMidi midi; 

	Read( &int32, 4 );
	midi.tempo = wxINT32_SWAP_ON_BE( int32 ); // tempo
	Read( &midi.minVeloc, 1 ); // minVeloc
	Read( &midi.maxVeloc, 1 );  // maxVeloc
	Read( &midi.collerBeamLiai, 1 ); // collerBeamLs
	Read( &midi.pedale, 1 ); // pedale
	Read( &midi.xResolution1, 1 ); // xResolution1
	Read( &midi.xResolution2, 1 ); // xResolution2
	Read( &midi.appogg, 1 );  // appogg
	Read( &midi.mes_proportion, 1 ); // mesProportion
	for (i = 0; i < MAXMIDICANAL; i++) // canal2patch
		Read( &midi.canal2patch[i], 1 );
	for (i = 0; i < MAXMIDICANAL; i++) // volume
		Read( &midi.volume[i], 1 );
	for (i = 0; i < MAXPORTNBRE + 1; i++) // piste2canal
		Read( &midi.piste2canal[i], 1 ); 
    
	return true;
}

bool MusWWGInput::ReadParameters2( MusParameters *param )
{
	int i;
    
    char unused_c;

	Read( &unused_c, 1 );// transpSil
	Read( &param->rapportPorteesNum, 1 ); // rpPorteesNum
	Read( &param->rapportPorteesDen, 1 ); // rpPorteesDen
	Read( &param->rapportDiminNum, 1 ); // rpDiminNum
	Read( &param->rapportDiminDen, 1 ); // rpDiminDen
	Read( &unused_c, 1 ); // autoValSil
	Read( &unused_c, 1 ); // nbPagesEnContinu
	Read( &unused_c, 1 ); // vertCorrEcr
	Read( &unused_c, 1 ); // vertCorrPrn	
	Read( &param->hampesCorr, 1 ); // hampesCorr	
	Read( &unused_c, 1 ); // epaisBezier
	Read( &uint32, 4 );
	param->entetePied = wxUINT32_SWAP_ON_BE( uint32 ); // entetePied
	param->entetePied &= (~FILEINFO); // ignore FILEINFO  
	for (i = 0; i < MAXPORTNBRE+1; i++) // transposition
		Read( &unused_c, 1 );

	return true;
}

bool MusWWGInput::ReadFonts( )
{
	int i;
	char buffer[MAXPOLICENAME + 1];

	MusFont font;
    for (i = 0; i < MAXPOLICES; i++) // lecture des polices
	{
		Read( &font.fonteJeu, 1 );
		Read( buffer, MAXPOLICENAME );
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
		if ( staff->indent )
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
	//staff->notAnc = true;// force notation ancienne
	Read( &staff->grise, 1 );
	Read( &staff->invisible, 1 );
	Read( &uint16, 2 );
	staff->ecart = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &staff->vertBarre, 1 );
	Read( &staff->brace, 1 );
	Read( &staff->pTaille, 1 );
    char indent;
    Read( &indent, 1 );
    if ( indent )
        staff->indent = 1;
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
			staff->m_elements.Add( note );
		}
		else
		{
			MusSymbol *symbol = new MusSymbol();
			symbol->no = k;
			ReadSymbole( symbol );
            // For the lyrics, we must attach them to the notes
            // We keep it and
            if ( (symbol->flag == CHAINE) && (symbol->fonte == LYRIC) ) {
                if ( m_lyric ) {
                    wxLogWarning("Lyric element will '%s' be lost", symbol->m_debord_str.c_str() ); 
                    delete m_lyric;
                }
                m_lyric = symbol;
                symbol->m_debord_str = wxString( (char*)symbol->pdebord,
                    (int)symbol->debordSize - 4);  // - sizeof( element->debordSize ) - sizeof( element->debordCode );
                //staff->nblement--; 
            } else {
                staff->m_elements.Add( symbol );
            }
		}
	}
    staff->CheckIntegrity();
			
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
        
    if ( m_lyric) {
        // default values
        m_lyric->dec_y = - STAFF_OFFSET;
        m_lyric->xrel = note->xrel - 20;
		m_lyric->m_note_ptr = note;	
		note->m_lyrics.Add( m_lyric );
        m_lyric = NULL;
	}
        
    return true;
}

bool MusWWGInput::ReadSymbole( MusSymbol *symbol )
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
	if ( symbol->existDebord ) 
		ReadDebord( symbol );
     
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
    wxLogDebug("read debord %d, %s", size, (char*)element->pdebord );
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


