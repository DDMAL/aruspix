/////////////////////////////////////////////////////////////////////////////
// Name:        mussystem.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SYSTEM_H__
#define __MUS_SYSTEM_H__

#include "musbarline.h"
#include "musobject.h"

#define SYSTEM_OFFSET 190

class MusDC;
class MusStaff;

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
    
    //virtual bool Check();
    
    virtual std::string MusClassName( ) { return "MusSystem"; };	    
    
    void Clear();
    
    //void SetDoc( ArrayPtrVoid params );
	
	void AddStaff( MusStaff *staff );
	
	MusStaff *GetFirst( );
	MusStaff *GetLast( );
	MusStaff *GetNext( MusStaff *staff );
	MusStaff *GetPrevious( MusStaff *staff );
    MusStaff *GetStaff( int StaffNo );
	MusStaff *GetAtPos( int y );
    
    /**
     * Return the default horizontal spacing of system.
     */
    int GetVerticalSpacing();

    void SetValues( int type );
	
	int GetStaffCount() const { return (int)m_children.size(); };
    
    int GetSystemNo() const;
    
    // functors
    virtual bool Save( ArrayPtrVoid params );

    /**
     * Adjust the size of a system according to its content (to be verified)
     */
    virtual bool TrimSystem( ArrayPtrVoid params );
    
private:
    
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
     * It is re-computed everytime the staff is drawn and it is not stored in the file.
     */
    int m_y_drawing;
	/** 
     * The x absolute position of the  system for facsimile layouts.
     * This is the top left corner of the system.
     */
    int m_x_abs;

private:
    
};

#endif
