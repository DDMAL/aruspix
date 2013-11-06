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
    const char * convertMusic(const char * c_notation, const char * c_options) {
        
        string notation(c_notation);
        string json_options(c_options);
        string out_str;
        
        string in_format;
        string out_format;
        int scale = 100;
        int border = 10;
        
        Object json;
        
        // Read JSON options
        json.parse(json_options);
        
        if (json.has<String>("InputFormat"))
            in_format = json.get<String>("InputFormat");
        
        if (json.has<String>("OutputFormat"))
            out_format = json.get<String>("OutputFormat");
        
        if (json.has<Number>("Scale"))
            scale = json.get<Number>("Scale");
        
        if (json.has<Number>("Border"))
            border = json.get<Number>("Border");
        
        MusController controller;
        
        controller.SetScale(scale);
        controller.SetBorder(border);
        
        Mus::SetResourcesPath("/data");
        
        // default to mei if unset.
        if (in_format == "pae") {
            controller.LoadString( notation, pae_file );
        } else {
            controller.LoadString( notation, mei_file );
        }
        
        // in the future we will be able to render to mei too
        out_str = controller.RenderToSvg();
        return out_str.c_str();
    }
}