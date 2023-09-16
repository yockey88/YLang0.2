MAIN_NAME = "YLang0.2"
FUZZER_NAME = "Fuzzer"

TOOLS_DIR = "tools"

VMAJOR = 0
VMINOR = 0
VPATCH = 2

import sys , platform
PLATFORM = sys.platform
for x in platform.uname():
    if "microsoft" in x.lower():
        PLATFORM = "windows"
        break

def IsWindows():
    return PLATFORM == "windows"

def IsLinux():
    return PLATFORM == "linux"

def IsMac():
    return PLATFORM == "darwin"

def ProcessArguments(argv):
    ret = []
    i = 0
    
    for arg in argv:
        if i != 0:
            ret.append(arg)
        i += 1
        
    return ret