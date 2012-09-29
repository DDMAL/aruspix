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

// App
class MusLayerRdg;
WX_DECLARE_OBJARRAY( MusLayerRdg, ArrayOfMusLayerRdgs );

// Layout classes
class MusLayout;
WX_DECLARE_OBJARRAY( MusLayout, ArrayOfMusLayouts );

class MusPage;
WX_DECLARE_OBJARRAY( MusPage, ArrayOfMusPages );

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
    
    virtual bool Check() = 0;
    
    bool IsActive() { return m_active; };
    void Deactivate() { m_active = false; };
    
    int GetId() { return 0; }; // used in SVG - TODO
    uuid_t* GetUuid() { return &m_uuid; };
    wxString GetUuidStr();
    void SetUuid( uuid_t uuid );
    
    
    
    virtual wxString MusClassName( ) { return "[MISSING]"; };
    
    // functors
    bool FindWithUuid( wxArrayPtrVoid params );
    bool ReplaceUuid( wxArrayPtrVoid params );
    bool CheckFunctor( wxArrayPtrVoid params );
    /**
     * Set the layout pointer in all a layout tree.
     * Virtual method redefined in MusLayoutObject and call from MusLayout::SetDoc.
     */
    virtual void SetLayout( wxArrayPtrVoid params ) {};
    /**
     * Set the div pointer in all a div tree.
     * Virtual method redefined in MusLogicalObject and call from MusDiv::SetDoc.
     */
    virtual void SetDiv( wxArrayPtrVoid params ) {};

private:
    bool m_active;

public:
    
protected:
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
    
	virtual bool Check();
    
    void SetDiv( MusDiv *div ) { m_div = div; };
    
    // functors
    virtual void SetDiv( wxArrayPtrVoid params );

private:

protected:
    MusDiv *m_div;
    
public:
    
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
    
	virtual bool Check();
    
    void UpdateContentBB( int x1, int y1, int x2, int y2);
    void UpdateSelfBB( int x1, int y1, int x2, int y2 );
    bool HasContentBB();
    bool HasSelfBB();
    void ResetBB();

    int m_contentBB_x1, m_contentBB_y1, m_contentBB_x2, m_contentBB_y2;
    int m_selfBB_x1, m_selfBB_y1, m_selfBB_x2, m_selfBB_y2; 
    
    void SetLayout( MusLayout *layout ) { m_layout = layout; };
    
    /**
     * Is true if the bounding box (self or content) has been updated at least once.
     * We need this to avoid not updating bounding boxes to screw up the layout with their intial values.
     */
    bool HasUpdatedBB( ) { return m_updatedBB; };
    
    // functors
    virtual void SetLayout( wxArrayPtrVoid params );
    
private:
    bool m_updatedBB;
    
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
    bool m_reverse;
    
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
