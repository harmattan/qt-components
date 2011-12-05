#!/bin/sh

SCRIPTS_DIR=$PWD/../../../src/fremantle/patches/scripts
REGEX_DIR=$SCRIPTS_DIR/re

QML_SOURCES=../../meego/`basename $PWD`
QML_DIR=qml
QML_LEVEL=4

# link meego sources
$SCRIPTS_DIR/pull.py $QML_SOURCES/$QML_DIR $QML_DIR

# create ssu and cssu sources
for version in ssu cssu; do
    $SCRIPTS_DIR/patch.py $version $QML_DIR patches $version $QML_LEVEL
    $SCRIPTS_DIR/migrate.py $REGEX_DIR/$version.re $QML_DIR/$version
done
