from distutils.core import setup
from Cython.Build import cythonize
import os

#for Cpython, run as root
if os.path.exists("Prot.c"):
    os.remove("Prot.c")
setup(
    ext_modules=cythonize("Prot.pyx"),
    script_args=['build_ext', '--inplace'],
    # ext_package='PackCpyProt', 
    # packages=["CpyProt"],
)