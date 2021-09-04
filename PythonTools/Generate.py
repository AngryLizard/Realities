import sys
import shutil
import os

args = sys.argv[1:]
root = os.path.dirname(os.path.abspath(__file__))
template = "TemplateSystem"

path = os.path.join("RealitiesGame", "Source")
if len(args) > 0: path = args[0]

modulepath = os.path.join(root,"modules.txt")
if len(args) > 1: modulepath = args[1]

# Rename file content
def renamefile(filepath, name):

    data = []
    with open(filepath, "rt") as fin:
        data = fin.read()

    data = data.replace(template, name)
    data = data.replace(template.upper(), name.upper())
    
    with open(filepath, "wt") as fout:
        fout.write(data)

# Rename file hierarchy
def renameall(dirpath, name):

    if os.path.isdir(dirpath):
        # Recursively rename all nodes in this folder
        for fn in os.listdir(dirpath):
            renameall(os.path.join(dirpath, fn), name)
    else:
        # Rename file content
        renamefile(dirpath, name)

    # Rename folder name
    filename = os.path.basename(dirpath)
    filepath = os.path.dirname(dirpath)
    newname = filename.replace(template, name)
    trgpath = os.path.join(filepath, newname)
    os.rename(dirpath, trgpath)

# Rename for each module
with open(modulepath, "rt") as fin:
    for line in fin.readlines():

        src = os.path.join(root, template)
        dst = os.path.join(root, "..", path, template)

        shutil.copytree(src, dst, symlinks=False, ignore=None, copy_function=shutil.copy2, ignore_dangling_symlinks=False, dirs_exist_ok=False)
        renameall(dst, line.replace(' ', '').replace('\n', '').replace('\r', ''))