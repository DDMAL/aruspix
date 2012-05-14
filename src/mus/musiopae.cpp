/////////////////////////////////////////////////////////////////////////////
// Name:        musiopae.cpp
// Author:      Rodolfo Zitellini // this is the important stuff!
// Created:     2012
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wx.h"

#include "wx/filename.h"
#include "wx/txtstrm.h"

#include "musiopae.h"

#include "musbarline.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"

#include "app/axapp.h"

#include <vector>
#include <regex.h>

using std::vector;


// User interface variables:
int       debugQ = 0;                // used with --debug option
int       stdoutQ = 0;
char      outdir[1024] = {0};        // used with -d option
char      extension[1024] = {0};     // used with -e option
char      hum2abc[1024] = {0};       // used with -a option
int       quietQ  = 0;               // used with -q option
int       quiet2Q = 0;               // used with -Q option

// Global variables:
char data_line[10001] = {0};
Array<char> data_key;
Array<char> data_value;


//////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// MusPaeInput
//----------------------------------------------------------------------------

MusPaeInput::MusPaeInput( MusDoc *doc, wxString filename ) :
// This is pretty bad. We open a bad fileoinputstream as we don't use it
MusFileInputStream( doc, -1 )
{
    m_filename = filename;
	
	m_div = NULL;
	m_score = NULL;
	m_parts = NULL;
	m_part = NULL;
	m_section = NULL;
	m_measure = NULL;
	m_staff = NULL;
	m_layer = NULL;
}

MusPaeInput::~MusPaeInput()
{
}

//////////////////////////////////////////////////////////////////////////

bool MusPaeInput::ImportFile()
{
    
    std::ifstream infile;
    infile.open(m_filename);
    
    convertPlainAndEasyToKern(infile, std::cout);
    
    return true;
}


//////////////////////////////
//
// convertPlainAndEasyToKern --
//

