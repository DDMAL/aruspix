//
//  musuuid.c
//  aruspix
//
//  Created by Rodolfo Zitellini on 11/10/13.
//  Copyright (c) 2013 com.aruspix.www. All rights reserved.
//

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>

#include "musuuid.h"

bool uuid_is_null(uuid_t uuid) {
    return uuid.empty();
}

int uuid_compare(uuid_t a, uuid_t b) {
    return (a == b);
}

void uuid_unparse(uuid_t uuid_in, char * str_out) {
    str_out = (char*)uuid_in.c_str();
}

void uuid_clear(uuid_t uuid) {
    uuid = "";
}

void uuid_parse(const char * str_in, uuid_t uuid_out) {
    uuid_out = uuid_t(str_in);
}

void uuid_generate(uuid_t uuid) {
    uuid = "1234567890123456789012345678901234567";    
}
void uuid_copy(uuid_t dst, const uuid_t src) {
    dst = src;
}