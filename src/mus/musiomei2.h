/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
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

#include "musfile.h"

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
private:
    wxString m_filename;
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
    
    // Knonw limitations:
    // staff @n need to be integers and > 0

    virtual bool ImportFile( );
	virtual bool ReadNote( MusNote *note );
	virtual	bool ReadNeume( MusNeume *neume );
	virtual bool ReadSymbol( MusSymbol *symbol );
    
private:
    bool ReadElement( MeiElement *el );
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
    wxString m_filename;
    MusPage *m_page; // the page on which we are loading the page (one page)
    MusStaff *m_currentStaff;
};

#endif
