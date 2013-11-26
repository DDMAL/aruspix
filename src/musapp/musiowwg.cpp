/////////////////////////////////////////////////////////////////////////////
// Name:        musiowwg.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/filename.h"

#include "musiowwg.h"

#include "muspage.h"
#include "mussystem.h"
#include "musstaff.h"
#include "musmeasure.h"
#include "muslayer.h"
#include "muslayerelement.h"
#include "musbarline.h"
#include "musclef.h"
#include "musmensur.h"
#include "musnote.h"
#include "mussymbol.h"
#include "musrest.h"
#include "musbeam.h"

#include "musstaff.h"
#include "muslayer.h"

#define WWG_SEPARATOR "#\376\364\365\376#"  // "#˛Ùı˛#"

/* pour analyse de var. _param2.m_headerType, et lecture/ecriture eventuelle 
 a la fin du fichier (offset: mx_byte_off + sizeof(sep)). Faire par ex: 
 m_headerType & ENTETE */
#define PAGINATION 1
#define ENTETE 2
#define PIEDDEPAGE 4
#define MARQUEDECOUPE 8
#define MASQUEFIXE 16
#define MASQUEVARIABLE 32
#define FILEINFO	64
#define MAXFILEINFO	2048

#define CUSTOS 12 // ax2 double check in 1.6

#define BARRE 0	/* pour flag (3 bits) de struct symbol */
#define ALTER 1
#define PNT	2
#define LIAI 3
#define CHAINE 4
#define IND_MES 5
#define CLE 6
#define VECT 7
#define BEZIER 8
#define DYNAMIC 9
#define CROCHET 10
#define VALBARRES 11
#define METAFILE_W 12
#define AX_VARIANT 13

/* groupe stocke in fonte, 3 bits in structure symbol */
#define DYN 0	/* indication pp, ff, mf etc. */
#define LYRIC 1	/* paroles de chant: plusieurs couches possibles */
#define INSTRUM 2	/* nom, identification de l'instr. */
#define MARQ_REPERE 3	/* reperes pour orchestre, etc. */
#define SYMB_ORN 4	/* trilles, etc. */
#define TEMPO 5
#define TITRE_DIV 6

/* groupe stocke in calte, 3 bits in structure symbol, si MARQ_REPERE */
#define SILENCESPECIAL 0	// jusqu'Ö v. 5.3, VARIA
#define NUMMES 1
#define CHIFFRAGE 2
#define MARQUEGLOBALE 3
#define VARIA 4

/* define windows */
#define WIN_MAX_FNAME 256
#define WIN_MAX_EXT 256

/* ajout */
#define HEADER_FOOTER_TEXT 100

/* note types */
#define NOTE 0	/* pour type (bit) de struct element */
#define SYMB 1
#define NEUME 2
#define NEUME_SYMB 3

/* pour bitflag silence/note de struct note */
#define _NOT 0
#define _SIL 1

/* pointages */
#define POINTAGE 1
#define D_POINTAGE 2

/* decalage en 3 bits de code in struct et val -4+3 */
#define OCTBIT 4


/* fichier */
#define MAJ_VERS_FICH	2
#define MIN_VERS_FICH	1



void MusWWGElement::WWGInitElement() 
{
    // WWG Page
    noMasqueFixe = 0;
    noMasqueVar = 0;
    reserve = 0;
    
    // WWG Note
    sil = false;
    val = 0;
    inv_val = false;
    note_point = 0;
    stop_rel = 0;
    acc = 0;
    accInvis = false;
    q_auto = true;
    queue = false;
    stacc = false;
    oblique = false;
    queue_lig = false;
    chord = false;
    fchord = false;
    lat = false;
    haste = false;
    code = 0;
    tetenot = 0;
    typStac = 0;

    // WWG Symbol
    calte = 0;
    carOrient = 0;
    carStyle = 0;
    flag = 0; 
    fonte = 0;
    l_ptch = 0;
    symbol_point = 0;
    s_lie_l = 0;
    
    // WWG Element
    liaison = false;
    dliai = false;
    fliai = false;
    lie_up = false;
    rel = false;
    drel = false;
    frel = false;
    oct = 0;
    dimin = 0;
    grp = 0;
    _shport = 0;
    ligat = false;
    ElemInvisible = false;
    pointInvisible = false;
    existDebord = false;
    fligat = false;
    notschowgrp = 0;
    cone = false;
    liaisonPointil = 0;
    reserve1 = 0;
    reserve2 = 0;
    ottava = 0;
    durNum = 1;
    durDen = 1;
    offset = 0;
    pdebord = NULL;
    debordCode = 0;
    debordSize = 0;
    xrel = 0;
    dec_y = 0;
    
}

//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

MusWWGOutput::MusWWGOutput( MusDoc *doc, std::string filename ) :
	wxFileOutputStream( filename.c_str() )
{
    m_doc = doc;
	m_filename = filename;
    m_current_system = NULL;
    m_current_staff = NULL;
}

MusWWGOutput::~MusWWGOutput()
{
}

bool MusWWGOutput::ExportFile( )
{
    wxASSERT_MSG( m_doc->m_children.size() > 0, "At least one page is required" );

	int i;

	if ( !IsOk() )
	{
		wxLogMessage(_("Cannot write file '%s'"), m_filename.c_str() );
		return false;
	}

    WriteFileHeader( &m_wwgData ); // fileheader
    WriteParametersMidi( &m_wwgData ); // parametres midi
    WriteParameters2( &m_wwgData ); // param2
	WriteFonts( &m_wwgData );
	
    MusPage *page = NULL;
    for (i = 0; i < (int)m_doc->GetPageCount(); i++)
    {
		page = (MusPage*)m_doc->m_children[i];
		WritePage( page );
    }
    
    MusStaff staff;
    //staff.m_id = 0;
    
    if (!WriteSeparator() ) 
		return false;
	if (  m_doc->m_env.m_headerType & PAGINATION )
		WritePagination( & m_wwgData );
	if (  m_doc->m_env.m_headerType & ENTETE )
		WriteHeader( & m_wwgData );
	if (  m_doc->m_env.m_headerType & PIEDDEPAGE )
		WriteFooter( & m_wwgData );
	//if (  m_doc->m_param2.m_headerType & MASQUEFIXE )
	//	WritePage( & m_doc->m_masqueFixe );
	//if (  m_doc->m_param2.m_headerType & MASQUEVARIABLE )
	//	WritePage( & m_doc->m_masqueVariable );

	//wxMessageBox("stop");
	//wxLogMessage("OK %d",  m_doc->m_pages.GetCount() );

	return true;
}

