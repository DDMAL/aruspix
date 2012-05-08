/////////////////////////////////////////////////////////////////////////////
// Name:        musdoc.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_DOC_H__
#define __MUS_DOC_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musdc.h"

class MusPageFunctor;
class MeiDocument;
class MusDiv;

#include "musobject.h"
#include "musdef.h"
#include "muswwg.h"



//----------------------------------------------------------------------------
// MusParameters
//----------------------------------------------------------------------------

// Should me move it at the document level? Or at the layout level?

/** 
 * This class contains the document parameters.
 * It remains from the Wolfgang parameters strcuture.
 */
class MusParameters 
{
public:
    // constructors and destructors
    MusParameters();
    virtual ~MusParameters();
    
        
public:
    /** orientation de la page */
    char orientation;
    /** epaisseur des lignes de portees */
    unsigned char EpLignesPortee;
    /** epaisseur des hampes */
    unsigned char EpQueueNote;
    /** epaisseur des barres de mesures */
    unsigned char EpBarreMesure;
    /** epaisseur des barres de valeur */
    unsigned char EpBarreValeur;
    /** epaisseur de l'espace entre les barres de valeur */
    unsigned char EpBlancBarreValeur;
    /** pente maximale des barres de valeur */
    unsigned char beamPenteMax;
    /** pente minimale des barres de valeur */
    unsigned char beamPenteMin;
    /** largueur de la page */
    int pageFormatHor;
    /** hauteur de la page */
    int pageFormatVer;
    /** marge au sommet */
    short MargeSOMMET;
    /** marge gauche sur la pages impaires */
    short MargeGAUCHEIMPAIRE;
    /** marge droite sur les pages paires */
    short MargeGAUCHEPAIRE;
        
    /* the following variables were originally in MusParameters2 */
    /** rapport entre grande et petite portee, numerateur */
    unsigned char rapportPorteesNum;
    /** rapport entre grande et petite portee, denominateur */
    unsigned char rapportPorteesDen;
    /** rapport entre grand et petit element, numerateur */
    unsigned char rapportDiminNum;
    /** rapport entre grand et petit element, denominateur */
    unsigned char rapportDiminDen;
    /** correction de la position de hampes*/
    signed char hampesCorr;
    /** type d'entete et de pied de page. */
    unsigned int entetePied;
    
    // not originally in Wolfgang
    int notationMode; // since Aruspix 1.6.1
    
private:
    
};


//----------------------------------------------------------------------------
// MusDoc
//----------------------------------------------------------------------------

/** 
 * This class is a hold the data and corresponds to the model of a MVC design pattern.
 */
class MusDoc
{
	//friend class MusFileOutputStream;
	friend class MusFileInputStream;

public:
    // constructors and destructors
    MusDoc();
    virtual ~MusDoc();
	
	void AddDiv( MusDiv *div );
	
	void AddLayout( MusLayout *layout );
    
    void CheckIntegrity();
    
    /*
     * Clear the content of the document.
     */ 
    void Reset();
    
	/*
     Calcul la taille de la page = calculFormatPapier()
     */
	void PaperSize( );
	/**
     Met ‡ jour la table des fontes en fonction de la definition de page
	 Doit etre appelee apres changement de definition de page
     */
	void UpdatePageFontValues();
    /**
     Initialise les donnees de visualisation par page
     */
	virtual void UpdatePageValues();
    /**
     Initialise les donnees de polices (music and lyrics)
     */
	virtual void UpdateFontValues();

    
    // moulinette
    virtual void Process(MusPageFunctor *functor, wxArrayPtrVoid params );
    void GetNumberOfVoices( int *min_voice, int *max_voice );
    MusLaidOutStaff *GetVoice( int i );

    MeiDocument *GetMeiDocument();
    void SetMeiDocument(MeiDocument *doc);
    
private:
    // method calculating the font size
    int CalcMusicFontSize( );
    int CalcNeumesFontSize( );
    
public:
    /** nom complet du fichier */
    wxString m_fname;
    /** FileHeader du fichier - contient Parametres */
    MusParameters m_parameters;
    
    /**  */
    ArrayOfMusLayouts m_layouts;
    ArrayOfMusDivs m_divs;
	
    /** Data loaded from the Wolfgang files but unused **/
	MusWWGData m_wwgData;
    
