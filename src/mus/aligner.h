/////////////////////////////////////////////////////////////////////////////
// Name:        musaligner.h
// Author:      Laurent Pugin
// Created:     2013
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_ALIGNER_H__
#define __MUS_ALIGNER_H__

#include "object.h"

namespace vrv {

class SystemAligner;
class StaffAlignment;
class MeasureAligner;

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
// SystemAligner
//----------------------------------------------------------------------------

/**
 * This class aligns the content of a system
 * It contains a vector of StaffAlignment
 */
class SystemAligner: public Object
{
public:
    // constructors and destructors
    SystemAligner();
    virtual ~SystemAligner();
    
    int GetStaffAlignmentCount() const { return (int)m_children.size(); };
    
    /**
     * Reset the aligner (clear the content) and creates the end (bottom) alignement
     */
    void Reset();
    
    /**
     * Get bottom StaffAlignment for the system.
     * For each SystemAligner, we keep and StaffAlignment for the bottom position.
     */
    StaffAlignment *GetBottomAlignment( ) { return m_bottomAlignment; };
    
    /**
     * Get the StaffAlignment at index idx.
     * Creates the StaffAlignment if not there yet.
     * Checks the they are created incrementally (without gap).
     */
    StaffAlignment* GetStaffAlignment( int idx );
    
private:
    
public:
    
private:
    /**
     * A pointer to the left StaffAlignment object kept for the system bottom position
     */
    StaffAlignment *m_bottomAlignment;
    
};

//----------------------------------------------------------------------------
// StaffAlignment
//----------------------------------------------------------------------------

/**
 * This class stores an alignement position staves will point to
 */
class StaffAlignment: public Object
{
public:
    // constructors and destructors
    StaffAlignment();
    virtual ~StaffAlignment();
    
    void SetYRel( int yRel ) { m_yRel = yRel; };
    int GetYRel() { return m_yRel; };
    
    void SetYShift( int yShift );
    int GetYShift() { return m_yShift; };
    
    /**
     * Set the position of the StaffAlignment.
     * Functor redirected from System.
     */
    virtual int SetAligmentYPos( ArrayPtrVoid params );
    
    /**
     * Correct the Y alignment once the the content of a system has been aligned and laid out.
     * Special case of functor redirected from System.
     */
    virtual int IntegrateBoundingBoxYShift( ArrayPtrVoid params );
    
private:
    
public:
    
private:
    /**
     * Stores the position relative to the system.
     */
    int m_yRel;
    int m_yShift;
};


//----------------------------------------------------------------------------
// Alignment
//----------------------------------------------------------------------------

/** 
 * This class stores an alignement position elements will point to
 */
class Alignment: public Object
{
public:
    // constructors and destructors
    Alignment( );
    Alignment( double time, MusAlignmentType type = ALIGNMENT_DEFAULT );
    virtual ~Alignment();
    
    void SetXRel( int x_rel );
    int GetXRel() { return m_xRel; };
    
    void SetXShift( int xShift );
    int GetXShift() { return m_xShift; };
    
    void SetMaxWidth( int max_width );
    int GetMaxWidth() { return m_maxWidth; };

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
     * Special case of functor redirected from Measure.
     */
    virtual int IntegrateBoundingBoxXShift( ArrayPtrVoid params );
    
    /**
     * Set the position of the Alignment.
     * Looks at the time different with the previous Alignment.
     */
    virtual int SetAligmentXPos( ArrayPtrVoid params );
    
    /**
     * Justify the X positions
     * Special case of functor redirected from Measure.
     */
    virtual int JustifyX( ArrayPtrVoid params );
    
private:
    
public:
    
private:
    /**
     * Stores the position relative to the measure.
     * This is instanciated the Object::SetAligmentXPos functor.
     * It takes into account a non-linear according to the time interval with
     * the previous MusAlignement
     */
    int m_xRel;
    /**
     * Stores temporally the maximum amount we need to shift the element pointing to it for 
     * avoiding collisions. This is set in Object::SetBoundingBoxXShift and then
     * integrated for all aligment in MusAligment::IntegrateBoundingBoxXShift.
     */
    int m_xShift;
    /**
     * Stores temporally the maximum width of the of the element pointing to it.
     * It is set and integrated as m_xShift and it is used only for shifting the
     * alignment of the end of the measure (ALIGNMENT_MEASURE_END).
     */
    int m_maxWidth;
    /**
     * Stores the time at which the alignment occur.
     * It is set by  Object::Align.
     */
    double m_time;
    /**
     * Defines the type of alignment (see the MusAlignmentType enum).
     * We have different types because we want events occuring at the same
     * time to be alignnemt separately. Example: the clef needs to be aligned
     * togeter, but key signature together and then the notes, even if all
     * of them occur at time 0.
     */
    MusAlignmentType m_type;
};

//----------------------------------------------------------------------------
// MeasureAligner
//----------------------------------------------------------------------------

/**
 * This class aligns the content of a measure
 * It contains a vector of Alignment
 */
class MeasureAligner: public Object
{
public:
    // constructors and destructors
    MeasureAligner();
    virtual ~MeasureAligner();
    
    int GetAlignmentCount() const { return (int)m_children.size(); };
    
    /**
     * Reset the aligner (clear the content) and creates the start (left) and end (right) alignement
     */
    void Reset();
    
    Alignment* GetAlignmentAtTime( double time, MusAlignmentType type );
    
    /**
     * Keep the maximum time of the measure.
     * This correspond to the whole duration of the measure and 
     * should be the same for all staves/layers.
     */
    void SetMaxTime( double time );
    
    /**
     * @name Set and get the total width
     */
    ///@{
    //void SetTotalWidh( int width ) { m_totalWidth = width; };
    //int GetTotalWidth() { return m_totalWidth; };
    ///@}
    
    /**
     * Get left Alignment for the measure.
     * For each MeasureAligner, we keep and Alignment for the left position.
     * The Alignment time will be always stay 0.0 and be the first in the list.
     */
    Alignment *GetLeftAlignment( ) { return m_leftAlignment; };
    
    /**
     * Get right Alignment for the measure.
     * For each MeasureAligner, we keep and Alignment for the right position.
     * The Alignment time will be increased whenever necessary when values are added.
     */
    Alignment *GetRightAlignment( ) { return m_rightAlignment; };
    
    /**
     * Correct the X alignment once the the content of a system has been aligned and laid out.
     * Special case of functor redirected from Measure.
     */
    virtual int IntegrateBoundingBoxXShift( ArrayPtrVoid params );
    
    /**
     * Set the position of the Alignment.
     * Looks at the time different with the previous Alignment.
     * For each MusMeasureAlignment, we need to reset the previous time position.
     */
    virtual int SetAligmentXPos( ArrayPtrVoid params );
    
    /**
     * Justify the X positions
     * Special case of functor redirected from Measure.
     */
    //virtual int JustifyX( ArrayPtrVoid params );

    
private:
    void AddAlignment( Alignment *alignment, int idx = -1 );
    
public:
    
private:
    /**
     * A pointer to the left Alignment object kept for the measure start position
     */
    Alignment *m_leftAlignment;
    
    /**
     * A pointer to the left Alignment object kept for the measure end position
     */
    Alignment *m_rightAlignment;
    
    /**
     * Store the system width in order to calculate the justification ratio
     */
    //int m_totalWidth;
};

} // namespace vrv
    
#endif
