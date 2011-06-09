#! /bin/sh
 
# autogen.sh
#
# Copyright (c) 2009  BMW
# Copyright (c) 2002  Daniel Elstner  <daniel.elstner@gmx.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License VERSION 2 as
# published by the Free Software Foundation.  You are not allowed to
# use any other version of the license; unless you got the explicit
# permission from the author to do so.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

PATHTOAUTOCONF=$(which autoconf)
PATHTOAUTOMAKE=$(which automake)
PATHTOLIBTOOL=$(which libtoolize)
PATHTOGLIBTOOL=
if [ "$PATHTOLIBTOOL" = "" ]; then
  PATHTOGLIBTOOL=$(which glibtoolize)
fi

if [ "$PATHTOAUTOCONF" = "" ]; then
  echo
  echo ">> GNU autoconf has not been found!"
  echo ">> Please install it first and then re-run the script!"
  echo
  exit 1
elif [ "$PATHTOAUTOMAKE" = "" ]; then
  echo
  echo ">> GNU automake has not been found!"
  echo ">> Please install it first and then re-run the script!"
  echo
  exit 1
elif [ "$PATHTOLIBTOOL" = "" ] && [ "$PATHTOGLIBTOOL" = "" ]; then
  echo
  echo ">> GNU libtool has not been found!"
  echo ">> Please install it first and then re-run the script!"
  echo
  exit 1
fi

if [ "$PATHTOLIBTOOL" = "" ]; then
  LIBTOOLIZE="glibtoolize"
else
  LIBTOOLIZE="libtoolize"
fi

dir=`echo "$0" | sed 's,[^/]*$,,'`
test "x${dir}" = "x" && dir='.'

if test "x`cd "${dir}" 2>/dev/null && pwd`" != "x`pwd`"
then
    echo "This script must be executed directly from the source directory!"
    exit 1
fi

if !(test -d admin); then
  mkdir admin
fi

rm -f config.cache

if test -e admin/depcomp          && \
   test -e admin/install-sh       && \
   test -e admin/ltmain.sh        && \
   test -e admin/missing          && \
   test -e aclocal.m4
then
    echo ">> autoreconf"          && \
    autoreconf                    && exit 0
else
    echo ""                       && \
    echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" && \
    echo "+ This script prepares the building environment for the configuration of musrfit. +" && \
    echo "+ During this process some informative messages might appear which can safely be  +" && \
    echo "+ ignored as long as they are not tagged as errors.                               +" && \
    echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" && \
    echo ""                       && \
    echo ">> libtoolize"          && \
    $LIBTOOLIZE --force           && \
    echo ">> aclocal"             && \
    aclocal                       && \
    echo ">> autoconf"		  && \
    autoconf                      && \
    echo ">> autoheader"          && \
    autoheader			  && \
    echo ">> automake"		  && \
    automake --add-missing --gnu  && exit 0
fi

exit 1

