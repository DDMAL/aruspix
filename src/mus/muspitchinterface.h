/////////////////////////////////////////////////////////////////////////////
// Name:        muspitchinterface.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_PITCH_INTERFACE_H__
#define __MUS_PITCH_INTERFACE_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


//----------------------------------------------------------------------------
// MusPitchInterface
//----------------------------------------------------------------------------

/** 
 * This class is an interface for elements with pitch, such as notes and neumes.
 * It is not an abstract class but should not be instanciate directly.
 */
class MusPitchInterface
{
public:
    // constructors and destructors
    MusPitchInterface();
    virtual ~MusPitchInterface();
 
    /** Set the pitch */
    virtual void SetPitch( int pname, int oct );
    
    /** Get the pitch */
    virtual bool GetPitch( int *pname, int *oct );
    
    /**
     * Interface comparison operator. 
     * Check if the MusLayerElement has a MusPitchInterface and compare attributes
     */
    bool HasIdenticalPitchInterface(MusPitchInterface  *otherPitchInterface);

    
private:
    
public:
    /** Indicates the accidental */
    unsigned char m_accid;
    /** Provides the octave information */
    char m_oct;
    /** Indicates the pitch name */
    unsigned char m_pname;

private:
    
};


#endif
