#!/bin/bash

BUILD="bind.hpp"
rm -f $BUILD

while IFS='' read -r line || [[ -n "$line" ]]; do
    file=`echo "$line" | awk -F'"' '{print $2}'`

    if [ -n "$file" ] && [ -f "src/$file" ]; then
        sed '/^\/\*/,/\*\// d' "src/$file" >> $BUILD
    else
        echo "$line" >> $BUILD
    fi
done < "src/build.hpp"
