/////////////////////////////////////////////////////////////////////////////
// Name:        mussystem.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SYSTEM_H__
#define __MUS_SYSTEM_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class MusDC;

#include "muspage.h"
#include "musbarline.h"

#define SYSTEM_OFFSET 190

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
    
    virtual wxString MusClassName( ) { return "MusSystem"; };	    
    
    void Clear();
    
    //void SetDoc( ArrayPtrVoid params );
	
	void AddStaff( MusStaff *staff );
	
	MusStaff *GetFirst( );
	MusStaff *GetLast( );
	MusStaff *GetNext( MusStaff *staff );
	MusStaff *GetPrevious( MusStaff *staff );
    MusStaff *GetStaff( int StaffNo );
	MusStaff *GetAtPos( int y );

    void SetValues( int type );
	
	int GetStaffCount() const { return (int)m_children.size(); };
    
    int GetSystemNo() const;
    
    // functors
    virtual bool Save( ArrayPtrVoid params );
    //void ProcessStaves( ArrayPtrVoid params );
    //void ProcessVoices( ArrayPtrVoid params );
    //void CountVoices( ArrayPtrVoid params );
    
private:
    
public:
    /** System left margin (MEI scoredef@system.leftmar). Saved if != 0 */
    int m_systemLeftMar;
    /** System right margin (MEI scoredef@system.rightmar). Saved if != 0 */
    int m_systemRightMar;
	/** 
     * The y absolute position of the system for facsimile layouts.
     * This is the top left corner of the system.
     */
    int m_y_abs;
	/** 
     * The x absolute position of the  system for facsimile layouts.
     * This is the top left corner of the system.
     */
    int m_x_abs;

private:
    
};

#endif
