/////////////////////////////////////////////////////////////////////////////
// Name:        musaligner.h
// Author:      Laurent Pugin
// Created:     2013
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_ALIGNER_H__
#define __MUS_ALIGNER_H__

#include "musobject.h"

class MusSystemAligner;
class MusStaffAlignment;
class MusMeasureAligner;

/**
 * Alignment types for aligning types together.
 * For example, we align notes and rests (default) together, clef separately, etc.
 */
enum MusAlignmentType {
    ALIGNMENT_DEFAULT = 0,
    ALIGNMENT_CLEF,
    ALIGNMENT_MENSUR,
    ALIGNMENT_KEYSIG,
    ALIGNMENT_BARLINE,
    ALIGNMENT_MULTIREST,
    ALIGNMENT_MEASURE_START,
    ALIGNMENT_MEASURE_END
};

//----------------------------------------------------------------------------
// MusSystemAligner
//----------------------------------------------------------------------------

/**
 * This class aligns the content of a system
 * It contains a vector of MusStaffAlignment
 */
class MusSystemAligner: public MusObject
{
public:
    // constructors and destructors
    MusSystemAligner();
    virtual ~MusSystemAligner();
    
    int GetStaffAlignmentCount() const { return (int)m_children.size(); };
    
    /**
     * Reset the aligner (clear the content) and creates the end (bottom) alignement
     */
    void Reset();
    
    /**
     * Get bottom MusStaffAlignment for the system.
     * For each MusSystemAligner, we keep and MusStaffAlignment for the bottom position.
     */
    MusStaffAlignment *GetBottomAlignment( ) { return m_bottomAlignment; };
    
    /**
     * Get the MusStaffAlignment at index idx.
     * Creates the MusStaffAlignment if not there yet.
     * Checks the they are created incrementally (without gap).
     */
    MusStaffAlignment* GetStaffAlignment( int idx );
    
private:
    
public:
    
private:
    /**
     * A pointer to the left MusStaffAlignment object kept for the system bottom position
     */
    MusStaffAlignment *m_bottomAlignment;
    
};

//----------------------------------------------------------------------------
// MusStaffAlignment
//----------------------------------------------------------------------------

/**
 * This class stores an alignement position staves will point to
 */
class MusStaffAlignment: public MusObject
{
public:
    // constructors and destructors
    MusStaffAlignment();
    virtual ~MusStaffAlignment();
    
    void SetYRel( int y_rel ) { m_y_rel = y_rel; };
    int GetYRel() { return m_y_rel; };
    
    void SetYShift( int y_shift );
    int GetYShift() { return m_y_shift; };
    
    /**
     * Set the position of the StaffAlignment.
     * Functor redirected from MusSystem.
     */
    virtual int SetAligmentYPos( ArrayPtrVoid params );
    
    /**
     * Correct the Y alignment once the the content of a system has been aligned and laid out.
     * Special case of functor redirected from MusSystem.
     */
    virtual int IntegrateBoundingBoxYShift( ArrayPtrVoid params );
    
private:
    
public:
    
private:
    /**
     * Stores the position relative to the system.
     */
    int m_y_rel;
    int m_y_shift;
};


//----------------------------------------------------------------------------
// MusAlignment
//----------------------------------------------------------------------------

/** 
 * This class stores an alignement position elements will point to
 */
class MusAlignment: public MusObject
{
public:
    // constructors and destructors
    MusAlignment( );
    MusAlignment( double time, MusAlignmentType type = ALIGNMENT_DEFAULT );
    virtual ~MusAlignment();
    
    void SetXRel( int x_rel );
    int GetXRel() { return m_x_rel; };
    
    void SetXShift( int x_shift );
    int GetXShift() { return m_x_shift; };
    
    void SetMaxWidth( int max_width );
    int GetMaxWidth() { return m_max_width; };

    /**
     * @name Set and get the time value of the alignment
     */
    ///@{
    void SetTime( double time ) { m_time = time; };
    double GetTime() { return m_time; };
    ///@}
    
    /**
     * @name Set and get the type of the alignment
     */
    ///@{
    void SetType( MusAlignmentType type ) { m_type = type; };
    MusAlignmentType GetType() { return m_type; };
    ///@}
    
    /**
     * Correct the X alignment once the the content of a system has been aligned and laid out.
     * Special case of functor redirected from MusMeasure.
     */
    virtual int IntegrateBoundingBoxXShift( ArrayPtrVoid params );
    
    /**
     * Set the position of the MusAlignment.
     * Looks at the time different with the previous MusAlignment.
     */
    virtual int SetAligmentXPos( ArrayPtrVoid params );
    
private:
    
public:
    
private:
    /**
     * Stores the position relative to the measure.
     * This is instanciated the MusObject::SetAligmentXPos functor.
     * It takes into account a non-linear according to the time interval with
     * the previous MusAlignement
     */
    int m_x_rel;
    int m_x_shift;
    int m_max_width;
    double m_time;
    MusAlignmentType m_type;
};

//----------------------------------------------------------------------------
// MusMeasureAligner
//----------------------------------------------------------------------------

/**
 * This class aligns the content of a measure
 * It contains a vector of MusAlignment
 */
class MusMeasureAligner: public MusObject
{
public:
    // constructors and destructors
    MusMeasureAligner();
    virtual ~MusMeasureAligner();
    
    int GetAlignmentCount() const { return (int)m_children.size(); };
    
    /**
     * Reset the aligner (clear the content) and creates the start (left) and end (right) alignement
     */
    void Reset();
    
    MusAlignment* GetAlignmentAtTime( double time, MusAlignmentType type );
    
    /**
     * Keep the maximum time of the measure.
     * This correspond to the whole duration of the measure and 
     * should be the same for all staves/layers.
     */
    void SetMaxTime( double time );
    
    /**
     * Get left MusAlignment for the measure.
     * For each MusMeasureAligner, we keep and MusAlignment for the left position.
     * The MusAlignment time will be always stay 0.0 and be the first in the list.
     */
    MusAlignment *GetLeftAlignment( ) { return m_leftAlignment; };
    
    /**
     * Get right MusAlignment for the measure.
     * For each MusMeasureAligner, we keep and MusAlignment for the right position.
     * The MusAlignment time will be increased whenever necessary when values are added.
     */
    MusAlignment *GetRightAlignment( ) { return m_rightAlignment; };
    
    /**
     * Correct the X alignment once the the content of a system has been aligned and laid out.
     * Special case of functor redirected from MusMeasure.
     */
    virtual int IntegrateBoundingBoxXShift( ArrayPtrVoid params );
    
    
    /**
     * Set the position of the MusAlignment.
     * Looks at the time different with the previous MusAlignment.
     * For each MusMeasureAlignment, we need to reset the previous time position.
     */
    virtual int SetAligmentXPos( ArrayPtrVoid params );

    
private:
    void AddAlignment( MusAlignment *alignment, int idx = -1 );
    
public:
    
private:
    /**
     * A pointer to the left MusAlignment object kept for the measure start position
     */
    MusAlignment *m_leftAlignment;
    
    /**
     * A pointer to the left MusAlignment object kept for the measure end position
     */
    MusAlignment *m_rightAlignment;
};


#endif
