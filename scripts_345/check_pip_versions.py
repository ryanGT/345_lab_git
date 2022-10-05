#!/usr/bin/env python3
import numpy, scipy, control
mod_list1 = [numpy, scipy, control]

for mod in mod_list1:
    print("%s : version = %s" % (mod.__name__, mod.__version__))


import pybd_gui, py_block_diagram, krauss_misc
krauss_mod_list = [pybd_gui, py_block_diagram, krauss_misc]

for mod in krauss_mod_list:
    print("%s : version = %s" % (mod.__name__, mod.version))

