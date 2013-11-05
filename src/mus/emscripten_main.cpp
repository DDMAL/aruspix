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
        
        string teststr(
                       "{"
                       "  \"foo\" : 1,"
                       "  \"bar\" : false,"
                       "  \"person\" : {\"name\" : \"GWB\", \"age\" : 60,},"
                       "  \"data\": [\"abcd\", 42],"
                       "}"
                       );
        
        // Parse string or stream
        Object o;
        assert(o.parse(teststr));
        
        // Validation. Checking for JSON types and values as well
        assert(1 == o.get<Number>("foo"));
        assert(o.has<Boolean>("bar"));
        assert(o.has<Object>("person"));
        assert(o.get<Object>("person").has<Number>("age"));
        assert(o.has<Array>("data"));
        assert(o.get<Array>("data").get<Number>(1) == 42);
        assert(o.get<Array>("data").get<String>(0) == "abcd");
        assert(!o.has<Number>("data"));
        cout << o.json() << endl;                     // JSON output
        cout << o.xml(JSONx) << endl;                 // JSON to XML conversion (JSONx subtype)
        cout << o.xml(JXML) << endl;                  // JSON to XML conversion (JXML subtype)
        cout << o.xml(JXMLex) << endl;                // JSON to XML conversion (JXMLex subtype)
        
        
        
        
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