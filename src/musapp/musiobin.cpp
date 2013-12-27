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

#include "mus/muspage.h"
#include "mus/mussystem.h"
#include "mus/musstaff.h"
#include "mus/musmeasure.h"
#include "mus/muslayer.h"
#include "mus/muslayerelement.h"
#include "mus/musbarline.h"
#include "mus/musclef.h"
#include "mus/musmensur.h"
#include "mus/musnote.h"
#include "mus/mussymbol.h"
#include "mus/musrest.h"


//----------------------------------------------------------------------------
// MusBinInput_1_X
//----------------------------------------------------------------------------

#define BIN_SEPARATOR "#\376\364\365\376#"  // "#˛Ùı˛#"

/* note types */
#define NOTE 0	/* pour type (bit) de struct element */
#define SYMB 1

/* symbol flags */
#define BARRE 0
#define ALTER 1
#define PNT	2
#define IND_MES 5
#define CLE 6

#define CUSTOS 11 // ax2 double check in 1.6


MusBinInput_1_X::MusBinInput_1_X( MusDoc *doc, wxString filename, int flag ) :
	wxFileInputStream( filename )
{
    m_doc = doc;
	m_flag = flag;
	m_vmaj = m_vmin = m_vrev = 10000; // arbitrary version, we assume we will never reach version 10000...
}

MusBinInput_1_X::~MusBinInput_1_X()
{
}

bool MusBinInput_1_X::ImportFile( )
{
	int i;

	if ( !IsOk() )
	{
		wxLogMessage(_("Cannot read file '%s'"), m_doc->m_fname.c_str() );
		return false;
	}
    
    // reset the MusDoc and create the logical tree
    m_doc->Reset( Transcription );	

    unsigned short nbpage;
    ReadFileHeader( &nbpage ); // fileheader
    
    for (i = 0; i < nbpage; i++ )
	{
		MusPage *page = new MusPage();
		ReadPage( page );
        m_doc->AddPage( page );
    }
    
    // update the system and staff y positions
    int j, k, l, m;
    for (j = 0; j < m_doc->GetChildCount(); j++)
    {
        MusPage *page = (MusPage*)m_doc->m_children[j];
        
        m_doc->SetRendPage( page );
        
        m = 0; // staff number on the page
        int yy =  m_doc->m_rendPageHeight;
        for (k = 0; k < page->GetSystemCount(); k++) 
        {
            MusSystem *system = (MusSystem*)page->m_children[k];
            // we always have one single measure per system
            MusMeasure *measure = (MusMeasure*)system->m_children[0];
            MusStaff *staff = NULL;
            
            for (l = 0; l < measure->GetStaffCount(); l++)
            {
                staff = (MusStaff*)measure->m_children[l];
                yy -= ecarts[m] * m_doc->m_rendInterl[ staff->staffSize ];
                staff->m_yAbs = yy;
                m++;
                
                // we are handling the first staff - update the position of the system as well
                if ( l == 0 ) { 
                    system->m_yAbs = yy;
                }
            }
        }
    }
    
	return true;
}

