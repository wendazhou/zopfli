from .._zopfli import lz77 as _lz77
from .. import Hash as _Hash, ZopfliOptions as _Options

import typing


Store = _lz77.Store
BlockState = _lz77.BlockState


def greedy_tree(data,
                block_state: typing.Optional[BlockState]=None,
                hash: typing.Optional[_Hash]=None,
                out: typing.Optional[Store]=None) -> Store:
    """Greedy LZ77 matching procedure.

    Parameters
    ----------
    data: the data to encode. A bytes buffer, string, or buffer object.
    block_state: the block configuration used to configure the parsing
    hash: optional hash block to speed up matching
    out: optional existing output to write into

    Returns
    -------
    If `out` was provided, `out`, otherwise a new instance of `Store`.
    """
    if out is None:
        out = _lz77.Store(data)
    
    if hash is None:
        hash = _Hash()

    if block_state is None:
        block_state = _lz77.BlockState(_Options(), 0, len(data))

    if isinstance(data, str):
        data = data.encode('utf-8')

    _lz77.greedy_tree(data, block_state, out, hash)
    return out


