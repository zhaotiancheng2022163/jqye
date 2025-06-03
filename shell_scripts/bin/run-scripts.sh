#!/bin/bash

readarray  SCRIPTS < <(fdfind -c never ".*" "${HOME}"/.local/bin)

if [[ -z "$*" ]];then
	for i in "${SCRIPTS[@]}";do
		# echo $(basename "$i")
        echo "${i##*/}"
	done
elif [[ -n "$*" ]];then

     declare -A array

     for i in  ${SCRIPTS[*]};do
	           # array[$(basename "$i")]="$i"
	           array[${i##*/}]="$i"
     done
    # coproc name {  ${array["$*"]} }
    # coproc ( ${array["$*"]} > /dev/null 2>&1 )
    coproc ( ${array["$*"]} &> /dev/null)
fi