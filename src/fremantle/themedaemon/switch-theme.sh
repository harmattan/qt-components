#!/bin/bash

### This script allows to switch between themes

if [ $# -lt 1 ]; then
  echo "Usage: ./switch-theme.sh theme"
else
  $(which gconftool-2) -s --type string /qtcomonents/theme "${1}"
fi
