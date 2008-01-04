/////////////////////////////////////////////////////////////////////////////
// Name:        imstaffsegment.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __imstaffsegment_H__
#define __imstaffsegment_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "imstaffsegment.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/dynarray.h"

#include "im/imoperator.h"
//#include "rec_wdr.h"

#define SS_FACTOR_1 3
#define SS_STAFF_ROI_W 120
#define SS_STAFF_ROI_H 120
#define SS_MIN_SEGMENT 50


// WDR: class declarations

//----------------------------------------------------------------------------
// ImStaffSegment
//----------------------------------------------------------------------------

class ImStaffSegment: public ImOperator
{
public:
    // constructors and destructors
	ImStaffSegment( );
    virtual ~ImStaffSegment();
    
    // WDR: method declarations for ImStaffSegment
    //bool Load( DOMNode *node );
    //bool Save( DOMNode *node, DOMDocument *dom );
    bool Load( TiXmlNode *node );
    bool Save( TiXmlNode *node );
	bool GetImageFromPage( _imImage **image, _imImage *page, int y );
	bool WriteMFC( wxString filename, int samplesCount, int period, int sampleSize, float *samples );
    // Processing
    bool AnalyzeSegment();

    // functors
    void PrintLabel( const int count, const int y, const int segment, wxArrayPtrVoid params  );
	void SaveImage(const int staff, const int segment, const int y, wxArrayPtrVoid params );
    void CalcCorrelation( const int count, const int y, const int segment, wxArrayPtrVoid params  );
	void CalcFeatures( const int count, const int y, const int segment, wxArrayPtrVoid params  );
	void CalcStaffHeight(const int staff, const int segment, const int y, wxArrayPtrVoid params );

protected:
	// calculate positions (all x) from/to save positions (one x evey POSITION_STEP )
	void SetValues( wxArrayInt *saved_values, int type );
	void static CalcMask( int height, int mask[] );
	// inverse - keep only one x every POSITION_STEP
	wxArrayInt GetValuesToSave( int type ); 
    
private:
    // WDR: member variable declarations for ImStaffSegment
    
public:
    // WDR: handler declarations for ImStaffSegment
    int m_x1, m_x2;
	wxArrayInt m_positions;
	wxArrayInt m_line_p; // line width (peak in run lengths)
	wxArrayInt m_line_m; // line width (median in run lengths)
	
    float m_compactness;
};

//----------------------------------------------------------------------------
// abstract base class ImStaffSegmentFunctor
//----------------------------------------------------------------------------
class ImStaffSegmentFunctor
{
private:
    void (ImStaffSegment::*fpt)(const int, const int, const int, wxArrayPtrVoid params );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    ImStaffSegmentFunctor( void(ImStaffSegment::*_fpt)(const int, const int, const int, wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ImStaffSegmentFunctor::~ImStaffSegmentFunctor() {};

    // override function "Call"
    virtual void Call( ImStaffSegment *ptr, const int staff, const int segment, const int y, wxArrayPtrVoid params )
        { (*ptr.*fpt)( staff, segment, y, params);};          // execute member function
};


#endif // __imstaffsegment_H__
