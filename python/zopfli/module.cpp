#include <pybind11/pybind11.h>
#include <zopfli.h>
#include <string>

namespace py = pybind11;

py::bytes compress(std::string const& input) {
    ZopfliOptions options;
    ZopfliInitOptions(&options);

    std::size_t outsize;
    unsigned char* output;

    ZopfliCompress(&options, ZopfliFormat::ZOPFLI_FORMAT_GZIP,
        reinterpret_cast<const unsigned char*>(input.data()), input.size(),
        &output, &outsize);

    PyObject* output_py = PYBIND11_BYTES_FROM_STRING_AND_SIZE(
        reinterpret_cast<const char*>(output),
        outsize);

    return py::reinterpret_steal<py::bytes>(output_py);
}


PYBIND11_MODULE(_zopfli, m) {
    m.doc() = "Python bindings for the Zopfli compression library";

    m.def("compress", &compress, "Compress the given data to a gzip-compatible stream using the default options.");
}