void MusPaeInput::convertPlainAndEasyToKern(std::istream &infile, std::ostream &out) {
    
    // buffers
    char c_clef[1024] = {0};
    char c_key[1024] = {0};
    char c_keysig[1024] = {0};
    char c_timesig[1024] = {0};
    char c_alttimesig[1024] = {0};
    char incipit[10001] = {0};
    
    string s_key;
    MeasureObject current_measure;
    NoteObject current_note;
    Array<int> current_key; // not the measure one, which will be altered by temporary alterations
    current_key.setSize(7);
    current_key.setAll(0);
    
    Array<MeasureObject> staff;
    staff.setSize(0);
    
    // read values
    while (!infile.eof()) {
        infile.getline(data_line, 10000, '\n');
        if (infile.eof()) {
            //std::cerr << "Truncated file or ending tag missing" << std::endl;
            exit(1);
        }
        getAtRecordKeyValue(data_key, data_value, data_line);
        if (strcmp(data_key.getBase(),"end")==0) {   
            break;
        } else if (strcmp(data_key.getBase(),"clef")==0) { 
            strcpy( c_clef, data_value.getBase() );
        } else if (strcmp(data_key.getBase(),"key")==0) { 
            strcpy( c_key, data_value.getBase() );
        } else if (strcmp(data_key.getBase(),"keysig")==0) { 
            strcpy( c_keysig, data_value.getBase() );
        } else if (strcmp(data_key.getBase(),"timesig")==0) { 
            strcpy( c_timesig, data_value.getBase() );
        } else if (strcmp(data_key.getBase(),"alttimesig")==0) { 
            strcpy( c_alttimesig, data_value.getBase() );
        } else if (strcmp(data_key.getBase(),"data")==0) { 
            strcpy( incipit, data_value.getBase() );
        } else if (strncmp(data_line,"!!", 2) == 0) { 
            out << data_line << "\n";
        }
    }
    
    // write as comment in the output
    /*
     if (!quietQ) {
     out << "!!!clef:" << c_clef << "\n";
     out << "!!!key:" << c_key << "\n";
     out << "!!!keysig:" << c_keysig << "\n";
     out << "!!!timesig:" << c_timesig << "\n";
     out << "!!!alttimesig:" << c_alttimesig << "\n";
     out << "!!!incipit:" << incipit << "\n";
     }
     */
    
    if (strlen(c_clef)) {
        getClefInfo(c_clef, &current_measure.clef );    // do we need to put a default clef?
    }
    if (strlen(c_key)) {
        getKey(c_key, &s_key); // key is not stored in the measure, only for the entire piece
    }
    if (strlen(c_keysig)) {
        getKeyInfo( c_keysig, current_measure.a_key, &current_measure.s_key);
        current_key = current_measure.a_key;
    }
    if (strlen(c_timesig)) {
        getTimeInfo( c_timesig, current_measure.a_timeinfo, &current_measure.s_timeinfo);
        current_measure.measure_duration = getMeasureDur(current_measure.a_timeinfo);
    }   
    
    // read the incipit string
    int length = strlen(incipit);
    int i = 0;
	while(i < length) {
        // eat the input...
        
		if (incipit[i] == ' ') {
            // just skip
            i++;
        }
        
        // octaves
        if ((incipit[i] == '\'') || (incipit[i] == ',')) {
            i += getOctave( incipit, &current_note.mnote->m_oct, i );
        }
        
        // rhythmic values
        else if (isdigit(incipit[i]) != 0) {
            i += getDurations( incipit, &current_measure, i );
        }
        
        //accidentals (1 = n; 2 = x; 3 = xx; 4 = b; 5 = bb)    
        else if (incipit[i] == 'n' || incipit[i] == 'x' || incipit[i] == 'b') {
            i += getAccidental( incipit, &current_note.mnote->m_accid, i );
        }
        
        // beaming starts
		else if (incipit[i] == '{') {
			current_note.beam = 1;
        }
        
        // beaming ends
		else if (incipit[i] == '}') {
			current_note.beam = 0; // should not have to be done, but just in case
		}
		
        // slurs are read when adding the note
		else if (incipit[i] == '+') {
        }
        
		// beginning tuplets & fermatas
		else if (incipit[i] == '(') {
            i += getTupletFermata( incipit, getDurationWithDot( current_measure.durations[0], current_measure.dots[0]), &current_note, i );
		}
        
        // end of tuplets
		else if ((incipit[i] == ';') || (incipit[i] == ')')) {
            i += getTupletFermataEnd( incipit, &current_note, i );
		}
        
		// trills are read when adding the note
		else if (incipit[i] == 't') {
        }
        
		//grace notes
		else if ((incipit[i] == 'g') || (incipit[i] == 'q')) {
			i += getGraceNote( incipit, &current_note, i );
		}
		
        
        // end of appogiatura
		else if (incipit[i] == 'r') {
			current_note.appoggiatura = 0; // should not have to be done, but just in case
		}
        
        //note and rest
        else if (((incipit[i]-'A'>=0) && (incipit[i]-'A'<7)) || (incipit[i]=='-')) {
            i += getNote( incipit, &current_note, &current_measure, i );
        }
        
  		// whole rest
		else if (incipit[i] == '=') {
            i += getWholeRest( incipit, &current_measure.wholerest, i );		
		} 
        
		// abbreviation
        else if (incipit[i] == '!') {
            i += getAbbreviation( incipit, &current_measure, i );
        }
        
        // measure repetition
        else if ((incipit[i] == 'i') && staff.getSize()) {
            MeasureObject last_measure = staff[staff.getSize()-1];
            current_measure.notes = last_measure.notes;
            current_measure.a_timeinfo = last_measure.a_timeinfo;
            current_measure.measure_duration = getMeasureDur(last_measure.a_timeinfo);
            current_measure.a_key = current_key;
        }
        
        //barline
        else if ((incipit[i] == ':') || (incipit[i] == '/')) {
            i += getBarline(incipit, &current_measure.barline, i);
            current_measure.abbreviation_offset = 0; // just in case...
            staff.append( current_measure );
            current_measure.reset();
            current_measure.a_key = current_key;
            current_measure.measure_duration = getMeasureDur(current_measure.a_timeinfo);
        }
        
		//clef change
		else if ((incipit[i] == '%') && (i+1 < length)) {
            i += getClefInfo(incipit, &current_measure.clef, i + 1);
        }
        
		//time signature change
		else if ((incipit[i] == '@') && (i+1 < length)) {
            i += getTimeInfo( incipit, current_measure.a_timeinfo, &current_measure.s_timeinfo, i + 1);
            current_measure.measure_duration = getMeasureDur(current_measure.a_timeinfo);
        } 
        
  		//key signature change
		else if ((incipit[i] == '$') && (i+1 < length)) {
            i += getKeyInfo( incipit, current_measure.a_key, &current_measure.s_key, i + 1);
            current_key = current_measure.a_key;
		} 
        
        i++;
    }
    
    // we need to add the last measure if it has no barline at the end
    if (current_measure.notes.getSize() != 0) {
        //current_measure.barline = "=-";
        staff.append( current_measure );
    }
    
    
    m_div = new MusDiv();
    m_score = new MusScore( );
    m_section = new MusSection( );
    
    // output
    out << "**kern\n";
    out << "*MM120\n";
    if ( s_key.length() ) {
        out << s_key << "\n";
    }
    int j = 0;
    for (j = 0; j < staff.getSize(); j++) {
        printMeasure( out, &staff[j] );
    }
    out << "*-\n\n";
    
    if (strlen(hum2abc)) {
        out << "!!!hum2abc:" << hum2abc << "\n";
    }
    
    wxLogError(_("Bonga"));
    
    m_section->AddSectionElement(m_measure);
    m_score->AddSection(m_section);
    m_div->AddScore(m_score);
    m_doc->AddDiv(m_div);
}



//////////////////////////////
//
// getOctave --
//

