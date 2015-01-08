/////////////////////////////////////////////////////////////////////////////
// Name:        imoperator.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __imoperator_H__
#define __imoperator_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/ffile.h"
#include "wx/file.h"

#include "app/axprogressdlg.h"
//struct _imImage;

// IMLIB
#include <im.h>
#include <im_counter.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_util.h>
#include <im_binfile.h>
#include <im_math_op.h>
#include <im_palette.h>

#include "imext.h"

#define MAX_STAVES 24
#define STAVES_CONV_REDUCTION 2
#define STAFF_HEIGHT 180
#define STAFF 100
#define STAFF_INTERLIGN 13

#define IMAGE_MUSIC 0
#define IMAGE_BLANK 1
#define IMAGE_ORNATE_LETTER 2
#define IMAGE_TEXT_IN_STAFF 3
#define IMAGE_LYRICS 4
#define IMAGE_TITLE 5

#define IMAGE_PLANES IMAGE_TITLE

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif

class imSize;
class imPoint;

enum
{
	IM_BINARIZATION_OTSU = 0,
	IM_BINARIZATION_MINMAX,
	IM_BINARIZATION_BRINK,
	IM_BINARIZATION_BRINK3CLASSES
};

enum
{
	IM_PRUNE_CLEAR_HEIGHT = 0,
	IM_PRUNE_CLEAR_WIDTH,
	IM_PRUNE_CLEAR_MIN
};



//----------------------------------------------------------------------------
// ImOperator
//----------------------------------------------------------------------------

#define AX_PI 3.14159265358979323846

class ImOperator
{
public:
    // constructors and destructors
	ImOperator();
    virtual ~ImOperator();
    
    int GetError( ) { return m_error; }
	void SetProgressDlg( AxProgressDlg *dlg );
    void SetMapImage( _imImage *image );
    //wxString GetShortName() { return m_shortname; }
    
protected:
    void MedianFilter( int values[], int size, int filter_size, int *avg_ptr = NULL);
    void PruneElementsZone( _imImage *image, int min_threshold, int max_threshold, int type = IM_PRUNE_CLEAR_HEIGHT );
    void MoveElements( _imImage *src, _imImage *dest, int bounding_boxes[],
        int count, int margins[4], int factor = 1 );
    void DistByCorrelation( _imImage *image1, _imImage *image2,
                                imSize window, int *decalageX, int *decalageY, int *maxCorr );
    //void DistByCorrelationFFT(const _imImage *image1, const _imImage *image2,
    //                            imSize window, int *decalageX, int *decalageY);
    
	// Memory managment methods
    virtual bool Terminate( int code = 0, ... );
    bool GetImagePlane( _imImage **image , int plane = 0, int factor = 1 );
    bool GetImage( _imImage **image, int factor = 1 , int binary_method = -1, bool median_filtering = false );
    bool Read( wxString file, _imImage **image, int index );
    bool Write( wxString file, _imImage **image );
    bool WriteAsMAP( wxString file, _imImage **image );
	bool ExtractPlane( _imImage **image, _imImage **extrated_plane, int plane_number  );
	bool ConvertToMAP( _imImage **image );
    void SwapImages( _imImage **image1, _imImage **image2 );
    void ImageDestroy( _imImage **image );

    // deg2rad
    inline double deg2rad( double deg ) { return deg * AX_PI / 180.0; }

protected:
    AxProgressDlg *m_progressDlg;
    int m_error;

    _imImage *m_opImMap;
    _imImage *m_opIm;
    _imImage *m_opImMain;
    _imImage *m_opImTmp1;
    _imImage *m_opImTmp2;
    _imImage *m_opImMask;
    _imImage *m_opImAlign;

    int *m_opHist;
    int *m_opLines1;
    int *m_opLines2;
    int *m_opCols1;


public: 
	// static binarization variable
	static int s_pre_image_binarization_method; 
	int *m_pre_image_binarization_methodPtr;
	
	// DEBUG variables
    wxString m_inputfile; // utilise dans la methode Read
};

// ---------------------------------------------------------------------------
// imSize
// borrowed from wxWidgets for avoiding include of wx/gdicmn.h
// ---------------------------------------------------------------------------

class imSize
{
public:
    // members are public for compatibility, don't use them directly.
    int x, y;
    
    // constructors
    imSize() : x(0), y(0) { }
    imSize(int xx, int yy) : x(xx), y(yy) { }
    
