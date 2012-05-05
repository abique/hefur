#! /bin/bash

VERSION=$(<VERSION)
DISTDIR=hefur-$VERSION

sed -i "s/PROJECT_NUMBER.*/PROJECT_NUMBER=$VERSION/g" Doxyfile
rm -rf $DISTDIR
mkdir $DISTDIR

cp -R AUTHORS CMakeLists.txt ChangeLog dist.sh LICENSE README src VERSION \
    $DISTDIR

tar -cvvv $DISTDIR | xz -ze9c >hefur-$VERSION.tar.xz
rm -rf $DISTDIR

md5sum hefur-$VERSION.tar.xz >hefur-$VERSION.tar.xz.md5sum