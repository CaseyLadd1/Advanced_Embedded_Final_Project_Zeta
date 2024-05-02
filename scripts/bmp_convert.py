#!/usr/bin/env python3
# Run with e.g.
# echo images/*.bmp | sed 's/ /\n/g' | grep -E '^images/[0-9]' | xargs ./scripts/bmp_convert.py > graphics/MOOD_graphics.h
from PIL import Image, ImageOps
import numpy as np

import io
from pathlib import Path
import re
import sys

def convert565(r, g, b):
    red = np.left_shift(np.bitwise_and(r, 0xF8), 8)
    green = np.left_shift(np.bitwise_and(g, 0xFC), 3)
    blue = np.right_shift(b, 3)
    return red + green + blue

def convert(path):
    im = ImageOps.flip(Image.open(path).convert("RGB"))
    data = np.array(im.getdata(), dtype='uint8')
    r, g, b = data.T.astype('uint16')
    return convert565(r, g, b)

def printHeader():
    print('''#ifndef MOOD_GRAPHICS_H
#define MOOD_GRAPHICS_H
#include <stdint.h>
''')

def printFooter():
    print('''};
#endif''')

def sanitizeMacro(s):
    return re.sub('[^A-Z0-9]+','_', s.upper())

file = io.StringIO("\nconst uint16_t BitMapValues[] = {\n")
file.seek(0, 2)
offset = 0

files = sys.argv[1:]
printHeader()
for i in range(len(files)):
    notLastFile = i != len(files)-1
    fname = files[i]
    path = Path(fname)
    if not path.exists():
        print("Doesn't exist, skipping: {}".format(fname))
        continue
    packed = convert(path)
    print("#define BMP_{}_OFFSET {:#x}".format(sanitizeMacro(path.stem), offset))
    offset += len(packed)
    file.write('/* begin {} */\n'.format(fname))
    for j in range(len(packed)):
        file.write('{:#06x}'.format(packed[j]))
        notLast = j != len(packed)-1

        # Write a comma unless we are the last element of the last file.
        if notLastFile or notLast:
            file.write(',')
        # Break every eight elements. Also break at the end of a file.
        if (j+1) % 8 and notLast:
            file.write(' ')
        else:
            file.write('\n')
    file.write('/* end {} */\n'.format(fname))
    if notLastFile:
        file.write('\n')

file.seek(0)
print(file.read(), end='')
printFooter()
