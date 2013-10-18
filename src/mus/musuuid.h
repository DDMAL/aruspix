//
//  musuuid.h
//  aruspix
//
//  Created by Rodolfo Zitellini on 11/10/13.
//  Copyright (c) 2013 com.aruspix.www. All rights reserved.
//

#ifndef aruspix_musuuid_h
#define aruspix_musuuid_h

#define  uuid_t std::string

bool uuid_is_null(uuid_t uuid);
int uuid_compare(uuid_t a, uuid_t b);
void uuid_unparse(uuid_t uuid_in, char * str_out);
void uuid_clear(uuid_t uuid);
void uuid_parse(const char * str_in, uuid_t uuid_out);
void uuid_generate(uuid_t uuid);
void uuid_copy(uuid_t dst, const uuid_t src);

#endif
