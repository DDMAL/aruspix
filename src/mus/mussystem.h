/////////////////////////////////////////////////////////////////////////////
// Name:        mussystem.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SYSTEM_H__
#define __MUS_SYSTEM_H__

#include "musaligner.h"
#include "musbarline.h"
#include "musobject.h"

#define SYSTEM_OFFSET 190

class DeviceContext;
class Measure;

//----------------------------------------------------------------------------
// System
//----------------------------------------------------------------------------

/**
 * This class represents a system in a laid-out score (MusDoc).
 * A System is contained in a Page.
 * It contains Staff objects.
*/
class System: public MusDocObject
{
public:
    // constructors and destructors
    System();
	System( const System& system ); // copy contructor
    virtual ~System();
        
    virtual std::string MusClassName( ) { return "System"; };
    
    void Clear();
	
	void AddMeasure( Measure *measure );
	
	Measure *GetFirst( );
	Measure *GetLast( );
	Measure *GetNext( Measure *measure );
	Measure *GetPrevious( Measure *measure );
    //Measure *GetMeasure( int MeasureNo );
	Measure *GetAtPos( int x );
    
    /**
     * Return the default horizontal spacing of system.
     */
    int GetVerticalSpacing();

    void SetValues( int type );
	
	int GetMeasureCount() const { return (int)m_children.size(); };
    
    /**
     * Return the index position of the system in its page parent
     */
    int GetSystemIdx() const { return MusObject::GetIdx(); };
    
    // functors
    virtual int Save( ArrayPtrVoid params );

    /**
     * Adjust the size of a system according to its content (to be verified)
     */
    virtual int TrimSystem( ArrayPtrVoid params );
    
    /**
     * Align the content of a system.
     */
    virtual int Align( ArrayPtrVoid params );
    
    /**
     * Set the position of the StaffAlignment.
     * Redirect the functor to the MusSytemAligner
     */
    virtual int SetAligmentYPos( ArrayPtrVoid params );
    
    /**
     * Correct the Y alignment once the the content of a system has been aligned and laid out.
     * Special case that redirects the functor to the MusSystemAligner.
     */
    virtual int IntegrateBoundingBoxYShift( ArrayPtrVoid params );
    
    /**
     * Align the system by adjusting the m_yRel position looking at the MusSystemAligner.
     */
    virtual int AlignSystems( ArrayPtrVoid params );
    
    /**
     * Align the measures by adjusting the m_xRel position looking at the MusMeasureAligner.
     * In System object resets the shift to 0;
     */
    virtual int AlignMeasures( ArrayPtrVoid params );
    
    /** 
     * Store the width of the system in the MusMeasureAligner for justification
     */
    virtual int AlignMeasuresEnd( ArrayPtrVoid params );
    
    /**
     * Justify the X positions
     * Special case that redirects the functor to the MusMeasureAligner.
     */
    virtual int JustifyX( ArrayPtrVoid params );
    
private:
    MusSystemAligner m_systemAligner;
    
public:
    /** System left margin (MEI scoredef@system.leftmar). Saved if != 0 */
    int m_systemLeftMar;
    /** System right margin (MEI scoredef@system.rightmar). Saved if != 0 */
    int m_systemRightMar;
	/** 
     * The Y absolute position of the staff for facsimile (transcription) encodings.
     * This is the top left corner of the system.
     */
    int m_yAbs;
    /**
     * The Y relative position of the system.
     * It is used internally when calculating the layout andd it is not stored in the file.
     */
    int m_yRel;
	/**
     * The Y drawing position of the system.
     * It is re-computed everytime the system is drawn and it is not stored in the file.
     */
    int m_yDrawing;
	/** 
     * The x absolute position of the  system for facsimile layouts.
     * This is the top left corner of the system.
     */
    int m_xAbs;
    /**
     * The X relative position of the system.
     * It is used internally when calculating the layout andd it is not stored in the file.
     */
    int m_xRel;
	/**
     * The X drawing position of the system.
     * It is re-computed everytime the system is drawn and it is not stored in the file.
     */
    int m_xDrawing;
    /**
     * The total width of the system.
     * It is computed during the layout processed and used for calculating the justification ratio.
     */
    int m_totalDrawingWidth;

private:
    
};

#endif
