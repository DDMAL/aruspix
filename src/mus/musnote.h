/////////////////////////////////////////////////////////////////////////////
// Name:        musnote.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_NOTE_H__
#define __MUS_NOTE_H__

#ifdef __GNUG__
    #pragma interface "musnote.cpp"    
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/dynarray.h"

#include "muselement.h"

class MusStaff;
class MusSymbol;

WX_DECLARE_OBJARRAY( MusSymbol, ArrayOfMusSymbols);


//----------------------------------------------------------------------------
// MusNote
//----------------------------------------------------------------------------

class MusNote: public MusElement
{
public:
    // constructors and destructors
    MusNote();
	MusNote( char _sil, unsigned char _val, unsigned char _code );
	MusNote( const MusNote& note ); // copy contructor
	MusNote& operator=( const MusNote& note ); // copy assignement;
    virtual ~MusNote();
    
	virtual void Draw( wxDC *dc, MusStaff *staff);
	void note ( wxDC *dc, MusStaff *staff);
	void silence ( wxDC *dc, MusStaff *staff);
	void leg_line( wxDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int pTaille);

	void s_special ( wxDC *dc, int a, MusStaff *staff);
	void s_lg ( wxDC *dc, int a, int b, MusStaff *staff);
	void s_br ( wxDC *dc, int a, int b, MusStaff *staff);
	void s_rd ( wxDC *dc, int a, int b, int valeur, MusStaff *staff);
	void s_nr ( wxDC *dc, int a, int b, int valeur, MusStaff *staff);
	void pointage ( wxDC *dc, int x1, int y1, int offy, unsigned int d_p, MusStaff *staff );
	void ligature ( wxDC *dc, int y, MusStaff *staff );
    //	virtual void SetPitch( int code, int oct, MusStaff *staff  );
	virtual void SetPitch( int code, int oct );
	virtual void SetValue( int value, MusStaff *staff = NULL, int vflag = 0 );
	virtual void ChangeColoration( MusStaff *staff = NULL );
	virtual void ChangeStem( MusStaff *staff = NULL );
	virtual void SetLigature( MusStaff *staff = NULL );
	
	// lyrics
	void DeleteLyricFromNote( MusSymbol *lyric );
	bool BelongsToTheNote( MusElement *element ); 
	void CheckLyricIntegrity( );
	MusSymbol *GetFirstLyric( );
	MusSymbol *GetLastLyric( );
    MusSymbol *GetLyricNo( int no );
	
public:
	    /** silence (true) ou note (false) */
    char sil;
    /** valeur de la note
     @see JwgDef#LG JwgDef, valeurs des notes */
    unsigned char val;
    /** couleur inverse */
    char inv_val;
    /** point. 0 = aucun1 = point2 = double point
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
	
	/** Associated lyric */
	ArrayOfMusSymbols m_lyrics;
    
    // static for ligatures
    //static int ligat_x[2], ligat_y[2];	// pour garder coord. des ligatures
    static unsigned int marq_obl;	// marque le 1e passage pour une oblique
	
private:
    };


#endif
