#!/bin/bash

set -e

if [ ! -f "$3" ] && [ ! -f "$1" ]; then
    echo "This script must be run via uscan or by manually specifying the tarball" >&2
    exit 1
fi

tarball=

[ -f "$3" ] && tarball="$3"
[ -z "$tarball" -a -f "$1" ] && tarball="$1"

fname="$(basename "$tarball")"
tarball="$(readlink -f "$tarball")"

tdir="$(mktemp -d)"
trap '[ ! -d "$tdir" ] || rm -r "$tdir"' EXIT

tar -xzf "$tarball" -C "$tdir"

cyrusdir="$(find "$tdir" -mindepth 1 -maxdepth 1 -type d)"

cd "$cyrusdir"

rm -f config.h config.h.in configure config.cache config.status config.log stamp-config confdefs.h
rm -rf autom4te.cache
rm -f mkinstalldirs install-sh config.sub config.guess
rm -f netnews/Makefile perl/imap/Makefile.old perl/sieve/managesieve/Makefile.old snmp/Makefile Makefile et/Makefile
rm -f doc/pod2htm* doc/murder.png
rm -f master/makedepend.log 
rm -rf doc/man
rm -f sieve/addr.c sieve/addr.h sieve/addr-lex.c sieve/sieve.c sieve/sieve.h sieve/sieve-lex.c

cd "$tdir"
tar -czf $tdir/${fname} "$(basename "$cyrusdir")"

mv "$tarball" "$tarball.bkp"
mv "$tdir/$fname" "$tarball"
