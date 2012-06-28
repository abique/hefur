#! /bin/bash

PROJECT=hefur
VERSION=$(<VERSION)
DISTDIR=$PROJECT-$VERSION
DISTFILES='AUTHORS CMakeLists.txt Doxyfile ChangeLog dist.sh LICENSE README hefur VERSION mimosa manual www INSTALL'

cd manual && make && cd -

sed -i "s/PROJECT_NUMBER.*/PROJECT_NUMBER=$VERSION/g" Doxyfile
rm -rf $DISTDIR
mkdir $DISTDIR

cp -R $DISTFILES $DISTDIR
rm -rf $DISTDIR/www/bootstrap
rm -rf $DISTDIR/www/update-bootstrap.sh
rm -rf $DISTDIR/mimosa/.git*

tar -cvvv $DISTDIR | xz -ze9c >$PROJECT-$VERSION.tar.xz
rm -rf $DISTDIR

md5sum $PROJECT-$VERSION.tar.xz >$PROJECT-$VERSION.tar.xz.md5sum