int MusPaeInput::getOctave (const char* incipit, char *octave, int index ) {
    
    int i = index;
    int length = strlen(incipit);
    if (incipit[i] == '\'') {
        *octave = 1;
        while ((i+1 < length) && (incipit[i+1] == '\'')) {
            (*octave)++;
            i++;
        }
    } else if (incipit[i] == ',') {
        //negative octave
        *octave = -1;
        while ((i+1 < length) && (incipit[i+1] == ',')) {
            (*octave)--;
            i++;
        }
    }
    
    // humdrum octave
    switch (*octave) {
        case  0:  *octave = 4;  break;
        case  1:  *octave = 4;  break;
        case  2:  *octave = 5;  break;
        case  3:  *octave = 6;  break;
        case  4:  *octave = 7;  break;
        case  5:  *octave = 8;  break;
        case  6:  *octave = 9;  break;
        case -1:  *octave = 3;  break;
        case -2:  *octave = 2;  break;
        case -3:  *octave = 1;  break;
        case -4:  *octave = 0;  break;
        default:  *octave = 4;
    }
    
    return i - index;
}



//////////////////////////////
//
// getDuration --
//

int MusPaeInput::getDuration(const char* incipit, int *duration, int *dot, int index ) {
    
    int i = index;
    int length = strlen(incipit);
    
    switch (incipit[i]) {
        case '0': *duration = DUR_LG; break;
        case '1': *duration = DUR_1; break;
        case '2': *duration = DUR_2; break;
        case '3': *duration = DUR_32; break;
        case '4': *duration = DUR_4; break;
        case '5': *duration = DUR_64; break;
        case '6': *duration = DUR_16; break;
        case '7': *duration = DUR_128; break;
        case '8': *duration = DUR_8; break;
        case '9': *duration = DUR_BR; break;
    }
     
    
    *dot=0;
    if ((i+1 < length) && (incipit[i+1] == '.')) {
        // one dot
        (*dot)++;
        i++;
    }
    if ((i+1 < length) && (incipit[i+1] == '.')) {
        // two dots
        (*dot)++;
        i++;
    }
    if ((*dot == 1) && (*duration == 7)) {
        // neumatic notation
        *duration = 1.0;
        *dot = 0;
        std::cout << "Warning: found a note in neumatic notation (7.), using quarter note instead" << std::endl;				
    }
    
    return i - index;
}



//////////////////////////////
//
// getDurations --
//

int MusPaeInput::getDurations(const char* incipit, MeasureObject* measure, int index ) {
    
    int i = index;
    int length = strlen(incipit);
    
    measure->durations_offset = 0;
    
    int j = 0;
    do {
        measure->durations.setSize(j+1);
        measure->dots.setSize(j+1);
        i += getDuration(incipit, &measure->durations[j], &measure->dots[j], i );
        j++;
        if ((i+1 < length) && isdigit(incipit[i+1])) {
            i++;
        } else {
            break;
        }
    } while ( 1 );
    //std::cout << "duration count:" << j << std::std::endl;
    
    return i - index;
}



//////////////////////////////
//
// getAccidental --
//

int MusPaeInput::getAccidental(const char* incipit, unsigned char *accident, int index ) {
    
    int i = index;
    int length = strlen(incipit);
    
    if (incipit[i] == 'n') {
        *accident = ACCID_NATURAL;
    } else if (incipit[i] == 'x') {
        *accident = ACCID_SHARP;
        if ((i+1 < length) && (incipit[i+1] == 'x')) {
            *accident = ACCID_DOUBLE_SHARP;
            i++;
        }
    } else if (incipit[i] == 'b') {
        *accident = ACCID_FLAT;
        if ((i+1 < length) && (incipit[i+1] == 'b')) {
            *accident = ACCID_DOUBLE_FLAT;
            i++;
        }
    }
    return i - index;
}



//////////////////////////////
//
// getTupletOrFermata --
//

