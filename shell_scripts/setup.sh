#!/bin/bash

# ~/.local/bin中的可执行脚本
#--------------------------------------
if [ ! -d "${HOME}/.local/bin" ];then
   mkdir -p "${HOME}/.local/bin"
fi

while read -r i;do
    file_name="${i}"
    sym_file_name="${i##*/}"
    rm -rf "${HOME}/.local/bin"/"${sym_file_name}"
    chmod 755 "${file_name}"
    ln -nfs "${file_name}"  "${HOME}/.local/bin"/"${sym_file_name}"
done < <(find "${path}"/bin -type f)

ln -nfs "${path}"/bin/imvs "${HOME}/.local/bin"/wimvs
ln -nfs "${path}"/bin/wdisk "${HOME}/.local/bin"/ddisk
ln -nfs "${path}"/bin/wdisk "${HOME}/.local/bin"/mdisk
ln -nfs "${path}"/bin/wmusic "${HOME}/.local/bin"/dmusic
ln -nfs "${path}"/bin/gclip "${HOME}/.local/bin"/xo
ln -nfs "${path}"/bin/pclip "${HOME}/.local/bin"/xc
rm  -rf "${HOME}/.local/bin"/wvimrc
ln -nfs "${path}"/bin/wvimrc "${HOME}/.local/bin"/wvim