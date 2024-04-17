#!/usr/bin/env python

import os

for path, subdirs, files in os.walk('buckets/'):
    for name in sorted(files):
        print('> ' + os.path.join(path, name))
        print(open(os.path.join(path, name), "r").read())