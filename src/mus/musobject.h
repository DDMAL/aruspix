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

class MusDoc;
class MusFunctor;

class MusObject;
WX_DECLARE_OBJARRAY( MusObject, ArrayOfMusObjects );


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
    
    virtual bool Check();
    
    virtual bool operator==( MusObject& other);
    
    bool IsActive() { return m_active; };
    void Deactivate() { m_active = false; };
    
    int GetId() { return 0; }; // used in SVG - TODO
    uuid_t* GetUuid() { return &m_uuid; };
    wxString GetUuidStr();
    void SetUuid( uuid_t uuid );
    void ResetUuid( );
    
    void ClearChildren() { m_children.Clear(); }
    
    void SetParent( MusObject *parent ) { m_parent = parent; }
    
    virtual wxString MusClassName( ) { return "[MISSING]"; };
    
    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    
    // functor methods
    virtual bool Save( wxArrayPtrVoid params ) { return false; };
    virtual bool TrimSystem( wxArrayPtrVoid params );
    virtual bool UpdateLayerElementXPos( wxArrayPtrVoid params );
    virtual bool UpdateStaffYPos( wxArrayPtrVoid params );

public:
    bool m_active;
    ArrayOfMusObjects m_children;
    MusObject *m_parent;

public:
    
protected:
    uuid_t m_uuid;

private:
    
};


//----------------------------------------------------------------------------
// MusDocObject
//----------------------------------------------------------------------------

/** 
 * This class represents a basic object in the layout domain
 */
class MusDocObject: public MusObject
{
public:
    // constructors and destructors
    MusDocObject();
    virtual ~MusDocObject();
    
    void UpdateContentBB( int x1, int y1, int x2, int y2);
    void UpdateSelfBB( int x1, int y1, int x2, int y2 );
    bool HasContentBB();
    bool HasSelfBB();
    void ResetBB();

    int m_contentBB_x1, m_contentBB_y1, m_contentBB_x2, m_contentBB_y2;
    int m_selfBB_x1, m_selfBB_y1, m_selfBB_x2, m_selfBB_y2; 
    
    /**
     * Is true if the bounding box (self or content) has been updated at least once.
     * We need this to avoid not updating bounding boxes to screw up the layout with their intial values.
     */
    bool HasUpdatedBB( ) { return m_updatedBB; };
    
private:
    bool m_updatedBB;
    
protected:
    MusDoc *m_doc;
    
public:
    
};


//----------------------------------------------------------------------------
// abstract base class MusFunctor
//----------------------------------------------------------------------------

/** 
 * This class is an abstact Functor for the object hierarchy.
 * Needs testing.
 */
class MusFunctor
{
private:
    bool (MusObject::*obj_fpt)( wxArrayPtrVoid params );   // pointer to member function
    
public:
    
    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusFunctor( );
    MusFunctor( bool(MusObject::*_obj_fpt)( wxArrayPtrVoid ));
	virtual ~MusFunctor() {};
    
    // override function "Call"
    virtual void Call( MusObject *ptr, wxArrayPtrVoid params );
    
    bool m_stopIt;
    bool m_reverse;
    
private:
    
};


//----------------------------------------------------------------------------
// MusEnv
//----------------------------------------------------------------------------

/** 
 * This class contains the document environment variables.
 * It remains from the Wolfgang parameters strcuture.
 * TODO - Integrate them in MEI.
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
