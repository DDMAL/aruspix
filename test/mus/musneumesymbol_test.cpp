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
    ASSERT_EQ(sym.getValue(), nC1);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "2"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_C); 
    ASSERT_EQ(sym.getValue(), nC2);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "3"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_C); 
    ASSERT_EQ(sym.getValue(), nC3);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "4"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_C); 
    ASSERT_EQ(sym.getValue(), nC4);
    
    // We should also handle lower-case shapes
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "c"));
    c.addAttribute(MeiAttribute("line", "4"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_C); 
    ASSERT_EQ(sym.getValue(), nC4);
}

TEST(NeumeSymbolTest, TestFClef) {
	MeiElement c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "F"));
    c.addAttribute(MeiAttribute("line", "1"));
    
    MusNeumeSymbol sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_F); 
    ASSERT_EQ(sym.getValue(), nF1);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "F"));
    c.addAttribute(MeiAttribute("line", "2"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_F); 
    ASSERT_EQ(sym.getValue(), nF2);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "F"));
    c.addAttribute(MeiAttribute("line", "3"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_F); 
    ASSERT_EQ(sym.getValue(), nF3);
    
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "F"));
    c.addAttribute(MeiAttribute("line", "4"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_F); 
    ASSERT_EQ(sym.getValue(), nF4);
    
    // Lower case shapes
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "f"));
    c.addAttribute(MeiAttribute("line", "4"));
    
    sym = MusNeumeSymbol(c);
	ASSERT_EQ(sym.GetSymbolType(), NEUME_SYMB_CLEF_F); 
    ASSERT_EQ(sym.getValue(), nF4);
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

TEST(NeumeSymbolTest, TestFlatNatural) {
    MeiElement a = MeiElement("accid");
    a.addAttribute(MeiAttribute("accid", "f"));
    a.addAttribute(MeiAttribute("pname", "c"));
    a.addAttribute(MeiAttribute("oct", "4"));
    MusNeumeSymbol s = MusNeumeSymbol(a);
    ASSERT_EQ(NEUME_SYMB_FLAT, s.GetSymbolType());
    
    a = MeiElement("accid");
    a.addAttribute(MeiAttribute("accid", "n"));
    a.addAttribute(MeiAttribute("pname", "c"));
    a.addAttribute(MeiAttribute("oct", "4"));
    s = MusNeumeSymbol(a);
    ASSERT_EQ(NEUME_SYMB_NATURAL, s.GetSymbolType());
}

TEST(NeumeSymbolTest, TestAccidNoAttr) {
    MeiElement a = MeiElement("accid");
    try {
        MusNeumeSymbol sym = MusNeumeSymbol(a);
        ASSERT_TRUE(false); // Shouldn't get here
    } catch (...) {
        ASSERT_TRUE(true);
    }
}

TEST(NeumeSymbolTest, TestAccitBadAttr) {
    MeiElement a = MeiElement("accid");
    a.addAttribute(MeiAttribute("accid", "m"));
    a.addAttribute(MeiAttribute("pname", "c"));
    a.addAttribute(MeiAttribute("oct", "4"));
    try {
        MusNeumeSymbol sym = MusNeumeSymbol(a);
        ASSERT_TRUE(false); // Shouldn't get here
    } catch (...) {
        ASSERT_TRUE(true);
    }
    
    a = MeiElement("accid");
    a.addAttribute(MeiAttribute("accid", "n"));
    a.addAttribute(MeiAttribute("oct", "4"));
    try {
        MusNeumeSymbol sym = MusNeumeSymbol(a);
        ASSERT_TRUE(false); // Shouldn't get here
    } catch (...) {
        ASSERT_TRUE(true);
    }
}

TEST(NeumeSymbolTest, TestCalcOffs) {
    MeiElement c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "2"));
    MusNeumeSymbol sym = MusNeumeSymbol(c);
    
    int off;
    sym.calcoffs(&off, sym.getValue());
    ASSERT_EQ(2, off);

    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "3"));
    sym = MusNeumeSymbol(c);
    sym.calcoffs(&off, sym.getValue());
    ASSERT_EQ(4, off);
    
    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "4"));
    sym = MusNeumeSymbol(c);
    sym.calcoffs(&off, sym.getValue());
    ASSERT_EQ(6, off);

    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "F"));
    c.addAttribute(MeiAttribute("line", "3"));
    sym = MusNeumeSymbol(c);
    sym.calcoffs(&off, sym.getValue());
    ASSERT_EQ(8, off);

    c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "F"));
    c.addAttribute(MeiAttribute("line", "4"));
    sym = MusNeumeSymbol(c);
    sym.calcoffs(&off, sym.getValue());
    ASSERT_EQ(10, off);
}

