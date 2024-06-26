# Copyright (c) 2020, Riverbank Computing Limited
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


import csv
import marshal
import os
import sys


def freeze_as_data(py_filename, data_filename, embedded_name):
    """ Freeze a Python source file and save it as data. """

    code = _get_marshalled_code(py_filename, embedded_name)

    data_file = open(data_filename, 'wb')
    data_file.write(code)
    data_file.close()


def freeze_as_c(py_filename, c_filename, embedded_name):
    """ Freeze a Python source file and save it as C source code. """

    code = _get_marshalled_code(py_filename, os.path.basename(py_filename))

    c_file = open(c_filename, 'wt')

    c_file.write(
            'static unsigned char frozen_%s[] = {' % embedded_name)

    for i in range(0, len(code), 16):
        c_file.write('\n    ')
        for j in code[i:i + 16]:
            c_file.write('%d, ' % j)

    c_file.write('\n};\n')

    c_file.close()


def _get_marshalled_code(py_filename, embedded_name):
    """ Convert a Python source file to a marshalled code object. """

    try:
        source_file = open(py_filename, 'rb')
    except Exception as e:
        sys.stderr.write("{0}: {1}\n".format(py_filename, str(e)))
        sys.exit(1)

    source = source_file.read()
    source_file.close()

    co = compile(source, embedded_name, 'exec')

    return marshal.dumps(co)

py_filename = r"_bootstrap.py"
py_filename = r"bootstrap-3.8.0.py"
out_filename = r"frozen_bootstrap.h"
embedded_name = "pyqtdeploy_bootstrap"
if True:
    freeze_as_c(py_filename, out_filename, embedded_name)
else:
    freeze_as_data(py_filename, out_filename, embedded_name)
    
py_filename = r"_bootstrap_external.py"
py_filename = r"bootstrap_external-3.8.10.py"
out_filename = r"frozen_bootstrap_external.h"
embedded_name = "pyqtdeploy_bootstrap_external"
if True:
    freeze_as_c(py_filename, out_filename, embedded_name)
else:
    freeze_as_data(py_filename, out_filename, embedded_name)
    
py_filename = r"_zipimport.py"
out_filename = r"frozen_zipimport.h"
embedded_name = "pyqtdeploy_zipimport"
if True:
    freeze_as_c(py_filename, out_filename, embedded_name)
else:
    freeze_as_data(py_filename, out_filename, embedded_name)
    
py_filename = r"iimp.py"
out_filename = r"frozen_iimp.h"
embedded_name = "pyqtdeploy_iimp"
if True:
    freeze_as_c(py_filename, out_filename, embedded_name)
else:
    freeze_as_data(py_filename, out_filename, embedded_name)
    
