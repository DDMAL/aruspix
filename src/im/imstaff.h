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
//#include "rec_wdr.h"

// WDR: class declarations

class ImStaffSegmentFunctor;

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
    
    // WDR: method declarations for ImStaff
    //bool Load( DOMNode *node );
    //bool Save( DOMNode *node, DOMDocument *dom );
    bool Load( TiXmlNode *node );
    bool Save( TiXmlNode *node );
    // Processing
    bool GetStaffBorders( int threshold_in_percent, bool analyse_segments );
    // moulinette
    virtual void Process(ImStaffSegmentFunctor *functor, int staff, int y, wxArrayPtrVoid params );
    // values
    void GetMinMax(int *minx, int *maxx);
    int CalcIndentation( int leftmargin );
    int CalcEcart( int previous );
	void GetXandPos( int posx, int *x, int *vpos ); // return the x position in the staff (remove margin)
													// and the vertical position (decalage) from m_positions in segment		
	
	// functors																								
	void MergeSegments(const int staff, const int y, wxArrayPtrVoid params, ImStaffSegmentFunctor *functor = NULL );
    
public:
    // WDR: member variable declarations for ImStaff
    ArrayOfStaffSegments m_segments;
    int m_y;
    int m_med;
    
private:
    // WDR: handler declarations for ImStaff

};

//----------------------------------------------------------------------------
// abstract base class ImStaffFunctor
//----------------------------------------------------------------------------
class ImStaffFunctor
{
private:
    void (ImStaff::*fpt)(const int, const int, wxArrayPtrVoid params, ImStaffSegmentFunctor *functor );   // pointer to member function

public:

    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    ImStaffFunctor( void(ImStaff::*_fpt)(const int, const int, wxArrayPtrVoid, ImStaffSegmentFunctor* )) { fpt=_fpt; };
	virtual ImStaffFunctor::~ImStaffFunctor() {};

    // override function "Call"
    virtual void Call( ImStaff *ptr, const int staff, const int y, wxArrayPtrVoid params, ImStaffSegmentFunctor *functor )
        { (*ptr.*fpt)( staff, y, params, functor);};          // execute member function
};

#endif