bool MusBinInput_1_X::ReadFileHeader( unsigned short *nbpage )
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
	*nbpage = wxUINT16_SWAP_ON_BE( uint16 ); // nbpage / ignored
	Read( &uint16, 2 ); 
	//header->nopage = wxUINT16_SWAP_ON_BE( uint16 ); // nopage / ignored
	Read( &uint16, 2 ); 
	//header->noligne = wxUINT16_SWAP_ON_BE( uint16 ); // noligne / ignored
	Read( &uint32, 4 );
	//header->xpos = wxUINT32_SWAP_ON_BE( uint32 );  // xpso / ignored
	Read( &m_doc->m_env.m_landscape, 1 ); // param - orientation
    m_doc->m_env.m_landscape = !m_doc->m_env.m_landscape; // reverse it?
	Read( &m_doc->m_env.m_staffLineWidth, 1 ); // param - epLignesPortee
    m_doc->m_env.m_staffLineWidth = 1;	// force it
	Read( &m_doc->m_env.m_stemWidth, 1 ); // param - epQueueNotes
    m_doc->m_env.m_stemWidth = 2; // force it
	Read( &m_doc->m_env.m_barlineWidth, 1 ); // param - epBarreMesure
	Read( &m_doc->m_env.m_beamWidth, 1 ); // param - epBarreValeur
	Read( &m_doc->m_env.m_beamWhiteWidth, 1 ); // param - epBlancBarreValeur
	Read( &int32, 4 );
	m_doc->SetPageHeight( wxINT32_SWAP_ON_BE( int32 ) * 10 ); // param - pageFormatHor
	Read( &int32, 4 );
	m_doc->SetPageWidth( wxINT32_SWAP_ON_BE( int32 ) * 10 ); // param - pageFormatVer
	Read( &int16, 2 );
	m_doc->SetPageTopMar( wxINT16_SWAP_ON_BE( int16 ) ); // param - margeSommet
	Read( &int16, 2 );
	m_doc->SetPageLeftMar( wxINT16_SWAP_ON_BE( int16 ) ); // param - margeGaucheImpaire
	Read( &int16, 2 );
	//m_doc->m_env.m_leftMarginEvenPage = wxINT16_SWAP_ON_BE( int16 ); // param - margeGauchePaire - ignore it
    
	Read( &m_doc->m_env.m_smallStaffNum, 1 ); // rpPorteesNum
	Read( &m_doc->m_env.m_smallStaffDen, 1 ); // rpPorteesDen
	Read( &m_doc->m_env.m_graceNum, 1 ); // rpDiminNum
	Read( &m_doc->m_env.m_graceDen, 1 ); // rpDiminDen	
	Read( &m_doc->m_env.m_stemCorrection, 1 ); // hampesCorr
    m_doc->m_env.m_stemCorrection = 1;	// force it 

	if ( Mus::GetFileVersion(m_vmaj, m_vmin, m_vrev) < Mus::GetFileVersion(1, 6, 1) )
		return true; // following values where added in 1.6.1
    // 1.6.1
    Read( &int32, 4 );
    m_doc->m_env.m_notationMode = wxINT32_SWAP_ON_BE( int32 );

	return true;
}


bool MusBinInput_1_X::ReadSeparator( )
{
	char buffer[7];

	Read( buffer, 7 );
	wxString str1( buffer, 0, 6 );
	wxString str2( BIN_SEPARATOR );
	if ( str1 != str2 ) {
		//wxLogDebug("'%s' - '%s' - Error while reading separator", str1.c_str(), str2.c_str() );
		return false;
    }
	else
		return true;
}

bool MusBinInput_1_X::ReadPage( MusPage *page )
{
	int j;
    
    int indent;
    int indentDroite;
    int lrg_lign; 
    char unused;

    if ( !ReadSeparator() )
		return false;

	Read( &int32, 4 );
	//page->npage = wxINT32_SWAP_ON_BE( int32 );
	Read( &int16, 2 );
	short nbrePortees = wxINT16_SWAP_ON_BE( int16 );
    Read( &unused, 1 );	
	Read( &unused, 1 );
	Read( &unused, 1 );
	Read( &page->defin, 1 );
	Read( &int32, 4 );
	indent = wxINT32_SWAP_ON_BE( int32 );	
	Read( &int32, 4 );
	indentDroite = wxINT32_SWAP_ON_BE( int32 ); 
	Read( &int32, 4 );
	lrg_lign = wxINT32_SWAP_ON_BE( int32 );
    
    /*
    for (j = 0; j < page->nbrePortees; j++) 
	{
		MusStaff *staff = new MusStaff();
		ReadStaff( staff );
        staff->voix = (j % 2 == 0) ? 1 : 0; // add voices
		page->m_staves.Add( staff );
	}
    */
    
    MusSystem *system = new MusSystem(); // first system of the page
    MusMeasure *measure = new MusMeasure( false );
    system->m_systemLeftMar = indent;
    system->m_systemRightMar = indentDroite;
    //system->lrg_lign = lrg_lign;  
    int system_no = 0; // we don't have no members in system anymore
    
    for (j = 0; j < nbrePortees; j++) 
	{
		MusStaff *staff = new MusStaff( j + 1 );
        MusLayer *layer = new MusLayer( 1 ); // we have always on layer per staff
		ReadStaff( staff, layer, j );
        if ( m_noLigne > system_no + 1 ) { // we have a new system
            system->AddMeasure( measure );
            page->AddSystem( system ); // add the current one
            system = new MusSystem(); // create the next one
            measure = new MusMeasure( false );
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
	}
    // add the last system
    system->AddMeasure( measure );
    page->AddSystem( system );    

	return true;

}
bool MusBinInput_1_X::ReadStaff( MusStaff *staff, MusLayer *layer, int staffNo )
{
	unsigned int k;

	Read( &uint32, 4 );
	unsigned int nblement  = wxUINT32_SWAP_ON_BE( uint32 );
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
	//staff->no = wxUINT16_SWAP_ON_BE( uint16 ); // ignored
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
    Read( &int32, 4 );
    m_indent = wxINT32_SWAP_ON_BE( int32 );
	Read( &m_indentDroite, 1 );
	Read( &staff->portNbLine, 1 );
	Read( &staff->accol, 1 );
	Read( &staff->accessoire, 1 );
	Read( &uint16, 2 );
	//staff->reserve = wxUINT16_SWAP_ON_BE( uint16 ); // ignored
	
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
		}
        /*
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
			Read( elem->m_im_docname.GetWriteBuf( wxINT32_SWAP_ON_BE( int32 ) + 1 ) , wxINT32_SWAP_ON_BE( int32 ) + 1 );
			elem->m_im_docname.UngetWriteBuf();
			Read( &int32, 4 );
			elem->m_im_staff = wxINT32_SWAP_ON_BE( int32 );
			Read( &int32, 4 );
			elem->m_im_pos = wxINT32_SWAP_ON_BE( int32 );
			Read( &int32, 4 );
			elem->m_cmp_flag = wxINT32_SWAP_ON_BE( int32 );
		}
        */
	}
			
	return true;
}


