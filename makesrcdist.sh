#!/bin/sh
# A shell-script to make an upplay source distribution. qmake is not
# too good at this...

fatal() {
    echo $*
    exit 1
}
usage() {
    echo 'Usage: makescrdist.sh -t -s do_it'
    echo ' -t : no tagging'
    echo ' -s : snapshot release: use date instead of VERSION'
    echo ' -s implies -t'
    exit 1
}

create_tag() {
    git tag -f -a $1 -m "Release $1 tagged"  || fatal tag failed
}
test_tag() {
    git tag -l | egrep '^'$1'$'
}

#set -x

TAR=${TAR-/bin/tar}

if test ! -f upplay.cpp;then
    echo "Should be executed in the master upplay directory"
    exit 1
fi
targetdir=${targetdir-/tmp}
dotag=${dotag-yes}
snap=${snap-no}

while getopts ts o
do	case "$o" in
	t)	dotag=no;;
	s)	snap=yes;dotag=no;;
	[?])	usage;;
	esac
done
shift `expr $OPTIND - 1`

test $dotag = "yes" -a $snap = "yes" && usage

test $# -eq 1 || usage

echo dotag $dotag snap $snap

if test $snap = yes ; then
  version=`date +%F_%H-%M-%S`
  TAG=""
else
    version=`egrep ^VERSION upplay.pro | cut -d'=' -f 2`
    # trim whitespace
    version=`echo $version | xargs`
    TAG="UPPLAY_$version"
fi

if test "$dotag" = "yes" ; then
    echo Creating AND TAGGING version $version
    test_tag $TAG  && fatal "Tag $TAG already exists"
else
    echo Creating version $version, no tagging
fi
sleep 2
	

editedfiles="`git status -s |\
egrep -v 'makesrcdist.sh|excludefile|manifest.txt'`"
if test "$dotag" = "yes" -a ! -z "$editedfiles"; then
    fatal  "Edited files exist: " $editedfiles
fi


if test $dotag = yes ; then
    releasename=upplay-$version
else
    releasename=betaupplay-$version
fi

topdir=$targetdir/$releasename
if test ! -d $topdir ; then
    mkdir $topdir || exit 1
else 
    echo "Removing everything under $topdir Ok ? (y/n)"
    read rep 
    if test $rep = 'y';then
    	rm -rf $topdir/*
    fi
fi

# Clean up this dir and copy the dist-specific files 
make distclean
yes | clean.O
rm -f lib/*.dep
# Possibly clean up the cmake stuff

$TAR chfX - excludefile .  | (cd $topdir;$TAR xf -)
if test $snap = "yes" ; then
    sed -e "/^VERSION/cVERSION=$version" $topdir/upplay.pro > $topdir/toto
    mv -f $topdir/toto $topdir/upplay.pro
fi

# Can't now put ./Makefile in excludefile, gets ignored everywhere. So delete
# the top Makefile here (its' output by configure on the target system):
rm -f $topdir/Makefile

out=$releasename.tar.gz
(cd $targetdir ; \
    $TAR chf - $releasename | \
    	gzip > $out)
echo "$targetdir/$out created"

# Check manifest against current reference
export LANG=C
tar tzf $targetdir/$out | sort | cut -d / -f 2- | \
    diff manifest.txt - || fatal "Please fix file list manifest.txt"

# We tag .. as there is the 'packaging/' directory in there
[ $dotag = "yes" ] && create_tag $TAG
