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
RPM=`which rpm`
RPMBUILD=`which rpmbuild`
RPMLINT=`which rpmlint`
WGET="`which wget` --quiet"

# RPM environment
SOURCEDIR=`${RPM} --eval %{_sourcedir}`
SPECDIR=`${RPM} --eval %{_specdir}`
SRCRPMDIR=`${RPM} --eval %{_srcrpmdir}`

# Project information
USER=pcolby
PROJECT=pcp-pmda-cpp
BRANCH=master

if [ $# -gt 1 ]; then echo "Usage: `basename $0` [branch|commit|tag]" >&2; exit; fi
if [ $# -eq 1 ]; then BRANCH=$1; shift; fi

URL=https://raw.githubusercontent.com/${USER}/${PROJECT}/$BRANCH/package/rpm/${PROJECT}.spec
SPECFILE=${SPECDIR}/${PROJECT}.spec
echo "Fetching: ${URL}"
${WGET} -O ${SPECFILE} ${URL} || exit
VERSION=`rpmspec --query --srpm --queryformat '%{version}' ${SPECFILE}`
RELEASE=`rpmspec --query --srpm --queryformat '%{release}' ${SPECFILE} --undefine dist`
echo "Wrote: ${SPECFILE} [${VERSION}-${RELEASE}]"
echo "Checking: ${SPECFILE}"
${RPMLINT} ${SPECFILE} || exit

URL=https://github.com/${USER}/${PROJECT}/archive/${BRANCH}.tar.gz
SOURCE=${SOURCEDIR}/${PROJECT}-${VERSION}.tar.gz
echo "Fetching: ${URL}"
${WGET} -O ${SOURCE} ${URL} || exit
echo "Wrote: ${SOURCE}"

SRCRPM=${PROJECT}-${VERSION}-${RELEASE}.src.rpm
echo "Building: ${SRCRPM}"
${RPMBUILD} -bs --undefine dist ${SPECFILE} || exit
echo "Checking: ${SRCRPM}"
${RPMLINT} ${SRCRPMDIR}/${SRCRPM} || exit
