/////////////////////////////////////////////////////////////////////////////
// Name:        muslaidoutstaff.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_LAID_OUT_STAFF_H__
#define __MUS_LAID_OUT_STAFF_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class MusDC;

#include "mussystem.h"

#define STAFF_OFFSET 190
#define MAXCLE 100	// maximum number of clef on one staff

class MusLaidOutLayer;
class MusClef;



//----------------------------------------------------------------------------
// MusLaidOutStaff
//----------------------------------------------------------------------------

/**
 * This class represents a staff in a laid-out score (MusLayout).
 * A MusLaidOutStaff is contained in a MusLaidOutSystem.
 * It contains MusLaidOutLayer objects.
*/
class MusLaidOutStaff: public MusLayoutObject
{
    
public:
    // constructors and destructors
    MusLaidOutStaff( int logStaffNb = -1 );
	MusLaidOutStaff( const MusLaidOutStaff& staff ); // copy contructor
    virtual ~MusLaidOutStaff();
    
    virtual bool Check();
    
    virtual wxString MusClassName( ) { return "MusLaidOutStaff"; };	    
    
    void Clear();
    
    /** The parent MusSystem setter */
    void SetSystem( MusSystem *system ) { m_system = system; }; 
	
	void AddLayer( MusLaidOutLayer *layer );
	
	int GetLayerCount() const { return (int)m_layers.GetCount(); };
    
    int GetStaffNo() const;

    
    // moulinette
    virtual void Process(MusFunctor *functor, wxArrayPtrVoid params );
    // functors
    void Save( wxArrayPtrVoid params );
    void Load( wxArrayPtrVoid params );
    
	void CopyAttributes( MusLaidOutStaff *staff ); // copy all attributes but none of the elements
	//void ClearElements( MusDC *dc , MusElement *start = NULL );

	MusLaidOutLayer *GetFirst( );
	MusLaidOutLayer *GetLast( );
	MusLaidOutLayer *GetNext( MusLaidOutLayer *staff );
	MusLaidOutLayer *GetPrevious( MusLaidOutLayer *staff );
    MusLaidOutLayer *GetLayer( int LayerNo );
    
public:
    /** The MusLaidOutLayer objects of the staff */
    ArrayOfMusLaidOutLayers m_layers;
    /** The MusSystem parent */
    MusSystem *m_system;
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
	/** ecart jusqu'a la portee, en interlignes */
	unsigned short ecart;
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
	/** position y relative de la portee (non-enregistre dans les fichiers) */
    int m_y_abs;
	/** postion x relative de la portee (non-enregistre dans les fichiers) */
    int m_x_abs;

private:
};


//----------------------------------------------------------------------------
// MusLaidOutStaffFunctor
//----------------------------------------------------------------------------

/**
 * This class is a Functor that processes MusLaidOutStaff objects.
 * Needs testing.
*/
class MusLaidOutStaffFunctor: public MusFunctor
{
private:
    void (MusLaidOutStaff::*fpt)( wxArrayPtrVoid params );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusLaidOutStaffFunctor( void(MusLaidOutStaff::*_fpt)( wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ~MusLaidOutStaffFunctor() {};

    // override function "Call"
    virtual void Call( MusLaidOutStaff *ptr, wxArrayPtrVoid params )
        { (*ptr.*fpt)( params);};          // execute member function
};


#endif
