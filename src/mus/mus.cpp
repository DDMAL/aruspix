/////////////////////////////////////////////////////////////////////////////
// Name:        wg.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_WG

#ifdef __GNUG__
    #pragma implementation "mus.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "mus.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// WgFont
//----------------------------------------------------------------------------

WgFont::WgFont()
{
}

WgFont::~WgFont()
{
}

// WDR: handler implementations for WgFont


//----------------------------------------------------------------------------
// WgParametersMidi
//----------------------------------------------------------------------------

WgParametersMidi::WgParametersMidi()
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

WgParametersMidi::~WgParametersMidi()
{
}

// WDR: handler implementations for WgParametersMidi


//----------------------------------------------------------------------------
// WgParameters2
//----------------------------------------------------------------------------

WgParameters2::WgParameters2()
{
	transp_sil = 0;
    rapportPorteesNum = 16;
    rapportPorteesDen = 20;
    rapportDiminNum = 3;
    rapportDiminDen = 4;
    autoval_sil = 1;
    nbPagesEncontinu = 3;
    vertCorrEcr = 2;
    vertCorrPrn = 0;
    //hampesCorr = 2;
	hampesCorr = 0;
    epaisBezier = 0;
    entetePied = 0;

	int i;
	for (i = 0; i < MAXPORTNBRE+1; i++) // transposition
		transposition[i] = 0;
}

WgParameters2::~WgParameters2()
{
}

// WDR: handler implementations for WgParameters2


//----------------------------------------------------------------------------
// WgParameters
//----------------------------------------------------------------------------

WgParameters::WgParameters()
{
    orientation = true;
    EpLignesPortee = 1;
    EpQueueNote = 3;
    EpBarreMesure = 3;
    EpBarreValeur = 10;
    EpBlancBarreValeur = 5;
    beamPenteMax = 30;
    beamPenteMin = 10;
    pageFormatHor = 210;
    pageFormatVer = 297;
    MargeSOMMET = 0;
    MargeGAUCHEIMPAIRE = 9;
    MargeGAUCHEPAIRE = 9;
    Epais1 = 1;
    Epais2 = 5;
    Epais3 = 9;
}

WgParameters::~WgParameters()
{
}

// WDR: handler implementations for WgParameters


//----------------------------------------------------------------------------
// WgPosKey
//----------------------------------------------------------------------------

WgPosKey::WgPosKey()
{
}

WgPosKey::~WgPosKey()
{
}

// WDR: handler implementations for WgPosKey


//----------------------------------------------------------------------------
// WgPagination
//----------------------------------------------------------------------------

WgPagination::WgPagination()
{
	numeroInitial = 0;	
	aussiPremierPage = 0;
	position = 0;	
	numeroFonte = 0;
	carStyle = 0;
	taille = 0;
	offsetDuBord = 0;	
}

WgPagination::~WgPagination()
{
}

// WDR: handler implementations for WgPagination


//----------------------------------------------------------------------------
// WgHeaderFooter
//----------------------------------------------------------------------------

WgHeaderFooter::WgHeaderFooter()
{
	aussiPremierPage = 0;
	position = 0;
	numeroFonte = 0;
	carStyle = 0;
	taille = 0;
	offsetDuBord = 0;
}

WgHeaderFooter::~WgHeaderFooter()
{
}

// WDR: handler implementations for WgHeaderFooter


//----------------------------------------------------------------------------
// WgFileHeader
//----------------------------------------------------------------------------

WgFileHeader::WgFileHeader()
{
	maj_ver = 2;
    min_ver = 1;
    filesize = 0;
    nbpage = 0;
    nopage = 1;
    noligne = 0;
    xpos = 0;
	reserve[0] = 0;
	reserve[0] = 0;
}

WgFileHeader::~WgFileHeader()
{
}

// WDR: handler implementations for WgFileHeader



#endif // AX_WG

