#!/bin/sh

SCRIPTS_DIR=fremantle/patches/scripts
REGEX_DIR=$SCRIPTS_DIR/re

# link meego sources
$SCRIPTS_DIR/pull.py meego fremantle

# create fremantle-ssu sources
$SCRIPTS_DIR/patch.py ssu fremantle fremantle/patches ssu
$SCRIPTS_DIR/migrate.py $REGEX_DIR/harmattan.re fremantle/ssu

# create fremantle-cssu sources
$SCRIPTS_DIR/patch.py cssu fremantle fremantle/patches cssu
