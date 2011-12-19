#!/usr/bin/python

"""
This scripts, based on a source file, inspect a set of dirs and fetch files
required by this source file in order of preference. In case of duplicates, 
more important files are fetched in order of preference from left ro right
"""

import re
import StringIO
from itertools import ifilter
from mmap import mmap
from os import walk, path, popen, makedirs

def traversedir(where):
    for root, dirs, candidates in walk(where):
        for filename in ifilter(lambda x: '.' in x, candidates):
            yield(path.join(root, filename))

def parse_tree(where, theme=[]):
    for abspath in ifilter(lambda x: '.' in x, traversedir(where)):
        if abspath.endswith('.qml'):
            theme.append(abspath)
    return theme

if __name__ == '__main__':
    import sys
    if len(sys.argv) < 3:
        sys.stderr.write("Usage: migrate.py <regex_file> <source_dir>\n")
        exit(255)

    re_expressions = []
    with open(sys.argv[1], "rb") as f:
        while(1):
            line = f.readline()
            if not line:
                break
            if len(line) == 1 or line.startswith(';;'):
                continue
            re_expressions.append((re.compile(line[:-1]), f.readline()[:-1],))

    qmlfiles = parse_tree(sys.argv[2])
    for qml in qmlfiles:
        # write a simple example file
        with open(qml, "r+b") as f:
            qml_mem = StringIO.StringIO()
            mem = mmap(f.fileno(), 0)
            while(1):
                line = mem.readline()
                if not line:
                    break
                for expr, value in re_expressions:
                    if expr.match(line):
                        line = re.sub(expr, value, line)
                        break
                qml_mem.write(line)

            # resize file buffer
            mem.resize(qml_mem.tell())

            # copy buffers
            mem.seek(0)
            mem.write(qml_mem.getvalue())

            # close buffers
            mem.close()
            qml_mem.close()
    


    
    
