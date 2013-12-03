/////////////////////////////////////////////////////////////////////////////
// Name:        muslayer.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_LAYER_H__
#define __MUS_LAYER_H__

#include <typeinfo>

#include "musobject.h"

class MusClef;
class MusDC;
class MusLayerElement;
class MusNote;

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
    MusLayer( int n );
    virtual ~MusLayer();
    
    virtual std::string MusClassName( ) { return "MusLayer"; };
    
    void Clear();
	
	void AddElement( MusLayerElement *element, int idx = -1 );
    	
	int GetElementCount() const { return (int)m_children.size(); };
    
    /**
     * Return the index position of the layer in its staff parent.
     * The index position is 0-based.
     */
    int GetLayerIdx() const { return MusObject::GetIdx(); };
    
    
    /**
     * @name Set and get the layer number which is 1-based.
     * This can be different from the index position in the parent staff.
     */
    ///@{
    int GetLayerNo() const { return m_n; };
    void SetLayerNo( int n ) { m_n = n; };
    ///@}
    

    // functors
    /**
     * Copy the elements to a MusLayer passed in parameters. 
     * Also take into account a start and end uuid for the page (if any)
     */ 
    virtual int CopyToLayer( ArrayPtrVoid params );
    void CopyElements( ArrayPtrVoid params ); // unused
    void GetMaxXY( ArrayPtrVoid params ); // unused
    virtual int Save( ArrayPtrVoid params );
    void CheckAndResetSectionOrMeasure( ArrayPtrVoid params ); // unused
    
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
     * Looks for the first MusLayerElement of type elementType.
     * Looks FORWARD of BACKWARD depending on the direction parameter.
     * Returns the retrieved element if *succ == true or the original element if not.
     */
    MusLayerElement *GetFirst( MusLayerElement *element, unsigned int direction, const std::type_info *elementType, bool *succ );
    
    /** 
     * Get the current clef for the test element.
     * Goes back on the layer until a clef is found.
     * This is used when inserting a note by passing a y position because we need
     * to know the clef in order to get the pitch.
     */
	MusClef *GetClef ( MusLayerElement *test );
    
    /** 
     * Return the clef offset for the position x.
     * The method uses MusLayer::GetClef first to find the clef before test.
     */
    int GetClefOffset( MusLayerElement *test  );
    
    /**
     * Add an element to the drawing list.
     * The drawing list is used to postponed the drawing of elements
     * that need to be drawn in a particular order.
     * For example, we need to draw beams before tuplets
     */
    void AddToDrawingList( MusLayerElement *element );

    /**
     * Return the darwing list.
     * This is used when actually darwing the list (see MusRC::DrawLayerList)
     */
    ListOfMusObjects *GetDrawingList( );

    /**
     * Reset the darwing list.
     * Clears the list - called when the layer starts to be drawn
     */
    void ResetDrawingList( );
    
    /**
     * Basic method that remove intermediate clefs and custos.
     * Used for building collations (See CmpFile::Collate).
     */
    void RemoveClefAndCustos( );
    
    /**
     * Checks that the X position of the currentElement is not before the previous element or after the next one.
     * This works for facsimile (transcription) encodings only because it looks at the m_x_abs position
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
    
    /**
     * Align the content of a system.
     */
    virtual int Align( ArrayPtrVoid params );
    
private:
    
public:
	/** voix de la portee*/
	unsigned short voix;  
    
protected:
    // drawing variables
    //MusLayerElement *beamListPremier; // we need to replace this with a proper beam class that handles a list of notes/rests
    ListOfMusObjects m_drawingList;

private:
    /** The layer number */
    int m_n;
    
};


#endif
