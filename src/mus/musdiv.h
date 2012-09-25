/////////////////////////////////////////////////////////////////////////////
// Name:        musdiv.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_DIV_H__
#define __MUS_DIV_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "musobject.h"

class MusScore;
class MusPartSet;
class MusDoc;

/** @file 
 * This file regroups several classes that map the MEI <body> element content up to the MEI <section> element.
 */ 


//----------------------------------------------------------------------------
// MusDiv
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <mdiv> element.
 * For now, no recursive use of MusDiv is allowed as with <mdiv> elements.
 */
class MusDiv: public MusLogicalObject
{
public:
    // constructors and destructors
    MusDiv();
    virtual ~MusDiv();
    
    virtual bool Check();
    
    virtual wxString MusClassName( ) { return "MusDiv"; };
	
	void AddScore( MusScore *score );
	void AddPartSet( MusPartSet *partSet );
    
    /** The partent MusDoc setter */
    void SetDoc( MusDoc *doc );
    
    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    // functors
    void Save( wxArrayPtrVoid params );
    
private:
    
public:
    /** The child MusScore (<score>) */
    MusScore *m_score;
    /** The child MusPartSet (<parts>) */
    MusPartSet *m_partSet;
    /** The parent MusDoc */
    MusDoc *m_doc;

private:
    
};

//----------------------------------------------------------------------------
// MusDivFunctor
//----------------------------------------------------------------------------

/** 
 * This class is a functor for processing MusDiv objects.
 */
class MusDivFunctor: public MusFunctor
{
private:
    void (MusDiv::*fpt)( wxArrayPtrVoid params );   // pointer to member function
    
public:
    
    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusDivFunctor( void(MusDiv::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusDivFunctor() {};
    
    // override function "Call"
    virtual void Call( MusDiv *ptr, wxArrayPtrVoid params )
    { (*ptr.*fpt)( params);};          // execute member function
};


//----------------------------------------------------------------------------
// MusScore
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <score> element.
 * For now, a MusScore can only contain MusSection (<section>) objects.
 */
class MusScore: public MusLogicalObject
{
public:
    // constructors and destructors
    MusScore();
    virtual ~MusScore();
    
    virtual bool Check();
    
    virtual wxString MusClassName( ) { return "MusScore"; };
	
	void AddSection( MusSection *section );

    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    // functors
    void Save( wxArrayPtrVoid params );
    
private:
    
public:
    /** The children MusSection (<section>) objects */
    ArrayOfMusSections m_sections;

private:
    
};


//----------------------------------------------------------------------------
// MusScoreFunctor
//----------------------------------------------------------------------------

/** 
 * This class is a functor for processing MusScore objects.
 */
class MusScoreFunctor: public MusFunctor
{
private:
    void (MusScore::*fpt)( wxArrayPtrVoid params );   // pointer to member function
    
public:
    
    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusScoreFunctor( void(MusScore::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusScoreFunctor() {};
    
    // override function "Call"
    virtual void Call( MusScore *ptr, wxArrayPtrVoid params )
    { (*ptr.*fpt)( params);};          // execute member function
};


//----------------------------------------------------------------------------
// MusPartSet
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <parts> element.
 */
class MusPartSet: public MusLogicalObject
{
public:
    // constructors and destructors
    MusPartSet();
    virtual ~MusPartSet();
    
    virtual bool Check();
    
    virtual wxString MusClassName( ) { return "MusPartSet"; };
	
	void AddPart( MusPart *part );
    
    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    // functors
    void Save( wxArrayPtrVoid params );
        
private:
    
public:
    /** The children MusPart (<part>) objects  */
    ArrayOfMusParts m_parts;

private:
    
};


//----------------------------------------------------------------------------
// MusPartSetFunctor
//----------------------------------------------------------------------------

/** 
 * This class is a functor for processing MusPartSet objects.
 */
class MusPartSetFunctor: public MusFunctor
{
private:
    void (MusPartSet::*fpt)( wxArrayPtrVoid params );   // pointer to member function
    
public:
    
    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusPartSetFunctor( void(MusPartSet::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusPartSetFunctor() {};
    
    // override function "Call"
    virtual void Call( MusPartSet *ptr, wxArrayPtrVoid params )
    { (*ptr.*fpt)( params);};          // execute member function
};


//----------------------------------------------------------------------------
// MusPart
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <part> element.
 */
class MusPart: public MusLogicalObject
{
public:
    // constructors and destructors
    MusPart();
    virtual ~MusPart();
    
    virtual bool Check();
	
    virtual wxString MusClassName( ) { return "MusPart"; };	
    
    void AddSection( MusSection *section );

    /** The partent MusPartSet setter */    
    void SetPartSet( MusPartSet *partSet ) { m_partSet = partSet; };
    
    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    // functors
    void Save( wxArrayPtrVoid params );
    
private:
    
public:
    /** The children MusSection (<section>) objects */
    ArrayOfMusSections m_sections;
    /** The parent MusPartSet */
    MusPartSet *m_partSet;

private:
    
};


//----------------------------------------------------------------------------
// MusPartFunctor
//----------------------------------------------------------------------------

/** 
 * This class is a functor for processing MusPart objects.
 */
class MusPartFunctor: public MusFunctor
{
private:
    void (MusPart::*fpt)( wxArrayPtrVoid params );   // pointer to member function
    
public:
    
    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusPartFunctor( void(MusPart::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusPartFunctor() {};
    
    // override function "Call"
    virtual void Call( MusPart *ptr, wxArrayPtrVoid params )
    { (*ptr.*fpt)( params);};          // execute member function
};


#endif
