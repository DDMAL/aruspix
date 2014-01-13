/////////////////////////////////////////////////////////////////////////////
// Name:        musio.cpp
// Author:      Laurent Pugin
// Created:     2012
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musio.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

MusFileOutputStream::MusFileOutputStream( Doc *doc, std::string filename ) :
    std::ofstream( filename.c_str() )
{
	assert(doc); // Document cannot be NULL"
	m_doc = doc;
}

MusFileOutputStream::MusFileOutputStream( Doc *doc ) :
    std::ofstream(  )
{
	assert(doc); // Document cannot be NULL"
	m_doc = doc;
}

MusFileOutputStream::~MusFileOutputStream()
{
    if ( this->is_open()) {
        this->close();
    }
}



//----------------------------------------------------------------------------
// MusFileInputStream
//----------------------------------------------------------------------------

MusFileInputStream::MusFileInputStream( Doc *doc, std::string filename  ) :
    std::ifstream( filename.c_str() )
{
	assert(doc); // Document cannot be NULL"
	m_doc = doc;
    m_hasLayoutInformation = false;
}

MusFileInputStream::MusFileInputStream( Doc *doc ) :
    std::ifstream(  )
{
	assert(doc); // Document cannot be NULL"
	m_doc = doc;
    m_hasLayoutInformation = false;
}

MusFileInputStream::~MusFileInputStream()
{
    if ( this->is_open()) {
        this->close();
    }
}

