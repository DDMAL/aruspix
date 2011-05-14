/*
 *  musneumesymbol_test.cpp
 *  aruspix
 *
 *  Created by Alastair Porter on 11-05-14.
 *  Copyright 2011 com.aruspix.www. All rights reserved.
 *
 */

#include <gtest/gtest.h>
#include <mei/mei.h>

#include "musneumesymbol.h"

TEST(NeumeSymbolTest, TestCClef) {
	MeiElement c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "1"));
    
    MusNeumeSymbol sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_C); 
    ASSERT_EQ(sym.code, nC1);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "2"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_C); 
    ASSERT_EQ(sym.code, nC2);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "3"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_C); 
    ASSERT_EQ(sym.code, nC3);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "4"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_C); 
    ASSERT_EQ(sym.code, nC4);
}

TEST(NeumeSymbolTest, TestFClef) {
	MeiElement c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "F"));
    c.addAttribute(MeiAttribute("line", "1"));
    
    MusNeumeSymbol sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_F); 
    ASSERT_EQ(sym.code, nF1);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "F"));
    c.addAttribute(MeiAttribute("line", "2"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_F); 
    ASSERT_EQ(sym.code, nF2);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "F"));
    c.addAttribute(MeiAttribute("line", "3"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_F); 
    ASSERT_EQ(sym.code, nF3);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "F"));
    c.addAttribute(MeiAttribute("line", "4"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_F); 
    ASSERT_EQ(sym.code, nF4);
}

TEST(NeumeSymbolTest, TestClefBadShapeLine) {
    // unknown shape
    MeiElement c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "x"));
    c.addAttribute(MeiAttribute("line", "1"));
    
    try {
        MusNeumeSymbol sym = MusNeumeSymbol(c);
        ASSERT_TRUE(false); // Shouldn't get here
    } catch (...) {
        ASSERT_TRUE(true);
    }
    
    // unknown line, C
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "m"));
    
    try {
        MusNeumeSymbol sym = MusNeumeSymbol(c);
        ASSERT_TRUE(false); // Shouldn't get here
    } catch (...) {
        ASSERT_TRUE(true);
    }
    
    // unknown line, F
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "F"));
    c.addAttribute(MeiAttribute("line", "m"));
    
    try {
        MusNeumeSymbol sym = MusNeumeSymbol(c);
        ASSERT_TRUE(false); // Shouldn't get here
    } catch (...) {
        ASSERT_TRUE(true);
    }
}

TEST(NeumeSymbolTest, TestClefNoShapeLine) {
    // shape but no line
    MeiElement c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    
    try {
        MusNeumeSymbol sym = MusNeumeSymbol(c);
        ASSERT_TRUE(false); // Shouldn't get here
    } catch (...) {
        ASSERT_TRUE(true);
    }
    
    // line but no shape
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("line", "1"));
    
    try {
        MusNeumeSymbol sym = MusNeumeSymbol(c);
        ASSERT_TRUE(false); // Shouldn't get here
    } catch (...) {
        ASSERT_TRUE(true);
    }
}

TEST(NeumeSymbolTest, TestDivision) {
    MeiElement d = MeiElement("division");
    d.addAttribute(MeiAttribute("form", "major"));
    MusNeumeSymbol sym = MusNeumeSymbol(d);
    ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_DIVISION_MAJOR);

    d = MeiElement("division");
    d.addAttribute(MeiAttribute("form", "final"));
    sym = MusNeumeSymbol(d);
    ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_DIVISION_FINAL);
    
    d = MeiElement("division");
    d.addAttribute(MeiAttribute("form", "minor"));
    sym = MusNeumeSymbol(d);
    ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_DIVISION_MINOR);
    
    d = MeiElement("division");
    d.addAttribute(MeiAttribute("form", "small"));
    sym = MusNeumeSymbol(d);
    ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_DIVISION_SMALL);
    
    d = MeiElement("division");
    d.addAttribute(MeiAttribute("form", "comma"));
    sym = MusNeumeSymbol(d);
    ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_COMMA);
}

TEST(NeumeSymbolTest, TestCalcOffs) {
    MeiElement c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "2"));
    MusNeumeSymbol sym = MusNeumeSymbol(c);
    
    int off;
    sym.calcoffs(&off, sym.code);
    ASSERT_EQ(2, off);
    
    // XXX: More to do here
}

TEST(NeumeSymbolTest, TestSetValueClef) {
    MeiElement c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "1"));
    MusNeumeSymbol sym = MusNeumeSymbol(c);
    
    // Set to C, line 2
    sym.SetValue('1', NULL, 0);
    ASSERT_EQ(sym.code, nC2);
    ASSERT_EQ(sym.getMeiRef()->getAttribute("shape")->getValue(), "C");
    ASSERT_EQ(sym.getMeiRef()->getAttribute("line")->getValue(), "2");
    
    // XXX: This currently fails because of SetValue code/value mixups
}

TEST(NeumeSymbolTest, TestSetValueDivision) {
    
}