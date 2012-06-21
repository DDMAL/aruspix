/////////////////////////////////////////////////////////////////////////////
// Name:        musmeasure.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_MEASURE_H__
#define __MUS_MEASURE_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mussection.h"

//----------------------------------------------------------------------------
// MusMeasure
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <measure> element. 
 */
class MusMeasure: public MusLogicalObject
{
public:
    // constructors and destructors
    MusMeasure();
    virtual ~MusMeasure();
    
    virtual bool Check();
    
    virtual wxString MusClassName( ) { return "MusMeasure"; };
    
    /** the parent MusSection setter (for unmeasured music) */
    void SetSection( MusSection *section ) { m_section = section; };
	
	void AddStaff( MusStaff *staff );
    
    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    // functors
    void Save( wxArrayPtrVoid params );
    void Load( wxArrayPtrVoid params );
    
private:
    
public:
    /** The children MusStaff objects */
    ArrayOfMusStaves m_staves;
    /** The parent MusSection (for unmeasured music) */
    MusSection *m_section;

private:
    
};


//----------------------------------------------------------------------------
// MusMeasureFunctor
//----------------------------------------------------------------------------

/**
 This class is a Functor that processes MusMeasure objects.
 Needs testing.
 */
class MusMeasureFunctor: public MusFunctor
{
private:
    void (MusMeasure::*fpt)( wxArrayPtrVoid params );   // pointer to member function
    
public:
    
    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusMeasureFunctor( void(MusMeasure::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusMeasureFunctor() {};
    
    // override function "Call"
    virtual void Call( MusMeasure *ptr, wxArrayPtrVoid params )
    { (*ptr.*fpt)( params);};          // execute member function
};

#endif
