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


//////////////////////////////////////////////////////////////////////////
template<class type>
class SigCollection {
public:
    SigCollection     (void);
    SigCollection     (int arraySize);
    SigCollection     (int arraySize, type *aCollection);
    SigCollection     (SigCollection<type>& aCollection);
    ~SigCollection     ();
    
    void      allowGrowth       (int status = 1);
    void      append            (type& element);
    void      appendcopy        (type element);
    void      append            (type* element);
    type     *getBase           (void) const;
    long      getAllocSize      (void) const;
    long      getSize           (void) const;
    type     *pointer           (void);
    void      setAllocSize      (long aSize);
    void      setGrowth         (long growth);
    long      getGrowth         (void) { return growthAmount; }
    void      setSize           (long newSize);
    type&     operator[]        (int arrayIndex);
    type      operator[]        (int arrayIndex) const;
    void      grow              (long growamt = -1);
    type&     last              (int index = 0);
    int       increase          (int addcount = 1);
    
    
protected:
    long      size;             // actual array size
    long      allocSize;        // maximum allowable array size
    type     *array;            // where the array data is stored
    char      allowGrowthQ;     // allow/disallow growth
    long      growthAmount;     // number of elements to grow by if index
    //    element one beyond max size is accessed
    long maxSize;               // the largest size the array is allowed 
    //    to grow to, if 0, then ignore max
    
    void      shrinkTo          (long aSize);
};

//////////////////////////////
//
// SigCollection::SigCollection --
//

template<class type>
SigCollection<type>::SigCollection(void) {
    this->allocSize = 0;
    this->size = 0;
    this->array = NULL;
    this->allowGrowthQ = 0;
    this->growthAmount = 8;
    this->maxSize = 0;
}

template<class type>
SigCollection<type>::SigCollection(int arraySize) {
    this->array = new type[arraySize];
    
    this->size = arraySize;
    this->allocSize = arraySize;
    this->allowGrowthQ = 0;
    this->growthAmount = arraySize;
    this->maxSize = 0;
}


template<class type>
SigCollection<type>::SigCollection(int arraySize, type *aSigCollection) {
    this->size = arraySize;
    this->allocSize = arraySize;
    this->array = new type[size];
    for (int i=0; i<size; i++) {
        this->array[i] = aSigCollection[i];
    }
    this->growthAmount = arraySize;
    this->allowGrowthQ = 0;
    this->maxSize = 0;
}


template<class type>
SigCollection<type>::SigCollection(SigCollection<type>& aSigCollection) {
    this->size = aSigCollection.size;
    this->allocSize = size;
    this->array = new type[size];
    for (int i=0; i<size; i++) {
        this->array[i] = aSigCollection.array[i];
    }
    this->allowGrowthQ = aSigCollection.allowGrowthQ;
    this->growthAmount = aSigCollection.growthAmount;
    this->maxSize = aSigCollection.maxSize;
}



//////////////////////////////
//
// SigCollection::~SigCollection --
//

template<class type>
SigCollection<type>::~SigCollection() {
    if (this->getAllocSize() != 0) {
        delete [] this->array;
    }
}



//////////////////////////////
//
// SigCollection::allowGrowth --
//	default value: status = 1 
//

template<class type>
void SigCollection<type>::allowGrowth(int status) {
    if (status == 0) {
        this->allowGrowthQ = 0;
    } else {
        this->allowGrowthQ = 1;
    }
}



//////////////////////////////
//
// SigCollection::append --
//

template<class type>
void SigCollection<type>::append(type& element) {
    if (this->size == this->getAllocSize()) {
        this->grow();
    }
    this->array[size] = element;
    this->size++;
}

template<class type>
void SigCollection<type>::appendcopy(type element) {
    if (this->size == this->getAllocSize()) {
        this->grow();
    }
    this->array[size] = element;
    this->size++;
}

template<class type>
void SigCollection<type>::append(type *element) {
    if (this->size == this->getAllocSize()) {
        this->grow();
    }
    this->array[size] = *element;
    this->size++;
}



