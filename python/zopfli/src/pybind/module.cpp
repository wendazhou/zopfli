#include "bindings.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <zopfli.h>
#include <hash.h>
#include <string>
#include <cstdlib>
#include <exception>
#include <optional>
#include <memory>

namespace py = pybind11;
using namespace zopfli_cpp;

namespace {

py::bytes compress(py::bytes const &data, ZopfliOptions const *options) {
    char *data_ptr;
    py::ssize_t data_size;

    PYBIND11_BYTES_AS_STRING_AND_SIZE(
        data.ptr(), &data_ptr, &data_size);

    ZopfliOptions default_options;
    ZopfliInitOptions(&default_options);

    std::size_t outsize = 0;
    unsigned char *output = nullptr;

    ZopfliCompress(options ? options : &default_options, ZOPFLI_FORMAT_GZIP,
                   reinterpret_cast<const unsigned char *>(data_ptr), data_size,
                   &output, &outsize);

    PyObject *output_py = PYBIND11_BYTES_FROM_STRING_AND_SIZE(
        reinterpret_cast<const char *>(output),
        outsize);

    free(output);

    if (!output_py) {
        throw std::runtime_error("Could not create bytes object.");
    }

    return py::reinterpret_borrow<py::bytes>(output_py);
}

ZopfliOptions *create_options_with_defaults(
    std::optional<int> verbose, std::optional<int> verbose_more, std::optional<int> num_iterations,
    std::optional<int> block_splitting, std::optional<int> block_splitting_max)
{
    auto result = new ZopfliOptions();
    ZopfliInitOptions(result);

    if (verbose) {
        result->verbose = *verbose;
    }

    if (verbose_more) {
        result->verbose_more = *verbose_more;
    }

    if (num_iterations) {
        result->numiterations = *num_iterations;
    }

    if (block_splitting) {
        result->blocksplitting = *block_splitting;
    }

    if (block_splitting_max) {
        result->blocksplittingmax = *block_splitting_max;
    }

    return result;
}

}

PYBIND11_MODULE(_zopfli, m) {
    m.doc() = "Python bindings for the Zopfli compression library";

    m.def("compress", &compress, py::arg("data"), py::arg("options") = py::none(),
          R"(Compress the given data to a gzip-compatible stream using the default options.

Parameters
----------
data: A `bytes` object containing the raw data.

Returns
-------
Returns a `bytes` object containing the compressed data.
)");

    py::class_<ZopfliOptions>(m, "ZopfliOptions")
        .def(py::init(&create_options_with_defaults),
             py::arg("verbose") = py::none(), py::arg("verbose_more") = py::none(),
             py::arg("num_iterations") = py::none(), py::arg("block_splitting") = py::none(),
             py::arg("block_splitting_max") = py::none())
        .def_readwrite("verbose", &ZopfliOptions::verbose)
        .def_readwrite("verbose_more", &ZopfliOptions::verbose_more)
        .def_readwrite("num_iterations", &ZopfliOptions::numiterations)
        .def_readwrite("block_splitting", &ZopfliOptions::blocksplitting)
        .def_readwrite("block_splitting_max", &ZopfliOptions::blocksplittingmax);

    auto lz77_module = m.def_submodule("lz77", "Utilities for LZ77 generation and histogram.");
    register_lz77(lz77_module);

    typedef std::unique_ptr<ZopfliHash, clean_and_delete<ZopfliCleanHash>> ZopfliHashHolder;

    py::class_<ZopfliHash, ZopfliHashHolder>(m, "Hash", "Utility hash map for longest match search.")
        .def(py::init([]() {
            auto hash = new ZopfliHash();
            ZopfliAllocHash(ZOPFLI_WINDOW_SIZE, hash);
            return ZopfliHashHolder(hash);
        }));
}
