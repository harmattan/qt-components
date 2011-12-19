#!/bin/sh

SCRIPTS_DIR=$PWD/../../../scripts
REGEX_DIR=$SCRIPTS_DIR/re
QML_DIR=qml

# create ssu and cssu sources
for version in ssu cssu; do
    $SCRIPTS_DIR/patch.py $version $QML_DIR patches $version
    $SCRIPTS_DIR/migrate.py $REGEX_DIR/$version.re $QML_DIR/$version
done
