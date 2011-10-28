/////////////////////////////////////////////////////////////////////////////
// Name:        musstaff.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_STAFF_H__
#define __MUS_STAFF_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "musmeasure.h"
#include "mussection.h"

class MusStaffInterface;


//----------------------------------------------------------------------------
// MusStaff
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <staff> element.
 * A MusStaff is a MusMeasureInterface (for measured music) or a MusSectionInterface (for unmeasured music)
 */
class MusStaff: public 	MusLogicalObject, public MusMeasureInterface, public MusSectionInterface
{
public:
    // constructors and destructors
    MusStaff();
    virtual ~MusStaff();
	
	void AddStaffElement( MusStaffInterface *staffElement );
    
public:
    /** The children MusStaffInterface objects */
    ArrayOfMusStaffElements m_staffElements;

private:
};


//----------------------------------------------------------------------------
// abstract base class MusStaffFunctor
//----------------------------------------------------------------------------

/**
    This class is a Functor that processes MusStaff objects.
    Needs testing.
*/
class MusStaffFunctor
{
private:
    void (MusStaff::*fpt)( wxArrayPtrVoid params );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusStaffFunctor( void(MusStaff::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusStaffFunctor() {};

    // override function "Call"
    virtual void Call( MusStaff *ptr, wxArrayPtrVoid params )
        { (*ptr.*fpt)( params);};          // execute member function
};



//----------------------------------------------------------------------------
// MusStaffInterface
//----------------------------------------------------------------------------

/** 
 * This class is an interface for the MusStaff (<staff>) content.
 * It is not an abstract class but should not be instanciate directly.
 */
class MusStaffInterface
{
public:
    // constructors and destructors
    MusStaffInterface();
    virtual ~MusStaffInterface();
    
    /** the parent MusStaff setter */    
    void SetStaff( MusStaff *staff ) { m_staff = staff; };
    
private:
    
public:
    /** The parent MusStaff */
    MusStaff *m_staff;

private:
    
};


#endif
