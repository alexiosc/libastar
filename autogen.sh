#!/bin/sh
#
# $Id: autogen.sh 240 2008-02-05 00:08:10Z alexios $
#
# $Log$

# Automake version. Do NOT remove the dash
amver="" # -1.10
# Autoconf version. No dash here.
acver="" #2.50

#echo Copying ltdl.
#libtoolize --copy --ltdl
#cat <<EOF
#
#Warnings about existing (or nonexistent) files above are acceptable.
#
#We will now use GNU automake$amver and GNU autoconf$acver. Change $0
#to select different versions.
#EOF
(automake${amver} --version) < /dev/null > /dev/null 2>&1 || {
	echo;
	echo "You must have automake${amver} installed";
	echo;
	exit;
}

(autoconf${acver} --version) < /dev/null > /dev/null 2>&1 || {
	echo;
	echo "You must have autoconf${acver} installed";
	echo;
	exit;
}

echo "Generating configuration files, please wait...."
echo;

#autopoint
aclocal${amver} $ACLOCAL_FLAGS -I m4
autoheader${acver};
automake${amver} --add-missing;
autoconf${acver};

echo "Running configure $@"
echo;

./configure $@
