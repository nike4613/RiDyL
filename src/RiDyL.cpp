#include "RiDyL.hpp"
#include "private/util.hpp"

extern "C" {
#include <dlfcn.h>
#include <link.h>
}

using namespace RiDyL;

int main() {

    util::callback_wrap(dl_iterate_phdr, [](dl_phdr_info* ptr, size_t size) {



        return 0;
    });

    return 0;
}