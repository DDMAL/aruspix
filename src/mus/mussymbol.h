/////////////////////////////////////////////////////////////////////////////
// Name:        mussymbol.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SYMBOL_H__
#define __MUS_SYMBOL_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "muslayer.h"
#include "muspositioninterface.h"
#include "musdef.h"

//----------------------------------------------------------------------------
// MusSymbol
//----------------------------------------------------------------------------

/** 
 * This class models various MEI that do not have their own class yet.
 * The element name is given by the m_type member.
 * The DOT type models the MEI <dot> element.
 * The ACCID type models the MEI <accid> element.
 */
class MusSymbol: public MusLayerElement, public MusPositionInterface
{
public:
    // constructors and destructors
    MusSymbol();
    MusSymbol( SymbolType type );
    virtual ~MusSymbol();
    
    virtual wxString MusClassName( ) { return "MusSymbol"; };
    
    void Init();
    
private:
    
public:
    /** Indicates the type of the symbole */
    SymbolType m_type;
    
    /** For DOT, indicates the number of dots */
    unsigned char m_dot;
    
    /** For ACCID, indicates the accidental */
    unsigned char m_accid;

private:
    
};


#endif
