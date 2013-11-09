/////////////////////////////////////////////////////////////////////////////
// Name:        musstaff.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_STAFF_H__
#define __MUS_STAFF_H__

#include "musobject.h"

#define STAFF_OFFSET 190
#define MAXCLE 100	// maximum number of clef on one staff

class MusClef;
class MusDC;
class MusLayer;
class MusStaffAlignment;

//----------------------------------------------------------------------------
// MusStaff
//----------------------------------------------------------------------------

/**
 * This class represents a staff in a laid-out score (MusDoc).
 * A MusStaff is contained in a MusSystem.
 * It contains MusMeasure objects.
 * For unmeasured music, on single MusMeasure is added for simplifying internal processing
*/
class MusStaff: public MusDocObject
{
    
public:
    // constructors and destructors
    MusStaff( int logStaffNb = -1 );
	MusStaff( const MusStaff& staff ); // copy contructor
    virtual ~MusStaff();

    virtual std::string MusClassName( ) { return "MusStaff"; };	    
    
    void Clear();
    
    void AddLayer( MusLayer *layer );
	
	int GetLayerCount() const { return (int)m_children.size(); };
    
    int GetStaffNo() const;
    
    /**
     * Return the default horizontal spacing of staves.
     */
    int GetVerticalSpacing();

    // functors
    virtual int Save( ArrayPtrVoid params );
    virtual bool GetPosOnPage( ArrayPtrVoid params );
    
	void CopyAttributes( MusStaff *staff ); // copy all attributes but none of the elements
	//void ClearElements( MusDC *dc , MusElement *start = NULL );

	MusLayer *GetFirst( );
	MusLayer *GetLast( );
	MusLayer *GetNext( MusLayer *layer );
	MusLayer *GetPrevious( MusLayer *layer );
    MusLayer *GetLayer( int LayerNo );
    
    
    /**
     * Lay out the system and staff Y positions looking that the bounding boxes of each staff.
     * The m_y_rel of systems and staves is updated appropriately.
     */
    virtual int LayOutSystemAndStaffYPos( ArrayPtrVoid params );
    
    /**
     * Align the content of a system.
     */
    virtual int Align( ArrayPtrVoid params );
    
public:
    /** The logical staff */
    int m_logStaffNb;
    
    
	/** numero dans le groupe auquel appartient la portee */
	unsigned short noGrp;
	/** nombre total de portee dans le groupe */
	unsigned short totGrp;
	/** numero de systeme */
	//unsigned short noLigne; // ax2
	/** type d'armure. 0 = aucune1 = dieses2 = bemols */
	unsigned char armTyp;
	/** nombre d'alterations a l'armures */
	unsigned char armNbr;
	/** portee en notation ancienne */
	char notAnc;
	/** portee grise */
	char grise;
	/** portee invisible */
	char invisible;
	/** barre verticale. 0 = aucun1 = debut2 = fin */
	unsigned char vertBarre;
	/** accolade ou bloc. 0 = aucun1 = debut2 = fin */
	unsigned char brace;
	/** taille. 0 = normale1 = petite  */
	unsigned char staffSize;
	
    /** portee indentee
		char dans Wolfgang - ici indentation par portee
	*/
	//int indent; // ax2 
	/** portee indentee a droite */
	//char indentDroite; // ax2
	/** type de portee (nombre de lignes) */
	unsigned char portNbLine;
	/** type d'accolade. 0 = bloc1 = accolade ronde */
	unsigned char accol;
	/** ???? */
	unsigned char accessoire;
	/** 
     * The Y absolute position of the staff for facsimile (transcription) encodings.
     * This is the top left corner of the staff (the X position is the position of the system).
     */
    int m_y_abs;
    /**
     * The Y relative position of the staff.
     * It is used internally when calculating the layout andd it is not stored in the file.
     */
    int m_y_rel;
	/** 
     * The Y drawing position of the staff.
     * It is re-computed everytime the staff is drawn and it is not stored in the file.
     */
    int m_y_drawing;
    
    /** indicates this staff is in anchent notation
     it has to be carried on to the LayedOutStaf */
    bool m_mensuralNotation;

private:
    
    /**
     * A pointer to a MusStaffAlignment for aligning the staves
     */
    MusStaffAlignment *m_staffAlignment;

    
};


#endif
