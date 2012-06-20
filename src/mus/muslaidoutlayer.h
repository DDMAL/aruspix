/////////////////////////////////////////////////////////////////////////////
// Name:        muslaidoutlayer.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_LAID_OUT_LAYER_H__
#define __MUS_LAID_OUT_LAYER_H__

#include <typeinfo>

class MusDC;

#include "muslaidoutstaff.h"

//#define STAFF_OFFSET 190

class MusNote;
class MusLaidOutLayerElement;

//----------------------------------------------------------------------------
// MusLaidOutLayer
//----------------------------------------------------------------------------

/**
 * This class represents a layer in a laid-out score (MusLayout).
 * A MusLaidOutLayer is contained in a MusLaidOutStaff.
 * It contains MusLaidOutLayerElement objects.
*/
class MusLaidOutLayer: public MusLayoutObject
{
public:
    // constructors and destructors
    MusLaidOutLayer( int logLayerNb, int logStaffNb, MusSection *section, MusMeasure *measure );
    virtual ~MusLaidOutLayer();
    
    virtual wxString MusClassName( ) { return "MusLaidOutLayer"; };	
    
    void Clear();
    
    /** The parent MusLaidOutStaff setter */
    void SetStaff( MusLaidOutStaff *staff ) { m_staff = staff; };
	
	void AddElement( MusLaidOutLayerElement *element );
    	
	int GetElementCount() const { return (int)m_elements.GetCount(); };
    
    int GetLayerNo() const;

    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    // functors
    void CopyElements( wxArrayPtrVoid params );
    void GetMaxXY( wxArrayPtrVoid params );
    void Save( wxArrayPtrVoid params );
    void Load( wxArrayPtrVoid params );
    
	void CopyAttributes( MusLaidOutLayer *layer ); // copy all attributes but none of the elements
    
	MusLaidOutLayerElement *GetFirst( );
	MusLaidOutLayerElement *GetLast( );
	MusLaidOutLayerElement *GetNext( MusLaidOutLayerElement *element );
	MusLaidOutLayerElement *GetPrevious( MusLaidOutLayerElement *element );
	MusLaidOutLayerElement *GetAtPos( int x );
	MusLaidOutLayerElement *Insert( MusLayerElement *element, int x ); // return a pointer on the inserted element
	void Delete( MusLaidOutLayerElement *element );
	/** 
     * Looks for the first MusLaidOutLayerElement with an LayoutElement of type elementType.
     * Looks FORWARD of BACKWARD depending on the direction parameter.
     * Returns the retrieved element if *succ == true or the original element if not.
     */
    MusLaidOutLayerElement *GetFirst( MusLaidOutLayerElement *element, unsigned int direction, const std::type_info *elementType, bool *succ );
    /** Get the current clef for the test element */
	MusClef *GetClef ( MusLaidOutLayerElement *test );
	void getOctDec (int ft, int _ot, int rupt, int *oct);
    /** Return the clef offset for the position x (retrieve the first clef before it) */
    int GetClefOffset( MusLaidOutLayerElement *test  );

    //Lyric related methods
    /*
	int GetLyricPos( MusSymbol *lyric );
	MusSymbol *GetPreviousLyric( MusSymbol *lyric );
	MusSymbol *GetNextLyric( MusSymbol *lyric );
	MusSymbol *GetFirstLyric( );
	MusSymbol *GetLastLyric( );
	MusSymbol *GetLyricAtPos( int x );
	void DeleteLyric( MusSymbol *symbol );
	MusNote *GetNextNote( MusSymbol * lyric );
	MusNote *GetPreviousNote( MusSymbol * lyric );
	void SwitchLyricNoteAssociation( MusSymbol *lyric, MusNote *oldNote, MusNote* newNote, bool beginning );
	void AdjustLyricLineHeight( int delta );
    */
    
private:
    
public:
    /** The MusLaidOutLayerElement objects of the layer */
    ArrayOfMusLaidOutLayerElements m_elements;
    /** The parent MusLaidOutStaff */
    MusLaidOutStaff *m_staff;
    /** The logical layer */
    int m_logLayerNb;
    /** The logical staff - used to overwrite the parent staff */
    int m_logStaffNb;
    /** The section in the logical tree */
    MusSection *m_section;
    /** The measure in the logical tree (for measure music only) */
    MusMeasure *m_measure;
    
	/** voix de la portee*/
	unsigned short voix;    
    
    // drawing variables
    MusLaidOutLayerElement *beamListPremier; // we need to replace this with a proper beam class that handles a list of notes/rests

private:
    
};


//----------------------------------------------------------------------------
// MusLaidOutLayerFunctor
//----------------------------------------------------------------------------

/**
 * This class is a Functor that processes MusLaidOutLayer objects.
 * Needs testing.
*/
class MusLaidOutLayerFunctor: public MusFunctor
{
private:
    void (MusLaidOutLayer::*fpt)( wxArrayPtrVoid params );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusLaidOutLayerFunctor( void(MusLaidOutLayer::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusLaidOutLayerFunctor() {};

    // override function "Call"
    virtual void Call( MusLaidOutLayer *ptr, wxArrayPtrVoid params )
        { (*ptr.*fpt)( params);};          // execute member function
};



#endif
