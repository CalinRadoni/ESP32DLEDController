#!/bin/bash

declare -i res

. ${IDF_PATH}/export.sh

export CodeLib_PATH="/github/workspace/lib"
cd /github/workspace/app/example

idf.py build
res=$?

echo "::set-output name=result::$res"
