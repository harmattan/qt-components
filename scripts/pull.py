#!/usr/bin/python

"""
This scripts, based on a source file, inspect a set of dirs and fetch files
required by this source file in order of preference. In case of duplicates, 
more important files are fetched in order of preference from left ro right
"""

from itertools import ifilter
from shutil import copy
from md5 import new as md5
from os import walk, path, popen, makedirs, sep, system

LN="ln -s {source} {destination}"

def traversedir(where):
    for root, dirs, candidates in walk(where):
        for filename in ifilter(lambda x: '.' in x, candidates):
            yield(path.join(root, filename))

def parse_tree(where, files=None):
    files = files or {}
    for abspath in ifilter(lambda x: '.' in x, traversedir(where)):
        if abspath.endswith(('.h', '.cpp','.qml', '.js',)):
            files.setdefault(abspath[len(where):], abspath)
    return files

def destination(dest ,partial=None):
    if partial:
        partial = path.dirname(partial)
        partial = partial[1:] if len(partial) > 0 and partial[0] == sep else partial
    return path.join(path.abspath(dest), partial or "")

def link(source, dest, partial=None):

    LN="ln -s {source} {destination}"

    try:
        dest = destination(dest, partial)
        path.exists(dest) or makedirs(dest)
        if not path.exists(dest + path.basename(source)):
            system(LN.format(source=path.abspath(source), destination=dest))
    except Exception, ex:
        pass


if __name__ == '__main__':
    import sys
    if len(sys.argv) < 3:
        sys.stderr.write("Usage: pull.py <source_dir> <result_dir>\n")
        exit(255)

    ignored = {}
    candidates, files = parse_tree(sys.argv[1]), parse_tree(sys.argv[2])
    for key, values in candidates.iteritems():
        if key in files:
            ignored[key] = values
            continue
        # copy file
        link(values, sys.argv[2], key)

    print "ignored: " + str(int((len(ignored) * 100) / len(candidates))) + "% (" + str(len(ignored)) + " of " + str(len(candidates)) + ")"
    


    
    