bool MusBinInput_1_X::ReadSymbol( MusLayer *layer, bool isLyric )
{
	ReadElementAttr(  );
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
    else if ( flag == BARRE ) {
        MusBarline *barline = new MusBarline();
        layer_element = barline;
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
    else if ( flag == PNT ) {
        MusSymbol *dot = new MusSymbol( SYMBOL_DOT );
        dot->m_oct = oct;
        dot->m_pname = code;
        layer_element = dot;
    }
    
    if ( layer_element ) {
        layer_element->m_xAbs = x_abs;
        layer->AddElement( layer_element );
    }
    
	if ( isLyric ) {
        // this will read the lyric but it will be ignored
        ReadLyric( );
    }
     
	return true;
}

bool MusBinInput_1_X::ReadNote( MusLayer *layer )
{
	ReadElementAttr( );
	Read( &sil, 1 );
	Read( &val, 1 );
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
	
    MusLayerElement *layer_element = NULL;
    if (val == CUSTOS) {
        MusSymbol *custos = new MusSymbol( SYMBOL_CUSTOS );
        custos->m_oct = oct;
        custos->m_pname = code;
        layer_element = custos;
    }
    else if ( !sil ) { // note
        MusNote *note = new MusNote( );
        note->m_dur = val;
        note->m_oct = oct;
        note->m_pname = code;
        // stem
        if ( !q_auto && ((note->m_dur ==  DUR_LG) || (note->m_dur > DUR_1 ))) {
            note->m_stemDir = 1;
        }
        // ligature
        if ( ligat && (note->m_dur >  DUR_LG) && (note->m_dur < DUR_2 )) {
            note->m_lig = LIG_INITIAL;
        }
        // coloration
        if ( inv_val && (note->m_dur < DUR_2 )) {
            note->m_colored = true;
        }
        else if (inv_val && (note->m_dur > DUR_4 )) {
            note->m_ligObliqua = true;
        }
        layer_element = note;
    }
    else { // rest
        MusRest *rest = new MusRest();
        rest->m_dur = val;
        rest->m_oct = oct;
        rest->m_pname = code;
        layer_element = rest;
    }
    
    // if we got something, add it to the LaidOutLayer
    if ( layer_element ) {
        layer_element->m_xAbs = x_abs;
        layer->AddElement( layer_element );
    }
    
	char count;
	Read( &count, 1 );
	
	for ( int i = 0; i < count; i++ ) {
        char TYPE;
		Read( &TYPE, 1 );
		ReadSymbol( NULL, true ); // the lyric will be ignored
	}
	
	return true;
}

bool MusBinInput_1_X::ReadNeume( )
{
    // ...
	return true;
}

bool MusBinInput_1_X::ReadElementAttr( )
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
	x_abs = wxINT32_SWAP_ON_BE( int32 );
	
	return true;
}


bool MusBinInput_1_X::ReadLyric( )
{
	Read( &int32, 4 );
    wxString to_be_ignored;
	Read( to_be_ignored.GetWriteBuf( wxINT32_SWAP_ON_BE( int32 ) + 1 ) , wxINT32_SWAP_ON_BE( int32 ) + 1 );
	to_be_ignored.UngetWriteBuf();
	return true;
}

/*
bool MusBinInput_1_X::ReadPagination( MusPagination *pagination )
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

bool MusBinInput_1_X::ReadHeaderFooter( MusHeaderFooter *headerfooter)
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
*/




