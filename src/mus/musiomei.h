/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMEI_H__
#define __MUS_IOMEI_H__

#ifdef __GNUG__
    #pragma interface "musiomei.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif

#include "musfile.h"

// Reading is now done with libxml2
typedef struct _xmlNode xmlNode;
//#include <libxml/parser.h>
//#include <libxml/tree.h>



//----------------------------------------------------------------------------
// MusMeiOutput
//----------------------------------------------------------------------------

class MusMeiOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusMeiOutput( MusFile *file, wxString filename );
    virtual ~MusMeiOutput();
    
    virtual bool ExportFile( );
	virtual bool WriteFileHeader( const MusFileHeader *header );
    virtual bool WritePage( const MusPage *page );
	virtual bool WriteStaff( const MusStaff *staff );
	virtual bool WriteNote( const MusNote *note );
	virtual	bool WriteNeume( const MusNeume *neume ); 
	virtual bool WriteSymbol( const MusSymbol *symbol );
    virtual bool WriteLyric( const MusElement *element );
	virtual bool WriteElementAttr( const MusElement *element );
    
private:
    void WriteParts( TiXmlElement *parts );
    void WriteScore( TiXmlElement *score );
    
private:
    wxString m_filename;
    TiXmlElement *m_xml_root;
    TiXmlElement *m_xml_current;
    TiXmlElement **m_xml_staves;


private:
	};


//----------------------------------------------------------------------------
// MusMeiInput
//----------------------------------------------------------------------------

class MusMeiInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusMeiInput( MusFile *file, wxString filename );
    virtual ~MusMeiInput();
    
/* Because this way of loading MEI do not seem to be adequate, I am trying 
   another way that matches the MEI element. It is still VERY experimental.
   A future option could be to use a XML / C++ binding
    
    virtual bool ImportFile( );
	virtual bool ReadFileHeader( MusFileHeader *header );
	virtual bool ReadSeparator( );
	virtual bool ReadPage( MusPage *page );
	virtual bool ReadStaff( MusStaff *staff );
	virtual bool ReadNote( MusNote *note );
	virtual	bool ReadNeume( MusNeume *neume );
	virtual bool ReadSymbol( MusSymbol *symbol );
    virtual bool ReadLyric( MusElement *element );
	virtual bool ReadElementAttr( MusElement *element );
	virtual bool ReadPagination( MusPagination *pagination );
	virtual bool ReadHeaderFooter( MusHeaderFooter *headerfooter);
    
    
    
    
private:
    TiXmlNode *GetFirstChild( TiXmlNode *node, wxString element );
    void ReadParts( TiXmlElement *parts );
    void ReadScore( TiXmlElement *score );
    
private:
    
private:
		wxString m_filename;
    TiXmlElement *m_xml_root;
    TiXmlElement *m_xml_current;
};

*/

    // Knonw limitations:
    // staff @n need to be integers and > 0


    virtual bool ImportFile( );
	virtual bool ReadNote( MusNote *note );
	virtual	bool ReadNeume( MusNeume *neume );
	virtual bool ReadSymbol( MusSymbol *symbol );
    
    
    
    
private:
    bool ReadElement( xmlNode *node );
    bool ReadAttributeBool( xmlNode *node, wxString name, bool *value, bool default_value = false );
    bool ReadAttributeInt( xmlNode *node, wxString name, int *value, int default_value = -1 );
    bool ReadAttributeString( xmlNode *node, wxString name, wxString *value, wxString default_value = "" );

    bool mei_accid( xmlNode *node );
    bool mei_barline( xmlNode *node );
    bool mei_clefchange( xmlNode *node );
    bool mei_custos( xmlNode *node ); 
    bool mei_dot( xmlNode *node );
    bool mei_layer( xmlNode *node );    
    bool mei_measure( xmlNode *node );
    bool mei_mensur( xmlNode *node );
    bool mei_note( xmlNode *node );
    bool mei_parts( xmlNode *node );
    bool mei_rest( xmlNode *node );
    bool mei_score( xmlNode *node );
    bool mei_staff( xmlNode *node );
    
    void mei_attr_dur( xmlNode *node, unsigned char *val );
    void mei_attr_pname( xmlNode *, unsigned short *code );
    
private:
    
private:
		wxString m_filename;
    MusPage *m_page; // the page on which we are loading the page (one page)
    MusStaff *m_currentStaff;
};



#endif
