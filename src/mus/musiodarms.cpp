//
//  musiodarms.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 02/07/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musdoc.h"
#include "muslayer.h"

#include "musbarline.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"

#include "musiodarms.h"

MusDarmsInput::MusDarmsInput( MusDoc *doc, wxString filename ) :
MusFileInputStream( doc, -1 )
{	
	m_div = NULL;
	m_score = NULL;
	m_parts = NULL;
	m_part = NULL;
	m_section = NULL;
    
    m_measure = NULL;
    m_layer = NULL;
    m_staff = NULL;
    
    m_filename = filename;
}

MusDarmsInput::~MusDarmsInput() {
    
}

void MusDarmsInput::UnrollKeysig(int quantity, char alter) {
    int flats[] = {PITCH_B, PITCH_E, PITCH_A, PITCH_D, PITCH_G, PITCH_C, PITCH_F};
    int sharps[] = {PITCH_F, PITCH_C, PITCH_G, PITCH_D, PITCH_A, PITCH_E, PITCH_B};
    int *alteration_set;
    unsigned char accid;
        
    if (quantity == 0) quantity++;
    
    if (alter == '-') {
        alteration_set = flats;
        accid = ACCID_FLAT;
    } else {
        alteration_set = sharps;
        accid = ACCID_SHARP;
    }
    
    for (int i = 0; i < quantity; i++) {
        MusSymbol *alter = new MusSymbol( SYMBOL_ACCID );
        alter->m_oct = 4;
        alter->m_pname = alteration_set[i];
        alter->m_accid = accid;
        m_layer->AddLayerElement(alter);
    }

}

/*
 Process the various headings: !I, !K, !N, !M
*/
int MusDarmsInput::do_globalSpec(int pos, const char* data) {
    char digit = data[++pos];
    int quantity = 0;
    
    switch (digit) {
        case 'I': // Voice nr.
            //the next digit should be a number, but we do not care what
            if (!isnumber(data[++pos])) {
                printf("Expected number after I\n");
            }
            break;
            
        case 'K': // key sig, !K2- = two flats
            if (isnumber(data[pos + 1])) { // is followed by number?
                pos++; // move forward
                quantity = data[pos] - 0x30; // get number from ascii char
            }
            // next we expect a flat or sharp, - or #
            pos++;
            if (data[pos] == '-' || data[pos] == '#') {
                UnrollKeysig(quantity, data[pos]);
            } else {
                printf("Invalid char for K: %c\n", data[pos]);
            }
            break;
            
        case 'M': // meter
            break;
            
        case 'N': // note type, ignore for the moment
            if (!isnumber(data[++pos])) {
                printf("Expected number after N\n");
            }
            break;
        
        default:
            break;
    }
    
    return pos;
}

int MusDarmsInput::do_Clef(int pos, const char* data) {
    int position = data[pos] - 0x30; // manual conversion from ASCII to int
    
    pos = pos + 2; // skip the '!' 3!F
    
    MusClef *mclef = new MusClef();
    
    if (data[pos] == 'C') {
        switch (position) {
            case 1: mclef->m_clefId = UT1; m_rest_position = PITCH_E; m_rest_octave = 4; break;
            case 3: mclef->m_clefId = UT2; m_rest_position = PITCH_C; m_rest_octave = 4; break;
            case 5: mclef->m_clefId = UT3; m_rest_position = PITCH_A; m_rest_octave = 3; break;
            case 7: mclef->m_clefId = UT4; m_rest_position = PITCH_F; m_rest_octave = 3; break;
            default: printf("Invalid C clef on line %i\n", position); break;
        }
    } else if (data[pos] == 'G') {
        switch (position) {
            case 1: mclef->m_clefId = SOL1; m_rest_position = PITCH_B; m_rest_octave = 4; break;
            case 3: mclef->m_clefId = SOL2; m_rest_position = PITCH_G; m_rest_octave = 4; break;
            default: printf("Invalid G clef on line %i\n", position); break;
        }
    } else if (data[pos] == 'F') {
        switch (position) {
            case 3: mclef->m_clefId = FA3; m_rest_position = PITCH_D; m_rest_octave = 3; break;
            case 5: mclef->m_clefId = FA4; m_rest_position = PITCH_B; m_rest_octave = 2; break;
            case 7: mclef->m_clefId = FA5; m_rest_position = PITCH_G; m_rest_octave = 2; break;
            default: printf("Invalid F clef on line %i\n", position); break;
        }
    } else {
        // what the...
        printf("Invalid clef specification: %c\n", data[pos]);
    }
    
    m_layer->AddLayerElement(mclef);
    return pos;
}