bool MusWWGOutput::WriteFileHeader( const MusWWGData *wwgData )
{
    std::string shortname = m_filename;
    //wxFileName::SplitPath( m_filename, NULL, &shortname, NULL );
    size_t pos;
    if ( (pos = m_filename.find_last_not_of( "/" ) != std::string::npos ) ){
        shortname = m_filename.substr( pos + 1, std::string::npos );
    }
    shortname += ".wwg";

	char buffer[WIN_MAX_FNAME + WIN_MAX_EXT + 1];
	memset( buffer, 0, WIN_MAX_FNAME + WIN_MAX_EXT + 1 );

    Write( "Wolfgang", 9); // (version 6)
	uint16 = wxUINT16_SWAP_ON_BE( wwgData->maj_ver );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( wwgData->min_ver );
	Write( &uint16, 2 );
	//if ( header->name.Length() > WIN_MAX_FNAME + WIN_MAX_EXT )
	//	return false;
	//memcpy( buffer, header->name.c_str(), header->name.Length() );
    memcpy( buffer, shortname.c_str(), shortname.length() );
	Write( buffer, WIN_MAX_FNAME + WIN_MAX_EXT ); // name, unused
    uint32 = wxUINT32_SWAP_ON_BE( wwgData->filesize ); // filesize
	Write( &uint32, 4 );
	uint16 = wxUINT16_SWAP_ON_BE( wwgData->nbpage ); // nbpage
	Write( &uint16, 2 ); 
	uint16 = wxUINT16_SWAP_ON_BE( wwgData->nopage ); // nopage
	Write( &uint16, 2 ); 
	uint16 = wxUINT16_SWAP_ON_BE( wwgData->noligne ); // noligne
	Write( &uint16, 2 ); 
	uint32 = wxUINT32_SWAP_ON_BE( wwgData->xpos ); // xpos
	Write( &uint32, 4 );
	Write( &m_doc->m_env.m_landscape, 1 ); // param - m_landscape
	Write( &m_doc->m_env.m_staffLineWidth, 1 ); // param - epLignesPortee
	Write( &m_doc->m_env.m_stemWidth, 1 ); // param - epQueueNotes
	Write( &m_doc->m_env.m_barlineWidth, 1 ); // param - epBarreMesure
	Write( &m_doc->m_env.m_beamWidth, 1 ); // param - epBarreValeur
	Write( &m_doc->m_env.m_beamWhiteWidth, 1 ); // param - epBlancBarreValeur
	Write( &m_doc->m_env.m_beamMaxSlope, 1 ); // param - m_beamMaxSlope
	Write( &m_doc->m_env.m_beamMinSlope, 1 ); // param - m_beamMinSlope
	int32 = wxINT32_SWAP_ON_BE( m_doc->m_pageWidth / 10 ); // param - m_paperWidth
	Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_BE( m_doc->m_pageHeight / 10 ); // param - m_paperHeight
	Write( &int32, 4 );
	int16 = wxINT16_SWAP_ON_BE( m_doc->m_pageTopMar ); // param - margeSommet
	Write( &int16, 2 );
	int16 = wxINT16_SWAP_ON_BE( m_doc->m_pageLeftMar ); // param - margeGaucheImpaire
	Write( &int16, 2 );
	//int16 = wxINT16_SWAP_ON_BE( m_doc->m_env.m_leftMarginEvenPage ); // write the same value
	Write( &int16, 2 );
	Write( &wwgData->Epais1, 1 ); // param - epais1
	Write( &wwgData->Epais2, 1 ); // param - epais2
	Write( &wwgData->Epais3, 1 ); // param - epais3
	Write( &wwgData->reserve[0], 1 ); // reserve_0
	Write( &wwgData->reserve[1], 1 ); // reserve_1
	return true;
}

bool MusWWGOutput::WriteParametersMidi( const MusWWGData *wwgData )
{
    int i;

	int32 = wxINT32_SWAP_ON_BE( wwgData->tempo ); // tempo
	Write( &int32, 4 );
	Write( &wwgData->minVeloc, 1 ); // minVeloc
	Write( &wwgData->maxVeloc, 1 );  // maxVeloc
	Write( &wwgData->collerBeamLiai, 1 ); // collerBeamLs
	Write( &wwgData->pedale, 1 ); // pedale
	Write( &wwgData->xResolution1, 1 ); // xResolution1
	Write( &wwgData->xResolution2, 1 ); // xResolution2
	Write( &wwgData->appogg, 1 );  // appogg
	Write( &wwgData->mes_proportion, 1 ); // mesProportion
    for (i = 0; i < MAXMIDICANAL; i++) // canal2patch
		Write( &wwgData->canal2patch[i], 1 );
	for (i = 0; i < MAXMIDICANAL; i++) // volume
		Write( &wwgData->volume[i], 1 );
    for (i = 0; i < MAXPORTNBRE + 1; i++) // piste2canal
		Write( &wwgData->piste2canal[i], 1 ); 
    return true;
}

