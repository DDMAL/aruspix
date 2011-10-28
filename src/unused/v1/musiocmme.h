/////////////////////////////////////////////////////////////////////////////
// Name:        musiocmme.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOCMME_H__
#define __MUS_IOCMME_H__

#ifdef __GNUG__
    #pragma interface "musiocmme.cpp"
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

#include "musdoc.h"





//----------------------------------------------------------------------------
// MusCmmeOutput
//----------------------------------------------------------------------------

class MusCmmeOutput: public MusFileOutputStream
{
public:
    // constructors and destructors
    MusCmmeOutput( MusDoc *file, wxString filename );
    virtual ~MusCmmeOutput();
    
    virtual bool ExportFile( );
	virtual bool WriteFileHeader( );
	virtual bool WriteStaff( const MusLaidOutStaff *staff );
	/*virtual bool WriteNote( const MusNote1 *note );
	virtual bool WriteSymbol( const MusSymbol1 *symbol );
    virtual bool WriteLyric( const MusElement *element );
	virtual bool WriteElementAttr( const MusElement *element );*/ // ax2
    
private:
    wxString m_filename;
    TiXmlElement *m_xml_root;
    TiXmlElement *m_xml_current;


private:
	};


//----------------------------------------------------------------------------
// MusCmmeInput
//----------------------------------------------------------------------------

class MusCmmeInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusCmmeInput( MusDoc *file, wxString filename );
    virtual ~MusCmmeInput();
    
    virtual bool ImportFile( );
    
private:
        int m_vmaj, m_vmin, m_vrev;

private:
	};


#endif
