/////////////////////////////////////////////////////////////////////////////
// Name:        mus.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "mus.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "muswwg.h"
#include "musdef.h"


//----------------------------------------------------------------------------
// MusWWGData
//----------------------------------------------------------------------------

MusWWGData::MusWWGData()
{
    int i;

    // header footer //
	h_aussiPremierPage = 0;
	h_position = 0;
	h_numeroFonte = 0;
	h_carStyle = 0;
	h_taille = 0;
	h_offsetDuBord = 0;
    
    // pagination //
    p_numeroInitial = 0;	
	p_aussiPremierPage = 0;
	p_position = 0;	
	p_numeroFonte = 0;
	p_carStyle = 0;
	p_taille = 0;
	p_offsetDuBord = 0;
    
    // unused parameters
    transp_sil = 0;
    autoval_sil = 1;
    nbPagesEncontinu = 3;
    vertCorrEcr = 2;
    vertCorrPrn = 0;
    epaisBezier = 0;
	for (i = 0; i < MAXPORTNBRE+1; i++) // transposition
		transposition[i] = 0;
    
    // file header
	maj_ver = 2;
    min_ver = 1;
    nbpage = 0;
    nopage = 1;
    noligne = 0;
    xpos = 0;
    // unused
    filesize = 0;
    Epais1 = 1;
    Epais2 = 5;
    Epais3 = 9;
    reserve[0] = 0;
    reserve[1] = 0;
    
    // midi parameters
    tempo = 500000;
    minVeloc = 30;
    maxVeloc = 110;
    collerBeamLiai = 1;
    pedale = 1;
    xResolution1 = 20;
    xResolution2 = 15;
    appogg = 0;
    mes_proportion = 0;
	for (i = 0; i < MAXMIDICANAL; i++) // canal2patch
		canal2patch[i] = 0;
	for (i = 0; i < MAXMIDICANAL; i++) // volume
		volume[i] = 64;
	for (i = 0; i < MAXPORTNBRE + 1; i++) // piste2canal
		piste2canal[i] = 0;
      
    // fonts      
    for (i = 0; i < MAXPOLICES; i++) // chargement des polices vides
	{
		fonts[i].fonteJeu = 0;
		fonts[i].fonteNom[0] = 0;
	}
    // set Leipzig font
	fonts[0].fonteJeu = 1;
	strcpy(fonts[0].fonteNom, "Leipzig 4.2");
}

MusWWGData::~MusWWGData()
{
}



