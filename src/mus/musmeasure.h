/////////////////////////////////////////////////////////////////////////////
// Name:        musmeasure.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_MEASURE_H__
#define __MUS_MEASURE_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mussection.h"

class MusMeasureInterface;

//----------------------------------------------------------------------------
// MusMeasure
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <measure> element.
 * A MusMeasure is a MusSectionInterface 
 */
class MusMeasure: public MusLogicalObject, public MusSectionInterface
{
public:
    // constructors and destructors
    MusMeasure();
    virtual ~MusMeasure();
	
	void AddMeasureElement( MusMeasureInterface *measureElement );
    
private:
    
public:
    /** The children MusMeasureInterface objects */
    ArrayOfMusMeasureElements m_measureElements;

private:
    
};

//----------------------------------------------------------------------------
// MusMeasureInterface
//----------------------------------------------------------------------------

/** 
 * This class is an interface for the MusMeasure (<measure>) content.
 * It is not an abstract class but should not be instanciate directly.
 */
class MusMeasureInterface
{
public:
    // constructors and destructors
    MusMeasureInterface();
    virtual ~MusMeasureInterface();
    
    /** The parent MusMeasure setter */
    void SetMeasure( MusMeasure *measure ) { m_measure = measure; };
        
private:
    
public:
    /** The parent MusMeasure */
    MusMeasure *m_measure;

private:
    
};

#endif
