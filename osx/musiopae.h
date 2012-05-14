/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMEI_H__
#define __MUS_IOMEI_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musdoc.h"
#include "muslayer.h"

#include <mei/meielement.h>
#include <mei/exceptions.h>
#include <mei/meidocument.h>
#include <mei/xmlimport.h>

#include <mei/header.h> 
#include <mei/cmn.h>
#include <mei/shared.h>
#include <mei/mensural.h>

using namespace mei;

//----------------------------------------------------------------------------
// MusMeiOutput
//----------------------------------------------------------------------------

/**
 * This class is a file output stream for writing MEI files.
 * It uses the libmei C++ library.
 * Not implemented.
*/
class MusMeiOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusMeiOutput( MusDoc *doc, wxString filename );
    virtual ~MusMeiOutput();
    
    virtual bool ExportFile( );

private:
	//
	std::string DurToStr(int dur);
	std::string OctToStr(int oct);
	std::string PitchToStr(int pitch);

    
public:


private:
};


//----------------------------------------------------------------------------
// MusMeiInput
//----------------------------------------------------------------------------

/**
 * This class is a file input stream for reading MEI files.
 * It uses the libmei C++ library.
 * Under development.
*/
class MusMeiInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusMeiInput( MusDoc *doc, wxString filename );
    virtual ~MusMeiInput();
    
    
    bool ImportFile( );    
    
private:
    bool ReadHeader( MeiHead *meihead );
    bool ReadDiv( Mdiv *mdiv );
    bool ReadScore( Score *score );
    bool ReadParts( Parts * parts );
    bool ReadPart( Part *part );
    bool ReadSection( Section *section );
    bool ReadMeasure( Measure *measure );
    bool ReadStaff( Staff *staff );
    bool ReadLayer( Layer *layer );
    bool ReadBarline( BarLine *barline );
    bool ReadClef( Clef *clef );
    bool ReadMensur( Mensur *mensur );
    bool ReadNote( Note *note );
    bool ReadRest( Rest *rest );
    bool ReadSymbol( MeiElement *symbol );
	//
	int StrToDur(std::string dur);
	int StrToOct(std::string oct);
	int StrToPitch( std::string pitch ); 
    
public:
    
private:
    wxString m_filename;
	MusDiv *m_div;
	MusScore *m_score;
	MusPartSet *m_parts;
	MusPart *m_part;
	MusSection *m_section;
	MusMeasure *m_measure;
	MusStaff *m_staff;
	MusLayer *m_layer;
};


#endif