    // no copy ctor or assignment operator - the defaults are ok
    
    imSize& operator+=(const imSize& sz) { x += sz.x; y += sz.y; return *this; }
    imSize& operator-=(const imSize& sz) { x -= sz.x; y -= sz.y; return *this; }
    imSize& operator/=(int i) { x /= i; y /= i; return *this; }
    imSize& operator*=(int i) { x *= i; y *= i; return *this; }
    imSize& operator/=(unsigned int i) { x /= i; y /= i; return *this; }
    imSize& operator*=(unsigned int i) { x *= i; y *= i; return *this; }
    imSize& operator/=(long i) { x /= i; y /= i; return *this; }
    imSize& operator*=(long i) { x *= i; y *= i; return *this; }
    imSize& operator/=(unsigned long i) { x /= i; y /= i; return *this; }
    imSize& operator*=(unsigned long i) { x *= i; y *= i; return *this; }
    imSize& operator/=(double i) { x = int(x/i); y = int(y/i); return *this; }
    imSize& operator*=(double i) { x = int(x*i); y = int(y*i); return *this; }
    
    void IncTo(const imSize& sz)
    { if ( sz.x > x ) x = sz.x; if ( sz.y > y ) y = sz.y; }
    void DecTo(const imSize& sz)
    { if ( sz.x < x ) x = sz.x; if ( sz.y < y ) y = sz.y; }
    void DecToIfSpecified(const imSize& sz)
    {
        if ( sz.x != wxDefaultCoord && sz.x < x )
            x = sz.x;
        if ( sz.y != wxDefaultCoord && sz.y < y )
            y = sz.y;
    }
    
    void IncBy(int dx, int dy) { x += dx; y += dy; }
    void IncBy(const imPoint& pt);
    void IncBy(const imSize& sz) { IncBy(sz.x, sz.y); }
    void IncBy(int d) { IncBy(d, d); }
    
    void DecBy(int dx, int dy) { IncBy(-dx, -dy); }
    void DecBy(const imPoint& pt);
    void DecBy(const imSize& sz) { DecBy(sz.x, sz.y); }
    void DecBy(int d) { DecBy(d, d); }
    
    
    imSize& Scale(float xscale, float yscale)
    { x = (int)(x*xscale); y = (int)(y*yscale); return *this; }
    
    // accessors
    void Set(int xx, int yy) { x = xx; y = yy; }
    void SetWidth(int w) { x = w; }
    void SetHeight(int h) { y = h; }
    
    int GetWidth() const { return x; }
    int GetHeight() const { return y; }
    
    bool IsFullySpecified() const { return x != wxDefaultCoord && y != wxDefaultCoord; }
    
    // combine this size with the other one replacing the default (i.e. equal
    // to wxDefaultCoord) components of this object with those of the other
    void SetDefaults(const imSize& size)
    {
        if ( x == wxDefaultCoord )
            x = size.x;
        if ( y == wxDefaultCoord )
            y = size.y;
    }
    
    // compatibility
    int GetX() const { return x; }
    int GetY() const { return y; }
};

inline bool operator==(const imSize& s1, const imSize& s2)
{
    return s1.x == s2.x && s1.y == s2.y;
}

inline bool operator!=(const imSize& s1, const imSize& s2)
{
    return s1.x != s2.x || s1.y != s2.y;
}

inline imSize operator+(const imSize& s1, const imSize& s2)
{
    return imSize(s1.x + s2.x, s1.y + s2.y);
}

inline imSize operator-(const imSize& s1, const imSize& s2)
{
    return imSize(s1.x - s2.x, s1.y - s2.y);
}

inline imSize operator/(const imSize& s, int i)
{
    return imSize(s.x / i, s.y / i);
}

inline imSize operator*(const imSize& s, int i)
{
    return imSize(s.x * i, s.y * i);
}

inline imSize operator*(int i, const imSize& s)
{
    return imSize(s.x * i, s.y * i);
}

inline imSize operator/(const imSize& s, unsigned int i)
{
    return imSize(s.x / i, s.y / i);
}

inline imSize operator*(const imSize& s, unsigned int i)
{
    return imSize(s.x * i, s.y * i);
}

inline imSize operator*(unsigned int i, const imSize& s)
{
    return imSize(s.x * i, s.y * i);
}

inline imSize operator/(const imSize& s, long i)
{
    return imSize(s.x / i, s.y / i);
}

