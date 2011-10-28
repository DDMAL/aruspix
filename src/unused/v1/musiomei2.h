/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOMEI_H__
#define __MUS_IOMEI_H__

#ifdef __GNUG__
    #pragma interface "musiomei2.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musdoc.h"

#include <mei/mei.h>
#include <vector>

//----------------------------------------------------------------------------
// MusMeiOutput
//----------------------------------------------------------------------------

class MusMeiOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusMeiOutput( MusDoc *file, wxString filename );
    virtual ~MusMeiOutput();
    
    virtual bool ExportFile( );
private:
    wxString m_filename;
};


//----------------------------------------------------------------------------
// MusMeiInput
//----------------------------------------------------------------------------

struct FacsEntry
{
	std::string key;
	int ulx, uly, lry;
};

class FacsTable //this should probably be a map and be more of a true hashtable, but it works as we don't deal with too many zones.
{
private:
	vector<FacsEntry> entries;
public:
	int GetX(std::string key);
	int GetUY(std::string key);
	int GetLY(std::string key);
	void add(std::string key, int X, int Y1, int Y2);
};

class MusMeiInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusMeiInput( MusDoc *file, wxString filename );
    virtual ~MusMeiInput();
    
    // Knonw limitations:
    // staff @n need to be integers and > 0

    virtual bool ImportFile( );
	virtual bool ReadNote( MusNote1 *note );
	virtual	bool ReadNeume( MusNeume *neume );
	virtual bool ReadSymbol( MusSymbol1 *symbol );
    void ReadFacsTable( MeiElement *el , FacsTable *table);
	
private:
    bool ReadElement( MeiElement *el );
	void ReadElement( MeiElement *element, FacsTable *table);
    bool ReadAttributeBool( xmlNode *node, wxString name, bool *value, bool default_value = false );
    bool ReadAttributeInt( xmlNode *node, wxString name, int *value, int default_value = -1 );
    bool ReadAttributeString( xmlNode *node, wxString name, wxString *value, wxString default_value = "" );

    bool mei_accid( xmlNode *node );
    bool mei_barline( xmlNode *node );
    bool mei_clefchange( xmlNode *node );
    bool mei_custos( xmlNode *node ); 
    bool mei_dot( xmlNode *node );
    bool mei_layer( MeiElement *element );    
    bool mei_measure( xmlNode *node );
    bool mei_mensur( xmlNode *node );
    bool mei_note( xmlNode *node );
    bool mei_parts( xmlNode *node );
    bool mei_rest( xmlNode *node );
    bool mei_score( xmlNode *node );
    bool mei_staff( MeiElement *element, FacsTable *table );
    
    void mei_attr_dur( xmlNode *node, unsigned char *val );
    void mei_attr_pname( xmlNode *, unsigned short *code );
    
private:
    wxString m_filename;
    MusPage *m_page; // the page on which we are loading the page (one page)
    MusLaidOutStaff *m_currentStaff;
};

#endif
