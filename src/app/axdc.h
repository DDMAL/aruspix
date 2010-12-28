/////////////////////////////////////////////////////////////////////////////
// Name:        axdc.h 
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __AX_DC_H__
#define __AX_DC_H__


#include "wx/defs.h"


#define AxBLACK  255 << 16 | 255 << 8 | 255
#define AxWHITE 0
#define AxRED  255 << 16
#define AxBLUE 255
#define AxGREEN  255 << 8
#define AxCYAN 255 << 8 | 255
#define AxLIGHT_GREY 127 << 16 | 127 << 8 | 127


class AxPoint;
class AxRect;
class AxFontInfo;

// ---------------------------------------------------------------------------
// AxDC
// ---------------------------------------------------------------------------

/*
    This class is an abstract device context 
    It enables different types of concrete classes to be implemented
    Examples:
        AxWxDC - a wrapper to wxDCs
        AxSvgDC - a non-gui file DC
        AxCairoDC - a wrapper to a Cairo surface
    The class uses int-based colour encoding (instead of wxColour in wxDC)
    The class uses AxFontInfo (instead of wxFont in wxDC)
*/

class AxDC
{
public:

    AxDC () {};
    virtual ~AxDC() {};
    
    // Setters
    
    virtual void SetBrush( int colour, int style = wxSOLID ) = 0;
    
    virtual void SetBackground( int colour, int style = wxSOLID ) = 0;
    
    virtual void SetBackgroundMode( int mode ) = 0;
    
    virtual void SetPen( int colour, int width = 1, int style = wxSOLID ) = 0;
    
    virtual void SetFont( AxFontInfo *font_info ) = 0;

    virtual void SetTextForeground( int colour ) = 0;
    
    virtual void SetTextBackground( int colour ) = 0;
    
    virtual void ResetBrush( ) = 0;
    
    virtual void ResetPen( ) = 0;
    
    // Getters
    
    virtual void GetTextExtent( wxString& string, int *w, int *h ) = 0;

    // Drawing methods
    
    virtual void DrawCircle(int x, int y, int radius) = 0;
    
    virtual void DrawEllipticArc(int x, int y, int width, int height, double start, double end) = 0;
    
    virtual void DrawLine(int x1, int y1, int x2, int y2) = 0;
    
    virtual void DrawPolygon(int n, AxPoint points[], int xoffset = 0, int yoffset = 0, int fill_style = wxODDEVEN_RULE) = 0;
    
    virtual void DrawRectangle(int x, int y, int width, int height) = 0;
    
    virtual void DrawText(const wxString& text, int x, int y) = 0;
    
    virtual void DrawSpline(int n, AxPoint points[]) = 0;
    
    // Colour conversion method
    
    static int RGB2Int( char red, char green, char blue ) { return (red << 16 | green << 8 | blue); };
};

// ---------------------------------------------------------------------------
// AxFontInfo
// ---------------------------------------------------------------------------

/*
    This class is store font properties
    It is very similar to wxNativeFontInfo, but we need it for non-gui AxDCs 
*/

class AxFontInfo
{
public:
    AxFontInfo () 
    {
        pointSize = 0;
        family = wxFONTFAMILY_DEFAULT;
        style = wxFONTSTYLE_NORMAL;
        weight = wxFONTWEIGHT_NORMAL;
        underlined = false;
        faceName.clear();
        encoding = wxFONTENCODING_DEFAULT;    
    }
    virtual ~AxFontInfo() {};
    
        // accessors and modifiers for the font elements
    int GetPointSize() { return pointSize; }
    int GetStyle() { return style; };
    int GetWeight() { return weight; }
    bool GetUnderlined() { return underlined; }
    wxString GetFaceName() { return faceName; }
    int GetFamily() { return family; }
    int GetEncoding() { return encoding; }

    void SetPointSize(int pointSize_) { pointSize = pointSize_; }
    void SetStyle(int style_) { style = style_; }
    void SetWeight(int weight_) { weight = weight_; }
    void SetUnderlined(bool underlined_) { underlined = underlined_; }
    void SetFaceName(wxString& faceName_) { faceName = faceName_; }
    void SetFamily(int family_) { family = family_; }
    void SetEncoding(int encoding_) { encoding = encoding_; }
    
    // in axdc.cpp
    bool FromString(const wxString& s);
    wxString ToString() const;
    
    int           pointSize;
    int           family;
    int           style;
    int           weight;
    bool          underlined;
    wxString      faceName;
    int           encoding;
};



// ---------------------------------------------------------------------------
// AxPoint
// ---------------------------------------------------------------------------

/*
    This class is similar to wxPoint, but we need it because wxPoint exists
    only with GUI in wxWidgets (why???)
    All operation that use wxSize do not exists
*/

class AxPoint
{
public:
    int x, y;

