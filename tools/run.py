import globals
import os , subprocess , sys

args = globals.ProcessArguments(sys.argv)

config = "Debug"

exepath = "{}/bin/{}/{}".format(os.getcwd() , config , globals.MAIN_NAME)
ret = 0

if globals.IsWindows():
    call_args = ["cmd.exe" , "/c" , "{}\\run.bat".format(globals.TOOLS_DIR) , config , globals.MAIN_NAME]
    call_args.extend(args)
    ret = subprocess.call(call_args , cwd=os.getcwd())
else:
    call_args = ["{}".format(exepath)]
    call_args.extend(args)
    ret = subprocess.call(call_args , cwd=exepath)

sys.exit(ret)