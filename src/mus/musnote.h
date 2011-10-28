/////////////////////////////////////////////////////////////////////////////
// Name:        musnote.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_NOTE_H__
#define __MUS_NOTE_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "muslayer.h"
#include "musdurationinterface.h"
#include "muspitchinterface.h"

#define SLUR_INITIAL  (1<<1)
#define SLUR_MEDIAL  (1<<2) 
#define SLUR_TERMINAL  (1<<3) 

#define LIG_INITIAL  (1<<1)
#define LIG_MEDIAL  (1<<2) 
#define LIG_TERMINAL  (1<<3) 

#define CHORD_INITIAL  (1<<1)
#define CHORD_MEDIAL  (1<<2) 
#define CHORD_TERMINAL  (1<<3) 


//----------------------------------------------------------------------------
// MusNote
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <note> element. 
 */
class MusNote: public MusLayerElement, public MusDurationInterface, public MusPitchInterface
{
public:
    // constructors and destructors
    MusNote();
    virtual ~MusNote();
    
private:
    
public:
    /** Indicates the articulation (0 == none) */
    int m_artic;
    /** Indicates if initial, medial or terminal part of a chord */
    unsigned char m_chord;
    /** Indicates if colored (inversed) */
    bool m_colored;
    /** Indicates if intial, medial or terminal part of a ligature */
    unsigned char m_lig;
    /** Indicates the headshape of the note */
    unsigned char m_headshape;
    /** Indicates if the ligature is obliqua (recta otherwise) */
    bool m_ligObliqua;
    /** Indicates if intial, medial or terminal part of a slur for up to 6 nesting/overlaping slur */
    unsigned char m_slur[6];
    /** Indicates the stem direction (0 == auto, -1 down, 1 up) */
    char m_stemDir;
    /** Indicates the stem length (0 == auto) */
    unsigned char m_stemLen;

private:
    
};


#endif
