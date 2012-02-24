/////////////////////////////////////////////////////////////////////////////
// Name:        muspage.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_PAGE_H__
#define __MUS_PAGE_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class MusDC;

#include "muslayout.h"

class MusSystem;
class MusLaidOutStaff;
class MusSystemFunctor;
class MusStaffFunctor;

enum {
    PAGE_VALUES_VOICES = 0,
    PAGE_VALUES_INDENT
};


//----------------------------------------------------------------------------
// MusPage
//----------------------------------------------------------------------------

/**
 * This class represents a page in a laid-out score (MusLayout).
 * A MusPage is contained in a MusLayout.
 * It contains MusSystem objects.
*/
class MusPage: public MusLayoutObject
{
public:
    // constructors and destructors
    MusPage();
    virtual ~MusPage();
    
    void Clear();
    
    /** The parent MusLayout setter */
    //void SetLayout( MusLayout *layout ) { m_layout = layout; };
	
	void AddSystem( MusSystem *system );
    
	MusSystem *GetFirst( );
	MusSystem *GetLast( );
	MusSystem *GetNext( MusSystem *system );
	MusSystem *GetPrevious( MusSystem *system );
	MusSystem *GetAtPos( int y );

    void SetValues( int type );
	
	int GetSystemCount() const { return (int)m_systems.GetCount(); };
    
    int GetPageNo() const;

    // moulinette
    virtual void Process(MusLayoutFunctor *functor, wxArrayPtrVoid params );
    // functors
    void ProcessStaves( wxArrayPtrVoid params );
    void ProcessVoices( wxArrayPtrVoid params );
    void CountVoices( wxArrayPtrVoid params );
    
    /**
     * Calculates the system positions for the page.
     */
    virtual void UpdateSystemPositions( );
    
    
private:
    
public:
    /** The MusSystem objects of the page */
    ArrayOfMusSystems m_systems;    
    
    /** numero de page */
    int npage;
    /** contient un masque fixe */
    char noMasqueFixe;
    /** contient un masque variable */
    char noMasqueVar;
    /** reserve */
    unsigned char reserve;
    /** definition en mm des portees de la page */
    unsigned char defin;
    /** longueur en mm de l'indentation des portees de la page */
    //int indent; // ax2
    /** longueur en mm de l'indentation droite des portees de la page */
    //int indentDroite; // ax2
    /** longueur en mm des lignes de la pages */
    //int lrg_lign; // ax2

private:
    
};


//----------------------------------------------------------------------------
// abstract base class MusPageFunctor
//----------------------------------------------------------------------------

/**
 * This class is a Functor that processes MusPage objects.
 * Needs testing.
*/
class MusPageFunctor: public MusLayoutFunctor
{
private:
    void (MusPage::*fpt)( wxArrayPtrVoid params );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusPageFunctor( void(MusPage::*_fpt)( wxArrayPtrVoid ) ) { fpt=_fpt; };
	virtual ~MusPageFunctor() {};

    // override function "Call"
    void Call( MusPage *ptr, wxArrayPtrVoid params )
        { (*ptr.*fpt)( params );};          // execute member function
};



#endif
