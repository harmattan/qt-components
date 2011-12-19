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

PATCH="patch -d {directory} -p{level} < {patch} "

def traversedir(where):
    for root, dirs, candidates in walk(where):
        for filename in ifilter(lambda x: '.' in x, candidates):
            yield(path.join(root, filename))

def parse_tree(where, files=None):
    files = files or {}
    for abspath in ifilter(lambda x: '.' in x, traversedir(where)):
        if abspath.endswith(('.h', '.cpp','.qml','.js',)) and 'ssu' not in abspath:
            files.setdefault(path.basename(abspath), []).append((abspath, abspath[len(where):],))
    return files

def parse_patch_tree(where, theme=None, offset=2):
    theme = type(theme) in (dict,) or {}
    for abspath in ifilter(lambda x: '.' in x, traversedir(where)):
        if abspath.endswith(('.diff',)):
            dirname = path.basename(path.dirname(abspath[len(where):]))
            theme.setdefault(path.basename(abspath), {})[dirname] = (abspath, str(abspath[len(where):]).count(sep) + offset)
    return theme

def destination(dest ,partial=None):
    if partial:
        partial = path.dirname(partial)
        partial = partial[1:] if len(partial) > 0 and partial[0] == sep else partial
    return path.join(path.abspath(dest), partial or "")

def link(source, dest, partial=None):

    COPY="cp {source} {destination}"

    try:
        dest = destination(dest, partial)
        path.exists(dest) or makedirs(dest)
        if not path.exists(dest + path.basename(source)):
            system(COPY.format(source=path.abspath(source), destination=dest))
    except Exception, ex:
        pass


if __name__ == '__main__':
    import sys
    if len(sys.argv) < 5:
        sys.stderr.write("Usage: patch.py <version> <source_dir> <patch_dir> <dest_dir> <extra_level>\n")
        exit(255)

    #patch offset
    offset = int(sys.argv[5]) if len(sys.argv) == 6 else 2
        
    patched = {}
    patches = {
        'common'    : parse_patch_tree(path.join(sys.argv[3], 'common'), None, offset),
        sys.argv[1] : parse_patch_tree(path.join(sys.argv[3], sys.argv[1]), None, offset),
        }

    candidates = parse_tree(sys.argv[2])
    for key, values in candidates.iteritems():
        # link files
        for value in values:
            dest = destination(path.join(sys.argv[2], sys.argv[4]), value[1])
            link(value[0], dest)
            for version in ('common', sys.argv[1],):
                base, diff = path.basename(dest), key + '.diff'
                if diff in patches[version] and base in patches[version][diff]:
                    ppath, dept = patches[version][diff][base]
                    #print PATCH.format(directory=dest, patch=path.abspath(ppath), level=dept)
                    system(PATCH.format(directory=dest, patch=path.abspath(ppath), level=dept))
                    patched[key + value[1]] = True

            
    print "patched: " + str(int((len(patched) * 100) / len(candidates))) + "% (" + str(len(patched)) + " of " + str(len(candidates)) + ")"
