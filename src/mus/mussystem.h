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

class MusDC;
class MusMeasure;

//----------------------------------------------------------------------------
// MusSystem
//----------------------------------------------------------------------------

/**
 * This class represents a system in a laid-out score (MusDoc).
 * A MusSystem is contained in a MusPage.
 * It contains MusStaff objects.
*/
class MusSystem: public MusDocObject
{
public:
    // constructors and destructors
    MusSystem();
	MusSystem( const MusSystem& system ); // copy contructor
    virtual ~MusSystem();
        
    virtual std::string MusClassName( ) { return "MusSystem"; };
    virtual std::string GetIdShortName() { return "system-"; };
    
    void Clear();
	
	void AddMeasure( MusMeasure *measure );
	
	MusMeasure *GetFirst( );
	MusMeasure *GetLast( );
	MusMeasure *GetNext( MusMeasure *measure );
	MusMeasure *GetPrevious( MusMeasure *measure );
    MusMeasure *GetMeasure( int MeasureNo );
	MusMeasure *GetAtPos( int x );
    
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
     * Align the system by adjusting the m_y_rel position looking at the MusSystemAligner.
     */
    virtual int AlignSystems( ArrayPtrVoid params );
    
    /**
     * Align the measures by adjusting the m_x_rel position looking at the MusMeasureAligner.
     * In MusSystem object resets the shift to 0;
     */
    virtual int AlignMeasures( ArrayPtrVoid params );
    
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
    int m_y_abs;
    /**
     * The Y relative position of the system.
     * It is used internally when calculating the layout andd it is not stored in the file.
     */
    int m_y_rel;
	/**
     * The Y drawing position of the system.
     * It is re-computed everytime the system is drawn and it is not stored in the file.
     */
    int m_y_drawing;
	/** 
     * The x absolute position of the  system for facsimile layouts.
     * This is the top left corner of the system.
     */
    int m_x_abs;
    /**
     * The X relative position of the system.
     * It is used internally when calculating the layout andd it is not stored in the file.
     */
    int m_x_rel;
	/**
     * The X drawing position of the system.
     * It is re-computed everytime the system is drawn and it is not stored in the file.
     */
    int m_x_drawing;

private:
    
};

#endif
