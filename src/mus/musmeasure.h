/////////////////////////////////////////////////////////////////////////////
// Name:        musmeasure.h
// Author:      Laurent Pugin
// Created:     2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_MEASURE_H__
#define __MUS_MEASURE_H__

#include "musobject.h"

class MusLayer;

//----------------------------------------------------------------------------
// MusMeasure
//----------------------------------------------------------------------------

/**
 * This class represents a measure in a page-based score (MusDoc).
 * A MusMeasure is contained in a MusStaff.
 * It contains MusLayer objects.
 * For internally simplication of processing, unmeasure music is contained in one single measure object
 */
class MusMeasure: public MusDocObject
{
    
public:
    // constructors and destructors
    MusMeasure( bool measuredMusic = true, int logMeasureNb = -1 );
    MusMeasure( const MusMeasure& measure ); // copy contructor
    virtual ~MusMeasure();
    
    virtual std::string MusClassName( ) { return "MusMeasure"; };
    
    /**
     * Return
     */
    bool IsMeasuredMusic() { return m_measuredMusic; };
    
    void Clear();
    
	void AddLayer( MusLayer *layer );
	
	int GetLayerCount() const { return (int)m_children.size(); };
    
    int GetMeasureNo() const;
       
    // functors
    virtual bool Save( ArrayPtrVoid params );
    
	void CopyAttributes( MusMeasure *measure ); // copy all attributes but none of the elements
	//void ClearElements( MusDC *dc , MusElement *start = NULL );
    
	MusLayer *GetFirst( );
	MusLayer *GetLast( );
	MusLayer *GetNext( MusLayer *layer );
	MusLayer *GetPrevious( MusLayer *layer );
    MusLayer *GetLayer( int LayerNo );
        
public:
    /** The logical staff */
    int m_logMeasureNb;
	/**
     * The X absolute position of the measure for facsimile (transcription) encodings.
     * This is the top left position of the measure.
     */
    int m_x_abs;
    /**
     * The X relative position of the measure.
     * It is used internally when calculating the layout and it is not stored in the file.
     */
    int m_x_rel;
	/**
     * The X drawing position of the measure.
     * It is re-computed everytime the measure is drawn and it is not stored in the file.
     */
    int m_x_drawing;
    
private:
    bool m_measuredMusic;
    
};


#endif
