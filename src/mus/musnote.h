/////////////////////////////////////////////////////////////////////////////
// Name:        wgnote.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __WG_NOTE_H__
#define __WG_NOTE_H__

#ifdef AX_WG

#ifdef __GNUG__
    #pragma interface "wgnote.cpp"    
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wgelement.h"

class WgStaff;

// WDR: class declarations

//----------------------------------------------------------------------------
// WgNote
//----------------------------------------------------------------------------

class WgNote: public WgElement
{
public:
    // constructors and destructors
    WgNote();
	WgNote( char _sil, unsigned char _val, unsigned char _code );
    virtual ~WgNote();
    
	virtual void WgNote::Draw( wxDC *dc, WgStaff *staff);
	void WgNote::note ( wxDC *dc, WgStaff *staff);
	void WgNote::silence ( wxDC *dc, WgStaff *staff);
	void WgNote::leg_line( wxDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int pTaille);

	void WgNote::s_special ( wxDC *dc, int a, WgStaff *staff);
	void WgNote::s_lg ( wxDC *dc, int a, int b, WgStaff *staff);
	void WgNote::s_br ( wxDC *dc, int a, int b, WgStaff *staff);
	void WgNote::s_rd ( wxDC *dc, int a, int b, int valeur, WgStaff *staff);
	void WgNote::s_nr ( wxDC *dc, int a, int b, int valeur, WgStaff *staff);
	void WgNote::pointage ( wxDC *dc, int x1, int y1, int offy, unsigned int d_p, WgStaff *staff );
	void WgNote::ligature ( wxDC *dc, int y, WgStaff *staff );
    // WDR: method declarations for WgNote
	virtual void WgNote::SetPitch( int code, int oct, WgStaff *staff = NULL );
	virtual void WgNote::SetValue( int value, WgStaff *staff = NULL, int vflag = 0 );
	virtual void WgNote::ChangeColoration(   WgStaff *staff = NULL );
	virtual void WgNote::ChangeStem( WgStaff *staff = NULL );
	virtual void WgNote::SetLigature( WgStaff *staff = NULL );
	
public:
	// WDR: member variable declarations for WgNote
    /** silence (true) ou note (false) */
    char sil;
    /** valeur de la note
     @see JwgDef#LG JwgDef, valeurs des notes */
    unsigned char val;
    /** couleur inverse */
    char inv_val;
    /** point. <pre>0 = aucun<br>1 = point<br>2 = double point</pre>
     @see JwgDef#POINTAGE JwgDef, valeurs de pointages de notes */
    unsigned char point;
    /** interruption de beaming */
    char stop_rel;
    /** alteration
     @see JwgDef#DIESE JwgDef, types d'alterations */
    unsigned char acc;
    /** accident invisible */
    char accInvis;
    /** position des hampes automatique */
    char q_auto;
    /** position de la hampe */
    char queue;
    /** staccato */
    char stacc;
    /** ???? */
    char oblique;
    /** ???? */
    char queue_lig;
    /** note faisant partie d'un accord */
    char chord;
    /** derniere note d'un accord */
    char fchord;
    /** note exentrique */
    char lat;
    /** ???? */
    char haste;
    /** code de la note (F2 a F8)
     @see JwgDef#F2 JwgDef, valeurs des codes de note */
    //unsigned char code; // moved in element
    /** type de tete de note (0 a 6)
     @see JwgDef#LOSANGEVIDE JwgDef, types de tetes de notes */
    unsigned char tetenot;
    /** type de staccato (0 a 6) */
    unsigned char typStac;

private:
    // WDR: handler declarations for WgNote
};

#endif //AX_WG

#endif
