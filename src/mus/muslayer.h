/////////////////////////////////////////////////////////////////////////////
// Name:        muslayer.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_LAYER_H__
#define __MUS_LAYER_H__

#include <typeinfo>

class MusDC;

#include "musstaff.h"

//#define STAFF_OFFSET 190

class MusNote;
class MusLayerElement;

//----------------------------------------------------------------------------
// MusLayer
//----------------------------------------------------------------------------

/**
 * This class represents a layer in a laid-out score (MusDoc).
 * A MusLayer is contained in a MusStaff.
 * It contains MusLayerElement objects.
*/
class MusLayer: public MusDocObject, public MusObjectListInterface
{
public:
    // constructors and destructors
    MusLayer( int logLayerNb );
    virtual ~MusLayer();
    
    virtual wxString MusClassName( ) { return "MusLayer"; };	
    
    void Clear();
	
	void AddElement( MusLayerElement *element, int idx = -1 );
    	
	int GetElementCount() const { return (int)m_children.size(); };
    
    int GetLayerNo() const;

    // functors
    /**
     * Copy the elements to a MusLayer passed in parameters. 
     * Also take into account a start and end uuid for the page (if any)
     */ 
    virtual bool CopyToLayer( wxArrayPtrVoid params );
    void CopyElements( wxArrayPtrVoid params ); // unused
    void GetMaxXY( wxArrayPtrVoid params ); // unused
    virtual bool Save( wxArrayPtrVoid params );
    void CheckAndResetSectionOrMeasure( wxArrayPtrVoid params ); // unused
    
	void CopyAttributes( MusLayer *layer ); // copy all attributes but none of the elements
    
	MusLayerElement *GetFirst( );
	MusLayerElement *GetLast( );
	MusLayerElement *GetNext( MusLayerElement *element );
	MusLayerElement *GetPrevious( MusLayerElement *element );
	MusLayerElement *GetAtPos( int x );
	MusLayerElement *Insert( MusLayerElement *element, int x ); // return a pointer on the inserted element
    
    void Insert( MusLayerElement *element, MusLayerElement *before );
    
	void Delete( MusLayerElement *element );
	/** 
     * Looks for the first MusLayerElement with an LayoutElement of type elementType.
     * Looks FORWARD of BACKWARD depending on the direction parameter.
     * Returns the retrieved element if *succ == true or the original element if not.
     */
    MusLayerElement *GetFirst( MusLayerElement *element, unsigned int direction, const std::type_info *elementType, bool *succ );
    /** Get the current clef for the test element */
	MusClef *GetClef ( MusLayerElement *test );
	void getOctDec (int ft, int _ot, int rupt, int *oct);
    /** Return the clef offset for the position x (retrieve the first clef before it) */
    int GetClefOffset( MusLayerElement *test  );
    
    
    /**
     * Basic method that remove intermediate clefs and custos.
     * Used for building collations (See CmpFile::Collate).
     */
    void RemoveClefAndCustos( );
    
    /**
     * Checks that the X position of the currentElement is not before the previous element or after the next one.
     */ 
    void CheckXPosition( MusLayerElement *currentElement );

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
    /** The logical layer */
    int m_logLayerNb;
    /** The logical staff - used to overwrite the parent staff */
    int m_logStaffNb;
	/** voix de la portee*/
	unsigned short voix;  
    
protected:
    // drawing variables
    //MusLayerElement *beamListPremier; // we need to replace this with a proper beam class that handles a list of notes/rests

private:
    
};


#endif