//////////////////////////////
//
// SigCollection::grow --
// 	default parameter: growamt = -1
//

template<class type>
void SigCollection<type>::grow(long growamt) {
    this->allocSize += growamt > 0 ? growamt : this->growthAmount;
    if (this->maxSize != 0 && this->getAllocSize() > this->maxSize) {
        //std::cerr << "Error: Maximum size allowed for array exceeded." << std::endl;
        exit(1);
    }
    
    type *temp = new type[this->getAllocSize()];
    for (int i=0; i<size; i++) {
        temp[i] = this->array[i];
    }
    this->array = temp;
}



//////////////////////////////
//
// SigCollection::pointer --
//

template<class type>
type* SigCollection<type>::pointer(void) {
    return this->array;
}



//////////////////////////////
//
// SigCollection::getBase --
//

template<class type>
type* SigCollection<type>::getBase(void) const {
    return this->array;
}



//////////////////////////////
//
// SigCollection::getAllocSize --
//

template<class type>
long SigCollection<type>::getAllocSize(void) const {
    return this->allocSize;
}



//////////////////////////////
//
// SigCollection::getSize --
//

template<class type>
long SigCollection<type>::getSize(void) const {
    return this->size;
}



//////////////////////////////
//
// SigCollection::last --
//      default value: index = 0
//

template<class type>
type& SigCollection<type>::last(int index) {
    return this->array[getSize()-1-abs(index)];
}



//////////////////////////////
//
// SigCollection::setAllocSize --
//

template<class type>
void SigCollection<type>::setAllocSize(long aSize) {
    if (aSize < this->getSize()) {
        //std::cerr << "Error: cannot set allocated size smaller than actual size." 
        //<< std::endl;
        exit(1);
    }
    
    if (aSize <= this->getAllocSize()) {
        this->shrinkTo(aSize);
    } else {
        this->grow(aSize-this->getAllocSize());
        this->size = aSize;
    }
}



//////////////////////////////
//
// SigCollection::setGrowth --
//	default parameter: growth = -1
//

template<class type>
void SigCollection<type>::setGrowth(long growth) {
    if (growth > 0) {
        this->growthAmount = growth;
    }
}



//////////////////////////////
//
// SigCollection::setSize --
//

