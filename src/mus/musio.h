/////////////////////////////////////////////////////////////////////////////
// Name:        musio.h
// Author:      Laurent Pugin
// Created:     2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IO_H__
#define __MUS_IO_H__

#include <fstream>
#include <iostream>
#include <string>

class MusDoc;
class MusLayer;
class MusLayerApp;
class LayerElement;
class MusLayerRdg;
class MusMeasure;
class MusPage;
class MusStaff;
class MusStaffDef;
class MusStaffGrp;
class MusSystem;

//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

/** 
 * This class is a base class for file output stream classes.
 * It is not an abstract class but should not be instanciate directly.
 */ 
class MusFileOutputStream: public std::ofstream
{
public:
    // constructors and destructors
    MusFileOutputStream( MusDoc *doc, std::string filename );
	MusFileOutputStream( MusDoc *doc );
    MusFileOutputStream() {};
	//MusFileOutputStream( MusDoc *file, wxFile *wxfile );
    virtual ~MusFileOutputStream();
    
    virtual bool ExportFile( ) { return true; }
    
    /** @name Writing element methods
     * The following methods actually write the elements.
     * They must be overriden in the child classes.
     * The children of the elements do not have to be writen from theses methods.
     * This actually happen in the Save method of the MusObject classes
     */
    ///@{
    virtual bool WriteDoc( MusDoc *doc ) { return true; };
    virtual bool WritePage( MusPage *page ) { return true; };
    virtual bool WriteSystem( MusSystem *system ) { return true; };
    virtual bool WriteStaffGrp( MusStaffGrp *system ) { return true; };
    virtual bool WriteStaffDef( MusStaffDef *system ) { return true; };
    virtual bool WriteMeasure( MusMeasure *measure ) { return true; };
    virtual bool WriteStaff( MusStaff *staff ) { return true; };
    virtual bool WriteLayer( MusLayer *layer ) { return true; };
    virtual bool WriteLayerElement( LayerElement *element ) { return true; };
    // app
    virtual bool WriteLayerApp( MusLayerApp *app ) { return true; };
    virtual bool WriteLayerRdg( MusLayerRdg *rdg ) { return true; };
    ///@}
    
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
class MusFileInputStream: public std::ifstream
{
public:
    // constructors and destructors
    MusFileInputStream( MusDoc *doc, std::string filename );
    MusFileInputStream( MusDoc *doc );
    MusFileInputStream() {};
    virtual ~MusFileInputStream();
    
    // read
    virtual bool ImportFile( ) { return true; }
    virtual bool ImportString( std::string data ) { return true; }
    
    bool HasLayoutInformation() { return m_hasLayoutInformation; };
    
public:
    
protected:
    MusDoc *m_doc;
    
    /**
     * Becomes true if layout information is found during the import.
     * This will be true if the file is page-based MEI or if the MEI
     * file contains <pb> and <sb>. This will stay wrong with PAE import
     */
    bool m_hasLayoutInformation;
    
    
};

#endif
