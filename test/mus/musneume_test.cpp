/*
 *  musneume_test.cpp
 *  aruspix
 *
 *  Created by Alastair Porter on 11-03-29.
 *  Copyright 2011 com.aruspix.www. All rights reserved.
 *
 */


#include <gtest/gtest.h>

#include <mei/mei.h>

#include "musneume.h"

TEST(NeumeTest, TestMakePunctum) {
    MeiElement ne = MeiElement("neume");
    MeiElement nc = MeiElement("nc");
    MeiElement no = MeiElement("note");
    ne.addAttribute(MeiAttribute("name", "punctum"));
    no.addAttribute(MeiAttribute("pname", "c"));
    no.addAttribute(MeiAttribute("oct", "4"));
    nc.addChild(no);
    ne.addChild(nc);
    
    MusNeume mus = MusNeume(ne);
    ASSERT_EQ(NEUME_TYPE_PUNCTUM, mus.getType());
    ASSERT_EQ(1, mus.pitch);
    ASSERT_EQ(1, mus.getPitches().size());
    ASSERT_EQ(0, mus.getPitches()[0].getPitchDifference());
}

TEST(NeumeTest, TestMeiRef) {
    MeiElement ne = MeiElement("neume");
    MeiElement nc = MeiElement("nc");
    MeiElement no = MeiElement("note");
    ne.addAttribute(MeiAttribute("name", "punctum"));
    no.addAttribute(MeiAttribute("pname", "c"));
    no.addAttribute(MeiAttribute("oct", "4"));
    nc.addChild(no);
    ne.addChild(nc);
    
    MusNeume mus = MusNeume(ne);
    ASSERT_EQ(mus.getMeiRef().getAttribute("name")->getValue(), "punctum");
}

TEST(NeumeTest, TestSetPitch) {
    MeiElement ne = MeiElement("neume");
    MeiElement nc = MeiElement("nc");
    MeiElement no1 = MeiElement("note");
    MeiElement no2 = MeiElement("note");
    MeiElement no3 = MeiElement("note");
    ne.addAttribute(MeiAttribute("name", "torculus"));
    no1.addAttribute(MeiAttribute("pname", "c"));
    no1.addAttribute(MeiAttribute("oct", "4"));
    no2.addAttribute(MeiAttribute("pname", "d"));
    no2.addAttribute(MeiAttribute("oct", "4"));
    no3.addAttribute(MeiAttribute("pname", "c"));
    no3.addAttribute(MeiAttribute("oct", "4"));
    nc.addChild(no1);
    nc.addChild(no2);
    nc.addChild(no3);
    ne.addChild(nc);
    MusNeume mus = MusNeume(ne);

    mus.SetPitch(2, 4);
    ASSERT_EQ(2, mus.pitch);
    ASSERT_EQ("d", mus.getMeiRef().getChildren().at(0).getChildren().at(0).getAttribute("pname")->getValue());
    ASSERT_EQ("4", mus.getMeiRef().getChildren().at(0).getChildren().at(0).getAttribute("oct")->getValue());
    ASSERT_EQ("e", mus.getMeiRef().getChildren().at(0).getChildren().at(1).getAttribute("pname")->getValue());
}

TEST(NeumeTest, TestMakeOtherTypes) {
    MeiElement ne = MeiElement("neume");
    MeiElement nc = MeiElement("nc");
    MeiElement no = MeiElement("note");
    no.addAttribute(MeiAttribute("pname", "c"));
    no.addAttribute(MeiAttribute("oct", "4"));
    ne.addAttribute(MeiAttribute("name", "virga"));
    nc.addChild(no);
    ne.addChild(nc);
    MusNeume mus = MusNeume(ne);
    ASSERT_EQ(mus.getType(), NEUME_TYPE_VIRGA);
    
    ne.removeAttribute("name");
    ne.addAttribute(MeiAttribute("name", "podatus"));
    mus = MusNeume(ne);
    ASSERT_EQ(mus.getType(), NEUME_TYPE_PODATUS);
    
    ne.removeAttribute("name");
    ne.addAttribute(MeiAttribute("name", "clivis"));
    mus = MusNeume(ne);
    ASSERT_EQ(mus.getType(), NEUME_TYPE_CLIVIS);
    
    ne.removeAttribute("name");
    ne.addAttribute(MeiAttribute("name", "porrectus"));
    mus = MusNeume(ne);
    ASSERT_EQ(mus.getType(), NEUME_TYPE_PORRECTUS);
    
    ne.removeAttribute("name");
    ne.addAttribute(MeiAttribute("name", "scandicus"));
    mus = MusNeume(ne);
    ASSERT_EQ(mus.getType(), NEUME_TYPE_SCANDICUS);
    
    ne.removeAttribute("name");
    ne.addAttribute(MeiAttribute("name", "torculus"));
    mus = MusNeume(ne);
    ASSERT_EQ(mus.getType(), NEUME_TYPE_TORCULUS);
}

