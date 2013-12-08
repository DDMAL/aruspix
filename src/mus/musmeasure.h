/////////////////////////////////////////////////////////////////////////////
// Name:        musmeasure.h
// Author:      Laurent Pugin
// Created:     2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_MEASURE_H__
#define __MUS_MEASURE_H__

#include "musobject.h"

#include "musaligner.h"

class MusStaff;

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
    virtual ~MusMeasure();
    
    virtual std::string MusClassName( ) { return "MusMeasure"; };
    
    /**
     * Return
     */
    bool IsMeasuredMusic() { return m_measuredMusic; };
    
    void Clear();
    
	void AddStaff( MusStaff *staff );
	
	int GetStaffCount() const { return (int)m_children.size(); };
    
    /**
     * Return the index position of the measure in its system parent
     */
    int GetMeasureNo() const { return MusObject::GetIdx(); };
       
    // functors
    virtual int Save( ArrayPtrVoid params );
    
	void CopyAttributes( MusMeasure *measure ); // copy all attributes but none of the elements
	//void ClearElements( MusDC *dc , MusElement *start = NULL );
    
	MusStaff *GetFirst( );
	MusStaff *GetLast( );
	MusStaff *GetNext( MusStaff *layer );
	MusStaff *GetPrevious( MusStaff *layer );
    MusStaff *GetStaff( int StaffNo );
    
    int GetXRel( );
    
    /**
     * Align the content of a system.
     */
    virtual int Align( ArrayPtrVoid params );
    
    /**
     * Correct the X alignment once the the content of a system has been aligned and laid out.
     * Special case that redirects the functor to the MusMeasureAligner.
     */
    virtual int IntegrateBoundingBoxXShift( ArrayPtrVoid params );
    
    /**
     * Set the position of the MusAlignment.
     * Special case that redirects the functor to the MusAligner.
     */
    virtual int SetAligmentXPos( ArrayPtrVoid params );
    
    /**
     * Align the measures by adjusting the m_x_rel position looking at the MusMeasureAligner.
     */
    virtual int AlignMeasures( ArrayPtrVoid params );
        
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
    
    MusMeasureAligner m_measureAligner;
};


#endif
