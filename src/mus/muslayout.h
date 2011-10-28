/////////////////////////////////////////////////////////////////////////////
// Name:        layout.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_LAYOUT_H__
#define __MUS_LAYOUT_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "musobject.h"

class MusScore;
class MusPage;


//----------------------------------------------------------------------------
// MusLayout
//----------------------------------------------------------------------------

/**
 * This class represents a laid-out score.
 * A MusLayout is contained in a MusDoc.
 * It contains MusPage objects.
*/
class MusLayout: public MusLayoutObject
{
public:
    // constructors and destructors
    MusLayout();
    virtual ~MusLayout();
	
	void AddPage( MusPage *page );
    
    void Clear();
    void CheckIntegrity();
    
    /** The parent MusDoc setter */
    void SetDoc( MusDoc *doc ) { m_doc = doc; };
	/** Realize the layout */
	void Realize( MusScore *score );
	
	int GetPageCount() const { return (int)m_pages.GetCount(); };

    
    // moulinette
    virtual void Process(MusLayoutFunctor *functor, wxArrayPtrVoid params );
    // functors
    
private:
    
public:
    /** The MusPage objects of the layout */
    ArrayOfMusPages m_pages;
    /** The parent MusDoc */
    MusDoc *m_doc;

private:
    
};


#endif
