/////////////////////////////////////////////////////////////////////////////
// Name:        iomlfclass.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __WG_IOMLFCLASS_H__
#define __WG_IOMLFCLASS_H__

#ifdef AX_WG

#ifdef __GNUG__
    #pragma interface "iomlfclass.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "iomlf.h"

// !!!! Changer aussi ces valeurs dans Decoder dans ClassModel.cpp !!!!
#define MLF_INTERVAL 0x00FF	// definit la hauteur maximal en valeur absolu d'une note
							// depend de l'encodage (par note, par 1/2 ton, 41, ...
#define MLF_INTERVAL_V1 0x0F00	// definit la hauteur maximal en valeur absolu d'une note
							// depend de l'encodage (par note, par 1/2 ton, 41, ...
//#define MLF_INTERVAL_V2 0xF000	// definit la hauteur maximal en valeur absolu d'une note
//							// depend de l'encodage (par note, par 1/2 ton, 41, ...
#define MLF_INTERVAL_SHIFT	8	// bit shift correspondant
//#define MLF_INTERVAL_SHIFT_V	12	// bit shift correspondant



// WDR: class declarations

//----------------------------------------------------------------------------
// MLFClassInterval
//----------------------------------------------------------------------------

class MLFClassInterval: public MLFSymbole
{
public:
    // constructors and destructors
    MLFClassInterval();
    virtual ~MLFClassInterval() {};
    
    // WDR: method declarations for MLFClassInterval
    virtual wxString GetLabel( );
    
protected:
    // WDR: member variable declarations for MLFClassInterval

private:
    // WDR: handler declarations for MLFClassInterval

	DECLARE_DYNAMIC_CLASS(MLFClassInterval)
};


//----------------------------------------------------------------------------
// MLFClassDuration
//----------------------------------------------------------------------------

class MLFClassDuration: public MLFSymbole
{
public:
    // constructors and destructors
    MLFClassDuration();
    virtual ~MLFClassDuration() {};
    
    // WDR: method declarations for MLFClassDuration
    virtual wxString GetLabel( );
    
protected:
    // WDR: member variable declarations for MLFClassDuration

private:
    // WDR: handler declarations for MLFClassDuration

	DECLARE_DYNAMIC_CLASS(MLFClassDuration)
};


//----------------------------------------------------------------------------
// MLFClassPitch
//----------------------------------------------------------------------------

class MLFClassPitch: public MLFSymbole
{
public:
    // constructors and destructors
    MLFClassPitch();
    virtual ~MLFClassPitch() {};
    
    // WDR: method declarations for MLFClassPitch
    virtual wxString GetLabel( );
    
protected:
    // WDR: member variable declarations for MLFClassPitch

private:
    // WDR: handler declarations for MLFClassPitch

	DECLARE_DYNAMIC_CLASS(MLFClassPitch)
};



//----------------------------------------------------------------------------
// MLFClassSymb
//----------------------------------------------------------------------------

class MLFClassSymb: public MLFSymbole
{
public:
    // constructors and destructors
    MLFClassSymb();
    virtual ~MLFClassSymb() {};
    
    // WDR: method declarations for MLFClassSymb
    virtual wxString GetLabel( );
    
protected:
    // WDR: member variable declarations for MLFClassSymb

private:
    // WDR: handler declarations for MLFClassSymb

	DECLARE_DYNAMIC_CLASS(MLFClassSymb)
};

#endif //AX_WG

#endif
