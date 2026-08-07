// Windows-only out-of-line implementation of uuid_generate.
//
// We can include aruspix_uuid.h here because it defines our libuuid-style
// type as `ax_uuid_t` rather than `uuid_t`, so it does not clash with
// <rpc.h>'s own UUID/uuid_t. The signature must match what other
// translation units see in the header, otherwise MSVC's name mangling
// drifts (ax_uuid_t parameter decays to `unsigned char *const`, not
// the `unsigned char *` the linker would otherwise look for).

#if defined(_WIN32)

#include "aruspix_uuid.h"

#include <cstring>
#include <rpc.h>

void uuid_generate(ax_uuid_t out)
{
    UUID u;
    ::UuidCreate(&u);
    std::memcpy(out, &u, 16);
}

#endif
