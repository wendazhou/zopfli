#include "bindings.h"
#include <lz77.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <optional>

namespace py = pybind11;

namespace {

py::tuple lz77store_get_histogram(ZopfliLZ77Store const* this_, std::optional<size_t> start, std::optional<size_t> end) {
    py::array_t<size_t> length_literal_counts(ZOPFLI_NUM_LL);
    py::array_t<size_t> distance_counts(ZOPFLI_NUM_D);

    auto ll_counts_buffer = length_literal_counts.request(true);
    auto d_counts_buffer = distance_counts.request(true);

    ZopfliLZ77GetHistogram(
        this_,
        start.value_or(0),
        end.value_or(this_->size > 0 ? this_->pos[this_->size - 1] : 0),
        static_cast<size_t*>(ll_counts_buffer.ptr),
        static_cast<size_t*>(d_counts_buffer.ptr));

    return py::make_tuple(
        std::move(length_literal_counts),
        std::move(distance_counts));
}

void greedy_lz77_tree(py::buffer& data, ZopfliBlockState* state, ZopfliLZ77Store* store, ZopfliHash* hash) {
    auto info = data.request();
    if (info.itemsize != 1) {
        throw py::value_error("Invalid data type for buffer.");
    }

    if (info.ndim != 1) {
        throw py::value_error("data must be one-dimensional.");
    }

    if (info.strides[0] != 1) {
        throw py::value_error("data must be contiguous");
    }

    ZopfliLZ77Greedy(state, static_cast<const unsigned char*>(info.ptr), 0, info.size, store, hash);
}

}

namespace zopfli_cpp {

typedef std::unique_ptr<ZopfliBlockState, clean_and_delete<ZopfliCleanBlockState>> ZopfliBlockStateHolder;
typedef std::unique_ptr<ZopfliLZ77Store, clean_and_delete<ZopfliCleanLZ77Store>> ZopfliLZ77StoreHolder;

void register_lz77(py::module& m) {
    py::class_<ZopfliLZ77Store, ZopfliLZ77StoreHolder>(m, "Store")
        .def(py::init([](const char* data) {
            auto store = new ZopfliLZ77Store();
            ZopfliInitLZ77Store(reinterpret_cast<const unsigned char*>(data), store);
            return ZopfliLZ77StoreHolder(store);
            }), py::arg("data").none(false), py::keep_alive<0, 1>{})
        .def("extend", [](ZopfliLZ77Store* this_, ZopfliLZ77Store const* other) {
            ZopfliAppendLZ77Store(other, this_);
        }, py::arg("other").none(false))
        .def("get_histogram", &lz77store_get_histogram,
             py::arg("start") = py::none(), py::arg("end") = py::none());

    py::class_<ZopfliBlockState, ZopfliBlockStateHolder>(m, "BlockState")
        .def(py::init([] (ZopfliOptions* options, size_t block_start, size_t block_end, bool add_lmc) {
            auto block_state = new ZopfliBlockState();
            ZopfliInitBlockState(options, block_start, block_end, add_lmc ? 1 : 0, block_state);
            return ZopfliBlockStateHolder{block_state};
        }), py::arg("options"), py::arg("block_start"), py::arg("block_end"), py::arg("add_longest_match_cache") = true,
            py::keep_alive<0, 1>());

    m.def("greedy_tree", &greedy_lz77_tree,
        py::arg("data"),
        py::arg("state").none(false),
        py::arg("store").none(false),
        py::arg("hash").none(false));
}

}