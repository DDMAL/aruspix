/////////////////////////////////////////////////////////////////////////////
// Name:        muslayer.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_LAYER_H__
#define __MUS_LAYER_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "musstaff.h"


//----------------------------------------------------------------------------
// MusLayer
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <layer> element.
 * A MusLayer is a MusStaffInterface
 */
class MusLayer: public MusLogicalObject, public MusStaffInterface
{
public:
    // constructors and destructors
    MusLayer();
    virtual ~MusLayer();	
	
	void AddLayerElement( MusLayerElement *element );
        
private:
    
public:
    /** The children MusLayerElement objects */
    ArrayOfMusLayerElements m_layerElements;

private:
    
};


//----------------------------------------------------------------------------
// abstract base class MusLayerFunctor
//----------------------------------------------------------------------------

/**
    This class is a Functor that processes MusLayer objects.
    Needs testing.
*/
class MusLayerFunctor
{
private:
    void (MusLayer::*fpt)( wxArrayPtrVoid params );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusLayerFunctor( void(MusLayer::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusLayerFunctor() {};

    // override function "Call"
    virtual void Call( MusLayer *ptr, wxArrayPtrVoid params )
        { (*ptr.*fpt)( params);};          // execute member function
};



//----------------------------------------------------------------------------
// MusLayerElement
//----------------------------------------------------------------------------

/** 
 * This class is a base class for the MusLayer (<layer>) content.
 * It is not an abstract class but should not be instanciate directly.
 */
class MusLayerElement: public MusLogicalObject
{
public:
    // constructors and destructors
    MusLayerElement();
    virtual ~MusLayerElement();
    
    /** The parent MusLayer setter */
    void SetLayer( MusLayer *layer ) { m_layer = layer; };
    
private:
    
public:
    /** The parent MusLayer */
    MusLayer *m_layer;
    
    /** Indicates if cue size */
    bool m_cueSize;
    /** Indicates an horizontal offset */
    int m_hOffset;
    /** Indicates if occurs on staff above (-1) or below (1) */
    char m_staffShift;
    /** Indicates if visible (default) or not */
    bool m_visible;

private:
    
};





#endif