bool MusWWGOutput::WriteParameters2( const MusWWGData *param )
{
	Write( &param->transp_sil, 1 );// transpSil
	Write( &m_doc->m_env.m_smallStaffNum, 1 ); // rpPorteesNum
	Write( &m_doc->m_env.m_smallStaffDen, 1 ); // rpPorteesDen
	Write( &m_doc->m_env.m_graceNum, 1 ); // rpDiminNum
	Write( &m_doc->m_env.m_graceDen, 1 ); // rpDiminDen
	Write( &param->autoval_sil, 1 ); // autoValSil
	Write( &param->nbPagesEncontinu, 1 ); // nbPagesEnContinu
	Write( &param->vertCorrEcr, 1 ); // vertCorrEcr
	Write( &param->vertCorrPrn, 1 ); // vertCorrPrn
	Write( &m_doc->m_env.m_stemCorrection, 1 ); // m_stemCorrection
	Write( &param->epaisBezier, 1 ); // epaisBezier
	uint32 = wxUINT32_SWAP_ON_BE( m_doc->m_env.m_headerType ); // m_headerType
	Write( &uint32, 4 );
    int i;
    for (i = 0; i < MAXPORTNBRE+1; i++) // transposition
		Write( &param->transposition[i], 1 );

	return true;
}

bool MusWWGOutput::WriteFonts( const MusWWGData *fonts  )
{   
    int i;
    for (i = 0; i < MAXPOLICES; i++) // ecriture des polices
	{
		Write( &fonts->fonts[i].fonteJeu, 1 );
		Write( &fonts->fonts[i].fonteNom, MAXPOLICENAME );
	}

	return true;
}

bool MusWWGOutput::WriteSeparator( )
{
	Write( WWG_SEPARATOR, 7 );
	return true;
}

bool MusWWGOutput::WritePage( const MusPage *page )
{
	int i, j, k, l;

    if ( !WriteSeparator() )
		return false;
	int32 = wxINT32_SWAP_ON_BE( 0 );
	Write( &int32, 4 );
	short nbStaves = 0;
	for (i = 0; i < page->GetSystemCount(); i++) {
        MusSystem *system = (MusSystem*)page->m_children[i];
        MusMeasure *measure = (MusMeasure*)system->m_children[0];
		nbStaves += measure->GetStaffCount();
	}
	int16 = wxINT16_SWAP_ON_BE( nbStaves );
	Write( &int16, 2 );
    Write( &noMasqueFixe, 1 );    
	Write( &noMasqueVar, 1 );
	Write( &reserve, 1 );
	Write( &page->defin, 1 );
    // get the first system for indent information
    MusSystem *system = NULL;
    if (page->GetSystemCount() > 0) {
        system = (MusSystem*)page->m_children[0];
    }
    int32 = system ? wxINT32_SWAP_ON_BE( system->m_systemLeftMar ) : 0;
	Write( &int32, 4 );
    int32 = system ? wxINT32_SWAP_ON_BE( system->m_systemRightMar ) : 0;
	Write( &int32, 4 );
	int32 = system ? wxINT32_SWAP_ON_BE( page->m_pageWidth - page->m_pageLeftMar ) : wxINT32_SWAP_ON_BE( 190 ); // more or less...
	Write( &int32, 4 );
    
    l = 0; // staff number on the page
    for (i = 0; i < page->GetSystemCount(); i++) {
        m_current_system = (MusSystem*)page->m_children[i];
        
        // TODO - We need to fill the ecarts[] array here because we now have m_y_abs positions for the staves
        
        for (j = 0; j < m_current_system->GetMeasureCount(); j++)
        {
            MusMeasure *measure = (MusMeasure*)m_current_system->m_children[j];
        
            for (k = 0; k < measure->GetStaffCount(); k++)
            {
                m_current_staff = (MusStaff*)measure->m_children[k];
                for (l = 0; l < m_current_staff->GetLayerCount(); l++)
                {
                    // This needs to be fixed
                    MusLayer *layer = (MusLayer*)m_current_staff->m_children[l];
                    WriteLayer( layer, k );
                }
            }
        }
    }

	return true;

}

bool MusWWGOutput::WriteLayer( const MusLayer *layer, int staffNo )
{
	int k;

	uint32 = wxUINT32_SWAP_ON_BE( layer->GetElementCount() );
	Write( &uint32, 4 );
	uint16 = wxUINT16_SWAP_ON_BE( layer->voix );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( m_current_staff->noGrp );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( m_current_staff->totGrp );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( m_current_system->GetSystemNo() ); // we don't have noLigne anymore - given by the current system being written
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( m_current_staff->GetStaffNo() );
	Write( &uint16, 2 );
	Write( &m_current_staff->armTyp, 1 );
	Write( &m_current_staff->armNbr, 1 );
	Write( &m_current_staff->notAnc, 1 );
	Write( &m_current_staff->grise, 1 );
	Write( &m_current_staff->invisible, 1 );
	uint16 = wxUINT16_SWAP_ON_BE( ecarts[staffNo] );
	Write( &uint16, 2 );
	Write( &m_current_staff->vertBarre, 1 );
	Write( &m_current_staff->brace, 1 );
	Write( &m_current_staff->staffSize, 1 );
    char indent = ( m_current_system->m_systemLeftMar > 0 );
    Write( &indent, 1 );
    indent = 0;
	Write( &indent, 1 ); // fake it
	Write( &m_current_staff->portNbLine, 1 );
	Write( &m_current_staff->accol, 1 );
	Write( &m_current_staff->accessoire, 1 );
	uint16 = wxUINT16_SWAP_ON_BE( 0 ); // reserve
	Write( &uint16, 2 );
	for (k = 0;k < layer->GetElementCount() ; k++ )
	{
        WWGInitElement();
    
        MusLayerElement *element = (MusLayerElement*)layer->m_children[k];
        // position x for all elmements
        xrel = element->m_x_abs;
        if (dynamic_cast<MusBarline*>(element)) {
            TYPE = SYMB;
            WriteSymbol();
        }
        if (dynamic_cast<MusClef*>(element)) {
            TYPE = SYMB;
            WriteSymbol();
        }
        if (dynamic_cast<MusMensur*>(element)) {
            TYPE = SYMB;
            WriteSymbol();
        }
        if (dynamic_cast<MusNote*>(element)) {
            TYPE = NOTE;
            WriteNote( );
        }
        else if (dynamic_cast<MusRest*>(element)) {
            TYPE = NOTE;
            WriteNote( );
        }
        else if (dynamic_cast<MusSymbol*>(element)) {
            TYPE = SYMB;
            WriteSymbol( );
        }
	}

	return true;
}

