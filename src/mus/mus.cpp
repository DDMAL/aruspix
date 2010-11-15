/////////////////////////////////////////////////////////////////////////////
// Name:        mus.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "mus.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "mus.h"


//----------------------------------------------------------------------------
// MusParameters
//----------------------------------------------------------------------------

MusParameters::MusParameters()
{
    orientation = true;
    EpLignesPortee = 2;
    EpQueueNote = 3;
    EpBarreMesure = 3;
    EpBarreValeur = 10;
    EpBlancBarreValeur = 5;
    beamPenteMax = 30;
    beamPenteMin = 10;
    pageFormatHor = 210;
    pageFormatVer = 297;
    MargeSOMMET = 0;
    MargeGAUCHEIMPAIRE = 10;
    MargeGAUCHEPAIRE = 10;

    // originally in MusParameters2
    rapportPorteesNum = 16;
    rapportPorteesDen = 20;
    rapportDiminNum = 3;
    rapportDiminDen = 4;
	hampesCorr = 0;
    entetePied = 0;
}

MusParameters::~MusParameters()
{
}



//----------------------------------------------------------------------------
// MusPosKey
//----------------------------------------------------------------------------

MusPosKey::MusPosKey()
{
}

MusPosKey::~MusPosKey()
{
}


//----------------------------------------------------------------------------
// MusPagination
//----------------------------------------------------------------------------

MusPagination::MusPagination()
{
	numeroInitial = 0;	
	aussiPremierPage = 0;
	position = 0;	
	numeroFonte = 0;
	carStyle = 0;
	taille = 0;
	offsetDuBord = 0;	
}

MusPagination::~MusPagination()
{
}



//----------------------------------------------------------------------------
// MusHeaderFooter
//----------------------------------------------------------------------------

MusHeaderFooter::MusHeaderFooter()
{
	aussiPremierPage = 0;
	position = 0;
	numeroFonte = 0;
	carStyle = 0;
	taille = 0;
	offsetDuBord = 0;
}

MusHeaderFooter::~MusHeaderFooter()
{
}



//----------------------------------------------------------------------------
// MusFileHeader
//----------------------------------------------------------------------------

MusFileHeader::MusFileHeader()
{
	maj_ver = 2;
    min_ver = 1;
    nbpage = 0;
    nopage = 1;
    noligne = 0;
    xpos = 0;
}

MusFileHeader::~MusFileHeader()
{
}






