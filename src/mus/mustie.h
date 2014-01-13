/////////////////////////////////////////////////////////////////////////////
// Name:        mustie.h
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_TIE_H__
#define __MUS_TIE_H__

#include "muslayer.h"
#include "musnote.h"
#include "musobject.h"

//----------------------------------------------------------------------------
// Tie
// A TIE class is a simple slur - it takes in account only two notes, the first and second:
// NOTE tie NOTE tie etc
//----------------------------------------------------------------------------

class Tie: public MusLayerElement
{
public:
    // constructors and destructors
    Tie();
    virtual ~Tie();
    
    virtual std::string MusClassName( ) { return "Tie"; };
    
    /**
     * @name Set and get the first and second note of the tie
     * The setter asserts that no note was previously set.
     */
    ///@{
    void SetFirstNote( MusNote *note );
    void SetSecondNote( MusNote *note );
    MusNote *GetFirstNote() { return m_first; };
    MusNote *GetSecondNote() { return m_second; };
    ///@}
    
private:
    
public:
    
private:
    MusNote *m_first;
    MusNote *m_second;
    
};

#endif