bool MusWWGOutput::WriteNote( )
{
	Write( &TYPE, 1 );
	WriteElementAttr( );
	Write( &sil, 1 );
	unsigned char val = 0; 
	if ( ( sil == _SIL ) && ( val == CUSTOS ) ) // do we still want this? Should be an option somewhere
		val = DUR_256;
	Write( &val, 1 );
	Write( &inv_val, 1 );
	Write( &note_point, 1 );
	Write( &stop_rel, 1 );
	Write( &acc, 1 );
	Write( &accInvis, 1 );
	Write( &q_auto, 1 );
	Write( &queue, 1 );
	Write( &stacc, 1 );
	Write( &oblique, 1 );
	Write( &queue_lig, 1 );
	Write( &chord, 1 );
	Write( &fchord, 1 );
	Write( &lat, 1 );
	Write( &haste, 1 );
	//Write( &code, sizeof( code ) );
	unsigned char code = (unsigned char)code;
	Write( &code, 1 ); // deplace dans element, mais unsigned short
	Write( &tetenot, 1 );
	Write( &typStac, 1 );
	if ( existDebord ) 
		WriteDebord( );
	
	return true;
}

bool MusWWGOutput::WriteSymbol( )
{
	Write( &TYPE, 1 );
	WriteElementAttr( );
	Write( &flag , 1 );
	Write( &calte , 1 );
	Write( &carStyle , 1 );
	Write( &carOrient , 1 );
	Write( &fonte , 1 );
	Write( &s_lie_l , 1 );
	Write( &symbol_point , 1 );
	uint16 = wxUINT16_SWAP_ON_BE( code );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( l_ptch );
	Write( &uint16, 2 );	
	int32 = wxINT32_SWAP_ON_BE( dec_y );
	Write( &int32, 4 );
	if ( existDebord ) 
		WriteDebord( );
    // if ( IsLyric() ) // To be fixed ??
    if ( (flag == CHAINE) && (fonte == LYRIC) ) {
		//WriteLyric( symbol );
    }
	
	return true;
}

bool MusWWGOutput::WriteElementAttr( )
{
	Write( &liaison , 1 );
	Write( &dliai , 1 );
	Write( &fliai , 1 );
	Write( &lie_up , 1 );
	Write( &rel , 1 );
	Write( &drel , 1 );
	Write( &frel , 1 );
	Write( &oct , 1 );
	Write( &dimin , 1 );
    Write( &grp , 1 );
	Write( &_shport , 1 );
	Write( &ligat , 1 );
	Write( &ElemInvisible , 1 );
	Write( &pointInvisible , 1 );
	Write( &existDebord , 1 );
	Write( &fligat , 1 );
	Write( &notschowgrp , 1 );
	Write( &cone , 1 );
	Write( &liaisonPointil , 1 );
	Write( &reserve1 , 1 );
	Write( &reserve2 , 1 );
	Write( &ottava , 1 );
	uint16 = wxUINT16_SWAP_ON_BE( durNum );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( durDen );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( offset );
	Write( &uint16, 2 );
	int32 = wxINT32_SWAP_ON_BE( xrel );
	Write( &int32, 4 );
	
	return true;
}

bool MusWWGOutput::WriteDebord( )
{
	uint16 = wxUINT16_SWAP_ON_BE( debordSize );
	Write( &uint16, 2 );
	uint16 = wxUINT16_SWAP_ON_BE( debordCode );
	Write( &uint16, 2 );

	int size = debordSize - 4; // - sizeof( debordSize ) - sizeof( debordCode );
	Write( pdebord, size );
	return true;
}

bool MusWWGOutput::WritePagination( const MusWWGData *pagination )
{
	int16 = wxINT16_SWAP_ON_BE( pagination->p_numeroInitial );
	Write( &int16, 2 );
	Write( &pagination->p_aussiPremierPage, 1 );
	Write( &pagination->p_position, 1 );
	Write( &pagination->p_numeroFonte, 1 );
	Write( &pagination->p_carStyle, 1 );
	Write( &pagination->p_taille, 1 );
	Write( &pagination->p_offsetDuBord, 1 );
	return true;
}

bool MusWWGOutput::WriteHeader( const MusWWGData *header )
{
	char buffer[HEADER_FOOTER_TEXT + 1];
	memset( buffer, 0, HEADER_FOOTER_TEXT + 1);
	if ( header->h_texte.length() > HEADER_FOOTER_TEXT + 1 )
		return false;
	memcpy( buffer, header->h_texte.c_str(), header->h_texte.length() );
	Write( buffer, HEADER_FOOTER_TEXT );
	Write( &header->h_aussiPremierPage, 1 );
	Write( &header->h_position, 1 );
	Write( &header->h_numeroFonte, 1 );
	Write( &header->h_carStyle, 1 );
	Write( &header->h_taille, 1 );
	Write( &header->h_offsetDuBord, 1 );
	return true;
}

bool MusWWGOutput::WriteFooter( const MusWWGData *footer )
{
	char buffer[HEADER_FOOTER_TEXT + 1];
	memset( buffer, 0, HEADER_FOOTER_TEXT + 1);
	if ( footer->f_texte.length() > HEADER_FOOTER_TEXT + 1 )
		return false;
	memcpy( buffer, footer->f_texte.c_str(), footer->f_texte.length() );
	Write( buffer, HEADER_FOOTER_TEXT );
	Write( &footer->f_aussiPremierPage, 1 );
	Write( &footer->f_position, 1 );
	Write( &footer->f_numeroFonte, 1 );
	Write( &footer->f_carStyle, 1 );
	Write( &footer->f_taille, 1 );
	Write( &footer->f_offsetDuBord, 1 );
	return true;
}




