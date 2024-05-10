import _io

with open("base_library.zip", "rb") as f:
    import _zipimport

    b = _zipimport.zipimporter("assets:/python38/base_library.zip", f.read(), "/end")
    # print(b.find_loader("encodings.utf_32_be",))

# b = _zipimport.zipimporter("../python3.8-importlib/base_library.zip/end", )
