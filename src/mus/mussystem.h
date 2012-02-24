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

class MusStaff;

//----------------------------------------------------------------------------
// MusSystem
//----------------------------------------------------------------------------

/**
 * This class represents a system in a laid-out score (MusLayout).
 * A MusSystem is contained in a MusPage.
 * It contains MusLaidOutStaff objects.
*/
class MusSystem: public MusLayoutObject
{
public:
    // constructors and destructors
    MusSystem();
	MusSystem( const MusSystem& system ); // copy contructor
    virtual ~MusSystem();
    
    void Clear();
    
    /** The parent MusPage setter */
    void SetPage( MusPage *page ) { m_page = page; };
    
    //void SetDoc( wxArrayPtrVoid params );
	
	void AddStaff( MusLaidOutStaff *staff );
	
	MusLaidOutStaff *GetFirst( );
	MusLaidOutStaff *GetLast( );
	MusLaidOutStaff *GetNext( MusLaidOutStaff *staff );
	MusLaidOutStaff *GetPrevious( MusLaidOutStaff *staff );
	MusLaidOutStaff *GetAtPos( int y );

    void SetValues( int type );
	
	int GetStaffCount() const { return (int)m_staves.GetCount(); };
    
    int GetSystemNo() const;
    
    // moulinette
    virtual void Process(MusLayoutFunctor *functor, wxArrayPtrVoid params );
    // functors
    //void ProcessStaves( wxArrayPtrVoid params );
    //void ProcessVoices( wxArrayPtrVoid params );
    //void CountVoices( wxArrayPtrVoid params );
    
    int UpdateStaffPositions();
    
    
private:
    
public:
    /** The MusLaidOutStaff objects of the system */
    ArrayOfMusLaidOutStaves m_staves;
    /** The parent MusPage */
    MusPage *m_page;

    /** longueur en mm de l'indentation des portees de la page */
    int indent;
    /** longueur en mm de l'indentation droite des portees de la page */
    int indentDroite;
    /** longueur en mm des lignes de la pages */
    int lrg_lign;
	/** position y relative du system (non-enregistre dans les fichiers) */
    int m_yrel;
	/** postion x relative du system (non-enregistre dans les fichiers) */
    int m_xrel;

private:
    
};


//----------------------------------------------------------------------------
// abstract base class MusSystemFunctor
//----------------------------------------------------------------------------

/**
 * This class is a Functor that processes MusSystem objects.
 * Needs testing.
*/
class MusSystemFunctor: public MusLayoutFunctor
{
private:
    void (MusSystem::*fpt)( wxArrayPtrVoid params );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusSystemFunctor( void(MusSystem::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusSystemFunctor() {};

    // override function "Call"
    virtual void Call( MusSystem *ptr, wxArrayPtrVoid params )
        { (*ptr.*fpt )( params ) ; };          // execute member function
};



#endif
