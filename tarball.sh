#! /bin/sh

warn () {
    echo "@@@" "$@" "@@@"
}
die () {
    warn "$@"
    exit 1
}
must () {
    "$@" || die "$@" "failed."
}
build_tooldir="/var/tmp/glenn/install/lanai"
V=`fgrep version_string gcc/version.c \
    | sed -e 's@[^"]*"\([^"]*\).*@\1@' \
    | sed -e 's/[^-._a-zA-Z0-9]/_/g'` || die "Could not extract version"
NM="lanai-gcc-$V"
tarball="../$NM.tar.bz2"

# Verify the build is likely to work.

test `git clean -nfxd | wc -l` = 0 || die "Need to \"git clean -fxd\" with care"
test `git status | fgrep modified: | wc -l` = 0 || die "Uncommitted modifications"

# Verify the user wants to build the tarball.

echo -n "Build $tarball? "
read yes
test "x$yes" = "xyes" || die "You did not type \"yes\"."

# Verify builds work, and cleanup.

must env CFLAGS="-O2 -g -m32" ./configure --target=lanai --enable-languages=c --prefix="`pwd`/install"
must make build_tooldir="$build_tooldir" -j8
must make build_tooldir="$build_tooldir" install
must git clean -fxd
test `git status | fgrep modified: | wc -l` = 0 || die "Build modified files."

# Create the tarball, including the Git commit ID in case we need to
# look at the source later.

( git log | head -1 > .git_id ) || die "Could not create .git_id"
must tar cjf "$tarball.tmp" --exclude-vcs --transform "s@^\.@$NM@" .
must rm .git_id
must mv -f "$tarball.tmp" "$tarball"
echo "Tarball is $tarball"
