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
	
	void AddSectionElement( MusSectionInterface *sectionElement );
    
    /** The parent MusScore setter */
    void SetScore( MusScore *score );
    /** The parent MusPart setter */
    void SetPart( MusPart *part );
    
public:
    /** The children MusSectionInterface objects */
    ArrayOfMusSectionElements m_sectionElements;
    /** The parent MusPart */
    MusPart *m_part;
    /** The parent MusScore */
    MusScore *m_score;

private:
};


//----------------------------------------------------------------------------
// abstract base class MusSectionFunctor
//----------------------------------------------------------------------------

/** 
 * This class is a functor for processing MusSection objects.
 */
class MusSectionFunctor
{
private:
    void (MusSection::*fpt)( wxArrayPtrVoid params );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusSectionFunctor( void(MusSection::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusSectionFunctor() {};

    // override function "Call"
    virtual void Call( MusSection *ptr, wxArrayPtrVoid params )
        { (*ptr.*fpt)( params);};          // execute member function
};



//----------------------------------------------------------------------------
// MusSectionInterface
//----------------------------------------------------------------------------

/** 
 * This class is an interface for the MusSection (<section>) content.
 * It is not an abstract class but should not be instanciate directly.
 */
class MusSectionInterface
{
public:
    // constructors and destructors
    MusSectionInterface();
    virtual ~MusSectionInterface();
    
    /** the parent MusSection setter */
    void SetSection( MusSection *section ) { m_section = section; };
    
private:
    
public:
    /** The parent MusSection */
    MusSection *m_section;

private:
    
};


#endif