//----------------------------------------------------------------------------
// MusWWGInput
//----------------------------------------------------------------------------

MusWWGInput::MusWWGInput( MusDoc *doc, std::string filename ) :
	wxFileInputStream( filename.c_str() )
{
    //
    m_doc = doc;
    m_currentBeam = NULL;
    m_isLastNoteInBeam = false;

}

MusWWGInput::~MusWWGInput()
{
}

bool MusWWGInput::ImportFile( )
{
	int i;

	if ( !IsOk() )
	{
		wxLogMessage(_("Cannot read file '%s'"),  m_doc->m_fname.c_str() );
		return false;
	}
    
    // reset the MusDoc and create the logical tree
    m_doc->Reset( Transcription);	
    
    // read WWG header
    ReadFileHeader( &m_wwgData ); // fileheader
    ReadParametersMidi( &m_wwgData  ); // parametres midi
    ReadParameters2( &m_wwgData  ); // param2
	ReadFonts( &m_wwgData  );
    
    for (i = 0; i <  m_wwgData.nbpage; i++ )
	{
		MusPage *page = new MusPage();
		ReadPage( page );
        m_doc->AddPage( page );
    }

    if ( !ReadSeparator() ) 
		return false;
	if (  m_doc->m_env.m_headerType & PAGINATION )
		ReadPagination( &m_wwgData  );
	if (  m_doc->m_env.m_headerType & ENTETE )
		ReadHeader( &m_wwgData );
	if (  m_doc->m_env.m_headerType & PIEDDEPAGE )
		ReadFooter( &m_wwgData );
	//if ( & m_doc->m_env.param.m_headerType & MASQUEFIXE )
	//	ReadPage( & m_doc->m_masqueFixe );
	//if ( & m_doc->m_env.param.m_headerType & MASQUEVARIABLE )
	//	ReadPage( & m_doc->m_masqueVariable );

	//wxLogMessage("OK %d",  m_doc->m_pages.GetCount() );
    
    // update the system and staff y positions
    m_doc->PaperSize();
    int j, k, l, m;
    for (j = 0; j < m_doc->GetPageCount(); j++)
    {
        MusPage *page = (MusPage*)m_doc->m_children[j];
        m = 0; // staff number on the page
        int yy =  m_doc->m_pageHeight + m_doc->m_staffSize[ 0 ];
        for (k = 0; k < page->GetSystemCount(); k++) 
        {
            MusSystem *system = (MusSystem*)page->m_children[k];
            // only one measure per staff
            MusMeasure *measure = (MusMeasure*)system->m_children[0];
            MusStaff *staff = NULL;
            
            for (l = 0; l < measure->GetStaffCount(); l++)
            {
                staff = (MusStaff*)system->m_children[l];
                yy -= ecarts[m] * m_doc->m_interl[ staff->staffSize ];;
                staff->m_y_abs = yy;
                m++;
                
                // we are handling the first staff - update the position of the system as well
                if ( l == 0 ) { 
                    system->m_y_abs = yy;
                }
            }
        }
    }
    
	return true;
}

bool MusWWGInput::ReadFileHeader( MusWWGData *wwgData )
{

	char buffer[WIN_MAX_FNAME + WIN_MAX_EXT + 1];
	Read( buffer, 9 ); // (version 6)

	Read( &uint16, 2 );
	wwgData->maj_ver = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	wwgData->min_ver= wxUINT16_SWAP_ON_BE( uint16 );
    if ((wwgData->maj_ver != 2) && (wwgData->min_ver < 1)) 
	{
		wxLogWarning(_("Invalid version (%d.%d"), wwgData->maj_ver, wwgData->min_ver );
        return false;
	}
    Read( buffer, WIN_MAX_FNAME + WIN_MAX_EXT); // name (version 6) - unused
	Read( &uint32, 4 ); // filesize - unused
    wwgData->filesize= wxUINT32_SWAP_ON_BE( uint32 );
	Read( &uint16, 2 ); 
	wwgData->nbpage = wxUINT16_SWAP_ON_BE( uint16 ); // ~nbpage
	Read( &uint16, 2 ); 
	wwgData->nopage = wxUINT16_SWAP_ON_BE( uint16 ); // ~nopage
	Read( &uint16, 2 ); 
	wwgData->noligne = wxUINT16_SWAP_ON_BE( uint16 ); // ~noligne
	Read( &uint32, 4 );
	wwgData->xpos = wxUINT32_SWAP_ON_BE( uint32 );  // ~xpso
	Read( &m_doc->m_env.m_landscape, 1 ); // ~param - ~m_landscape
    m_doc->m_env.m_landscape = !m_doc->m_env.m_landscape; // flip it? why?
    Read( &m_doc->m_env.m_staffLineWidth, 1 ); // ~param - ~epLignesPortee
	Read( &m_doc->m_env.m_stemWidth, 1 ); // ~param - ~epQueueNotes
	Read( &m_doc->m_env.m_barlineWidth, 1 ); // ~param - ~epBarreMesure
	Read( &m_doc->m_env.m_beamWidth, 1 ); // ~param - ~epBarreValeur
	Read( &m_doc->m_env.m_beamWhiteWidth, 1 ); // ~param - ~epBlancBarreValeur
	Read( &m_doc->m_env.m_beamMaxSlope, 1 ); // ~param - ~m_beamMaxSlope
	Read( &m_doc->m_env.m_beamMinSlope, 1 ); // ~param - ~m_beamMinSlope
	Read( &int32, 4 );
	m_doc->m_pageWidth = wxINT32_SWAP_ON_BE( int32 ); // ~param - ~m_paperWidth
    m_doc->m_pageWidth *= 10; // changed
	Read( &int32, 4 );
	m_doc->m_pageHeight = wxINT32_SWAP_ON_BE( int32 ); // ~param - ~m_paperHeight
    m_doc->m_pageHeight *= 10; 
	Read( &int16, 2 );
	m_doc->m_pageTopMar = wxINT16_SWAP_ON_BE( int16 ); // ~param - ~margeSommet
	Read( &int16, 2 );
	m_doc->m_pageLeftMar = wxINT16_SWAP_ON_BE( int16 ); // ~param - ~margeGaucheImpaire
	Read( &int16, 2 );
	//m_doc->m_env.m_leftMarginEvenPage = wxINT16_SWAP_ON_BE( int16 ); // ignore it
	Read( &wwgData->Epais1, 1 ); // ~param - ~epais1
	Read( &wwgData->Epais2, 1 ); // ~param - ~epais2
	Read( &wwgData->Epais3, 1 ); // ~param - ~epais3
	Read( &wwgData->reserve[0], 1 ); // ~reserve_0
	Read( &wwgData->reserve[1], 1 ); // ~reserve_1

	return true;
}

