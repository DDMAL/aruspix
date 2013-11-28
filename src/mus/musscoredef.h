/////////////////////////////////////////////////////////////////////////////
// Name:        musscoredef.h
// Author:      Laurent Pugin
// Created:     2013/11/08
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SCOREDEF_H__
#define __MUS_SCOREDEF_H__

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
class MusScoreDef: public MusObject, public MusObjectListInterface
{
public:
    // constructors and destructors
    MusScoreDef();
    virtual ~MusScoreDef();
        
    virtual std::string MusClassName( ) { return "MusScoreDef"; };	    
    
	void AddStaffGrp( MusStaffGrp *staffGrp );
    
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

    // functors
    virtual int Save( ArrayPtrVoid params );
    
private:
    
public:
    
private:
    
};

#endif
