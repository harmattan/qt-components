#!/bin/sh

SCRIPTS_DIR=../../../src/fremantle/patches/scripts
REGEX_DIR=$SCRIPTS_DIR/re

# create ssu sources
$SCRIPTS_DIR/patch.py ssu qml patches ssu
$SCRIPTS_DIR/migrate.py $REGEX_DIR/harmattan.re qml/ssu

# create cssu sources
$SCRIPTS_DIR/patch.py cssu qml patches cssu