def gen_h(symbol):
    return \
f"""#pragma once

#include "AstralbrewEngine/resources/font_data.hpp"

namespace Astralbrew::Resources::Fonts
{{
    extern const Astralbrew::FontData {symbol};
}}
"""

def gen_s(symbol, data):    
    str_dat=".word "
    if(len(data)%2==1):
        data.append(0)
    data=data.hex()
    data = [data[i:i+2] for i in range(0,len(data),2)]
    data = ["0x"+"".join(data[i:i+2][::-1]) for i in range(0,len(data),2)]
    data = "\n".join(["\t.hword "+", ".join(data[i:i+8]) for i in range(0,len(data),8)])
    #print(data)    
    return \
f"""
    .section .rodata
	.align	4
	.global _ZN10Astralbrew9Resources5Fonts{len(symbol)}{symbol}E
	.hidden _ZN10Astralbrew9Resources5Fonts{len(symbol)}{symbol}E
_ZN10Astralbrew9Resources5Fonts{len(symbol)}{symbol}E:
{data}
"""


import sys
import re
import os
import struct

curdir = sys.argv[1]
symbol_name = sys.argv[2]

print(curdir, symbol_name)

font_file = os.path.dirname(curdir)+"/fonts/"+symbol_name+".astralfont"
print(font_file)

data = open(font_file,"rb").read()
res = bytearray(b'')
res+=struct.pack('h',16)
res+=struct.pack('i',len(data))
res+=struct.pack('i',0)
res+=struct.pack('h',2)
res+=data

open(symbol_name+".astralfont.s","w").write(gen_s(symbol_name, res))
open(symbol_name+".astralfont.h","w").write(gen_h(symbol_name))


#open(fname,"w").write(f"""
#pragma once
#include "Astralbrew/text/font.hpp"

#extern const Astralbrew::Text::ReadOnlyFont {name};
#""")

#sname = name+".s"
#oname = name+".astralfont.o"

#open(sname,"w").write(".section text")
