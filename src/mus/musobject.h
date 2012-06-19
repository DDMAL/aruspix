 /////////////////////////////////////////////////////////////////////////////
// Name:        musobject.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_OBJECT_H__
#define __MUS_OBJECT_H__

#include <wx/wxprec.h>
#include <uuid/uuid.h>

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class MusRC;
class MusDoc;
class MusEnv;



// Logical classes
class MusDiv;
WX_DECLARE_OBJARRAY( MusDiv, ArrayOfMusDivs );

class MusPart;
WX_DECLARE_OBJARRAY( MusPart, ArrayOfMusParts );

class MusSection;
WX_DECLARE_OBJARRAY( MusSection, ArrayOfMusSections );

class MusMeasure;
WX_DECLARE_OBJARRAY( MusMeasure, ArrayOfMusMeasures );

class MusStaff;
WX_DECLARE_OBJARRAY( MusStaff, ArrayOfMusStaves );

class MusLayer;
WX_DECLARE_OBJARRAY( MusLayer, ArrayOfMusLayers );

class MusLayerElement;
WX_DECLARE_OBJARRAY( MusLayerElement, ArrayOfMusLayerElements );

// Layout classes
class MusLayout;
WX_DECLARE_OBJARRAY( MusLayout, ArrayOfMusLayouts);

class MusPage;
WX_DECLARE_OBJARRAY( MusPage, ArrayOfMusPages);

class MusSystem;
WX_DECLARE_OBJARRAY( MusSystem, ArrayOfMusSystems);

class MusLaidOutStaff;
WX_DECLARE_OBJARRAY( MusLaidOutStaff, ArrayOfMusLaidOutStaves );

class MusLaidOutLayer;
WX_DECLARE_OBJARRAY( MusLaidOutLayer, ArrayOfMusLaidOutLayers );

class MusLaidOutLayerElement;
WX_DECLARE_OBJARRAY( MusLaidOutLayerElement, ArrayOfMusLaidOutLayerElements );

// Generic
class MusLayoutObject;
WX_DECLARE_OBJARRAY( MusLayoutObject, ArrayOfMusLayoutObjects );


//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

/** 
 * This class represents a basic object
 */
class MusObject: public wxObject
{
public:
    // constructors and destructors
    MusObject();
    virtual ~MusObject();
    
    int GetId() { return m_id; };
    uuid_t* GetUuid() { return &m_uuid; };
    void SetUuid( uuid_t uuid );
    
    virtual wxString MusClassName( ) { return "[MISSING]"; };
    
    bool FindWithUuid( wxArrayPtrVoid params );

private:

public:
    
protected:
    int m_id;
    uuid_t m_uuid;

private:
    
};


//----------------------------------------------------------------------------
// MusLogicalObject
//----------------------------------------------------------------------------

/** 
 * This class represents a basic object in the logical domain
 */
class MusLogicalObject: public MusObject
{
public:
    // constructors and destructors
    MusLogicalObject();
    virtual ~MusLogicalObject();

private:

public:
    
private:
    
};


//----------------------------------------------------------------------------
// MusLayoutObject
//----------------------------------------------------------------------------

/** 
 * This class represents a basic object in the layout domain
 */
class MusLayoutObject: public MusObject
{
public:
    // constructors and destructors
    MusLayoutObject();
    virtual ~MusLayoutObject();
    
    void SetLayout( wxArrayPtrVoid params );
	bool Check() { return true; }; // { return m_ok; };
    void UpdateContentBB( int x1, int y1, int x2, int y2);
    void UpdateOwnBB( int x1, int y1, int x2, int y2 );
    void ResetBB();

    int m_contentBB_x1, m_contentBB_y1, m_contentBB_x2, m_contentBB_y2;
    int m_selfBB_x1, m_selfBB_y1, m_selfBB_x2, m_selfBB_y2; 
    
private:
    
protected:
	MusLayout *m_layout;


public:
    
    
};



//----------------------------------------------------------------------------
// abstract base class MusFunctor
//----------------------------------------------------------------------------

/** 
 * This class is an abstact Functor for the layout domain.
 * Needs testing.
 */

//----------------------------------------------------------------------------
// abstract base class MusFunctor
//----------------------------------------------------------------------------

/** 
 * This class is an abstact Functor for the layout domain.
 * Needs testing.
 */
class MusFunctor
{
private:
    void (MusObject::*obj_fpt)( wxArrayPtrVoid params );   // pointer to member function
    bool (MusObject::*obj_fpt_bool)( wxArrayPtrVoid params );   // pointer to member function
    
public:
    
    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusFunctor( );
    MusFunctor( void(MusObject::*_obj_fpt)( wxArrayPtrVoid ));
    MusFunctor( bool(MusObject::*_ojb_fpt_bool)( wxArrayPtrVoid ));
	virtual ~MusFunctor() {};
    
    // override function "Call"
    virtual void Call( MusObject *ptr, wxArrayPtrVoid params );
    //virtual void Call( MusObject *ptr, wxArrayPtrVoid params, bool *success );
    
    bool m_success;
    
private:
    
};

/*
class MusFunctor
{
public:

    MusFunctor( ) { m_success = false; };
	virtual ~MusFunctor() {};
    
    // override function "Call"
    virtual void Call( MusLayoutObject *ptr, wxArrayPtrVoid params ) {};
    virtual void Call( MusLayoutObject *ptr, wxArrayPtrVoid params, bool *success ) {};
        
    bool m_success;
    
private:
       
};
*/


//----------------------------------------------------------------------------
// MusEnv
//----------------------------------------------------------------------------

/** 
 * This class contains the document environment variables.
 * It remains from the Wolfgang parameters strcuture.
 */
class MusEnv 
{
public:
    // constructors and destructors
    MusEnv();
    virtual ~MusEnv();
    
    
public:
    /** landscape paper orientation */
    char m_landscape;
    /** staff line width */
    unsigned char m_staffLineWidth;
    /** stem width */
    unsigned char m_stemWidth;
    /** barline width */
    unsigned char m_barlineWidth;
    /** beam width */
    unsigned char m_beamWidth;
    /** beam white width */
    unsigned char m_beamWhiteWidth;
    /** maximum beam slope */
    unsigned char m_beamMaxSlope;
    /** minimum beam slope */
    unsigned char m_beamMinSlope;
    /** paper width */
    int m_paperWidth;
    /** paper height */
    int m_paperHeight;
    /** top margin */
    short m_topMargin;
    /** left margin on odd pages */
    short m_leftMarginOddPage;
    /** left margin on even pages */
    short m_leftMarginEvenPage;        
    /** small staff size ratio numerator */
    unsigned char m_smallStaffNum;
    /** small staff size ratio denominator */
    unsigned char m_smallStaffDen;
    /** grace size ratio numerator */
    unsigned char m_graceNum;
    /** grace size ratio denominator */
    unsigned char m_graceDen;
    /** stem position correction */
    signed char m_stemCorrection;
    /** header and footer type */
    unsigned int m_headerType;
    /** notation mode. Since since Aruspix 1.6.1 */
    int m_notationMode;
};


#endif
