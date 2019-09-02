#include <pybind11/pybind11.h>
#include <zopfli.h>
#include <string>
#include <cstdlib>
#include <exception>

namespace py = pybind11;

py::bytes compress(py::bytes const& data) {
    char* data_ptr;
    py::ssize_t data_size;

    PYBIND11_BYTES_AS_STRING_AND_SIZE(
        data.ptr(), &data_ptr, &data_size);

    ZopfliOptions options;
    ZopfliInitOptions(&options);

    std::size_t outsize = 0;
    unsigned char* output = nullptr;

    ZopfliCompress(&options, ZOPFLI_FORMAT_GZIP,
        reinterpret_cast<const unsigned char*>(data_ptr), data_size,
        &output, &outsize);

    PyObject* output_py = PYBIND11_BYTES_FROM_STRING_AND_SIZE(
        reinterpret_cast<const char*>(output),
        outsize);
    
    free(output);

    if(!output_py) {
        throw std::runtime_error("Could not create bytes object.");
    }

    return py::reinterpret_borrow<py::bytes>(output_py);
}


PYBIND11_MODULE(_zopfli, m) {
    m.doc() = "Python bindings for the Zopfli compression library";

    m.def("compress", &compress, py::arg("data"),
    R"(Compress the given data to a gzip-compatible stream using the default options.

    Parameters
    ----------
    data: A `bytes` object containing the raw data.

    Returns
    -------
    Returns a `bytes` object containing the compressed data.
    )");
}

