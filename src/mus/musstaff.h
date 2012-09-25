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


//----------------------------------------------------------------------------
// MusStaff
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <staff> element.
 * A MusStaff is a MusMeasureInterface (for measured music) or a MusSectionInterface (for unmeasured music)
 */
class MusStaff: public MusLogicalObject
{
public:
    // constructors and destructors
    MusStaff();
    virtual ~MusStaff();
    
    virtual bool Check();
    
    virtual wxString MusClassName( ) { return "MusStaff"; };
    
    /** the parent MusSection setter (for unmeasured music) */
    void SetSection( MusSection *section );

    /** the parent MusMeasure setter (for measured music) */
    void SetMeasure( MusMeasure *measure );  
	
	void AddLayer( MusLayer *layer );
    
    MusLayer *GetLayer( int layerNo );
    
    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    // functors
    void Save( wxArrayPtrVoid params );
    
public:
    /** The children MusLayer objects */
    ArrayOfMusLayers m_layers;
    /** The parent MusSection (for unmeasured music) */
    MusSection *m_section;
    /** The parent MusMeasure (for measured music) */
    MusMeasure *m_measure;
    /** indicates this staff is in anchent notation
     it has to be carried on to the LayedOutStaf */
    bool m_mensuralNotation;
    
private:
};


//----------------------------------------------------------------------------
// MusStaffFunctor
//----------------------------------------------------------------------------

/**
    This class is a Functor that processes MusStaff objects.
    Needs testing.
*/
class MusStaffFunctor: public MusFunctor
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

#endif
