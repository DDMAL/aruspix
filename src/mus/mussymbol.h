/////////////////////////////////////////////////////////////////////////////
// Name:        mussymbol.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SYMBOL_H__
#define __MUS_SYMBOL_H__

#include "musdef.h"
#include "muslayerelement.h"
#include "muspositioninterface.h"

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
    
    /**
     * Comparison operator. 
     * Check if the MusLayerElement if a Symbol and compare attributes
     */
    virtual bool operator==(MusObject& other);
    
    virtual std::string MusClassName( ) { return "MusSymbol"; };
    
    /**
     * Set the value for the symbol.
     */
	virtual void SetValue( int value, int flag = 0 );
    
    /** Set the position */
    //virtual void SetPitchOrPosition( int pname, int oct ) { MusPositionInterface::SetPosition( pname, oct ); };
    
    /** Get the position */
    //virtual bool GetPitchOrPosition( int *pname, int *oct ) { return MusPositionInterface::GetPosition( pname, oct ); };

    
    void Init( SymbolType type );
    
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