    AxPoint() : x(0), y(0) { }
    AxPoint(int xx, int yy) : x(xx), y(yy) { }

    // no copy ctor or assignment operator - the defaults are ok

    // comparison
    bool operator==(const AxPoint& p) const { return x == p.x && y == p.y; }
    bool operator!=(const AxPoint& p) const { return !(*this == p); }

    // arithmetic operations (component wise)
    AxPoint operator+(const AxPoint& p) const { return AxPoint(x + p.x, y + p.y); }
    AxPoint operator-(const AxPoint& p) const { return AxPoint(x - p.x, y - p.y); }

    AxPoint& operator+=(const AxPoint& p) { x += p.x; y += p.y; return *this; }
    AxPoint& operator-=(const AxPoint& p) { x -= p.x; y -= p.y; return *this; }

    //AxPoint& operator+=(const wxSize& s) { x += s.GetWidth(); y += s.GetHeight(); return *this; }
    //AxPoint& operator-=(const wxSize& s) { x -= s.GetWidth(); y -= s.GetHeight(); return *this; }

    //AxPoint operator+(const wxSize& s) const { return AxPoint(x + s.GetWidth(), y + s.GetHeight()); }
    //AxPoint operator-(const wxSize& s) const { return AxPoint(x - s.GetWidth(), y - s.GetHeight()); }

    AxPoint operator-() const { return AxPoint(-x, -y); }
};



// ---------------------------------------------------------------------------
// AxRect
// ---------------------------------------------------------------------------

/*
    This class is similar to wxRect, but we need it because wxRect exists
    only with GUI in wxWidgets (why???)
    All operations that use wxSize do not exists
    All operations on rectangles (inflate, etc.) do not exists
*/

class AxRect
{
public:
    AxRect()
        : x(0), y(0), width(0), height(0)
        { }
    AxRect(int xx, int yy, int ww, int hh)
        : x(xx), y(yy), width(ww), height(hh)
        { }
    AxRect(const AxPoint& topLeft, const AxPoint& bottomRight);
    //AxRect(const AxPoint& pt, const wxSize& size)
    //    : x(pt.x), y(pt.y), width(size.x), height(size.y)
    //    { }
    //AxRect(const wxSize& size)
    //    : x(0), y(0), width(size.x), height(size.y)
    //    { }

    // default copy ctor and assignment operators ok

    int GetX() const { return x; }
    void SetX(int xx) { x = xx; }

    int GetY() const { return y; }
    void SetY(int yy) { y = yy; }

    int GetWidth() const { return width; }
    void SetWidth(int w) { width = w; }

    int GetHeight() const { return height; }
    void SetHeight(int h) { height = h; }

    AxPoint GetPosition() const { return AxPoint(x, y); }
    void SetPosition( const AxPoint &p ) { x = p.x; y = p.y; }

    //wxSize GetSize() const { return wxSize(width, height); }
    //void SetSize( const wxSize &s ) { width = s.GetWidth(); height = s.GetHeight(); }

    bool IsEmpty() const { return (width <= 0) || (height <= 0); }

    int GetLeft()   const { return x; }
    int GetTop()    const { return y; }
    int GetBottom() const { return y + height - 1; }
    int GetRight()  const { return x + width - 1; }

    void SetLeft(int left) { x = left; }
    void SetRight(int right) { width = right - x + 1; }
    void SetTop(int top) { y = top; }
    void SetBottom(int bottom) { height = bottom - y + 1; }

    AxPoint GetTopLeft() const { return GetPosition(); }
    AxPoint GetLeftTop() const { return GetTopLeft(); }
    void SetTopLeft(const AxPoint &p) { SetPosition(p); }
    void SetLeftTop(const AxPoint &p) { SetTopLeft(p); }

    AxPoint GetBottomRight() const { return AxPoint(GetRight(), GetBottom()); }
    AxPoint GetRightBottom() const { return GetBottomRight(); }
    void SetBottomRight(const AxPoint &p) { SetRight(p.x); SetBottom(p.y); }
    void SetRightBottom(const AxPoint &p) { SetBottomRight(p); }

    AxPoint GetTopRight() const { return AxPoint(GetRight(), GetTop()); }
    AxPoint GetRightTop() const { return GetTopRight(); }
    void SetTopRight(const AxPoint &p) { SetRight(p.x); SetTop(p.y); }
    void SetRightTop(const AxPoint &p) { SetTopLeft(p); }

    AxPoint GetBottomLeft() const { return AxPoint(GetLeft(), GetBottom()); }
    AxPoint GetLeftBottom() const { return GetBottomLeft(); }
    void SetBottomLeft(const AxPoint &p) { SetLeft(p.x); SetBottom(p.y); }
    void SetLeftBottom(const AxPoint &p) { SetBottomLeft(p); }

public:
    int x, y, width, height;
};

#endif // __AX_DC_H__
