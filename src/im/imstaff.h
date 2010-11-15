/////////////////////////////////////////////////////////////////////////////
// Name:        imstaff.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __imstaff_H__
#define __imstaff_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "imstaff.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "im/imoperator.h"


#define SS_FACTOR_1 3
#define SS_STAFF_ROI_W 120
#define SS_STAFF_ROI_H 120
#define SS_MIN_SEGMENT 50

class ImStaffSegment;
WX_DECLARE_OBJARRAY( ImStaffSegment, ArrayOfStaffSegments);


//----------------------------------------------------------------------------
// ImStaff
//----------------------------------------------------------------------------

class ImStaff: public ImOperator 
{
public:
    // constructors and destructors
	ImStaff( );
    virtual ~ImStaff();
    
    //WDR: method declarations for ImStaff
    bool Load( TiXmlNode *node );
    bool Save( TiXmlNode *node );
    // Processing
    bool GetStaffBorders( int threshold_in_percent, bool analyse_segments );
    // moulinette
    // values
	bool IsVoid( ) { return (m_x2 == 0); } // a staff is void if its length is 0; This should not happen as void staves
		// are removed (in ImPage::ExtractStaves). Ideally, staff detection would have to be redone once the preprocessing
		// errors have been corrected.
    void GetMinMax(int *minx, int *maxx);
    int CalcIndentation( int leftmargin );
    int CalcEcart( int previous );
	void GetXandPos( int posx, int *x, int *vpos ); // return the x position in the staff (remove margin)
													// and the vertical position (decalage) from m_positions in segment	
	void CorrectLyricCurvature( imImage *src, imImage *dest );
	void FindLyricBaseLine( imImage *src, double *overallProjection, int *offsets, int windowWidth );
	
	// functors																								
	bool GetImageFromPage( _imImage **image, _imImage *page, int y1, int y2 = -1 );
	bool WriteMFC( wxString filename, int samplesCount, int period, int sampleSize, float *samples );

    // functors
	void SaveImage(const int staff, wxArrayPtrVoid params );
    void CalcCorrelation( const int count, wxArrayPtrVoid params  );
	void CalcFeatures( const int count, wxArrayPtrVoid params  );
	void CalcLyricFeatures(const int staff, wxArrayPtrVoid params );
	void CalcStaffHeight(const int staff, wxArrayPtrVoid params );
	void ExtractLyricImages( const int staff, wxArrayPtrVoid params );
	
protected:
	// calculate positions (all x) from/to save positions (one x evey POSITION_STEP )
	void SetValues( wxArrayInt *saved_values, int type );
	void static CalcMask( int height, int numstafflines, int mask[] );
	// inverse - keep only one x every POSITION_STEP
	wxArrayInt GetValuesToSave( int type ); 
	
public:
    int m_x1, m_x2;
	wxArrayInt m_positions;
	wxArrayInt m_line_p; // line width (peak in run lengths)
	wxArrayInt m_line_m; // line width (median in run lengths)
    ArrayOfStaffSegments m_segments; // now used only during processing, not stored anymore
	int m_lyricCentre;		// y-coordinate of center of lyric
	int m_lyricBaseline;	// y-coordinate of baseline of lyric
	int m_lyricTopline;		// y-coordinate of topline of lyric
		    
public:
        int m_y;
    int m_med;
    
private:
    
};


//----------------------------------------------------------------------------
// abstract base class ImStaffFunctor
//----------------------------------------------------------------------------
class ImStaffFunctor
{
private:
    void (ImStaff::*fpt)(const int, wxArrayPtrVoid params );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    ImStaffFunctor( void(ImStaff::*_fpt)(const int, wxArrayPtrVoid )) { fpt=_fpt; };
	virtual ImStaffFunctor::~ImStaffFunctor() {};

    // override function "Call"
    virtual void Call( ImStaff *ptr, const int staff, wxArrayPtrVoid params )
        { (*ptr.*fpt)( staff, params);};          // execute member function
};


#endif
