#!/usr/bin/env python

from datetime import datetime
import sys

origin = datetime(2021, 1, 27)
now = datetime.now()
build_number = int((now - origin).total_seconds())

bc_src_string ='// This file was generated automatically. Do not modify it\n#include "version.h"\n'

from importlib.util import spec_from_loader, module_from_spec
from importlib.machinery import SourceFileLoader

spec = spec_from_loader("version", SourceFileLoader("version", "version.ini"))
version = module_from_spec(spec)
spec.loader.exec_module(version)

def get_book_variable_module_name(module_name):
    module = globals().get(module_name, None)
    book = {}
    if module:
        book = {key: value for key, value in module.__dict__.items() if not (key.startswith('__') or key.startswith('_'))}
    return book


book = get_book_variable_module_name('version')
book['BUILD'] = build_number;
for key, value in book.items():
    if isinstance(value, int):
        if(value<256):
            bc_src_string += "const u8   {:<10} = {:>12};\n".format(key,value)
        else:
            bc_src_string += "const u32  {:<10} = {:>12};\n".format(key, value)
    elif isinstance(value, str):
        if(len(value)==1):
            bc_src_string += "const char {:<10} = {:>12};\n".format(key, f"'{value}'")

print(f"{book['MAJOR']}.{book['MINOR']}.{build_number}{book['BUILD_TYPE']}")

bc_src = open("arm9/source/build_counter.c", "w")
bc_src.write(bc_src_string)
bc_src.close()