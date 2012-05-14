/////////////////////////////////////////////////////////////////////////////
// Name:        muslaidoutlayerelement.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_LAID_OUT_LAYER_ELEMENT_H__
#define __MUS_LAID_OUT_LAYER_ELEMENT_H__

class MusDC;
class MusLayerElement;

#include "muslaidoutlayer.h"
#include "mussymbol.h"

//----------------------------------------------------------------------------
// MusLaidOutLayerElement
//----------------------------------------------------------------------------

/**
 * This class represents a elements in a laid-out score (MusLayout).
 * A MusLaidOutLayerElement is contained in a MusLaidOutLayer.
 * It contains a pointer to a MusLayerElement object (MusNote, MusClef, etc.).
*/
class MusLaidOutLayerElement: public MusLayoutObject
{
public:
    // constructors and destructors
    MusLaidOutLayerElement();
    MusLaidOutLayerElement( MusLayerElement *element );
    virtual ~MusLaidOutLayerElement();
    
    virtual wxString MusClassName( ) { return "MusLaidOutLayerElement"; };	    
    
    void Clear();
    void CheckIntegrity();
    
    /** The parent MusLaidOutLayer setter */
    void SetLayer( MusLaidOutLayer *layer ) { m_layer = layer; }; 
    
    int GetElementNo() const;
    
    bool IsBarline();
    bool IsClef();
    bool IsMensur();
    bool IsNeume();
    bool IsNote();
    bool IsPitchInterface();
    bool IsPositionInterface();
    bool IsRest();
    bool IsSymbol( SymbolType type );
    
    // functors
    void Save( wxArrayPtrVoid params );
    void Load( wxArrayPtrVoid params );
        
private:
    
public:
    /** The parent MusLaidOutStaff */
    MusLaidOutLayer *m_layer;

	/** Position X */
    int m_xrel;
	/** Position Y */
	int m_yrel ; 
    /** Pointer to the MusLayerElement */
    MusLayerElement *m_layerElement;

private:
    
};



//----------------------------------------------------------------------------
// MusLaidOutLayerElementFunctor
//----------------------------------------------------------------------------

/**
 * This class is a Functor that processes MusLaidOutLayerElement objects.
 * Needs testing.
*/
class MusLaidOutLayerElementFunctor: public MusFunctor
{
private:
    void (MusLaidOutLayerElement::*fpt)( wxArrayPtrVoid params );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusLaidOutLayerElementFunctor( void(MusLaidOutLayerElement::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusLaidOutLayerElementFunctor() {};

    // override function "Call"
    virtual void Call( MusLaidOutLayerElement *ptr, wxArrayPtrVoid params )
        { (*ptr.*fpt)( params);};          // execute member function
};

#endif
