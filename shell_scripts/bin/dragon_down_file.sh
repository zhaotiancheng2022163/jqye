#!/bin/sh
# This toy example uses wget and the --target mode to act
# as a drag-and-drop download tool.

dragon --target -T -x | while read url
do
    if [ -n "$url" ]; then
        printf "File Name: "
        name=""
        while [ -z $name ] || [ -e $name ]
        do
            read -r name
            if [ -e "$name" ]; then
                printf "File already exists, overwrite (y|n): "
                read -r ans

                if [ "$ans" = "y" ]; then
                    break
                else
                    printf "File Name: "
                fi
            fi
        done

  # Download the file with curl
  [ -n "$name" ] && curl -o "$name" "$url" || exit 1
else
    exit 1
    fi
done
