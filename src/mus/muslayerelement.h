/////////////////////////////////////////////////////////////////////////////
// Name:        muslayerelement.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_LAYER_ELEMENT_H__
#define __MUS_LAYER_ELEMENT_H__

#include "musdc.h" // for MusPoint
#include "musdef.h"
#include "musobject.h"

class MusAlignment;

//----------------------------------------------------------------------------
// MusLayerElement
//----------------------------------------------------------------------------

/** 
 * This class is a base class for the MusLayer (<layer>) content.
 * It is not an abstract class but should not be instanciate directly.
 */
class MusLayerElement: public MusDocObject
{
public:
    // constructors and destructors
    MusLayerElement();
    virtual ~MusLayerElement();
    
    MusLayerElement& operator=( const MusLayerElement& element ); // copy assignement;
    
    /**
     * Return a copy of the MusLayerElement (child class).
     * By default, a new uuid is generated
     */
    MusLayerElement *GetChildCopy( bool newUuid = true );
    
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
    
    
    int GetElementNo() const;

    
    bool IsBarline();
    bool IsBeam();
    bool IsClef();
    bool HasDurationInterface();
    bool IsKeySig();
    bool IsMensur();
    bool IsNeume();
    bool IsNeumeSymbol();
    bool IsNote();
    bool HasPitchInterface();
    bool HasPositionInterface();
    bool IsRest();
    bool IsSymbol( SymbolType type );
    bool IsSymbol( );
    
    virtual double GetAlignementDuration();
    
    MusAlignment *GetAlignment() { return m_alignment; };
    
    int GetXRel();
    
    // functors
    /**
     * Save the object (virtual).
     * Most of the child classes do not override it. In these cases, the actual 
     * saving occurs in the MusFileOutputStream::WriteLayerElement method
     * A few classes, such as MusLayerApp, have an overriden version.
     */
    virtual int Save( ArrayPtrVoid params );
    
    /**
     * Align the content of a system.
     */
    virtual int Align( ArrayPtrVoid params );
    
private:
    
public:
	/** Absolute position X. This is used for facsimile (transcription) encoding */
    int m_x_abs;
	/** Relative position X. This is used internally when calculating the layout */
    //int m_x_rel;
	/** Drawing position X. This is used internally when drawing */
    int m_x_drawing;
	/** Drawing position Y. This is used internally when drawing */
	int m_y_drawing; 
    /** for elements in MusLayerApp. They will be drawn from the MusLayerElement of the app (and not from the layer) */
    bool m_in_layer_app;
    /** Indicates if cue size */
    bool m_cueSize;
    /** Indicates an horizontal offset */
    int m_hOffset;
    /** Indicates if occurs on staff above (-1) or below (1) */
    char m_staffShift;
    /** Indicates if visible (default) or not */
    bool m_visible;

    /** If this is a note, store here the stem coordinates (useful for ex. tuplets) */
    MusPoint m_stem_start; // beginning point, the one near the note
    MusPoint m_stem_end; // end point (!), near beam or stem
    /** stem direction as drawn, true = up, false = down */
    bool m_drawn_stem_dir;
    
private:
    MusAlignment *m_alignment;
    
};




#endif