TEST(NeumeSymbolTest, TestSetValueClef) {
    MeiElement c = MeiElement("clef");
    c.addAttribute(MeiAttribute("shape", "C"));
    c.addAttribute(MeiAttribute("line", "1"));
    MusNeumeSymbol sym = MusNeumeSymbol(c);
    
    // Set to C, line 2
    sym.SetValue('1', NULL, 0);
    ASSERT_EQ(nC2, sym.getValue());
    ASSERT_EQ("c", c.getAttribute("shape")->getValue());
    ASSERT_EQ("2", c.getAttribute("line")->getValue());

    sym.SetValue('2', NULL, 0);
    ASSERT_EQ(nC3, sym.getValue());
    ASSERT_EQ("c", c.getAttribute("shape")->getValue());
    ASSERT_EQ("3", c.getAttribute("line")->getValue());

    sym.SetValue('3', NULL, 0);
    ASSERT_EQ(nC4, sym.getValue());
    ASSERT_EQ("c", c.getAttribute("shape")->getValue());
    ASSERT_EQ("4", c.getAttribute("line")->getValue());

    sym.SetValue('4', NULL, 0);
    ASSERT_EQ(nF3, sym.getValue());
    ASSERT_EQ("f", c.getAttribute("shape")->getValue());
    ASSERT_EQ("3", c.getAttribute("line")->getValue());

    sym.SetValue('5', NULL, 0);
    ASSERT_EQ(nF4, sym.getValue());
    ASSERT_EQ("f", c.getAttribute("shape")->getValue());
    ASSERT_EQ("4", c.getAttribute("line")->getValue());
}

TEST(NeumeSymbolTest, TestSetValueDivision) {
    MeiElement d = MeiElement("division");
    d.addAttribute(MeiAttribute("form", "major"));
    MusNeumeSymbol sym = MusNeumeSymbol(d);
    
    ASSERT_EQ(NEUME_SYMB_DIVISION_MAJOR, sym.getType());
    sym.SetValue('6', NULL, 0);
    ASSERT_EQ(NEUME_SYMB_COMMA, sym.getType());
    ASSERT_EQ("comma", d.getAttribute("form")->getValue());

    sym.SetValue('7', NULL, 0);
    ASSERT_EQ(NEUME_SYMB_DIVISION_FINAL, sym.getType());
    ASSERT_EQ("final", d.getAttribute("form")->getValue());

    sym.SetValue('8', NULL, 0);
    ASSERT_EQ(NEUME_SYMB_DIVISION_MAJOR, sym.getType());
    ASSERT_EQ("major", d.getAttribute("form")->getValue());

    sym.SetValue('9', NULL, 0);
    ASSERT_EQ(NEUME_SYMB_DIVISION_MINOR, sym.getType());
    ASSERT_EQ("minor", d.getAttribute("form")->getValue());

    sym.SetValue('0', NULL, 0);
    ASSERT_EQ(NEUME_SYMB_DIVISION_SMALL, sym.getType());
    ASSERT_EQ("small", d.getAttribute("form")->getValue());
}

TEST(NeumeSymbolTest, TestSetValueFlatNatural) {
    MeiElement a = MeiElement("accid");
    a.addAttribute(MeiAttribute("pname", "c"));
    a.addAttribute(MeiAttribute("oct", "4"));
    a.addAttribute(MeiAttribute("accid", "f"));
    MusNeumeSymbol sym = MusNeumeSymbol(a);
    
    sym.SetValue('N', NULL, 0);
    ASSERT_EQ(NEUME_SYMB_NATURAL, sym.getType());
    ASSERT_EQ("n", a.getAttribute("accid")->getValue());

    sym.SetValue('F', NULL, 0);
    ASSERT_EQ(NEUME_SYMB_FLAT, sym.getType());
    ASSERT_EQ("f", a.getAttribute("accid")->getValue());
}

TEST(NeumeSymbolTest, TestSetPitchFlatNatural) {
    MeiElement a = MeiElement("accid");
    a.addAttribute(MeiAttribute("pname", "c"));
    a.addAttribute(MeiAttribute("oct", "4"));
    a.addAttribute(MeiAttribute("accid", "f"));
    MusNeumeSymbol sym = MusNeumeSymbol(a);
    
    sym.SetPitch(4, 2);
    ASSERT_EQ("f", a.getAttribute("pname")->getValue());
    ASSERT_EQ("2", a.getAttribute("oct")->getValue());
}

TEST(NeumeSymbolTest, TestReset) {
    MusNeumeSymbol sym = MusNeumeSymbol();
    sym.ResetToClef();
    ASSERT_EQ(NEUME_SYMB_CLEF_C, sym.getType());
    ASSERT_EQ(nC2, sym.getValue());
    
    sym.ResetToNeumeSymbol();
    ASSERT_EQ(NEUME_SYMB_FLAT, sym.getType());
    ASSERT_EQ(nC2, sym.getValue());
}