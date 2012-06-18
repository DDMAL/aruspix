/////////////////////////////////////////////////////////////////////////////
// Name:        mussection.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SECTION_H__
#define __MUS_SECTION_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "musdiv.h"

class MusSectionInterface;


//----------------------------------------------------------------------------
// MusSection
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <section> element.
 */
class MusSection: public MusLogicalObject
{
public:
    // constructors and destructors
    MusSection();
    virtual ~MusSection();
    
    virtual wxString MusClassName( ) { return "MusSection"; };
	
	void AddMeasure( MusMeasure *measure );
    
    void AddStaff( MusStaff *staff );
    
    /** The parent MusScore setter */
    void SetScore( MusScore *score );
    /** The parent MusPart setter */
    void SetPart( MusPart *part );
    
    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    // functors
    void Save( wxArrayPtrVoid params );
    void Load( wxArrayPtrVoid params );    
    
public:
    /** The children MusMeasure objects */
    ArrayOfMusMeasures m_measures;
    /** The children MusStaff objects */
    ArrayOfMusStaves m_staves;    
    /** The parent MusPart */
    MusPart *m_part;
    /** The parent MusScore */
    MusScore *m_score;

private:
};


//----------------------------------------------------------------------------
// MusSectionFunctor
//----------------------------------------------------------------------------

/** 
 * This class is a functor for processing MusSection objects.
 */
class MusSectionFunctor: public MusFunctor
{
private:
    void (MusSection::*fpt)( wxArrayPtrVoid params );   // pointer to member function
    
public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusSectionFunctor( void(MusSection::*_fpt)( wxArrayPtrVoid ))  { fpt=_fpt; };
	virtual ~MusSectionFunctor() {};

    // override function "Call"
    virtual void Call( MusSection *ptr, wxArrayPtrVoid params )
    { (*ptr.*fpt)( params);};          // execute member function
};

#endif
