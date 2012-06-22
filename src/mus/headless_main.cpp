//
//  headless_main.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 22/06/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#include <iostream>

#include <wx/string.h>

// I think this is not good...
//#define wxUSE_GUI 0

#ifndef WX_PRECOMP
//#include "wx/wx.h"
#endif
#include "wx/wx.h"

int main(int argc, char** argv) {
    
    
    wxString *test = new wxString("This is a string test.\n");
    
    printf("%s", test->c_str());
    
}