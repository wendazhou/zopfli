import pyzopfli

def test_compress_roundtrip():
    data = 'this is a test bytes sequence'
    raw = pyzopfli.compress(data.encode('utf-8'))
    decoded = pyzopfli.decompress(raw).decode('utf-8')
    assert data == decoded