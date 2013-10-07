/////////////////////////////////////////////////////////////////////////////
// Name:        muspositioninterface.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_POSITION_INTERFACE_H__
#define __MUS_POSITION_INTERFACE_H__

//----------------------------------------------------------------------------
// MusPositionInterface
//----------------------------------------------------------------------------

/** 
 * This class is an interface for elements with a position on the staff, such as rests.
 * It is not an abstract class but should not be instanciate directly.
 * For now, the position is handled in a similar way that for MusPitchInterface, that is with a pitch and octave. 
 */
class MusPositionInterface
{
public:
    // constructors and destructors
    MusPositionInterface();
    virtual ~MusPositionInterface();
    
    /** Set the position */
    virtual void SetPosition( int pname, int oct );
    
    /** Get the position */
    virtual bool GetPosition( int *pname, int *oct );
    
    /**
     * Inteface comparison operator. 
     * Check if the MusLayerElement has a MusPositionInterface and compare attributes
     */
    bool HasIdenticalPositionInterface(MusPositionInterface  *otherPitchInterface);

    
private:
    
public:
    /** Indicates the octave information */
    char m_oct;
    /** Indicates the pitch name */
    unsigned char m_pname;

private:
    
};


#endif
