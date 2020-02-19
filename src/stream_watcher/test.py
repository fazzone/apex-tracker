print("Test python")

import os

path = '.'

files = []
# r=root, d=directories, f = files
for r, d, f in os.walk(path):
    for file in f:
        print(f)
with open('MANIFEST') as f:
    print(f.read())
