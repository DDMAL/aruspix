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
	
	void AddScore( MusScore *score );
	void AddPartSet( MusPartSet *partSet );
    
    /** The partent MusDoc setter */
    void SetDoc( MusDoc *doc ) { m_doc = doc; };
    
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
	
	void AddSection( MusSection *section );
    
    /** The partent MusDiv setter */
    void SetDiv( MusDiv *div ) { m_div = div; };
    
private:
    
public:
    /** The children MusSection (<section>) objects */
    ArrayOfMusSections m_sections;
    /** The parent MusDiv */
    MusDiv *m_div;

private:
    
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
	
	void AddPart( MusPart *part );
    
    /** The partent MusDiv setter */
    void SetDiv( MusDiv *div ) { m_div = div; };
        
private:
    
public:
    /** The children MusPart (<part>) objects  */
    ArrayOfMusParts m_parts;
    /* The parent MusDiv */
    MusDiv *m_div;


private:
    
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
	
	void AddSection( MusSection *section );

    /** The partent MusPartSet setter */    
    void SetPartSet( MusPartSet *partSet ) { m_partSet = partSet; };
    
private:
    
public:
    /** The children MusSection (<section>) objects */
    ArrayOfMusSections m_sections;
    /** The parent MusPartSet */
    MusPartSet *m_partSet;

private:
    
};


#endif
