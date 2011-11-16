/////////////////////////////////////////////////////////////////////////////
// Name:        imstaffsegment.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __imstaffsegment_H__
#define __imstaffsegment_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "im/imoperator.h"



//----------------------------------------------------------------------------
// ImStaffSegment
//----------------------------------------------------------------------------

class ImStaffSegment: public ImOperator
{
public:
    // constructors and destructors
	ImStaffSegment( );
    virtual ~ImStaffSegment();
    
        // Processing
    bool AnalyzeSegment();

	int m_x1;
	int m_x2;
    float m_compactness;
    
private:
        

};

#endif // __imstaffsegment_H__
