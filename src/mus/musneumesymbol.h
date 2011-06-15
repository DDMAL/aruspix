/*
 *  musneumesymbol.h
 *  aruspix
 *
 *  Created by Alastair Porter on 11-03-29.
 *  Copyright 2011 com.aruspix.www. All rights reserved.
 *
 */

#ifndef __MUS_NEUME_SYMBOL_H__
#define __MUS_NEUME_SYMBOL_H__


#include "wx/wx.h"

#include "muselement.h"

#include <mei/mei.h>

enum MusNeumeSymbolType {
    NEUME_SYMB_COMMA,
    NEUME_SYMB_FLAT,
    NEUME_SYMB_NATURAL,
    NEUME_SYMB_CLEF_C,
    NEUME_SYMB_CLEF_F,
    NEUME_SYMB_DIVISION_FINAL,
    NEUME_SYMB_DIVISION_MAJOR,
    NEUME_SYMB_DIVISION_MINOR,
    NEUME_SYMB_DIVISION_SMALL
};

class MusNeumeSymbol : public MusElement {
public:
	MusNeumeSymbol();
	MusNeumeSymbol(MeiElement &meielement);
	MusNeumeSymbol(const MusNeumeSymbol &symbol);
	virtual ~MusNeumeSymbol() {}
	
	void calcoffs (int *offst, int value);
    MusNeumeSymbolType GetSymbolType();
	void SetValue(int value, MusStaff *staff, int flag);
	int getValue();
	void SetPitch(int pitch, int oct);
	
	void ResetToNeumeSymbol();
	void ResetToClef();
	MusNeumeSymbolType getType();
	
	//drawing code
	virtual void Draw(AxDC *dc, MusStaff *staff);
	void DrawClef(AxDC *dc, int i, MusStaff *staff);
	void DrawComma(AxDC *dc, MusStaff *staff);
	void DrawFlat(AxDC *dc, MusStaff *staff);
	void DrawNatural(AxDC *dc, MusStaff *staff);
	void DrawDivFinal(AxDC *dc, MusStaff *staff);
	void DrawDivMajor(AxDC *dc, MusStaff *staff);
	void DrawDivMinor(AxDC *dc, MusStaff *staff);
	void DrawDivSmall(AxDC *dc, MusStaff *staff);
    
	//MEI stuff
	MeiElement *getMeiRef();
	void setMeiRef(MeiElement *element);
	void setMeiStaffZone(MeiElement *element);
	void updateMeiRef();
	void updateMeiZone();
	void deleteMeiRef();

private:
    MusNeumeSymbolType symbolType;
	MeiElement *m_meiref;
	MeiElement *m_meistaffzone;
    void updateMeiRefClef(string shape, string line);
    void updateMeiRefDiv(string form);
    void updateMeiRefAccid(string accid, int pitch, int octave);
	int value; //this is to denote the line a clef lies on
};

#endif // __MUS_NEUME_SYMBOL_H__