bool MusWWGInput::ReadParametersMidi( MusWWGData *midi )
{
    int i;

	Read( &int32, 4 );
	midi->tempo = wxINT32_SWAP_ON_BE( int32 ); // tempo
	Read( &midi->minVeloc, 1 ); // ~minVeloc
	Read( &midi->maxVeloc, 1 );  // ~maxVeloc
	Read( &midi->collerBeamLiai, 1 ); // ~collerBeamLs
	Read( &midi->pedale, 1 ); // ~pedale
	Read( &midi->xResolution1, 1 ); // ~xResolution1
	Read( &midi->xResolution2, 1 ); // ~xResolution2
	Read( &midi->appogg, 1 );  // ~appogg
	Read( &midi->mes_proportion, 1 ); // ~mesProportion
	for (i = 0; i < MAXMIDICANAL; i++) // ~canal2patch
		Read( &midi->canal2patch[i], 1 );
	for (i = 0; i < MAXMIDICANAL; i++) // ~volume
		Read( &midi->volume[i], 1 );
	for (i = 0; i < MAXPORTNBRE + 1; i++) // ~piste2canal
		Read( &midi->piste2canal[i], 1 ); 
    
	return true;
}

bool MusWWGInput::ReadParameters2( MusWWGData *param )
{
	int i;
    
	Read( &param->transp_sil, 1 );// ~transpSil
	Read( &m_doc->m_env.m_smallStaffNum, 1 ); // ~rpPorteesNum
	Read( &m_doc->m_env.m_smallStaffDen, 1 ); // ~rpPorteesDen
	Read( &m_doc->m_env.m_graceNum, 1 ); // ~rpDiminNum
	Read( &m_doc->m_env.m_graceDen, 1 ); // ~rpDiminDen
	Read( &param->autoval_sil, 1 ); // ~autoValSil
	Read( &param->nbPagesEncontinu, 1 ); // ~nbPagesEnContinu
	Read( &param->vertCorrEcr, 1 ); // ~vertCorrEcr
	Read( &param->vertCorrPrn, 1 ); // ~vertCorrPrn	
	Read( &m_doc->m_env.m_stemCorrection, 1 ); // ~m_stemCorrection	
	Read( &param->epaisBezier, 1 ); // ~epaisBezier
	Read( &uint32, 4 );
	m_doc->m_env.m_headerType = wxUINT32_SWAP_ON_BE( uint32 ); // ~m_headerType
	m_doc->m_env.m_headerType &= (~FILEINFO); // ignore FILEINFO  
	for (i = 0; i < MAXPORTNBRE+1; i++) // ~transposition
		Read( &param->transposition[i], 1 );

	return true;
}

bool MusWWGInput::ReadFonts( MusWWGData *fonts )
{
	int i;

    for (i = 0; i < MAXPOLICES; i++) // lecture des polices
	{
		Read( &fonts->fonts[i].fonteJeu, 1 );
		Read( &fonts->fonts[i], MAXPOLICENAME );
	}

	return true;
}

bool MusWWGInput::ReadSeparator( )
{
	char buffer[7];

	Read( buffer, 7 );
	std::string str1( buffer, 0, 6 );
	std::string str2( WWG_SEPARATOR );
	if ( str1 != str2 ) {
		wxLogDebug("'%s' - '%s' - Error while reading separator", str1.c_str(), str2.c_str() );
        return false;
	}
    else
		return true;
}

