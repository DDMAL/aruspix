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

// Ok, this is ugly, but since this is static data, why not?
pitchmap MusDarmsInput::PitchMap[] = {
    /* 00 */ {1, PITCH_C}, {1, PITCH_D}, {1, PITCH_E}, {1, PITCH_F}, {1, PITCH_G}, {1, PITCH_A}, {1, PITCH_B},
    /* 07 */ {2, PITCH_C}, {2, PITCH_D}, {2, PITCH_E}, {2, PITCH_F}, {2, PITCH_G}, {2, PITCH_A}, {2, PITCH_B},
    /* 14 */ {3, PITCH_C}, {3, PITCH_D}, {3, PITCH_E}, {3, PITCH_F}, {3, PITCH_G}, {3, PITCH_A}, {3, PITCH_B},
    /* 21 */ {4, PITCH_C}, {4, PITCH_D}, {4, PITCH_E}, {4, PITCH_F}, {4, PITCH_G}, {4, PITCH_A}, {4, PITCH_B},
    /* 28 */ {5, PITCH_C}, {5, PITCH_D}, {5, PITCH_E}, {5, PITCH_F}, {5, PITCH_G}, {5, PITCH_A}, {5, PITCH_B},
    /* 35 */ {6, PITCH_C}, {6, PITCH_D}, {6, PITCH_E}, {6, PITCH_F}, {6, PITCH_G}, {6, PITCH_A}, {6, PITCH_B},
    /* 42 */ {7, PITCH_C}, {7, PITCH_D}, {7, PITCH_E}, {7, PITCH_F}, {7, PITCH_G}, {7, PITCH_A}, {7, PITCH_B},
    /* 49 */ {8, PITCH_C}, {8, PITCH_D}, {8, PITCH_E}, {8, PITCH_F}, {8, PITCH_G}, {8, PITCH_A}, {8, PITCH_B},
};

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
 Read the meter
 */
int MusDarmsInput::parseMeter(int pos, const char* data) {
 
    MusMensur *meter = new MusMensur;
    
    pos++;
    if (data[pos] == 'C') {
        if (data[pos + 1] == '/') {
            pos++;
            meter->m_meterSymb = METER_SYMB_CUT;
        } else {
            meter->m_meterSymb = METER_SYMB_COMMON;
        }
    } else if (isnumber(data[pos])) { // Coupound meter
        int n1, n2;
        n1 = data[pos] - 0x30; // old school conversion to int
        if (isnumber(data[pos + 1])) {
            n2 = data[++pos] - 0x30; // idem
            n1 = (n1 * 10) + n2;
        }
        meter->m_num = n1;
        
        //we expect the next char a ':', or make a single digit meter
        if (data[++pos] != ':') {
            meter->m_numBase = 1;
        } else {
            // same as above, get one or two nums
            n1 = data[++pos] - 0x30; // old school conversion to int
            if (isnumber(data[pos + 1])) {
                n2 = data[++pos] - 0x30; // idem
                n1 = (n1 * 10) + n2;
            }
            
            meter->m_numBase = n1;
        }
        printf("Meter is: %i %i\n", meter->m_num, meter->m_numBase);
    }
    
    m_layer->AddLayerElement(meter);
    return pos;
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
            pos = parseMeter(pos, data);
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
        m_clef_offset = 21 - position; // 21 is the position in the array, position is of the clef
    } else if (data[pos] == 'G') {
        switch (position) {
            case 1: mclef->m_clefId = SOL1; m_rest_position = PITCH_B; m_rest_octave = 4; break;
            case 3: mclef->m_clefId = SOL2; m_rest_position = PITCH_G; m_rest_octave = 4; break;
            default: printf("Invalid G clef on line %i\n", position); break;
        }
        m_clef_offset = 25 - position;
    } else if (data[pos] == 'F') {
        switch (position) {
            case 3: mclef->m_clefId = FA3; m_rest_position = PITCH_D; m_rest_octave = 3; break;
            case 5: mclef->m_clefId = FA4; m_rest_position = PITCH_B; m_rest_octave = 2; break;
            case 7: mclef->m_clefId = FA5; m_rest_position = PITCH_G; m_rest_octave = 2; break;
            default: printf("Invalid F clef on line %i\n", position); break;
        }
        m_clef_offset = 15 - position;
    } else {
        // what the...
        printf("Invalid clef specification: %c\n", data[pos]);
        return 0; // fail
    }
    
    m_layer->AddLayerElement(mclef);
    return pos;
}

int MusDarmsInput::do_Note(int pos, const char* data, bool rest) {
    int position;
    int accidental = 0;
    int duration;
    int dot = 0;
    
    //pos points to the first digit of the note
    // it cn be 5W, 12W, -1W
    
    // Negative number, only '-' and one digit
    if (data[pos] == '-') {
        // be sure following char is a number
        if (!isnumber(data[pos + 1])) return 0;
        position = -(data[++pos] - 0x30);
    } else {
        // as above
        if (!isnumber(data[pos]) && data[pos] != 'R') return 0; // this should not happen, as it is checked in the caller
        // positive number
        position = data[pos] - 0x30;
        //check for second digit
        if (isnumber(data[pos + 1])) {
            pos++;
            position = (position * 10) + (data[pos] - 0x30);
        }
    }
    
    if (data[pos + 1] == '-') {
        accidental = ACCID_FLAT;
        pos++;
    } else if (data[pos + 1] == '#') {
        accidental = ACCID_SHARP;
        pos++;
    } else if (data[pos + 1] == '*') {
        accidental = ACCID_NATURAL;
        pos++;
    }
    
    switch (data[++pos]) {
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
            printf("Unkown note duration: %c\n", data[pos]);
            return 0;
            break;
    }
    
    if (data[pos + 1] =='.') {
        pos++;
        dot = 1;
    }
    
    if (rest) {
        MusRest *rest =  new MusRest;
        rest->m_dur = duration;
        rest->m_oct = m_rest_octave;
        rest->m_pname = m_rest_position;
        rest->m_dots = dot;
        m_layer->AddLayerElement(rest);
    } else {
        MusNote *note = new MusNote;
        note->m_dur = duration;
        note->m_accid = accidental;
        note->m_oct = PitchMap[position + m_clef_offset].oct;
        note->m_pname = PitchMap[position + m_clef_offset].pitch;
        note->m_dots = dot;
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
    printf("len: %i, %s\n", len, data);
    
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
        } else if (isnumber(c) || c == '-' ) { // check for '-' too as note positions can be negative
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
            //if (!isspace(c))
                //printf("Other %c\n", c);
        }
 
        pos++;
    }
    
    m_score->AddSection(m_section);
    m_div->AddScore(m_score);
    m_doc->AddDiv(m_div);
    
    return true;
}