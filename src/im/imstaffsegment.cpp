/////////////////////////////////////////////////////////////////////////////
// Name:        imstaffsegment.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "imstaffsegment.h"


//----------------------------------------------------------------------------
// ImStaffSegment
//----------------------------------------------------------------------------

ImStaffSegment::ImStaffSegment(  ) :
    ImOperator( )
{

}

ImStaffSegment::~ImStaffSegment()
{
}

bool ImStaffSegment::AnalyzeSegment()
{
    wxASSERT_MSG( m_opImMap, wxT("MAP Image cannot be NULL") );
    int i;

    if ( !GetImagePlane( &m_opImMain ) )
        return false;

    // margins
    m_opImTmp1 = imImageCreate( m_opImMain->width + 2, m_opImMain->height + 2, m_opImMain->color_space, m_opImMain->data_type );
    if (!m_opImTmp1)
        return this->Terminate( ERR_MEMORY );
    imProcessAddMargins( m_opImMain, m_opImTmp1, 1, 1);
    SwapImages( &m_opImMain, &m_opImTmp1 );


    // close
    m_opImTmp1 = imImageClone( m_opImMain );
    if (!m_opImTmp1)
        return this->Terminate( ERR_MEMORY );
    imProcessBinMorphClose( m_opImMain, m_opImTmp1, 5, 1);
    SwapImages( &m_opImMain, &m_opImTmp1 );

    
    m_opIm = imImageCreate(m_opImMain->width, m_opImMain->height, IM_GRAY, IM_USHORT);
    int region_count = imAnalyzeFindRegions ( m_opImMain, m_opIm, 8, 1);
    
    int* area = (int*)malloc( region_count * sizeof(int) );
    memset(area, 0, region_count * sizeof(int) );
    float* perim = (float*)malloc( region_count * sizeof(float) );
    memset(perim, 0, region_count * sizeof(float) );

    imAnalyzeMeasureArea( m_opIm, area, 1 );
    imAnalyzeMeasurePerimeter( m_opIm, perim, 1 );

    float c = 0;
    for (i = 0; i < region_count; i++ )
    {
        c += pow(perim[i],2) / (4 * AX_PI * area[i]) * (area[i] / m_opIm->width);
    }
    //a /= median( area, region_count);
    //p /= medianf( perim, region_count);
    //wxLogMessage("compactness %f", c / region_count  );
    this->m_compactness = c;

    free( area );
    free( perim );

    return this->Terminate( ERR_NONE );
}

