 /////////////////////////////////////////////////////////////////////////////
// Name:        musobject.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_OBJECT_H__
#define __MUS_OBJECT_H__

#ifdef __GNUG__
	#pragma interface "musobject.cpp"
#endif

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class MusRC;
class MusDoc;
class MusParameters;


// Logical classes
class MusDiv;
WX_DECLARE_OBJARRAY( MusDiv, ArrayOfMusDivs );

class MusPart;
WX_DECLARE_OBJARRAY( MusPart, ArrayOfMusParts );

class MusSection;
WX_DECLARE_OBJARRAY( MusSection, ArrayOfMusSections );

class MusSectionInterface;
WX_DECLARE_OBJARRAY( MusSectionInterface, ArrayOfMusSectionElements );

class MusMeasureInterface;
WX_DECLARE_OBJARRAY( MusMeasureInterface, ArrayOfMusMeasureElements );

class MusStaffInterface;
WX_DECLARE_OBJARRAY( MusStaffInterface, ArrayOfMusStaffElements );

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

private:

public:
    
protected:
    int m_id;

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
    
    bool Init( MusRC *renderer );
	bool Check() { return true; }; // { return m_ok; };

private:

private:
	bool m_ok;
    
protected:
    //MusRC *m_r;
	MusDoc *m_doc;
	//MusParameters *m_p;

public:
    
    
};



//----------------------------------------------------------------------------
// abstract base class MusLogicalFunctor
//----------------------------------------------------------------------------

/** 
 * This class is an abstact Functor for the logical domain.
 * Needs testing.
 */
class MusLogicalFunctor
{
public:
	
    MusLogicalFunctor( ) {};
	virtual ~MusLogicalFunctor() {};
    
    // override function "Call"
    virtual void Call( MusLogicalObject *ptr, wxArrayPtrVoid params ) {};
	
private:
	
};


//----------------------------------------------------------------------------
// abstract base class MusLayoutFunctor
//----------------------------------------------------------------------------

/** 
 * This class is an abstact Functor for the layout domain.
 * Needs testing.
 */
class MusLayoutFunctor
{
public:

    MusLayoutFunctor( ) {};
	virtual ~MusLayoutFunctor() {};
    
    // override function "Call"
    virtual void Call( MusLayoutObject *ptr, wxArrayPtrVoid params ) {};
        
private:
       
};



#endif