    // Previously in MusRC
    /** valeur du pas (10 par defaut) */
    int _pas;
    /** valeur du bond (60 par defaut) */
    int _bond;
    /** valeur du pas3 (3 * pas par defaut) */
    int _pas3;
    /** valeurs d'un espace (demi-note) grand et petit (10 et 8 par defaut)*/
    int _espace[2];
    /** valeurs d'un interligne grand et petit (20 et 16 par defaut)*/
    int _interl[2];
    /** valeurs d'une portee grande et petite (80 et 64 par defaut)*/
    int _portee[2];
    /** valeurs d'une octave grand et petit (70 et 56 par defaut)*/
    int _octave[2];
    /** hauteur de la fonte (100 par defaut) */
    int hautFont;
	int hautFontAscent[2][2]; // avec correction par platforme
    /** rapport entre grande et petite portees (16 / 20 par defaut) */
    int RapportPortee[2];
    /** rapport en element normal et element diminue (3 / 4 par defaut)*/
    int RapportDimin[2];
    /** valeurs d'une barre de valeur grande et petite (10 et 6 par defaut)*/
    int DELTANbBAR[2];
    /** valeurs d'un espace blanc entre les barres de valeur grand et petit (6 et 4 par defaut)*/
    int DELTABLANC[2];
    /** tailles des fontes normales et diminues sur des portees grandes et petites */
    int nTailleFont[2][2];
    /** tailles des notes normales et diminues sur des portees grandes et petites */
    int rayonNote[2][2];
    /** tailles des lignes aditionnelles normales et diminues sur des portees grandes et petites */
    int ledgerLine[2][3];
    /** tailles des fontes normales et diminues sur des portees grandes et petites */
    int largeurBreve[2];
    /** tailles des alterations normales et diminues sur des portees grandes et petites */
    int largAlter[2][2];
    /** ???? */
    float v_unit[2];
    /** ???? */
    float v4_unit[2];
    /** ???? */
    int DELTABAR;
    /** fontes actuelles mises a jour selon la taille du zoom */
    MusFontInfo m_activeFonts[2][2];				
    /** fonte Leipzig par defaut */
    MusFontInfo m_ftLeipzig;
	
	MusFontInfo m_activeChantFonts[2][2];
	/** FestaDiesA for neume notation */
	MusFontInfo m_ftFestaDiesA;
    /** fontes actuelles mises a jour selon la taille du zoom */
    MusFontInfo m_activeLyricFonts[2];
    /** fonte Leipzig par defaut */
    MusFontInfo m_ftLyrics;
	
	float beamPenteMin, beamPenteMx;
	int pageFormatHor, pageFormatVer;
	//int margeMorteHor, margeMorteVer;
	//int portNoIndent, portIndent;
	int mrgG;
	int mesureNum, mesureDen;
	float MesVal;

    /** indique si la definition de page poue laquelle fontes actuelles est a jour */
    int m_charDefin;

private:
    MeiDocument *m_meidoc;
    
	
};


//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

/** 
 * This class is a base class for file output stream classes.
 * It is not an abstract class but should not be instanciate directly.
 */ 
class MusFileOutputStream: public wxFileOutputStream
{
public:
    // constructors and destructors
    MusFileOutputStream( MusDoc *doc, wxString filename );
	MusFileOutputStream( MusDoc *doc, int fd );
    MusFileOutputStream() {};
	//MusFileOutputStream( MusDoc *file, wxFile *wxfile );
    virtual ~MusFileOutputStream();
    
    bool ExportFile( ) { return true; }
	/*
    virtual bool WriteFileHeader( const MusFileHeader *header ) { return true; }
	virtual bool WriteParametersMidi( ) { return true; }
	virtual bool WriteParameters2( const MusParameters *param ) { return true; }
	virtual bool WriteFonts( ) { return true; }
	virtual bool WriteSeparator( ) { return true; }
	virtual bool WritePage( const MusPage *page ) { return true; }
    virtual bool WriteSystem( const MusSystem *system ) { return true; }
	virtual bool WriteStaff( const MusLaidOutStaff *staff ) { return true; }
	virtual bool WriteNote( const MusNote1 *note ) { return true; }
	virtual bool WriteSymbol( const MusSymbol1 *symbol ) { return true; }
	virtual bool WriteElementAttr( const MusElement *element ) { return true; }
	virtual bool WriteDebord( const MusElement *element ) { return true; }
	virtual bool WriteLyric( const MusElement * element ) { return true; }
	virtual bool WritePagination( const MusWWGData *pagination ) { return true; }
	virtual bool WriteHeaderFooter( const MusWWGData *headerfooter) { return true; }
    */
    
public:
    
protected:
    MusDoc *m_doc;

};

//----------------------------------------------------------------------------
// MusFileInputStream
//----------------------------------------------------------------------------

/** 
 * This class is a base class for file input stream classes.
 * It is not an abstract class but should not be instanciate directly.
 */ 
class MusFileInputStream: public wxFileInputStream
{
public:
    // constructors and destructors
    MusFileInputStream( MusDoc *doc, wxString filename );
    MusFileInputStream( MusDoc *doc, int fr );
    MusFileInputStream() {};
    virtual ~MusFileInputStream();
    
    // read
    bool ImportFile( ) { return true; }
    /*
	virtual bool ReadFileHeader( MusFileHeader *header ) { return true; }
	virtual bool ReadParametersMidi( ) { return true; }
	virtual bool ReadParameters2( MusParameters *param ) { return true; }
	virtual bool ReadFonts( ) { return true; }
	virtual bool ReadSeparator( ) { return true; }
	virtual bool ReadPage( MusPage *page ) { return true; }
    virtual bool ReadSystem( MusSystem *system ) { return true; }
	virtual bool ReadStaff( MusLaidOutStaff *staff ) { return true; }
	virtual bool ReadNote( MusNote1 *note ) { return true; }
	virtual bool ReadSymbol( MusSymbol1 *symbol ) { return true; }
	virtual bool ReadElementAttr( MusElement *element ) { return true; }
	virtual bool ReadDebord( MusElement *element ) { return true; }
	virtual bool ReadLyric( MusElement * element ) { return true; }
	virtual bool ReadPagination( MusWWGData *pagination ) { return true; }
	virtual bool ReadHeaderFooter( MusWWGData *headerfooter) { return true; }
    */
    
public:
    
protected:
    MusDoc *m_doc;

};


#endif
