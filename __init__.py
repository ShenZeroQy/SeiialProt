
from distutils.core import setup
from Cython.Build import cythonize
import os

#for Cpython, run as root

current_dir = os.getcwd()
print("Current working directory:", current_dir)

# 改变当前工作目录
new_dir = current_dir+'/CpyProt'
print("go to working directory:", new_dir)
os.chdir(new_dir)

if os.path.exists("Prot.c"):
    os.remove("Prot.c")
setup(
    ext_modules=cythonize("Prot.pyx"),
    script_args=['build_ext', '--inplace'],
    # ext_package='Pyserial_main.CpyProt', 
    # packages=["CpyProt_package"],

)
os.chdir(current_dir)
print("return to working directory:", current_dir)
