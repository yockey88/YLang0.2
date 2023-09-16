import subprocess , sys , os

print("Building Documentation\n")
ret = subprocess.call(["doxygen" , "docs/ylang_doxyfile"])

if (ret == 0):
    ret = subprocess.call(["wsl.exe" , "make"] , cwd="{}/docs/latex".format(os.getcwd()))


sys.exit(ret)