int MusPaeInput::getTupletFermata(const char* incipit, double current_duration, NoteObject *note, int index ) {
    
    int i = index;
    int length = strlen(incipit);
    
    // detect if it is a fermata or a tuplet
    regex_t re;
    regcomp(&re, "^([^)]*[ABCDEFG-][^)]*[ABCDEFG-][^)]*)", REG_EXTENDED);
    int is_tuplet = regexec(&re, incipit + i, 0, NULL, 0);
    regfree(&re);
    
    if (is_tuplet == 0) {
        int t = i;
        int note_duration = current_duration; // just in case the duration is missing in the tuplet
        bool is_triplet = false; // in triplets, there is no duration given before the tuplet.
        bool is_first_event = false; // we use the first duration if there is one (they should be one)
        int note_count = 0; // we need to count notes for triplets as sometimes in the data several triplets appear together
        // we need to change the tuplet duration in consequence
        int note_dot = 0;
        // the duration for the tuplet given before it, or by the first duration value in triplets
        if ((index == 0) || (isdigit(incipit[index-1]) == 0)) {
            is_triplet = true; // this means that we have to keep the first value for triplets
            current_duration *= 2; // in case there is no value, use the current value
            // this is wrong syntax wise but it appears in the data
            is_first_event = true;
        }
        
        double tuplet_duration = 0.0;
        while ((t < length) && (incipit[t] != ')') && (incipit[t] != ';')) {
            if (isdigit(incipit[t]) != 0) { // new duration in the tuplet
                t += getDuration( incipit, &note_duration, &note_dot, t );
                note_duration = getDurationWithDot(note_duration, note_dot);
                if (is_triplet && is_first_event) { // it is a triplet
                    current_duration = note_duration * 2;
                    is_first_event = false; // we don't need to get the value again
                }
            } else if (((incipit[t]-'A'>=0) && (incipit[t]-'A'<7)) || (incipit[t]=='-')) { // note or rest
                //std::cout << "tuplet note:" << incipit[t] << std::endl;
                tuplet_duration += note_duration;
                is_first_event = false;
                note_count++;
            }
            t++;
        }
        
        // the overall tuplet duration
        if (!is_triplet) {
            note->tuplet = tuplet_duration / current_duration;
        } else {
            // how many triplets we have is given by the note_count
            // several triplet in one tuplet is wrong syntax wise but it appears in the data
            note->tuplet = tuplet_duration / (current_duration * ceil(double(note_count)/3));
        }
        
    } else {
        if ( note->tuplet != 1.0 ) {
            std::cout << "Warning: fermata within a tuplet. Won't be handled correctly" << std::endl;
        }
        note->fermata = true;
    }
    
    return i - index;
    
}



//////////////////////////////
//
// getTupletFermataEnd --
//

int MusPaeInput::getTupletFermataEnd(const char* incipit, NoteObject *note, int index ) {
    
    int i = index;
    int length = strlen(incipit);
    
    if (incipit[i] == ';') {
        while ((i+1 < length) && (incipit[i+1] != ')')) {
            // we don't need the number of notes in humdrum, just skip it
            i++;
        }
    }
    
    // TODO currently fermatas inside tuplets won't be handled correctly
    // close both now
    note->tuplet = 1.0;
    note->fermata = false;
    
    return i - index;
}



//////////////////////////////
//
// getGraceNote --
//

int MusPaeInput::getGraceNote(const char* incipit, NoteObject *note, int index ) {
    
    int i = index;
    int length = strlen(incipit);
    
    //acciaccatura
    if (incipit[i] == 'g') {
        note->acciaccatura = true;
    }
    
    // appoggiatura
    else if (incipit[i] == 'q') {
        note->appoggiatura = 1;
        if ((i+1 < length) && (incipit[i+1] == 'q')) {
            note->appoggiatura_multiple = true;
            i++;
            int r = i;
            while ((r < length) && (incipit[r] != 'r')) {
                if ((incipit[r]-'A'>=0) && (incipit[r]-'A'<7)) {
                    note->appoggiatura++;
                    //std::cout << note->appoggiatura << std::endl; 
                }
                r++;
            }
        }
    }
    return i - index;
}



//////////////////////////////
//
// getKey --
//

void MusPaeInput::getKey(const char* key_str, string *output) {
    
    std::ostringstream sout;  
    
    if (strcmp(key_str, "G") == 0) {
        sout << "*G:";
    } else if (strcmp(key_str, "C") == 0) {
        sout << "*C:";
    } else if (strcmp(key_str, "D") == 0) {
        sout << "*D:";
    } else if (strcmp(key_str, "F") == 0) {
        sout << "*F:";
    } else if (strcmp(key_str, "Bb") == 0) {
        sout << "*B-:";
    } else if (strcmp(key_str, "A") == 0) {
        sout << "*A:";
    } else if (strcmp(key_str, "Eb") == 0) {
        sout << "*E-:";
    } else if (strcmp(key_str, "g") == 0) {
        sout << "*g:";
    } else if (strcmp(key_str, "d") == 0) {
        sout << "*d:";
    } else if (strcmp(key_str, "a") == 0) {
        sout << "*a:";
    } else if (strcmp(key_str, "c") == 0) {
        sout << "*c:";
    } else if (strcmp(key_str, "e") == 0) {
        sout << "*e:";
    } else if (strcmp(key_str, "E") == 0) {
        sout << "*E:";
    } else if (strcmp(key_str, "f") == 0) {
        sout << "*f:";
    } else if (strcmp(key_str, "b") == 0) {
        sout << "*b:";
    } else if (strcmp(key_str, "Ab") == 0) {
        sout << "*A-:";
    } else if (strcmp(key_str, "f#") == 0) {
        sout << "*f#:";
    } else if (strcmp(key_str, "B") == 0) {
        sout << "*B:";
    } else if (strcmp(key_str, "c#") == 0) {
        sout << "*c#:";
    } else if (strcmp(key_str, "Bb") == 0) {
        sout << "*B-:";
    } else if (strcmp(key_str, "Eb") == 0) {
        sout << "*E-:";
    } else if (strcmp(key_str, "D>") == 0) {
        sout << "*D-:";
    } else if (strcmp(key_str, "F#") == 0) {
        sout << "*F#:";
    } else if (strcmp(key_str, "eb") == 0) {
        sout << "*e-:";
    } else if (strcmp(key_str, "bb") == 0) {
        sout << "*b-:";
    } else if (strcmp(key_str, "g#") == 0) {
        sout << "*g#:";
    } else if (strcmp(key_str, "G#") == 0) {
        sout << "*G#:";
    } else if (strcmp(key_str, "Gb") == 0) {
        sout << "*G-:";
    }  else {
        sout << "!! unknown key";
        std::cout << "Warning: unknown key: " << key_str << std::endl;
    }
    
    *output = sout.str();
    
}




