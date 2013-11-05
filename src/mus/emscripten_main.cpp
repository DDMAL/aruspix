//
//  emscripten_main.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 05/11/13.
//  Copyright (c) 2013 com.aruspix.www. All rights reserved.
//

#include <sstream>
#include <string>

#include "mus.h"
#include "muscontroller.h"

#include "jsonxx.h"

using namespace std;
using namespace jsonxx;


extern "C" {
    const char * convertMusic(ConvertFileType input_format, ConvertFileType output_format, const char * c_data) {
        
        string data(c_data);
        string out_str;
        
        MusController controller;
        
        Mus::SetResourcesPath("/data");
        
        // default to mei if unset.
        if (input_format == pae_file) {
            controller.LoadString( data, pae_file );
        } else {
            controller.LoadString( data, mei_file );
        }
        
        // in the future we will be able to render to mei too
        out_str = controller.RenderToSvg();
        return out_str.c_str();
    }
}