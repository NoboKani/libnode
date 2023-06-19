from . import config
import os
import subprocess
import shutil

os.chdir('node-{}'.format(config.nodeVersion))

print(os.getcwd())

shutil.copyfile('../patch/node/src/node_embedding_api.cc', './src/node_embedding_api.cc')
shutil.copyfile('../patch/node/src/node_embedding_api.h',  './src/node_embedding_api.h')

try:
    subprocess.check_call(['patch', '-p1', '-i', '../patch/node.patch'])
except subprocess.CalledProcessError:
    print("error")