//////////////////////////////
//
// getPitch --
//

int MusPaeInput::getPitch( char c_note ) {
    int pitch;
    
    switch (c_note) {
        case 'A':
            pitch = PITCH_A;
            break;
        case 'B': 
            pitch = PITCH_B;
            break;
        case 'C': 
            pitch = PITCH_C;
            break;
        case 'D': 
            pitch = PITCH_D;
            break;
        case 'E': 
            pitch = PITCH_E;
            break;
        case 'F': 
            pitch = PITCH_F;
            break;
        case 'G': 
            pitch = PITCH_G;
            break;
#warning "What is this?"
        //case '-': pitch = -1000; break;
        default:
            break;
    }
    return pitch;
}



//////////////////////////////
//
// getDurationWithDot -- return the duration note given the note duration and the dot
//

double MusPaeInput::getDurationWithDot(double duration, int dot) {
    
    double duration_with_dot = duration;
    int i;
    for (i = 0; i < dot; i++) {
        duration_with_dot = duration_with_dot + duration * pow(2.0, -(i+1));
    }
    return duration_with_dot;
}



//////////////////////////////
//
// getMeasureDur -- return the duration of the full measure
//

double MusPaeInput::getMeasureDur(Array<double>& timeinfo) {
    return timeinfo[0] * timeinfo[1];
}



//////////////////////////////
//
// getTimeInfo -- read the key signature.
//

int MusPaeInput::getTimeInfo( const char* incipit, Array<double>& timeinfo, string *output, int index) {
    
    int i = index;
    int length = strlen(incipit);
    
    if (!isdigit(incipit[i]) && (incipit[i] != 'c') && (incipit[i] != 'o'))
        return 0;
    
    // find the end of time signature end
    i++; // the time signature length is a least 1
    while (i < length) {
        if (!isdigit(incipit[i]) && (incipit[i] != '/') && (incipit[i] != '.')) {
            break;
        }
        i++;
    }
    
    timeinfo.setAll(0);
    
    // use a substring for the time signature 
    char timesig_str[1024];
    memset( timesig_str, 0, 1024 );
    // strncpy not always put the \0 in the end!
    strncpy( timesig_str, incipit + index, i - index); 
    
    std::ostringstream sout;
    regex_t re;
    
    // check if format X/X or one digit only
    regcomp(&re, "^[0-9]*/[0-9]*$", REG_EXTENDED);
    int is_standard = regexec(&re, timesig_str, 0, NULL, 0);
    regfree(&re);
    regcomp(&re, "^[0-9]*$", REG_EXTENDED);
    int is_one_number = regexec(&re, timesig_str, 0, NULL, 0);
    regfree(&re);
    
    if ( is_standard == 0) {
        char buf_str[1024];
        strcpy(buf_str, timesig_str);
        int beats = atoi(strtok(buf_str, "/"));
        int note_value = atoi(strtok(NULL, "/")); 
        timeinfo[0] = (double)beats; timeinfo[1] = 4.0/(double)note_value;
        sout << "*M" << beats << "/" << note_value;
        //std::cout << output << std::endl;
    } else if ( is_one_number == 0) {
        int beats = atoi(timesig_str);
        timeinfo[0] = (double)beats; timeinfo[1] = 4.0/1.0;
        sout << "*M" << beats << "/1\n*met(" << beats << ")";
        //std::cout << output << std::endl;
    } else if (strcmp(timesig_str, "c") == 0) {
        // C
        timeinfo[0] = 4.0; timeinfo[1] = 4.0/4.0;
        sout << "*M4/4\n*met(C)"; 
    } else if (strcmp(timesig_str, "c/") == 0) {
        // C|
        timeinfo[0] = 2.0; timeinfo[1] = 4.0/2.0;
        sout << "*M2/2\n*met(C|)";
    } else if (strcmp(timesig_str, "c3") == 0) {
        // C3
        timeinfo[0] = 3.0; timeinfo[1] = 4.0/1.0;
        sout << "*M3/1\n*met(c3)";
    } else if (strcmp(timesig_str, "c3/2") == 0) {
        // C3/2
        timeinfo[0] = 3.0; timeinfo[1] = 4.0/2.0;
        sout << "*M3/2\n*met(c3/2)";
    } else {
        timeinfo[0] = 4.0; timeinfo[1] = 4.0/4.0;
        sout << "*M4/4\n!! unknown time signature";
        std::cout << "Warning: unknown time signature: " << timesig_str << std::endl;
    }
    *output = sout.str();
    return i - index;
}



