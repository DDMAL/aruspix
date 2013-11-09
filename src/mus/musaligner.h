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
    ALIGNMENT_BARLINE
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
     * Get the MusStaffAlignment at index idx.
     * Creates the MusStaffAlignment if not there yet.
     * Checks the they are created incrementally (without gap).
     */
    MusStaffAlignment* GetStaffAlignment( int idx );
    
private:
    
public:
    
private:
    
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
    
private:
    
public:
    
private:
    /**
     * Stores the position relative to the system.
     */
    int m_y_rel;
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
     * Correct the alignment once the the content of a system has been aligned and laid out.
     * Special case of functor redirected from MusSystem.
     */
    virtual int IntegrateBoundingBoxShift( ArrayPtrVoid params );
    
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
    double m_time;
    double m_max_duration;
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
    
    MusAlignment* GetAlignmentAtTime( double time, MusAlignmentType type );
    
    /**
     * Keep the maximum time of the measure.
     * This correspond to the whole duration of the measure and 
     * should be the same for all staves/layers.
     */
    void SetMaxTime( double time );
    
    /**
     * Correct the alignment once the the content of a system has been aligned and laid out.
     * Special case of functor redirected from MusSystem.
     */
    virtual int IntegrateBoundingBoxShift( ArrayPtrVoid params );
    
    
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
     *
     */
    double m_max;
};


#endif
