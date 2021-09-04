import sys
import shutil
import re
import os

args = sys.argv[1:]
root = os.path.dirname(os.path.abspath(__file__))

srcmodule = os.path.join("Realities")
if len(args) > 0: srcmodule = args[0]
srcpath = os.path.join("Source", srcmodule)

dstpath = os.path.join("RealitiesGame", "Source")
if len(args) > 1: dstpath = args[1]

redirectorpath = os.path.join(root, "redirectors.txt")
if len(args) > 2: redirectorpath = args[2]

modulepath = os.path.join(root, "modules.txt")
if len(args) > 3: modulepath = args[3]

redirects = []

# Build file content
def buildfile(filepath, module):

    data = []
    with open(filepath, "rt") as fin:
        data = fin.read()

    # Generate redirects
    if filepath.endswith(".h"):

        for match in re.finditer(r'UCLASS.*[.|\n]*class( [A-Z]*_API)? [UA]([a-zA-Z_]*) :', data):
            redirects.append("+ClassRedirects=(OldName=\"/Script/{srcmodule}.{classname}\",NewName=\"/Script/{module}.{classname}\")\n".format(srcmodule=srcmodule, module=module, classname=match.group(2)))

        for match in re.finditer(r'UINTERFACE.*[.|\n]*class( [A-Z]*_API)? U([a-zA-Z_]*)(\s:|\n)', data):
            redirects.append("+ClassRedirects=(OldName=\"/Script/{srcmodule}.{classname}\",NewName=\"/Script/{module}.{classname}\")\n".format(srcmodule=srcmodule, module=module, classname=match.group(2)))

        for match in re.finditer(r'UENUM.*[.|\n]*enum class (E[a-zA-Z_]*) :', data):
            redirects.append("+EnumRedirects=(OldName=\"/Script/{srcmodule}.{enumname}\",NewName=\"/Script/{module}.{enumname}\")\n".format(srcmodule=srcmodule, module=module, enumname=match.group(1)))

        for match in re.finditer(r'USTRUCT.*[.|\n]*struct( [A-Z]*_API)? F([a-zA-Z_]*)(\s:|\n)', data):
            redirects.append("+StructRedirects=(OldName=\"/Script/{srcmodule}.{structname}\",NewName=\"/Script/{module}.{structname}\")\n".format(srcmodule=srcmodule, module=module, structname=match.group(2)))
    
    # Remove API definitions
    data = data.replace(srcmodule.upper(), module.upper())

    # Remove paths
    data = data.replace(srcmodule + "/", "")

    with open(filepath, "wt") as fout:
        fout.write(data)

# Build all files
def buildAll(dirpath, module):

    if os.path.isdir(dirpath):
        # Recursively build all nodes in this folder
        for fn in os.listdir(dirpath):
            buildAll(os.path.join(dirpath, fn), module)
    else:
        # Build file content
        buildfile(dirpath, module)

# Rename for each module
with open(modulepath, "rt") as fin:
    for line in fin.readlines():

        dstmodule = line.replace(' ', '').replace('\n', '').replace('\r', '')

        srcdir = os.path.join(root, "..", srcpath, dstmodule)
        dstdir = os.path.join(root, "..", dstpath, dstmodule)

        if os.path.exists(srcdir):
            for fn in os.listdir(srcdir):
                src = os.path.join(srcdir, fn)
                dst = os.path.join(dstdir, fn)

                if os.path.isdir(src):
                    shutil.copytree(src, dst, symlinks=False, ignore=None, copy_function=shutil.copy2, ignore_dangling_symlinks=False, dirs_exist_ok=False)
                else:
                    shutil.copy2(src, dst)

        buildAll(dstdir, dstmodule)

with open(redirectorpath, "wt") as fout:
    fout.writelines(redirects)