//////////////////////////////
//
// getClefInfo -- read the key signature.
//

int MusPaeInput::getClefInfo( const char *incipit, string *output, int index ) {
    
    // a clef is maximum 3 character length
    // go through the 3 character and retrieve the letter (clef) and the line
    // mensural clef (with + in between) currently ignored
    // clef with octava correct?
    int length = strlen(incipit);
    int i = 0;
    char clef = 'G';
    char line = '2';
    while ((index < length) && (i < 3)) {
        if (i == 0) {
            clef = incipit[index];
        } else if (i == 2) {
            line = incipit[index];
        }
        i++;
        index++;
    }
    
    std::ostringstream sout;   
    sout << "*clef" << clef << line;
    *output = sout.str();
    //std::cout << output << std::endl;
    return i;
}



//////////////////////////////
//
// getWholeRest -- read the getWholeRest.
//

int MusPaeInput::getWholeRest( const char *incipit, int *wholerest, int index ) {
    
    int length = strlen(incipit);
    int i = index;
    
    *wholerest = 1;
    if ((i+1 < length) && isdigit(incipit[i+1])) {
        sscanf(&(incipit[i+1]), "%d", wholerest);
        char buf[10];
        memset( buf, 0, 10 );
        sprintf( buf, "%d", *wholerest );
        i += strlen( buf );
    }
    return i - index;
}



//////////////////////////////
//
// getBarline -- read the barline.
//

int MusPaeInput::getBarline( const char *incipit, string *output, int index ) {
    
    regex_t re;
    regcomp(&re, "^://:", REG_EXTENDED);
    int is_rep_db_rep = regexec(&re, incipit + index, 0, NULL, 0);
    regfree(&re);
    regcomp(&re, "^://", REG_EXTENDED);
    int is_rep_db = regexec(&re, incipit + index, 0, NULL, 0);
    regfree(&re);
    regcomp(&re, "^//:", REG_EXTENDED);
    int is_db_rep = regexec(&re, incipit + index, 0, NULL, 0);
    regfree(&re);
    regcomp(&re, "^//", REG_EXTENDED);
    int is_db = regexec(&re, incipit + index, 0, NULL, 0);
    regfree(&re);
    
    int i = 0; // number of characters
    if (is_rep_db_rep == 0) {
        *output = "=:||:";
        i = 3;
    } else if (is_rep_db == 0) {
        *output = "=:|!";
        i = 2;
    } else if (is_db_rep == 0) {
        *output = "=!|:";
        i = 2;
    } else if (is_db == 0) {
        *output = "=||";
        i = 1;
    } else {
        *output = "=";
        i = 0;
    }
    return i;
}



//////////////////////////////
//
// getAbbreviation -- read abbreviation
//

int MusPaeInput::getAbbreviation(const char* incipit, MeasureObject *measure, int index ) {
    
    int length = strlen(incipit);
    int i = index;
    int j;
    
    if (measure->abbreviation_offset == -1) { // start
        measure->abbreviation_offset = measure->notes.getSize();
    } else { //
        int abbreviation_stop = measure->notes.getSize();
        while ((i+1 < length) && (incipit[i+1]=='f')) {
            i++;
            for(j=measure->abbreviation_offset; j<abbreviation_stop; j++) {
                measure->notes.append( measure->notes[j] );
            }
        }
        measure->abbreviation_offset = -1;   
    }
    
    return i - index;
}



//////////////////////////////
//
// getKeyInfo -- read the key signature.
//