inline imSize operator*(const imSize& s, long i)
{
    return imSize(int(s.x * i), int(s.y * i));
}

inline imSize operator*(long i, const imSize& s)
{
    return imSize(int(s.x * i), int(s.y * i));
}

inline imSize operator/(const imSize& s, unsigned long i)
{
    return imSize(int(s.x / i), int(s.y / i));
}

inline imSize operator*(const imSize& s, unsigned long i)
{
    return imSize(int(s.x * i), int(s.y * i));
}

inline imSize operator*(unsigned long i, const imSize& s)
{
    return imSize(int(s.x * i), int(s.y * i));
}

inline imSize operator*(const imSize& s, double i)
{
    return imSize(int(s.x * i), int(s.y * i));
}

inline imSize operator*(double i, const imSize& s)
{
    return imSize(int(s.x * i), int(s.y * i));
}

// ---------------------------------------------------------------------------
// Point classes: with real or integer coordinates
// borrowed from wxWidgets for avoiding include of wx/gdicmn.h
// ---------------------------------------------------------------------------

class imRealPoint
{
public:
    double x;
    double y;
    
    imRealPoint() : x(0.0), y(0.0) { }
    imRealPoint(double xx, double yy) : x(xx), y(yy) { }
    imRealPoint(const imPoint& pt);
    
    // no copy ctor or assignment operator - the defaults are ok
    
    //assignment operators
    imRealPoint& operator+=(const imRealPoint& p) { x += p.x; y += p.y; return *this; }
    imRealPoint& operator-=(const imRealPoint& p) { x -= p.x; y -= p.y; return *this; }
    
    imRealPoint& operator+=(const imSize& s) { x += s.GetWidth(); y += s.GetHeight(); return *this; }
    imRealPoint& operator-=(const imSize& s) { x -= s.GetWidth(); y -= s.GetHeight(); return *this; }
};


inline bool operator==(const imRealPoint& p1, const imRealPoint& p2)
{
    return wxIsSameDouble(p1.x, p2.x) && wxIsSameDouble(p1.y, p2.y);
}

inline bool operator!=(const imRealPoint& p1, const imRealPoint& p2)
{
    return !(p1 == p2);
}

inline imRealPoint operator+(const imRealPoint& p1, const imRealPoint& p2)
{
    return imRealPoint(p1.x + p2.x, p1.y + p2.y);
}


inline imRealPoint operator-(const imRealPoint& p1, const imRealPoint& p2)
{
    return imRealPoint(p1.x - p2.x, p1.y - p2.y);
}


inline imRealPoint operator/(const imRealPoint& s, int i)
{
    return imRealPoint(s.x / i, s.y / i);
}

inline imRealPoint operator*(const imRealPoint& s, int i)
{
    return imRealPoint(s.x * i, s.y * i);
}

inline imRealPoint operator*(int i, const imRealPoint& s)
{
    return imRealPoint(s.x * i, s.y * i);
}

inline imRealPoint operator/(const imRealPoint& s, unsigned int i)
{
    return imRealPoint(s.x / i, s.y / i);
}

inline imRealPoint operator*(const imRealPoint& s, unsigned int i)
{
    return imRealPoint(s.x * i, s.y * i);
}

inline imRealPoint operator*(unsigned int i, const imRealPoint& s)
{
    return imRealPoint(s.x * i, s.y * i);
}

inline imRealPoint operator/(const imRealPoint& s, long i)
{
    return imRealPoint(s.x / i, s.y / i);
}

inline imRealPoint operator*(const imRealPoint& s, long i)
{
    return imRealPoint(s.x * i, s.y * i);
}

inline imRealPoint operator*(long i, const imRealPoint& s)
{
    return imRealPoint(s.x * i, s.y * i);
}

inline imRealPoint operator/(const imRealPoint& s, unsigned long i)
{
    return imRealPoint(s.x / i, s.y / i);
}

inline imRealPoint operator*(const imRealPoint& s, unsigned long i)
{
    return imRealPoint(s.x * i, s.y * i);
}

inline imRealPoint operator*(unsigned long i, const imRealPoint& s)
{
    return imRealPoint(s.x * i, s.y * i);
}

inline imRealPoint operator*(const imRealPoint& s, double i)
{
    return imRealPoint(int(s.x * i), int(s.y * i));
}