bool MusWWGInput::ReadPage( MusPage *page )
{	
	int j;
    
    int indent;
    int indentDroite;
    int lrg_lign;   

    if ( !ReadSeparator() )
		return false;

	Read( &int32, 4 );
	//page->npage = wxINT32_SWAP_ON_BE( int32 );
	Read( &int16, 2 );
	short nbrePortees = wxINT16_SWAP_ON_BE( int16 );
    Read( &noMasqueFixe, 1 );	
	Read( &noMasqueVar, 1 );
	Read( &reserve, 1 );
	Read( &page->defin, 1 );
    //page->defin = 16;
	Read( &int32, 4 );
	indent = wxINT32_SWAP_ON_BE( int32 );	// page value in wwg
	Read( &int32, 4 );
	indentDroite = wxINT32_SWAP_ON_BE( int32 ); // page value in wwg
	Read( &int32, 4 );
	lrg_lign = wxINT32_SWAP_ON_BE( int32 ); // page value in wwg

    MusSystem *system = new MusSystem(); // first system of the page 
    system->m_systemLeftMar = indent;
    system->m_systemRightMar = indentDroite;
    //system->lrg_lign = lrg_lign;  
    int system_no = 0; // we don't have no members in system anymore 
    
    for (j = 0; j < nbrePortees; j++) 
	{
        MusMeasure *measure = new MusMeasure( false );
		MusStaff *staff = new MusStaff( j + 1 );
        MusLayer *layer = new MusLayer( 1 ); // only one layer per staff
		ReadStaff( staff, layer, j );
        if ( m_noLigne > system_no + 1 ) { // we have a new system
            page->AddSystem( system ); // add the current one
            system = new MusSystem(); // create the next one
            system_no = m_noLigne - 1; 
            system->m_systemRightMar = indentDroite;
            //system->lrg_lign = lrg_lign;
        }
        if ( m_indent ) {
			system->m_systemLeftMar = indent;
        }
        if ( m_indentDroite ) {      
            system->m_systemRightMar = m_indentDroite;
        }
        staff->AddLayer( layer );
        measure->AddStaff( staff );
        system->AddMeasure( measure );
	}
    // add the last system
    page->AddSystem( system );

	return true;

}
bool MusWWGInput::ReadStaff( MusStaff *staff, MusLayer *layer, int staffNo )
{	
	unsigned int k;

	Read( &uint32, 4 );
	unsigned int nblement = wxUINT32_SWAP_ON_BE( uint32 );
	Read( &uint16, 2 );
	layer->voix = wxUINT16_SWAP_ON_BE( uint16 );
	//wxLogDebug("voix: %d", layer->voix);
	Read( &uint16, 2 );
	staff->noGrp = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	staff->totGrp = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	m_noLigne = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	//staff->no = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &staff->armTyp, 1 );
	Read( &staff->armNbr, 1 );
	Read( &staff->notAnc, 1 );
	//staff->notAnc = true;// force notation ancienne
	Read( &staff->grise, 1 );
	Read( &staff->invisible, 1 );
	Read( &uint16, 2 );
	ecarts[staffNo] = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &staff->vertBarre, 1 );
	Read( &staff->brace, 1 );
	Read( &staff->staffSize, 1 );
    Read( &m_indent, 1 );
	Read( &m_indentDroite, 1 );
	Read( &staff->portNbLine, 1 );
	Read( &staff->accol, 1 );
	Read( &staff->accessoire, 1 );
	Read( &uint16, 2 );
	//staff->reserve = wxUINT16_SWAP_ON_BE( uint16 );
	
	unsigned char c;
	for ( k = 0; k < nblement; k++ )
	{
		Read( &c, 1 );
		if ( c == NOTE )
		{
			ReadNote( layer );
		}
		else if ( c == SYMB )
		{
			ReadSymbol( layer );
            // For the lyrics, we must attach them to the notes
            // We keep it and
            /*
            if ( (flag == CHAINE) && (fonte == LYRIC) ) {
                m_debord_str = std::string( (char*)pdebord,
                    (int)debordSize - 4);  // - sizeof( debordSize ) - sizeof( debordCode );
                m_lyrics.Add( symbol );
            }
            */ // ax2
		}
	}
			
	return true;
}

bool MusWWGInput::ReadNote( MusLayer *layer )
{
	
	ReadElementAttr( );
	Read( &sil, 1 );
	Read( &val, 1 );
	if ( ( sil == _SIL ) && ( val == DUR_256 ) ) // do we still want this? Should be an option somewhere
		val = CUSTOS;
	Read( &inv_val, 1 );
	Read( &note_point, 1 );
	Read( &stop_rel, 1 );
	Read( &acc, 1 );
	Read( &accInvis, 1 );
	Read( &q_auto, 1 );
	Read( &queue, 1 );
	Read( &stacc, 1 );
	Read( &oblique, 1 );
	Read( &queue_lig, 1 );
	Read( &chord, 1 );
	Read( &fchord, 1 );
	Read( &lat, 1 );
	Read( &haste, 1 );
	unsigned char c;
	Read( &c, 1 );
	code = (unsigned short)c; // code deplace dans element, unsigned SHORT !!
	Read( &tetenot, 1 );
	Read( &typStac, 1 );
	if ( existDebord ) 
		ReadDebord( );

    MusLayerElement *layer_element = NULL;
    if ( !sil ) { // note
        MusNote *note = new MusNote( );
        note->m_dur = val;
        note->m_oct = oct;
        note->m_pname = code;
        if ( drel ) {
            wxASSERT( !m_currentBeam );
            m_currentBeam = new MusBeam();
            m_isLastNoteInBeam = false;
        }
        if ( rel ) {
        }
        if ( frel ) {
            m_isLastNoteInBeam = true;
        }
        // ligature
        if ( ligat ) {
            // in WWG, all notes in a ligature get a ligat flag
            // it means that all notes (but the last) will get a LIG_INITIAL
            // seems to work like this.
            note->m_lig = LIG_INITIAL;
        }
        if ( fligat ) {
            note->m_lig = LIG_TERMINAL;
        }
        if ( oblique && (note->m_dur < DUR_2)) {
            note->m_ligObliqua = true;
        }
        // coloration
        if ( inv_val && (note->m_dur < DUR_2 )) {
            note->m_colored = true;
        }
        else if (oblique && (note->m_dur > DUR_2 )) { // ??
            note->m_colored = true;
        }
        layer_element = note;
    }
    else if (val == CUSTOS) {
        MusSymbol *custos = new MusSymbol( SYMBOL_CUSTOS );
        custos->m_oct = oct;
        custos->m_pname = code;
        layer_element = custos;
    }
    else { // rest
        MusRest *rest = new MusRest();
        rest->m_dur = val;
        rest->m_oct = oct;
        rest->m_pname = code;
        layer_element = rest;
    }
    
    // if we got something, add it to the Layer
    if ( layer_element ) {
        layer_element->m_x_abs = xrel;
        if ( m_currentBeam ) {
            m_currentBeam->AddElement( layer_element );
            m_currentBeam->m_x_abs = layer_element->m_x_abs;
            if ( m_isLastNoteInBeam ) {
                layer->AddElement( m_currentBeam );
                m_isLastNoteInBeam = false;
                m_currentBeam = NULL;
            }
        }
        else{
            layer->AddElement( layer_element );
        }
    }
       
    /*
    if ( !m_lyrics.IsEmpty() ) {
        int i = 0;
        int nb_lyrics = (int)m_lyrics.GetCount();
        for (i = 0; i < nb_lyrics; i++) {
            // default values
            MusSymbol1 *lyric = m_lyrics.Detach(0);
            lyric->dec_y = - STAFF_OFFSET;
            lyric->m_x_abs = x_abs - 20;
            lyric->m_note_ptr = note;	
            m_lyrics.Add( lyric );
        }
	}
    */ // ax2
        
    return true;
}

