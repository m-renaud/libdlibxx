# Maintainer: Matt Renaud <mrenaud92@gmail.com>
pkgname=libdlibxx
pkgver=1.0.0
pkgrel=1
epoch=
pkgdesc="A C++11 wrapper around dlfcn.h"
arch=('i686' 'x86_64')
url="https://github.com/mrenaud92/libdlibxx"
license=('BSD')
depends=('gcc-libs' 'cmake' 'boost-libs')
provides=('libdlibxx')
options=()
install=$pkgname.install
source=("http://mrenaud.ca/projects/$pkgname/$pkgname-$pkgver.tar.gz")
md5sums=('084321531b7755ca2bcf579506620bef')

build() {
  cd $pkgname-$pkgver

  cmake -DCMAKE_INSTALL_PREFIX="$pkgdir" .
  make
}

package() {
  cd $pkgname-$pkgver

  make install
}

# vim:set ts=2 sw=2 et:
