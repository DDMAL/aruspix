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
#include "musdef.h"

//----------------------------------------------------------------------------
// MusLayer
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <layer> element.
 * A MusLayer is a MusStaffInterface
 */
class MusLayer: public MusLogicalObject
{
public:
    // constructors and destructors
    MusLayer();
    virtual ~MusLayer();	
    
    virtual bool Check();
    
    virtual wxString MusClassName( ) { return "MusLayer"; };
	
	void AddLayerElement( MusLayerElement *element, int idx = -1 );
    
    void SetStaff( MusStaff *staff ) { m_staff = staff; };
    
    void Insert( MusLayerElement *element, MusLayerElement *before );
    
    int GetElementCount() const { return (int)m_elements.GetCount(); };
    
    /**
     * Copy the content of the layer to the dest layer.
     * If start and end are not null (see uuid_clear), then ony element from and to 
     * the element with the uuid are copied.
     * By default, the uuid of the elements are kept.
     */
    void CopyContent( MusLayer *dest, uuid_t start, uuid_t end, bool newUuid = false );
    
    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    // functors
    void Save( wxArrayPtrVoid params );
    void Load( wxArrayPtrVoid params );
        
private:
    
public:
    /** The children MusLayerElement objects */
    ArrayOfMusLayerElements m_elements;
    /** the parent MusStaff */
    MusStaff *m_staff;

private:
    
};


//----------------------------------------------------------------------------
// MusLayerFunctor
//----------------------------------------------------------------------------

/**
    This class is a Functor that processes MusLayer objects.
    Needs testing.
*/
class MusLayerFunctor: public MusFunctor
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
    
    MusLayerElement& operator=( const MusLayerElement& element); // copy assignement;
    
    virtual bool Check();
    
    virtual bool operator==( MusLayerElement& other);
    
    /**
     * Return a copy of the MusLayerElement (child class).
     * By default, a new uuid is generated
     */
    MusLayerElement *GetChildCopy( bool newUuid = true );
    
    /** The parent MusLayer setter */
    void SetLayer( MusLayer *layer ) { m_layer = layer; };
    
    /**
     * Return the default horizontal spacing of elements.
     * This method should be redefined whenever necessary in the child classes.
     */
    virtual int GetHorizontalSpacing( );
    
    /** Adjust the pname and the octave for values outside the range */
    static void AdjustPname( int *pname, int *oct );
    
    /** 
     * Set the pitch or position for MusPitchInterface or MusPositionInterface elements.
     * Because MusPitchInterface and PositionInterface are not child classes of MusLayerElement,
     * the call had to be done explicitly from this method. The method can still be overriden.
     */
    virtual void SetPitchOrPosition( int pname, int oct );
    
    /**
     * Get the pitch or position for MusPitchInterface or MusPositionInterface elements.
     * See MusLayerElement::SetPitchOrPosition for more comments.
     */
    virtual bool GetPitchOrPosition( int *pname, int *oct );
    
    /**
     * Set the value for child element of MusLayerElement.
     * For example, set the duration for MusDurationInterface elements (call explicitly) 
     */
	virtual void SetValue( int value, int flag = 0 );
    
    /**
     * Change the coloration for MusNote elements.
     */ 
	virtual void ChangeColoration( ) {};
    
    /**
     * Change the stem direction for MusNote elements.
     */
	virtual void ChangeStem( ) {};
    
    /**
     * Set the ligature flag for MusNote elements.
     */
	virtual void SetLigature( ) {};

    
    bool IsBarline();
    bool IsBeam();
    bool IsClef();
    bool HasDurationInterface();
    bool IsMensur();
    bool IsNeume();
    bool IsNeumeSymbol();
    bool IsNote();
    bool HasPitchInterface();
    bool HasPositionInterface();
    bool IsRest();
    bool IsSymbol( SymbolType type );
    bool IsSymbol( );
    
    // functors
    void Save( wxArrayPtrVoid params );
    
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


//----------------------------------------------------------------------------
// MusLayerElementFunctor
//----------------------------------------------------------------------------

/**
 * This class is a Functor that processes MusLayerElement objects.
 * Needs testing.
 */
class MusLayerElementFunctor: public MusFunctor
{
private:
    void (MusLayerElement::*fpt)( wxArrayPtrVoid params );   // pointer to member function
    
public:
    
    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusLayerElementFunctor( void(MusLayerElement::*_fpt)( wxArrayPtrVoid ))  { fpt=_fpt; };
    MusLayerElementFunctor( bool(MusLayerElement::*_fpt_bool)( wxArrayPtrVoid ));
	virtual ~MusLayerElementFunctor() {};
    
    // override function "Call"
    virtual void Call( MusLayerElement *ptr, wxArrayPtrVoid params )
    { (*ptr.*fpt)( params);};          // execute member function
};



#endif