bool MusWWGInput::ReadSymbol( MusLayer *layer )
{	
	ReadElementAttr( );
	Read( &flag , 1 );
	Read( &calte , 1 );
	Read( &carStyle , 1 );
	Read( &carOrient , 1 );
	Read( &fonte , 1 );
	Read( &s_lie_l , 1 );
	Read( &symbol_point , 1 );
	Read( &uint16, 2 );
	code = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	l_ptch = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &int32, 4 );
	dec_y = wxINT32_SWAP_ON_BE( int32 );
	if ( existDebord ) 
		ReadDebord( );
    
    MusLayerElement *layer_element = NULL;        
    if ( flag == CLE ) {
        MusClef *clef = new MusClef();
        clef->m_clefId = (ClefId)code;
        layer_element = clef;

    } 
    else if ( flag == ALTER ) {
        MusSymbol *alter = new MusSymbol( SYMBOL_ACCID );
        alter->m_oct = oct;
        alter->m_pname = code;
        alter->m_accid = calte;
        layer_element = alter;
    }
    else if ( flag == IND_MES ) {
        MusMensur *mensur = new MusMensur( );
		if ( code & 64 )
		{
			//symbol->code = 64; ?? // ax2
			if ( calte == 0 )
				mensur->m_meterSymb = METER_SYMB_COMMON;
			else if ( calte == 1 )
				mensur->m_meterSymb = METER_SYMB_CUT;
			else if ( calte == 2 )
				mensur->m_meterSymb = METER_SYMB_2;
			else if ( calte == 3 )
				mensur->m_meterSymb = METER_SYMB_3;
			else if ( calte == 4 )
				mensur->m_meterSymb = METER_SYMB_2_CUT;
			else if ( calte == 5 )
				mensur->m_meterSymb = METER_SYMB_3_CUT;
			else
				wxLogWarning( _("Unkown mesure signe indication") );
		}
		else if ( code != 1 )
		{
			if ( code & 32  )
                mensur->m_sign = MENSUR_SIGN_O;
			else if ( code & 16 )
				mensur->m_sign = MENSUR_SIGN_C;
            else if ( code & 8 ) {
				mensur->m_sign = MENSUR_SIGN_C;
                mensur->m_reversed = true;
            }
			else
				wxLogWarning( _("Unkown mesure indication") );
			if ( code & 4 )
				mensur->m_slash = true;
			if ( code & 2 )
				mensur->m_dot = true;
		}
		else
		{
			mensur->m_num = durNum;
			mensur->m_numBase = durDen;
		}
        layer_element = mensur;
    }
    
    if ( layer_element ) {
        layer_element->m_x_abs = xrel;
        layer->AddElement( layer_element );
    }

	return true;
}

bool MusWWGInput::ReadElementAttr( )
{
	Read( &liaison , 1 );
	Read( &dliai , 1 );
	Read( &fliai , 1 );
	Read( &lie_up , 1 );
	Read( &rel , 1 );
	Read( &drel , 1 );
	Read( &frel , 1 );
	Read( &oct , 1 );
	Read( &dimin , 1 );
	Read( &grp , 1 );
	Read( &_shport , 1 );
	Read( &ligat , 1 );
	Read( &ElemInvisible , 1 );
	Read( &pointInvisible , 1 );
	Read( &existDebord , 1 );
	Read( &fligat , 1 );
	Read( &notschowgrp , 1 );
	Read( &cone , 1 );
	Read( &liaisonPointil , 1 );
	Read( &reserve1 , 1 );
	Read( &reserve2 , 1 );
	Read( &ottava , 1 );
	Read( &uint16, 2 );
	durNum = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	durDen = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	offset = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &int32, 4 );
	xrel = wxINT32_SWAP_ON_BE( int32 );
	
	return true;
}

bool MusWWGInput::ReadDebord( )
{
	Read( &uint16, 2 );
	debordSize = wxUINT16_SWAP_ON_BE( uint16 );
	Read( &uint16, 2 );
	debordCode = wxUINT16_SWAP_ON_BE( uint16 );

	int size = debordSize - 4; // - sizeof( debordSize ) - sizeof( debordCode );
	pdebord = malloc( size );
	Read( pdebord, size );
    //wxLogDebug("read debord %d, %s", size, (char*)pdebord );
	return true;
}


bool MusWWGInput::ReadPagination( MusWWGData *pagination )
{
	Read( &int16, 2 );
	pagination->p_numeroInitial = wxINT16_SWAP_ON_BE( int16 );
	Read( &pagination->p_aussiPremierPage, 1 );
	Read( &pagination->p_position, 1 );
	Read( &pagination->p_numeroFonte, 1 );
	Read( &pagination->p_carStyle, 1 );
	Read( &pagination->p_taille, 1 );
	Read( &pagination->p_offsetDuBord, 1 );
	return true;
}

bool MusWWGInput::ReadHeader( MusWWGData *header )
{
	char buffer[HEADER_FOOTER_TEXT + 1];
	Read( buffer, HEADER_FOOTER_TEXT );
	header->h_texte = buffer;
	Read( &header->h_aussiPremierPage, 1 );
	Read( &header->h_position, 1 );
	Read( &header->h_numeroFonte, 1 );
	Read( &header->h_carStyle, 1 );
	Read( &header->h_taille, 1 );
	Read(&header->h_offsetDuBord, 1 );
	return true;
}

bool MusWWGInput::ReadFooter( MusWWGData *footer )
{
	char buffer[HEADER_FOOTER_TEXT + 1];
	Read( buffer, HEADER_FOOTER_TEXT );
	footer->f_texte = buffer;
	Read( &footer->f_aussiPremierPage, 1 );
	Read( &footer->f_position, 1 );
	Read( &footer->f_numeroFonte, 1 );
	Read( &footer->f_carStyle, 1 );
	Read( &footer->f_taille, 1 );
	Read(&footer->f_offsetDuBord, 1 );
	return true;
}



