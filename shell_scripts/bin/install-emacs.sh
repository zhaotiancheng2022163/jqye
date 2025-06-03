#!/bin/sh

[ "${1}" = "-h" ] && echo "Usage: EMACS_VERSION=25.1 install-emacs.sh or EMACS_VERSION=snapshot install-emacs.sh" && exit 1

[ "${1}" = "--help" ] && echo "Usage: EMACS_VERSION=25.1 install-emacs.sh or EMACS_VERSION=snapshot install-emacs.sh" && exit 1

[ -z "$EMACS_VERSION" ] && echo "Usage: EMACS_VERSION=25.1 install-emacs.sh or EMACS_VERSION=snapshot install-emacs.sh" && exit 1
[ -z "$EMACS_URL" ] && EMACS_URL="http://mirror.aarnet.edu.au/pub/gnu/emacs/"
# I've assign 12G memory to /tmp as ramdisk
[ -z "$EMACS_TMP" ] && EMACS_TMP="/tmp"

if [ "$EMACS_VERSION" != "snapshot" ]; then
    echo "curl $EMACS_URL/emacs-$EMACS_VERSION.tar.gz"
    curl $EMACS_URL/emacs-$EMACS_VERSION.tar.gz | tar xvz -C $EMACS_TMP
fi

# @see http://wiki.gentoo.org/wiki/Project:Emacs/GNU_Emacs_developer_guide
# @see http://packages.gentoo.org/package/app-editors/emacs for info on Gentoo Linux
# --without-gtk and --without-gtk3 is optional
echo "Installing Emacs ..."
if [ "$EMACS_VERSION" = "snapshot" ]; then
    cd $HOME/projs/emacs && mkdir -p $HOME/myemacs/snapshot && rm -rf $HOME/myemacs/snapshot/* && ./autogen.sh && ./configure CFLAGS="-g3 -O2 -no-pie" --prefix=$HOME/myemacs/snapshot --with-x=no --without-dbus --without-sound --with-gnutls=no && make && make install
   echo "Emacs snapshot was installed!"
elif [ "$EMACS_VERSION" = "28.1" ]; then
    cd $EMACS_TMP/emacs-$EMACS_VERSION && mkdir -p $HOME/myemacs/$EMACS_VERSION && rm -rf $HOME/myemacs/$EMACS_VERSION/* && ./configure CFLAGS="-g3 -O2 -no-pie" --prefix=$HOME/myemacs/$EMACS_VERSION --without-sound --with-modules --with-native-compilation --without-compress-install --without-gif --without-tiff && make && make install
else
    cd $EMACS_TMP/emacs-$EMACS_VERSION && mkdir -p $HOME/myemacs/$EMACS_VERSION && rm -rf $HOME/myemacs/$EMACS_VERSION/* && ./configure CFLAGS="-g3 -O2 -no-pie" --prefix=$HOME/myemacs/$EMACS_VERSION --with-x=no --without-sound  --with-modules && make && make install
    rm -rf $EMACS_TMP/emacs-$EMACS_VERSION
    echo "Emacs $EMACS_VERSION was installed!"
fi
