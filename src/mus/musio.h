/////////////////////////////////////////////////////////////////////////////
// Name:        musio.h
// Author:      Laurent Pugin
// Created:     2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IO_H__
#define __MUS_IO_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

class MusDoc;
// logical
class MusDiv;
class MusScore;
class MusPartSet;
class MusPart;
class MusSection;
class MusMeasure;
class MusStaff;
class MusLayer;
class MusLayerElement;
// layout
class MusLayout;
class MusPage;
class MusSystem;
class MusLaidOutStaff;
class MusLaidOutLayer;
class MusLaidOutLayerElement;

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
    
    virtual bool WriteDoc( MusDoc *doc ) { return true; };
    // logical
    virtual bool WriteDiv( MusDiv *div ) { return true; }; 
    virtual bool WriteScore( MusScore *score ) { return true; }; 
    virtual bool WritePartSet( MusPartSet *parts ) { return true; };
    virtual bool WritePart( MusPart *part ) { return true; };
    virtual bool WriteSection( MusSection *section ) { return true; };
    virtual bool WriteMeasure( MusMeasure *measure ) { return true; };
    virtual bool WriteStaff( MusStaff *staff ) { return true; };
    virtual bool WriteLayer( MusLayer *layer ) { return true; };
    virtual bool WriteLayerElement( MusLayerElement *element ) { return true; };
    // layout
    virtual bool WriteLayout( MusLayout *layout ) { return true; };
    virtual bool WritePage( MusPage *page ) { return true; };
    virtual bool WriteSystem( MusSystem *system ) { return true; };
    virtual bool WriteLaidOutStaff( MusLaidOutStaff *laidOutStaff ) { return true; };
    virtual bool WriteLaidOutLayer( MusLaidOutLayer *laidOutLayer ) { return true; };
    virtual bool WriteLaidOutLayerElement( MusLaidOutLayerElement *laidOutLayerElement ) { return true; };  
    
public:
    
protected:
    MusDoc *m_doc;
    
private:
    
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
    
    virtual bool ReadDoc( MusDoc *doc ) { return true; };
    // logical
    virtual MusDiv* ReadDiv( ) { return NULL; }; 
    virtual MusScore* ReadScore( ) { return NULL; }; 
    virtual MusPartSet* ReadPartSet( ) { return NULL; };
    virtual MusPart* ReadPart( ) { return NULL; };
    virtual MusSection* ReadSection( ) { return NULL; };
    virtual MusMeasure* ReadMeasure( ) { return NULL; };
    virtual MusStaff* ReadStaff(  ) { return NULL; };
    virtual MusLayer* ReadLayer( ) { return NULL; };
    virtual MusLayerElement *ReadLayerElement( ) { return NULL; };
    // layout
    virtual MusLayout* ReadLayout( ) { return NULL; };
    virtual MusPage* ReadPage( ) { return NULL; };
    virtual MusSystem* ReadSystem( ) { return NULL; };
    virtual MusLaidOutStaff* ReadLaidOutStaff( ) { return NULL; };
    virtual MusLaidOutLayer* ReadLaidOutLayer(  ) { return NULL; };
    virtual MusLaidOutLayerElement* ReadLaidOutLayerElement( ) { return NULL; };  
    
public:
    
protected:
    MusDoc *m_doc;
    
};

#endif
