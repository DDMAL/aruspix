/////////////////////////////////////////////////////////////////////////////
// Name:        musiopae.h
// Author:      Rodolfo Zitellini
// Created:     2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __MUS_IOPAE_H__
#define __MUS_IOPAE_H__

//#include <iostream>
//#include <fstream>
//#include <sstream>

#include <string>
#include <vector>

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
#include "musbeam.h"
#include "musslur.h"
#include "mustie.h"
#include "mustuplet.h"

class NoteObject {
public:
    NoteObject(const NoteObject &old) { // for STL vector
        //mnote = old.mnote;
        //mrest = old.mrest;         
        tuplet = old.tuplet;
        tie = old.tie;
        acciaccatura = old.acciaccatura;
        appoggiatura = old.appoggiatura;
        appoggiatura_multiple = old.appoggiatura_multiple;
        fermata = old.fermata;
        trill = old.trill;
        
        octave = old.octave;
        beam = old.beam;
        pitch = old.pitch;
        duration = old.duration;
        accidental = old.accidental;
        dots = old.dots;
        rest = old.rest;
    }
    NoteObject(void) { clear(); };
    void   clear(void) {
        appoggiatura = 0;
        acciaccatura = appoggiatura_multiple = fermata = trill = false;
        tuplet = 1.0; // no tuplet
        tie = 0;
        
        octave = 4;
        beam = 0;
        pitch = 0;
        duration = 0;
        accidental = 0;
        dots = 0;
        rest = false;
        
    };
    
    NoteObject& operator=(const NoteObject& d){ // for STL vector
        //mnote = d.mnote;
        //mrest = d.mrest;         
        tuplet = d.tuplet;
        tie = d.tie;
        acciaccatura = d.acciaccatura;
        appoggiatura = d.appoggiatura;
        appoggiatura_multiple = d.appoggiatura_multiple;
        fermata = d.fermata;
        trill = d.trill;
        
        octave = d.octave;
        beam = d.beam;
        pitch = d.pitch;
        duration = d.duration;
        accidental = d.accidental;
        dots = d.dots;
        rest = d.rest;
        
        return *this;
    }
    
    //MusNote *mnote;
    //MusRest *mrest; // this is not too nice
    
    double tuplet;
    int    tie;
    bool   acciaccatura;
    int    appoggiatura;
    bool   appoggiatura_multiple;
    bool   fermata;
    bool   trill;
    
    unsigned char octave;
    unsigned char beam;
    unsigned char pitch;
    unsigned char duration;
    unsigned char accidental;
    unsigned int dots;
    bool rest;
};


class MeasureObject {
public:
    
    MeasureObject(const MeasureObject& d){ // for STL vector
        clef = d.clef;
        time = d.time;
        notes = d.notes;
        
        key = d.key;
        key_alteration = d.key_alteration;
        
        durations = d.durations;
        dots = d.dots; 
        durations_offset = d.durations_offset;
        barline = d.barline;
        abbreviation_offset = d.abbreviation_offset;  
        wholerest = d.wholerest;
    } 
    MeasureObject(void) { clear(); };
    
    MeasureObject& operator=(const MeasureObject& d){ // for STL vector
        clef = d.clef;
        time = d.time;
        notes = d.notes;
        
        key = d.key;
        key_alteration = d.key_alteration;
        
        durations = d.durations;
        dots = d.dots; 
        durations_offset = d.durations_offset;
        barline = d.barline;
        abbreviation_offset = d.abbreviation_offset;  
        wholerest = d.wholerest;   
        return *this;
    } 
     
    void   clear(void) {
        durations.clear();
        dots.clear();
        notes.clear();
        durations_offset = 0;
        reset();
    };
    void   reset(void) {
        clef = NULL;
        time = NULL;
        key.clear();
        notes.clear();
        barline = "";
        wholerest = 0; 
        abbreviation_offset = -1;
    };
    MusClef *clef;
    MusMensur *time;
    vector<NoteObject> notes;

    vector<int> key;
    char key_alteration;
    
    vector<int> durations;
    vector<int> dots; // use the same offset as durations, they are used in parallel
    unsigned int durations_offset;
    std::string barline;
    int    abbreviation_offset;  
    int    wholerest;   // number of whole rests to process
};


//////////////////////////////////////////////////////////////////////////









//----------------------------------------------------------------------------
// MusPaeInput
//----------------------------------------------------------------------------


class MusPaeInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusPaeInput( MusDoc *doc, wxString filename );
    virtual ~MusPaeInput();
    
    bool ImportFile( );    
    
private:
    // function declarations:
    
     void      convertPlainAndEasyToKern( std::istream &infile, std::ostream &outfile);
     
     // parsing functions
     int       getKeyInfo          (const char* incipit, MeasureObject *measure, int index = 0);
     int       getTimeInfo         (const char* incipit, MeasureObject *measure, int index = 0);
     int       getClefInfo         (const char* incipit, MeasureObject *measure, int index = 0 );
     int       getBarline          (const char* incipit, std::string *output, int index = 0 );
     int       getAccidental       (const char* incipit, unsigned char *accident, int index = 0);
     int       getOctave           (const char* incipit, unsigned char *octave, int index = 0 );
     int       getDurations        (const char* incipit, MeasureObject *measure, int index = 0);
     int       getDuration         (const char* incipit, int *duration, int *dot, int index );
     int       getTupletFermata    (const char* incipit, double current_duration, NoteObject *note, int index = 0);
     int       getTupletFermataEnd (const char* incipit, NoteObject *note, int index = 0);
     int       getGraceNote        (const char* incipit, NoteObject *note, int index = 0);
     int       getWholeRest        (const char* incipit, int *wholerest, int index );
     int       getAbbreviation     (const char* incipit, MeasureObject *measure, int index = 0 ); 
     int       getNote             (const char* incipit, NoteObject *note, MeasureObject *measure, int index = 0 );
     
     int       getPitch            (char c_note );
     double    getDurationWithDot  (double duration, int dot);
     
     // output functions
     void      printMeasure        (std::ostream& out, MeasureObject *measure);
     
     // input functions
     void      getAtRecordKeyValue (char *key, char* value, const char* input);
     
    
public:
    
private:
    wxString m_filename;
	MusDiv *m_div;
	MusScore *m_score;
	MusPartSet *m_parts;
	MusPart *m_part;
	MusSection *m_section;
	MusMeasure *m_measure;
	MusStaff *m_staff;
	MusLayer *m_layer;
    MusTie *m_current_tie;
    MusTuplet *m_current_tuplet;
    
    //unsigned char m_rest_position;
    //unsigned int m_rest_octave;
};

#endif
