#!/bin/bash
IFS=$'\n'        #bash循环默认使用空格作为分隔的依
set -Exeuo pipefail
trap cleanup SIGINT SIGTERM ERR EXIT
cleanup() {
  trap - SIGINT SIGTERM ERR EXIT
  # script cleanup here
}

while read -r name;do
    sed -i 's/；/; /g;  s/，/, /g;  s/。/. /g; s/）/) /g;  s/（/ (/g; s/！/! /g; s/：/: /g; s/？/? /g;'  "${name}"
    sed -i 's/“/\" /g;'  "${name}"
    sed -i 's/”/\" /g'   "${name}"
    sed -i 's/《/<< /g'  "${name}"
    sed -i 's/》/>> /g'   "${name}"
done < <(fdfind -e={org,md})
