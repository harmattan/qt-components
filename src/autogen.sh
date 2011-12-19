#!/bin/sh

SCRIPTS_DIR=$PWD/../scripts
REGEX_DIR=$SCRIPTS_DIR/re

# link meego sources
$SCRIPTS_DIR/pull.py meego fremantle

# create ssu and cssu sources
for version in ssu cssu; do
    $SCRIPTS_DIR/patch.py $version fremantle fremantle/patches $version
    $SCRIPTS_DIR/migrate.py $REGEX_DIR/$version.re fremantle/$version
done