inline imRealPoint operator*(double i, const imRealPoint& s)
{
    return imRealPoint(int(s.x * i), int(s.y * i));
}


// ----------------------------------------------------------------------------
// imPoint: 2D point with integer coordinates
// borrowed from wxWidgets for avoiding include of wx/gdicmn.h
// ----------------------------------------------------------------------------

class imPoint
{
public:
    int x, y;
    
    imPoint() : x(0), y(0) { }
    imPoint(int xx, int yy) : x(xx), y(yy) { }
    imPoint(const imRealPoint& pt) : x(int(pt.x)), y(int(pt.y)) { }
    
    // no copy ctor or assignment operator - the defaults are ok
    
    //assignment operators
    imPoint& operator+=(const imPoint& p) { x += p.x; y += p.y; return *this; }
    imPoint& operator-=(const imPoint& p) { x -= p.x; y -= p.y; return *this; }
    
    imPoint& operator+=(const imSize& s) { x += s.GetWidth(); y += s.GetHeight(); return *this; }
    imPoint& operator-=(const imSize& s) { x -= s.GetWidth(); y -= s.GetHeight(); return *this; }
    
    // check if both components are set/initialized
    bool IsFullySpecified() const { return x != wxDefaultCoord && y != wxDefaultCoord; }
    
    // fill in the unset components with the values from the other point
    void SetDefaults(const imPoint& pt)
    {
        if ( x == wxDefaultCoord )
            x = pt.x;
        if ( y == wxDefaultCoord )
            y = pt.y;
    }
};


// comparison
inline bool operator==(const imPoint& p1, const imPoint& p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

inline bool operator!=(const imPoint& p1, const imPoint& p2)
{
    return !(p1 == p2);
}


// arithmetic operations (component wise)
inline imPoint operator+(const imPoint& p1, const imPoint& p2)
{
    return imPoint(p1.x + p2.x, p1.y + p2.y);
}

inline imPoint operator-(const imPoint& p1, const imPoint& p2)
{
    return imPoint(p1.x - p2.x, p1.y - p2.y);
}

inline imPoint operator+(const imPoint& p, const imSize& s)
{
    return imPoint(p.x + s.x, p.y + s.y);
}

inline imPoint operator-(const imPoint& p, const imSize& s)
{
    return imPoint(p.x - s.x, p.y - s.y);
}

inline imPoint operator+(const imSize& s, const imPoint& p)
{
    return imPoint(p.x + s.x, p.y + s.y);
}

inline imPoint operator-(const imSize& s, const imPoint& p)
{
    return imPoint(s.x - p.x, s.y - p.y);
}

inline imPoint operator-(const imPoint& p)
{
    return imPoint(-p.x, -p.y);
}

inline imPoint operator/(const imPoint& s, int i)
{
    return imPoint(s.x / i, s.y / i);
}

inline imPoint operator*(const imPoint& s, int i)
{
    return imPoint(s.x * i, s.y * i);
}

inline imPoint operator*(int i, const imPoint& s)
{
    return imPoint(s.x * i, s.y * i);
}

inline imPoint operator/(const imPoint& s, unsigned int i)
{
    return imPoint(s.x / i, s.y / i);
}

inline imPoint operator*(const imPoint& s, unsigned int i)
{
    return imPoint(s.x * i, s.y * i);
}

inline imPoint operator*(unsigned int i, const imPoint& s)
{
    return imPoint(s.x * i, s.y * i);
}

inline imPoint operator/(const imPoint& s, long i)
{
    return imPoint(s.x / i, s.y / i);
}

inline imPoint operator*(const imPoint& s, long i)
{
    return imPoint(int(s.x * i), int(s.y * i));
}

inline imPoint operator*(long i, const imPoint& s)
{
    return imPoint(int(s.x * i), int(s.y * i));
}

inline imPoint operator/(const imPoint& s, unsigned long i)
{
    return imPoint(s.x / i, s.y / i);
}

inline imPoint operator*(const imPoint& s, unsigned long i)
{
    return imPoint(int(s.x * i), int(s.y * i));
}

inline imPoint operator*(unsigned long i, const imPoint& s)
{
    return imPoint(int(s.x * i), int(s.y * i));
}

inline imPoint operator*(const imPoint& s, double i)
{
    return imPoint(int(s.x * i), int(s.y * i));
}

inline imPoint operator*(double i, const imPoint& s)
{
    return imPoint(int(s.x * i), int(s.y * i));
}





#endif
