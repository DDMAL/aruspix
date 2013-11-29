/////////////////////////////////////////////////////////////////////////////
// Name:        musscoredef.h
// Author:      Laurent Pugin
// Created:     2013/11/08
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SCOREDEF_H__
#define __MUS_SCOREDEF_H__

#include "musclef.h"
#include "muskeysig.h"
#include "musmensur.h"
#include "musobject.h"

class MusStaffGrp;
class MusStaffDef;

//----------------------------------------------------------------------------
// MusScoreDef
//----------------------------------------------------------------------------

/**
 * This class represents a MEI scoreDef.
 * It contains MusStaffGrp objects.
*/
class MusScoreDef: public MusObject
{
public:
    // constructors and destructors
    MusScoreDef();
    virtual ~MusScoreDef();
        
    virtual std::string MusClassName( ) { return "MusScoreDef"; };	    
    
	void AddStaffGrp( MusStaffGrp *staffGrp );
    
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
class MusStaffDef: public MusObject
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

    // functors
    virtual int Save( ArrayPtrVoid params );
    
    /**
     * Find the MusStaffDef with number m_n value.
     * param 0: the n we are looking for.
     * param 1: the pointer to pointer to the MusStaffDef retrieved (if found).
     */
    virtual int FindStaffDefByNumber( ArrayPtrVoid params );
    
private:
    
public:
    
private:
    /** The staff number */
    int m_n;
    /** The clef attribute */
    MusClef m_clef;
    /** The key signature */
    MusKeySig m_keySig;
    /** The mensure (time signature */
    MusMensur m_mensur;
    
};

#endif
