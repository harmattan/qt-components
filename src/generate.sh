#!/bin/sh

SCRIPTS_DIR=fremantle/patches/scripts
REGEX_DIR=$SCRIPTS_DIR/re

# link meego sources
$SCRIPTS_DIR/pull.py meego fremantle

# create fremantle sources
$SCRIPTS_DIR/patch.py 1.0 fremantle fremantle/patches PR1.3
$SCRIPTS_DIR/migrate.py $REGEX_DIR/harmattan.re fremantle/PR1.3

# create cssu sources
$SCRIPTS_DIR/patch.py 1.1 fremantle fremantle/patches PR1.4