int MusPaeInput::getKeyInfo(const char *incipit, Array<int>& key, string *output, int index ) {
    
    //const char* keyline = getField("112D", hfile, incipitnum);
    
    key.setAll(0);
    
    // at the key information line, extract data
    int type  = 1;  		 // 1 = sharps, -1 = flats
    int paren = 1;  		 // 1 = normal, 2 = inside of square brackets
    // currently ignored. Don't know what it means
    int length = strlen(incipit);
    int i = index;
    bool end_of_keysig = false;
    while ((i < length) && (!end_of_keysig)) {
        switch (incipit[i]) {
            case 'b': type  = -1; break;
            case 'x': type  =  1; break;
            case '[': paren =  1; break; // change to 2 to enable paren. Meaning is unclear, though
            case ']': paren =  1; break;
            case 'F': key[0] = type * paren; break;
            case 'C': key[1] = type * paren; break;
            case 'G': key[2] = type * paren; break;
            case 'D': key[3] = type * paren; break;
            case 'A': key[4] = type * paren; break;
            case 'E': key[5] = type * paren; break;
            case 'B': key[6] = type * paren; break;
            default:
                end_of_keysig = true;
                //if (debugQ) {
                //  std::cout << "Warning: unknown character: " << keysig_str[i] << " in key signature " << keysig_str
                //       << std::endl;
                //exit(1);
                //}
                break;
        }
        if (!end_of_keysig)
            i++;
    }
    
    std::ostringstream sout;
    sout << "*k[";
    if (key[0] > 0) {
        if (key[0] > 0) sout << "f#"; else if (key[0] < 0) sout << "f-";
        if (key[1] > 0) sout << "c#"; else if (key[0] < 0) sout << "c-";
        if (key[2] > 0) sout << "g#"; else if (key[0] < 0) sout << "g-";
        if (key[3] > 0) sout << "d#"; else if (key[0] < 0) sout << "d-";
        if (key[4] > 0) sout << "a#"; else if (key[0] < 0) sout << "a-";
        if (key[5] > 0) sout << "e#"; else if (key[0] < 0) sout << "e-";
        if (key[6] > 0) sout << "b#"; else if (key[0] < 0) sout << "b-";
    } else {
        if (key[6] < 0) sout << "b-"; else if (key[0] > 0) sout << "b#";
        if (key[5] < 0) sout << "e-"; else if (key[0] > 0) sout << "e#";
        if (key[4] < 0) sout << "a-"; else if (key[0] > 0) sout << "a#";
        if (key[3] < 0) sout << "d-"; else if (key[0] > 0) sout << "d#";
        if (key[2] < 0) sout << "g-"; else if (key[0] > 0) sout << "g#";
        if (key[1] < 0) sout << "c-"; else if (key[0] > 0) sout << "c#";
        if (key[0] < 0) sout << "f-"; else if (key[0] > 0) sout << "g#";
    }
    sout << "]";
    *output = sout.str();
    
    return i - index;
}



//////////////////////////////
//
// getNote --
//

int MusPaeInput::getNote( const char* incipit, NoteObject *note, MeasureObject *measure, int index ) {
    
    regex_t re;
    
    int i = index;
    
    note->mnote->m_dur = measure->durations[measure->durations_offset];
#warning "Removed dot and turplet info"
    //note->dot = measure->dots[measure->durations_offset];
    //if ( note->tuplet != 1.0 ) {
    //    note->duration /= note->tuplet;
        //std::cout << durations[0] << ":" << note->tuplet << std::endl;
    //}
    note->mnote->m_pname = getPitch( incipit[i] );
    
    // beaming
    // detect if it is a fermata or a tuplet
    if (note->beam > 0) {
        regcomp(&re, "^[^}/]*[ABCDEFG-].*", REG_EXTENDED);
        int is_not_last_note = regexec(&re, incipit + i + 1, 0, NULL, 0);
        regfree(&re);
        //std::cout << "regexp " << is_not_last_note << std::endl;
        if ( is_not_last_note != 0 ) {
            note->beam = -1; // close the beam
        }
    }
    
    // trills
    regcomp(&re, "^[^ABCDEFG]*t", REG_EXTENDED);
    int has_trill = regexec(&re, incipit + i + 1, 0, NULL, 0);
    regfree(&re);
    if ( has_trill == 0 ) {
        note->trill = true;
    }
    
    // tie
    regcomp(&re, "^[^ABCDEFG]*\\+", REG_EXTENDED);
    int has_tie = regexec(&re, incipit + i + 1, 0, NULL, 0);
    regfree(&re);
    //std::cout << "regexp " << has_tie << std::endl;
    if ( has_tie == 0 ) {
        note->tie = 1; // 1 for the first note, 2 for the second one
    }
    
    measure->notes.append( *note );
    
    // reset values
    // beam
    if (note->beam > 0) {
        note->beam++;
    } else if (note->beam == -1) {
        note->beam = 0;
    }
    // tie
    if (note->tie == 1) {
        note->tie++;
    } else if (note->tie == 2) {
        note->tie = 0;
    }
    // grace notes
    note->acciaccatura = false;
    if (note->appoggiatura > 0) {
        //std::cout << note->appoggiatura << std::endl; 
        note->appoggiatura--;
        note->appoggiatura_multiple = false;
    }
    // durations
    if (measure->durations.getSize()) {
        measure->durations_offset++;
        if (measure->durations_offset >= measure->durations.getSize()) {
            measure->durations_offset = 0;
        }
    }
    
    note->fermata = false; // only one note per fermata;
    note->trill = false;
    
    return i - index;
}



//////////////////////////////
//
// printMeasure --
//

