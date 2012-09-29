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
    MusLaidOutLayerElement( MusLayerElement *element );
    virtual ~MusLaidOutLayerElement();
    
    virtual bool Check();
    
    virtual wxString MusClassName( ) { return "MusLaidOutLayerElement"; };	    
    
    void Clear();
    
    /** The parent MusLaidOutLayer setter */
    void SetLayer( MusLaidOutLayer *layer ) { m_layer = layer; }; 
    
    int GetElementNo() const;
    
    bool IsBarline() { return m_layerElement->IsBarline(); };
    bool IsBeam() { return m_layerElement->IsBeam(); };
    bool IsClef() { return m_layerElement->IsClef(); };
    bool IsMensur() { return m_layerElement->IsMensur(); };
    bool IsNeume() { return m_layerElement->IsNeume(); };
    bool IsNeumeSymbol() { return m_layerElement->IsNeumeSymbol(); };
    bool IsNote() { return m_layerElement->IsNote(); };
    bool IsRest() { return m_layerElement->IsRest(); };
    bool IsSymbol( SymbolType type ) { return m_layerElement->IsSymbol( type ); };
    bool IsSymbol( ) { return m_layerElement->IsSymbol(); };

    bool HasDurationInterface() { return m_layerElement->HasDurationInterface(); };
    bool HasPitchInterface() { return m_layerElement->HasPitchInterface(); };
    bool HasPositionInterface() { return m_layerElement->HasPositionInterface(); };
    
    // functors
    void Save( wxArrayPtrVoid params );
    void Delete( wxArrayPtrVoid params );
    void UpdateXPosition( wxArrayPtrVoid params );
    void CheckAndResetLayerElement( wxArrayPtrVoid params );
        
private:
    
public:
    /** The parent MusLaidOutStaff */
    MusLaidOutLayer *m_layer;

	/** Position X */
    int m_x_abs;
	/** Position Y */
	int m_y_drawing ; 
    /** If this is a note, store here the stem coordinates (useful for ex. tuplets) */
    MusPoint m_stem_start; // beginning point, the one near the note
    MusPoint m_stem_end; // end point (!), near beam or stem
    /** stem direction as drawn, true = up, false = down */
    bool m_drawn_stem_dir;
    /** for elements in MusLayerApp. They will be drawn from the MusLaidOutLayerElement of the app (and not from the layer) */
    bool m_in_layer_app;
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
