#            Copyright Paul Colby 2013 - 2015.
# Distributed under the Boost Software License, Version 1.0.
#       (See accompanying file LICENSE.md or copy at
#          http://www.boost.org/LICENSE_1_0.txt)
#
# Basic utility script for creating source RPMs from Github.
#
# Usage: make-src-rpm.sh [branch|commit|tag]
#
# If not specified, the 'master' branch will be used by default.
#

# Command aliases
BASENAME=`which basename` || { echo 'Required command not found: basename' >&2; exit 1; }
RPM=`which rpm`           || { echo 'Required command not found: rpm'      >&2; exit 1; }
RPMBUILD=`which rpmbuild` || { echo 'Required command not found: rpmbuild' >&2; exit 1; }
RPMLINT=`which rpmlint`   || { echo 'Required command not found: rpmlint'  >&2; exit 1; }
WGET=`which wget`         || { echo 'Required command not found: wget'     >&2; exit 1; }

# RPM environment
SOURCEDIR=`${RPM} --eval %{_sourcedir}` || { echo 'Failed to eval %{_sourcedir}' >&2; exit 2; }
SPECDIR=`${RPM} --eval %{_specdir}`     || { echo 'Failed to eval %{_sourcedir}' >&2; exit 2; }
SRCRPMDIR=`${RPM} --eval %{_srcrpmdir}` || { echo 'Failed to eval %{_sourcedir}' >&2; exit 2; }

# Project information
USER=pcolby
PROJECT=pcp-pmda-cpp
BRANCH=master

if [ $# -gt 1 ]; then echo "Usage: `$BASENAME $0` [branch|commit|tag]" >&2; exit 3; fi
if [ $# -eq 1 ]; then BRANCH=$1; shift; fi

echo 'Setting umask 0133'
umask 0133 || exit

URL=https://raw.githubusercontent.com/${USER}/${PROJECT}/$BRANCH/package/rpm/${PROJECT}.spec
SPECFILE=${SPECDIR}/${PROJECT}.spec
echo "Fetching: ${URL}"
${WGET} -qO ${SPECFILE} ${URL} || exit
VERSION=`rpmspec --query --srpm --queryformat '%{version}' ${SPECFILE}`
RELEASE=`rpmspec --query --srpm --queryformat '%{release}' ${SPECFILE} --undefine dist`
echo "Wrote: ${SPECFILE} [${VERSION}-${RELEASE}]"
echo "Checking: ${SPECFILE}"
${RPMLINT} ${SPECFILE} || exit

URL=https://github.com/${USER}/${PROJECT}/archive/${BRANCH}.tar.gz
SOURCE=${SOURCEDIR}/${PROJECT}-${VERSION}.tar.gz
echo "Fetching: ${URL}"
${WGET} -qO ${SOURCE} ${URL} || exit
echo "Wrote: ${SOURCE}"

SRCRPM=${PROJECT}-${VERSION}-${RELEASE}.src.rpm
echo "Building: ${SRCRPM}"
${RPMBUILD} -bs --undefine dist ${SPECFILE} || exit
echo "Checking: ${SRCRPM}"
${RPMLINT} ${SRCRPMDIR}/${SRCRPM} || exit
