/////////////////////////////////////////////////////////////////////////////
// Name:        musscoredef.h
// Author:      Laurent Pugin
// Created:     2013/11/08
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SCOREDEF_H__
#define __MUS_SCOREDEF_H__

#include "musobject.h"

class MusClef;
class MusKeySig;
class MusMensur;
class MusStaffGrp;
class MusStaffDef;

//----------------------------------------------------------------------------
// MusScoreOrStaffDefAttrInterface
//----------------------------------------------------------------------------

/**
 * This class is an interface for MEI scoreDef or staffDef attributes clef, keysig and mensur.
 * For simplification, the attributes are stored as MusClef, MusKeySig and MusMensur.
 */
class MusScoreOrStaffDefAttrInterface
{
public:
    // constructors and destructors
    MusScoreOrStaffDefAttrInterface();
    virtual ~MusScoreOrStaffDefAttrInterface();
    
    /**
     * Replace the clef (if any) with the newClef (if any).
     */
    void ReplaceClef( MusClef *newClef );
    
    /**
     * Replace the keysig (if any) with the newKeysig (if any).
     */
    void ReplaceKeySig( MusKeySig *newKeySig );
    
    /**
     * Replace the mensur (if any) with the newMensur (if any).
     */
    void ReplaceMensur( MusMensur *newMensur );
    
public:
    /** The clef attribute */
    MusClef *m_clef;
    /** The key signature */
    MusKeySig *m_keySig;
    /** The mensure (time signature */
    MusMensur *m_mensur;
    
};



//----------------------------------------------------------------------------
// MusScoreDef
//----------------------------------------------------------------------------

/**
 * This class represents a MEI scoreDef.
 * It contains MusStaffGrp objects.
*/
class MusScoreDef: public MusObject, public MusScoreOrStaffDefAttrInterface
{
public:
    // constructors and destructors
    MusScoreDef();
    virtual ~MusScoreDef();
        
    virtual std::string MusClassName( ) { return "MusScoreDef"; };
    
	void AddStaffGrp( MusStaffGrp *staffGrp );
    
    /**
     * Replace the scoreDef with the content of the newScoreDef.
     */
    void Replace( MusScoreDef *newScoreDef );
    
    /**
     * Replace the corresponding staffDef with the content of the newStaffDef.
     * Looks for the staffDef with the same m_n (@n) and replace the attribute set.
     * Attribute set is provided by the MusScoreOrStaffDefAttrInterface.
     */
    void Replace( MusStaffDef *newStaffDef );
    
    /**
     * Get the staffDef with number n (NULL if not found).
     */
    MusStaffDef *GetStaffDef( int n );
    
private:
    
public:

private:
    
};


//----------------------------------------------------------------------------
// MusStaffGrp
//----------------------------------------------------------------------------

/**
 * This class represents a MEI staffGrp.
 * It contains MusStaffDef objects.
 */
class MusStaffGrp: public MusObject
{
public:
    // constructors and destructors
    MusStaffGrp();
    virtual ~MusStaffGrp();
    
    virtual std::string MusClassName( ) { return "MusStaffGrp"; };
	
	void AddStaffDef( MusStaffDef *staffDef );
    
    // functors
    virtual int Save( ArrayPtrVoid params );
    
private:
    
public:
    
private:
    
};


//----------------------------------------------------------------------------
// MusStaffDef
//----------------------------------------------------------------------------

/**
 * This class represents a MEI staffDef.
 */
class MusStaffDef: public MusObject, public MusScoreOrStaffDefAttrInterface
{
public:
    // constructors and destructors
    MusStaffDef();
    virtual ~MusStaffDef();
    
    virtual std::string MusClassName( ) { return "MusStaffDef"; };
    
    /**
     * @name Set and get  the staff number which is 1-based.
     * This can be different from the index position in the parent measure.
     */
    ///@{
    int GetStaffNo() const { return m_n; };
    void SetStaffNo( int n ) { m_n = n; };
    ///@}
    
    /**
     * We
     */
    void SetScoreDefPtr();

    // functors
    virtual int Save( ArrayPtrVoid params );
    
    /**
     * Find the MusStaffDef with number m_n value.
     * param 0: the n we are looking for.
     * param 1: the pointer to pointer to the MusStaffDef retrieved (if found).
     */
    virtual int FindStaffDefByNumber( ArrayPtrVoid params );
    
    /**
     * Replace all the staffDefs in a scoreDef.
     * Calls MusScoreDef::Replace.
     * param 0: a pointer to the scoreDef we are going to replace the staffDefs
     */
    virtual int ReplaceStaffDefsInScoreDef( ArrayPtrVoid params );
    
private:
    
public:
    
private:
    /** The staff number */
    int m_n;
    
    /**
     * The scoreDef parent.
     * We cache this for efficiency.
     */
    MusScoreDef *m_parentScoreDef;
    
};

#endif