template<class type>
void SigCollection<type>::setSize(long newSize) {
    if (newSize <= this->getAllocSize()) { 
        this->size = newSize;
    } else {
        this->grow(newSize-this->getAllocSize());
        this->size = newSize;
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// SigCollection operators
//

//////////////////////////////
//
// SigCollection::operator[] --
//

template<class type>
type& SigCollection<type>::operator[](int elementIndex) {
    if (this->allowGrowthQ && elementIndex == this->size) {
        if (this->size == this->getAllocSize()) {
            this->grow();
        }
        this->size++;
    } else if ((elementIndex >= this->size) || (elementIndex < 0)) {
        //std::cerr << "Error: accessing invalid array location " 
        //<< elementIndex 
        //<< " Maximum is " << this->size-1 << std::endl;
        exit(1);
    }
    return this->array[elementIndex];
}


//////////////////////////////
//
// SigCollection::operator[] const --
//

template<class type>
type SigCollection<type>::operator[](int elementIndex) const {
    if ((elementIndex >= this->size) || (elementIndex < 0)) {
        //std::cerr << "Error: accessing invalid array location " 
        //<< elementIndex 
        //<< " Maximum is " << this->size-1 << std::endl;
        exit(1);
    }
    return this->array[elementIndex];
}



//////////////////////////////
//
// SigCollection::shrinkTo --
//

template<class type>
void SigCollection<type>::shrinkTo(long aSize) {
    if (aSize < this->getSize()) {
        exit(1);
    }
    
    type *temp = new type[aSize];
    for (int i=0; i<this->size; i++) {
        temp[i] = this->array[i];
    }
    delete [] this->array;
    this->array = temp;
    
    allocSize = aSize;
    if (size > allocSize) {
        size = allocSize;
    }
}


//////////////////////////////
//
// SigCollection::increase -- equivalent to setSize(getSize()+addcount)
//

template<class type>
int SigCollection<type>::increase(int addcount) {
    if (addcount > 0) {
        this->setSize(this->getSize() + addcount);
    }
    return this->getSize();
}

//////////////////////////////////////////////////////////////////////////
template<class type>
class Array : public SigCollection<type> {
public:
    Array             (void);
    Array             (int arraySize);
    Array             (Array<type>& aArray);
    Array             (int arraySize, type *anArray);
    ~Array             ();
    
    void           setAll            (type aValue);
    type           sum               (void);
    type           sum               (int lowIndex, int hiIndex);
    void           zero              (int minIndex = -1, int maxIndex = -1);
    
    int            operator==        (const Array<type>& aArray);
    Array<type>&   operator=         (const Array<type>& aArray);
    Array<type>&   operator+=        (const Array<type>& aArray);
    Array<type>&   operator-=        (const Array<type>& aArray);
    Array<type>&   operator*=        (const Array<type>& aArray);
    Array<type>&   operator/=        (const Array<type>& aArray);
    
    Array<type>    operator+         (const Array<type>& aArray) const;
    Array<type>    operator+         (type aNumber) const;
    Array<type>    operator-         (const Array<type>& aArray) const;
    Array<type>    operator-         (void) const;
    
    Array<type>    operator-         (type aNumber) const;
    Array<type>    operator*         (const Array<type>& aArray) const;
    Array<type>    operator*         (type aNumber) const;
    Array<type>    operator/         (const Array<type>& aArray) const;
};


// special function for printing Array<char> values:
std::ostream& operator<<(std::ostream& out, Array<char>& astring);

//////////////////////////////
//
// Array::Array 
//

template<class type>
Array<type>::Array(void) : SigCollection<type>(4) { 
}

template<class type>
Array<type>::Array(int arraySize) : SigCollection<type>(arraySize) { 
}

template<class type>
Array<type>::Array(Array<type>& anArray) : SigCollection<type>(anArray) { 
}

template<class type>
Array<type>::Array(int arraySize, type *anArray) : 
SigCollection<type>(arraySize, anArray) { 
}




//////////////////////////////
//
// Array::~Array
//

template<class type>
Array<type>::~Array() { 
} 



//////////////////////////////
//
// Array::setAll -- sets the contents of each element to the 
//   specified value
//

template<class type>
void Array<type>::setAll(type aValue) {
    for (int i=0; i<this->getSize(); i++) {
        this->array[i] = aValue;
    }
}



//////////////////////////////
//
// Array::sum
//

template<class type>
type Array<type>::sum(void) {
    type theSum = 0;
    for (int i=0; i<this->getSize(); i++) {
        theSum += this->array[i];
    }
    return theSum;
}

template<class type>
type Array<type>::sum(int loIndex, int hiIndex) {
    type theSum = 0;
    for (int i=loIndex; i<=hiIndex; i++) {
        theSum += this->array[i];
    }
    return theSum;
}



//////////////////////////////
//
// Array::zero -- (-1, -1)
//

template<class type>
void Array<type>::zero(int minIndex, int maxIndex) {
    if (this->size == 0) return;
    if (minIndex == -1) minIndex = 0;
    if (maxIndex == -1) maxIndex = this->size-1;
    
    if (minIndex < 0 || maxIndex < 0 || minIndex > maxIndex ||
        maxIndex >= this->size) {
        //cerr << "Error in zero function: min = " << minIndex
        //<< " max = " << maxIndex << " size = " << this->size << endl;
        exit(1);
    }
    
    for (int i=minIndex; i<=maxIndex; i++) {
        this->array[i] = 0;
    }
}


////////////////////////////////////////////////////////////////////////////
//
// operators
//


//////////////////////////////
//
// Array::operator== --
//

template<class type>
int Array<type>::operator==(const Array<type>& aArray) {
    if (this->getSize() != aArray.getSize()) {
        return 0;
    }
    Array<type>& t = *this;
    int i;
    for (i=0; i<this->getSize(); i++) {
        if (t[i] != aArray[i]) {
            return 0;
        }
    }
    return 1;
}



//////////////////////////////
//
// Array::operator= --
//

template<class type>
Array<type>& Array<type>::operator=(const Array<type>& anArray) {
    if (this == &anArray) {
        return *this;
    }
    if (this->allocSize < anArray.size) {
        if (this->array != NULL) {
            delete [] this->array;
            this->array = NULL;
        }
        this->allocSize = anArray.size;
        this->size = anArray.size;
        this->array = new type[this->size];
        this->allowGrowthQ = anArray.allowGrowthQ;
        this->growthAmount = anArray.growthAmount;
        this->maxSize = anArray.maxSize;
    }
    this->size = anArray.size;
    for (int i=0; i<this->size; i++) {
        this->array[i] = anArray.array[i];
    }
    
    return *this;
}



//////////////////////////////
//
// Array::operator+=
//

template<class type>
Array<type>& Array<type>::operator+=(const Array<type>& anArray) {
    if (this->size != anArray.size) {
        //cerr << "Error: different size arrays " << this->size << " and " 
        //<< anArray.size << endl;
        exit(1);
    }
    
    for (int i=0; i<this->size; i++) {
        this->array[i] += anArray.array[i];
    }
    
    return *this;
}



//////////////////////////////
//
// Array::operator+
//

template<class type>
Array<type> Array<type>::operator+(const Array<type>& anArray) const {
    if (this->size != anArray.size) {
        //cerr << "Error: different size arrays " << this->size << " and " 
        //<< anArray.size << endl;
        exit(1);
    }
    
    Array<type> bArray(*this);
    bArray += anArray;
    return bArray;
}


template<class type>
Array<type> Array<type>::operator+(type aNumber) const {
    Array<type> anArray(*this);
    for (int i=0; i<this->size; i++) {
        anArray[i] += aNumber;
    }
    return anArray;
}



//////////////////////////////
//
// Array::operator-=
//

template<class type>
Array<type>& Array<type>::operator-=(const Array<type>& anArray) {
    if (this->size != anArray.size) {
        //cerr << "Error: different size arrays " << this->size << " and " 
        //<< anArray.size << endl;
        exit(1);
    }
    
    for (int i=0; i<this->size; i++) {
        this->array[i] -= anArray.array[i];
    }
    
    return *this;
}



//////////////////////////////
//
// Array::operator-
//

template<class type>
Array<type> Array<type>::operator-(const Array<type>& anArray) const {
    if (this->size != anArray.size) {
        //cerr << "Error: different size arrays " << this->size << " and " 
        //<< anArray.size << endl;
        exit(1);
    }
    
    Array<type> bArray(*this);
    bArray -= anArray;
    return bArray;
}


template<class type>
Array<type> Array<type>::operator-(void) const {
    Array<type> anArray(*this);
    for (int i=0; i<this->size; i++) {
        anArray[i] = -anArray[i];
    }
    return anArray;
}

template<class type>
Array<type> Array<type>::operator-(type aNumber) const {
    Array<type> anArray(*this);
    for (int i=0; i<this->size; i++) {
        anArray[i] -= aNumber;
    }
    return anArray;
}



//////////////////////////////
//
// Array::operator*=
//

template<class type>
Array<type>& Array<type>::operator*=(const Array<type>& anArray) {
    if (this->size != anArray.size) {
        //cerr << "Error: different size arrays " << this->size << " and " 
        //<< anArray.size << endl;
        exit(1);
    }
    
    for (int i=0; i<this->size; i++) {
        this->array[i] *= anArray.array[i];
    }
    
    return *this;
}



//////////////////////////////
//
// Array::operator*
//

template<class type>
Array<type> Array<type>::operator*(const Array<type>& anArray) const {
    if (this->size != anArray.size) {
        //cerr << "Error: different size arrays " << this->size << " and " 
        //<< anArray.size << endl;
        exit(1);
    }
    
    Array<type> bArray(*this);
    bArray *= anArray;
    return bArray;
}


template<class type>
Array<type> Array<type>::operator*(type aNumber) const {
    Array<type> anArray(*this);
    for (int i=0; i<this->size; i++) {
        anArray[i] *= aNumber;
    }
    return anArray;
}



//////////////////////////////
//
// Array::operator/=
//

template<class type>
Array<type>& Array<type>::operator/=(const Array<type>& anArray) {
    if (this->size != anArray.size) {
        //cerr << "Error: different size arrays " << this->size << " and " 
        //<< anArray.size << endl;
        exit(1);
    }
    
    for (int i=0; i<this->size; i++) {
        this->array[i] /= anArray.array[i];
    }
    
    return *this;
}



//////////////////////////////
//
// Array::operator/
//

template<class type>
Array<type> Array<type>::operator/(const Array<type>& anArray) const {
    if (this->size != anArray.size) {
        //cerr << "Error: different size arrays " << this->size << " and " 
        //<< anArray.size << endl;
        exit(1);
    }
    
    Array<type> bArray(*this);
    bArray /= anArray;
    return bArray;
}
//////////////////////////////////////////////////////////////////////////

typedef Array<char> ArrayChar;

//////////////////////////////////////////////////////////////////////////

class NoteObject {
public:
    NoteObject(void) { clear(); };
    void   clear(void) {
        //pitch = octave = accident = dot = tie = 
        //beam = 
        appoggiatura = 0;
        acciaccatura = appoggiatura_multiple = fermata = trill = false;
        //duration = 0.0;
        tuplet = 1.0;
        mnote = new MusNote();
        mrest = NULL;
    };
    //int    pitch;
    //int    octave;
    //int    accident;
    //double duration;
    //int    dot;
    
    MusNote *mnote;
    MusRest *mrest; // this is not too nice
    
    double tuplet;
    int    tie;
    //int    beam; use the one in mnote
    bool   acciaccatura;
    int    appoggiatura;
    bool   appoggiatura_multiple;
    bool   fermata;
    bool   trill;
};


class MeasureObject {
public:
    MeasureObject(void) { clear(); };
    void   clear(void) {
        //a_timeinfo.setSize(2);
        //a_key.setSize(7);
        //a_timeinfo.setAll(0);
        //a_key.setAll(0);
        durations.setSize(1);
        dots.setSize(1);
        durations.setAll(1.0);
        dots.setAll(0);
        durations_offset = 0;
        reset();
    };
    void   reset(void) {
        notes.setSize(0);
        clef = NULL;
        time = NULL;
        key.clear();
        barline = "";
        wholerest = 0; 
        abbreviation_offset = -1;
    };
    MusClef *clef;
    MusMensur *time;
    //double measure_duration;
    Array<NoteObject> notes;
    //Array<int> a_key;
    //std::string s_key;
    
    //Array<double> a_timeinfo;   
    //std::string s_timeinfo;
    
    vector<int> key;
    char key_alteration;
    
    Array<int> durations;
    Array<int> dots; // use the same offset as durations, they are used in parallel
    int durations_offset;
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
     int       getOctave           (const char* incipit, char *octave, int index = 0 );
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
     void      getKey              (const char* key_str, std::string *output);
     int       getLineIndex        (Array<ArrayChar>& pieces, const char* string);
     
     // output functions
     void      printMeasure        (std::ostream& out, MeasureObject *measure);
     
     // input functions
     void      getAtRecordKeyValue (Array<char>& key, Array<char>& value, const char* input);
     
    
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
    
    unsigned char m_rest_position;
    unsigned int m_rest_octave;
};

#endif