void MusPaeInput::printMeasure(std::ostream& out, MeasureObject *measure ) {
    m_measure =  new MusMeasure;
    m_staff = new MusStaff();
    m_layer = new MusLayer();
    
    if ( measure->clef.length() ) {
        out << measure->clef << "\n";
    }
    if ( measure->s_key.length() ) {
        out << measure->s_key << "\n";
    }   
    if ( measure->s_timeinfo.length() ) {
        out << measure->s_timeinfo << "\n";
    }
    

    char buffer[1024] = {0};
    int i,j;

    
    // RZ ignore whole rest forn now
    if ( measure->wholerest > 0 ) {
        //out << Convert::durationToKernRhythm(buffer, measure->measure_duration);
        out << "rr\n";
        for (i=1; i<measure->wholerest; i++) {
            out << "=\n";
            //out << Convert::durationToKernRhythm(buffer, measure->measure_duration);
            out << "rr\n";
        }
        
        //if (measure->notes[i].fermata) {
        //      out << ";";
        //}
    }
    
    for (i=0; i<measure->notes.getSize(); i++) {
        if (measure->notes[i].mnote->m_pname == 111110) {
            //if (measure->notes[i].tie == 1) {
            //    out << "[";
            //}
    
            //out << Convert::durationToKernRhythm(buffer, measure->notes[i].duration);
            //for(j=0; j<measure->notes[i].dot; j++) {
            //    out << ".";
            //}
            //out << "r";
            
            if (measure->notes[i].fermata) {
                out << ";";
            } 
            
            if (measure->notes[i].beam == 1) {
                out << "L";
            } else if (measure->notes[i].beam == -1) {
                out << "J";
            }
            
            if (measure->notes[i].tie == 2) {
                out << "]";
                //} else if (measure->notes[i].tie == 3) {
                //   out << "_";
            }
        } else {
            if (measure->notes[i].tie == 1) {
                out << "[";
            }
            if (!measure->notes[i].acciaccatura) {
                //out << Convert::durationToKernRhythm(buffer, measure->notes[i].duration);
                //for(j=0; j<measure->notes[i].dot; j++) {
                //    out << ".";
                //}
            }
            //This does the trick
            //out << Convert::base40ToKern(buffer, measure->notes[i].pitch);
            
            
            m_layer->AddLayerElement(measure->notes[i].mnote);
            
            if (measure->notes[i].acciaccatura) {
                out << "q";
            } else if (measure->notes[i].appoggiatura > 0) {
                out << "Q";
            }
            
            if (measure->notes[i].fermata) {
                out << ";";
            } 
            
            if (measure->notes[i].tie == 2) {
                out << "]";
                //} else if (measure->notes[i].tie == 3) {
                //   out << "_";
            }
            
            if (measure->notes[i].beam == 1) {
                out << "L";
            } else if (measure->notes[i].beam == -1) {
                out << "J";
            }
            
            if (measure->notes[i].trill == true) {
                out << "t";
            }
        }
        out << "\n";
    }
    if ( measure->barline.length() ) {
        out << measure->barline << "\n";
    }
    
    m_staff->AddStaffElement(m_layer);
    m_measure->AddMeasureElement(m_staff);
}



//////////////////////////////
//
// getAtRecordKeyValue --
//   Formats: @key: value
//            @key:value
//            @key :value
//            @ key :value
//            @ key : value
//   only one per line
//

void MusPaeInput::getAtRecordKeyValue(Array<char>& key, Array<char>& value,
                                      const char* input) {
    
#define SKIPSPACE while((index<length)&&isspace(input[index])){index++;}
    
    char MARKER    = '@';
    char SEPARATOR = ':';
    char EMPTY     = '\0';
    
    int length = strlen(input);
    
    
    if (length == 0) {
        key.setSize(1);
        value.setSize(1);
        key[0] = EMPTY;
        value[0] = EMPTY;
        return;
    }
    
    // pre-allocate storage space in character arrays to avoid
    // additional allocation will appending data:
    key.setSize(length);
    value.setSize(length);
    key[0] = EMPTY;
    value[0] = EMPTY;
    key.setSize(0);
    value.setSize(0);
    
    char ch;
    int index = 0;
    
    // find starting @ symbol (ignoring any starting space)
    SKIPSPACE
    if (input[index] != MARKER) {
        // invalid record format since it does not start with @
        return;
    }
    index++;
    SKIPSPACE
    
    // start storing the key value:
    while ((index < length) && (input[index] != SEPARATOR)) {
        if (isspace(input[index])) {
            continue;
            index++;
        }
        ch = input[index]; key.append(ch);
        index++;
    }
    // check to see if valid format: (:) must be the current character
    if (input[index] != SEPARATOR) {
        key[0] = EMPTY;
        key.setSize(0);
        return;
    }
    key.append(EMPTY); // terminate the string for regular C use of char*
    index++;
    SKIPSPACE
    value.setSize(length - index + 1);
    strcpy(value.getBase(), &input[index]);
    int i;
    for (i=value.getSize()-2; i>0; i--) {
        if (isspace(value[i])) {
            value[i] = EMPTY;
            value.setSize(value.getSize()-1);
            continue;
        }
        break;
    }
}



//////////////////////////////
//
// getLineIndex -- get the index location of the given string.
//

int MusPaeInput::getLineIndex(Array<ArrayChar>& pieces, const char* string) {
    int index = -1;
    int i;
    for (i=0; i<pieces.getSize(); i++) {
        if (strstr(pieces[i].getBase(), string) != NULL) {
            index = i;
            break;
        }
    }
    return index;
}



// md5sum: 00d5e9dedeb47c815390eac97f8c9f42 pae2kern.cpp [20050403]