TEST(NeumeTest, TestMakeBadNeume) {
    MeiElement ne = MeiElement("neume");
    MeiElement nc = MeiElement("nc");
    ne.addAttribute(MeiAttribute("name", "not-a-type"));
    ne.addChild(nc);
    
    try {
        MusNeume mus = MusNeume(ne);
        ASSERT_TRUE(false); // Shouldn't get here
    } catch (...) {
        ASSERT_TRUE(true);
    }
}

TEST(NeumeTest, TestNoNcElement) {
    MeiElement ne = MeiElement("neume");
    MeiElement other = MeiElement("something-else");
    ne.addChild(other);
    try {
        MusNeume mus = MusNeume(ne);
        ASSERT_TRUE(false); // Shouldn't get here
    } catch (...) {
        ASSERT_TRUE(true);
    }
}

TEST(NeumeTest, TestMakeOneNoteElement) {
    MeiElement ne = MeiElement("neume");
    MeiElement nc = MeiElement("nc");
    MeiElement no1 = MeiElement("note");
    ne.addAttribute(MeiAttribute("name", "punctum"));
    no1.addAttribute(MeiAttribute("pname", "g"));
    no1.addAttribute(MeiAttribute("oct", "4"));
    nc.addChild(no1);
    ne.addChild(nc);
    
    MusNeume mus = MusNeume(ne);
    ASSERT_EQ(mus.pitch, 5);
    ASSERT_EQ(mus.oct, 4);
    ASSERT_EQ(mus.getPitches().size(), 1);
    ASSERT_EQ(mus.getPitches().at(0).getPitchDifference(), 0);
}

TEST(NeumeTest, TestMakeManyNoteElements) {
    MeiElement ne = MeiElement("neume");
    MeiElement nc = MeiElement("nc");
    MeiElement no1 = MeiElement("note");
    MeiElement no2 = MeiElement("note");
    MeiElement no3 = MeiElement("note");
    ne.addAttribute(MeiAttribute("name", "torculus"));
    no1.addAttribute(MeiAttribute("pname", "c"));
    no1.addAttribute(MeiAttribute("oct", "4"));
    no2.addAttribute(MeiAttribute("pname", "d"));
    no2.addAttribute(MeiAttribute("oct", "4"));
    no3.addAttribute(MeiAttribute("pname", "c"));
    no3.addAttribute(MeiAttribute("oct", "4"));
    nc.addChild(no1);
    nc.addChild(no2);
    nc.addChild(no3);
    ne.addChild(nc);
    
    MusNeume mus = MusNeume(ne);
    ASSERT_EQ(mus.getType(), NEUME_TYPE_TORCULUS);
    
    ASSERT_EQ(mus.getPitches().size(), 3);
    ASSERT_EQ(mus.pitch, 1);
    ASSERT_EQ(mus.oct, 4);
    ASSERT_EQ(mus.getPitches().at(0).getPitchDifference(), 0);
    ASSERT_EQ(mus.getPitches().at(1).getPitchDifference(), 1);
    ASSERT_EQ(mus.getPitches().at(2).getPitchDifference(), 0);
}

TEST(NeumeTest, TestSkipOctaveUp) {
    MeiElement ne = MeiElement("neume");
    MeiElement nc = MeiElement("nc");
    MeiElement no1 = MeiElement("note");
    MeiElement no2 = MeiElement("note");
    ne.addAttribute(MeiAttribute("name", "podatus"));
    no1.addAttribute(MeiAttribute("pname", "g"));
    no1.addAttribute(MeiAttribute("oct", "4"));
    no2.addAttribute(MeiAttribute("pname", "a"));
    no2.addAttribute(MeiAttribute("oct", "5")); 
    nc.addChild(no1);
    nc.addChild(no2);
    ne.addChild(nc);
    
    MusNeume mus = MusNeume(ne);
    ASSERT_EQ(mus.getType(), NEUME_TYPE_PODATUS);
    
    ASSERT_EQ(mus.getPitches().size(), 2);
    ASSERT_EQ(mus.pitch, 5);
    ASSERT_EQ(mus.oct, 4);
    ASSERT_EQ(mus.getPitches().at(0).getPitchDifference(), 0);
    ASSERT_EQ(mus.getPitches().at(1).getPitchDifference(), 8);
}

TEST(NeumeElementTest, TestMakeElement) {
    MeiElement note = MeiElement("note");
    note.addAttribute(MeiAttribute("pname", "c"));
    note.addAttribute(MeiAttribute("oct", "4"));
    
    MusNeumeElement mus = MusNeumeElement(note, 1, 4);
    ASSERT_EQ(mus.getPitchDifference(), 0);
}

TEST(NeumeElementTest, TestMissingPitch) {
    MeiElement note = MeiElement("note");
    note.addAttribute(MeiAttribute("oct", "4"));
    try { 
        MusNeumeElement mus = MusNeumeElement(note, 1, 4);
        ASSERT_TRUE(false);
    } catch (...) {
        ASSERT_TRUE(true);
    }
}

TEST(NeumeElementTest, TestMissingOctave) {
    MeiElement note = MeiElement("note");
    note.addAttribute(MeiAttribute("pname", "c"));
    try {
        MusNeumeElement mus = MusNeumeElement(note, 1, 4);
        ASSERT_TRUE(false);
    } catch (...) {
        ASSERT_TRUE(true);
    }
}