int MusDarmsInput::do_Note(int pos, const char* data, bool rest) {
    char digit;
    int accidental = 0;
    int duration;
    
    
    if (data[pos + 1] == '-') {
        accidental = ACCID_FLAT;
        pos++;
    } else if (data[pos + 1] == '#') {
        accidental = ACCID_SHARP;
        pos++;
    }
    
    digit = data[++pos];
    switch (digit) {
        case 'W':
            duration = DUR_1;
            // wholes can be repeated, yes this way is not nice
            if (data[pos + 1] == 'W') { // WW = BREVE
                duration = DUR_BR;
                pos++;
                if (data[pos + 1] == 'W') { // WWW - longa
                    pos++;
                    duration = DUR_LG;
                }
            }
            break;
        case 'H': duration = DUR_2; break;
        case 'Q': duration = DUR_4; break;
        case 'E': duration = DUR_8; break;
        case 'S': duration = DUR_16; break;
        case 'T': duration = DUR_32; break;
        case 'X': duration = DUR_64; break;
        case 'Y': duration = DUR_128; break;
        case 'Z': duration = DUR_256; break;
            
        default:
            printf("Unkown note duration: %c\n", digit);
            return 0;
            break;
    }
    
    if (rest) {
        MusRest *rest =  new MusRest;
        rest->m_dur = duration;
        rest->m_oct = m_rest_octave;
        rest->m_pname = m_rest_position;
        m_layer->AddLayerElement(rest);
    } else {
        MusNote *note = new MusNote;
        note->m_dur = duration;
        note->m_accid = accidental;
        note->m_oct = 4;
        note->m_pname = PITCH_A;
        m_layer->AddLayerElement(note);
    }
    
    return pos;
}

bool MusDarmsInput::ImportFile() {
    char data[10000];
    int len, res;
    int pos = 0;
    std::ifstream infile;
    
    infile.open(m_filename);
    
    if (infile.eof()) {
        infile.close();
        return false;
    }
    
    infile.getline(data, sizeof(data), '\n');
    len = strlen(data);
    infile.close();
    printf("len: %i\n", len);
    
    // Prepare the aruspix document
    m_div = new MusDiv();
    m_score = new MusScore( );
    m_section = new MusSection( );
    
    // Create here, for now we have just one huge measure
    m_measure = new MusMeasure;
    m_staff = new MusStaff;
    m_layer = new MusLayer;
    
    
    m_staff->AddLayer(m_layer);
    m_measure->AddStaff(m_staff);
    m_section->AddMeasure(m_measure);
    
    // do this the C style, char by char
    while (pos < len) {
        char c = data[pos];
        
        if (c == '!') {
            printf("Global spec. at %i\n", pos);
            res = do_globalSpec(pos, data);
            if (res) pos = res;
        } else if (isnumber(c)) {
            //is number followed by '!' ? it is a clef
            if (data[pos + 1] == '!') {
                res = do_Clef(pos, data);
                if (res) pos = res;
            } else { // we assume it is a note
                res = do_Note(pos, data, false);
                if (res) pos = res;
            }
        } else if (c == 'R') {
            res = do_Note(pos, data, true);
            if (res) pos = res;
        } else {
            if (!isspace(c))
                printf("Other %c\n", c);
        }
 
        pos++;
    }
    
    m_score->AddSection(m_section);
    m_div->AddScore(m_score);
    m_doc->AddDiv(m_div);
    
    return true;
}