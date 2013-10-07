/////////////////////////////////////////////////////////////////////////////
// Name:        musrc_bezier.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musrc.h"

/** static members **/
MusPoint MusRC::point_[];
MusPoint MusRC::bcoord[];
 
void MusRC::pntswap (MusPoint *x1, MusPoint *x2)
{	MusPoint a;
	a = *x1;
	*x1 = *x2;
	*x2 = a;
}

