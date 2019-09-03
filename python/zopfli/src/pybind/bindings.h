#ifndef ZOPFLI_PYTHON_BINDINGS_H_INCLUDED
#define ZOPFLI_PYTHON_BINDINGS_H_INCLUDED

#include <pybind11/pybind11.h>
#include <type_traits>

namespace zopfli_cpp {
    void register_lz77(pybind11::module& m);

    template<auto cleaner>
    struct clean_and_delete {
        template<typename T>
        void operator()(T* ptr) const {
            cleaner(ptr);
            delete ptr;
        }
    };
}


#endif