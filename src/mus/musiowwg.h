/////////////////////////////////////////////////////////////////////////////
// Name:        musiowwg.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOWWG_H__
#define __MUS_IOWWG_H__

#ifdef __GNUG__
    #pragma interface "musiowwg.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musfile.h"


class MusFont;

WX_DECLARE_OBJARRAY( MusFont, ArrayOfMusFonts);

//----------------------------------------------------------------------------
// MusFont
//----------------------------------------------------------------------------

class MusFont
{
public:
    // constructors and destructors
    MusFont();
    virtual ~MusFont();
    
        
public:
    signed char fonteJeu;
	wxString fonteNom;
    
private:
    
};

//----------------------------------------------------------------------------
// MusParametersMidi
//----------------------------------------------------------------------------

class MusParametersMidi
{
public:
    // constructors and destructors
    MusParametersMidi();
    virtual ~MusParametersMidi();
    
        
public:
        /** tempo */
    long tempo;
    /** ???? */
    unsigned char minVeloc;
    /** ???? */
    unsigned char maxVeloc;
    /** style legato de jeu. <pre>0 = aucun<br>1 = beam<br>2 = liaison</pre> */
    unsigned char collerBeamLiai;
    /** valeur de la pedale. <pre>1 = silences<br>2 = pedale</pre> */
    unsigned char pedale;
    /** tolerance horizontale x max pour la simultaneite */
    unsigned char xResolution1;
    /** tolerance horizontale x max en alignement verticale pour la simultaneite */
    unsigned char xResolution2;
    /** traiter les notes ornementales comme des appogiatures */
    unsigned char appogg;
    /** traiter les mesures commes de proportions temporelles */
    unsigned char mes_proportion;
    /** table des numeros de canal pour chaque instrument */
	unsigned char canal2patch[MAXMIDICANAL];	// contient le no d'instr pour chaque canal
	unsigned char volume[MAXMIDICANAL];	// 64, volume, entre 0 et 128
	unsigned char piste2canal[MAXPORTNBRE+1];
    
private:
    
};


//----------------------------------------------------------------------------
// MusWWGOutput
//----------------------------------------------------------------------------

class MusWWGOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusWWGOutput( MusFile *file, wxString filename );
    virtual ~MusWWGOutput();
    
    virtual bool ExportFile( );
	virtual bool WriteFileHeader( const MusFileHeader *header );
	virtual bool WriteParametersMidi( );
	virtual bool WriteParameters2( const MusParameters *param );
	virtual bool WriteFonts( );
	virtual bool WriteSeparator( );
	virtual bool WritePage( const MusPage *page );
	virtual bool WriteStaff( const MusStaff *staff );
	virtual bool WriteNote( const MusNote *note );
	virtual bool WriteSymbol( const MusSymbol *symbol );
	virtual bool WriteElementAttr( const MusElement *element );
	virtual bool WriteDebord( const MusElement *element );
	virtual bool WritePagination( const MusPagination *pagination );
	virtual bool WriteHeaderFooter( const MusHeaderFooter *headerfooter);
    
private:
    wxUint16 uint16;
	wxInt16 int16;
	wxUint32 uint32;
	wxInt32 int32;
	wxString m_filename;

private:
	};


//----------------------------------------------------------------------------
// MusWWGInput
//----------------------------------------------------------------------------

class MusWWGInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusWWGInput( MusFile *file, wxString filename);
    virtual ~MusWWGInput();
    
    virtual bool ImportFile( );
	virtual bool ReadFileHeader( MusFileHeader *header );
	virtual bool ReadParametersMidi( );
	virtual bool ReadParameters2( MusParameters *param );
	virtual bool ReadFonts( );
	virtual bool ReadSeparator( );
	virtual bool ReadPage( MusPage *page );
	virtual bool ReadStaff( MusStaff *staff );
	virtual bool ReadNote( MusNote *note );
	virtual bool ReadSymbol( MusSymbol *symbol );
	virtual bool ReadElementAttr( MusElement *element );
	virtual bool ReadDebord( MusElement *element );
	virtual bool ReadPagination( MusPagination *pagination );
	virtual bool ReadHeaderFooter( MusHeaderFooter *headerfooter);
    
private:
    wxUint16 uint16;
	wxInt16 int16;
	wxUint32 uint32;
	wxInt32 int32;
    // 
    ArrayOfMusSymbols m_lyrics;

private:
	};


#endif
