//
//  musmultirest.h
//  aruspix
//
//  Created by Rodolfo Zitellini on 02/12/13.
//  Copyright (c) 2013 com.aruspix.www. All rights reserved.
//

#ifndef __MUS_MULTIREST_H__
#define __MUS_MULTIREST_H__

#include "musdef.h"
#include "muslayerelement.h"

//----------------------------------------------------------------------------
// MusClef
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <musmultirest> element. 
 */
class MusMultiRest: public MusLayerElement
{
public:
    // constructors and destructors
    MusMultiRest();
    MusMultiRest(int number);
    virtual ~MusMultiRest();
    
    /**
     * Comparison operator. 
     * Check if the MusLayerElement if a MusClef and compare attributes
     */
    virtual bool operator==(MusObject& other);
    
    virtual std::string MusClassName( ) { return "MusMultiRest"; };
    
    /**
     * Get the multi measure rest number
     */
    int GetNumber() {return m_number;};
    
    /**
     * Set the multi measure rest number.
     */
	void SetNumber( int number ) {m_number = number;};
    
private:
    /** Indicates the shape and the line of the clef using clef ids  */
    int m_number;
    
public:

private:
    
};


#endif
