#!/bin/sh

SCRIPTS_DIR=fremantle/patches/scripts
REGEX_DIR=$SCRIPTS_DIR/re

$SCRIPTS_DIR/pull.py meego fremantle
$SCRIPTS_DIR/patch.py 1.0 fremantle fremantle/patches generated 
$SCRIPTS_DIR/migrate.py $REGEX_DIR/harmattan.re fremantle/generated/1.0
