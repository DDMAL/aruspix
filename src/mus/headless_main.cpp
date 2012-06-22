//
//  headless_main.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 22/06/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#include <iostream>

#include <wx/string.h>
#include "wx/wx.h"

#include "musdoc.h"
#include "musiopae.h"
#include "muslayout.h"
#include "musrc.h"
#include "mussvgdc.h"
#include "musbboxdc.h"

int main(int argc, char** argv) {
    
    MusDoc *doc =  new MusDoc();

    MusPaeInput meiinput( doc, "/Users/xhero/Documents/Aruspix.localized/00000400003641-1.1.1.pae" );
	if ( !meiinput.ImportFile() )
		return -1;
    
	MusLayout *layout = new MusLayout( Raw );
	layout->Realize(doc->m_divs[0].m_score);
	doc->AddLayout( layout );
    
    MusRC rc;
    MusBBoxDC bb_dc( &rc, 0, 0 );
    rc.SetLayout(layout);
    rc.DrawPage(  &bb_dc, &layout->m_pages[0] , false );
    
    
    
    MusSvgDC *svg = new MusSvgDC("/Users/xhero/test.svg", 2000, 2000);
    rc.DrawPage(svg, &layout->m_pages[0] , false);
    
    delete svg;
    
    return 0;
}