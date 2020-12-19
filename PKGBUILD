pkgname=qtopenguidance-git # '-bzr', '-git', '-hg' or '-svn'
pkgver=git
pkgrel=1
pkgdesc="GPS based guidance for farming equipment"
arch=('x86_64' 'armv7h' 'aarch64')
url="https://github.com/eringerli/QtOpenGuidance"
license=('GPL')
groups=()
depends=('qt5-3d' 'gmp' 'mpfr' 'kddockwidgets-eringerli-git' 'libspnav' 'boost' 'eigen')
makedepends=('git' 'cmake')
provides=("${pkgname}")
conflicts=("${pkgname}")
replaces=()
backup=()
options=()
install=
source=('QtOpenGuidance-git::git+https://github.com/eringerli/QtOpenGuidance'
        'https://github.com/CGAL/cgal/releases/download/v5.1.1/CGAL-5.1.1.tar.xz')
noextract=()
sha256sums=('SKIP'
            '162250d37ab85017041ad190afa1ef5146f8b08ed908d890a64d8dbaa5910ca0')

# Please refer to the 'USING VCS SOURCES' section of the PKGBUILD man page for
# a description of each element in the source array.

pkgver() {
  cd "QtOpenGuidance-git"
  ( set -o pipefail
    git describe --long 2>/dev/null | sed 's/\([^-]*-g\)/r\1/;s/-/./g' ||
    printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
  )
}

prepare() {
  cd "QtOpenGuidance-git"
  git submodule init
  git submodule update

  cd lib/
  if [ ! -L "CGAL-5.1.1" ]; then
    ln -s "$srcdir/CGAL-5.1.1"
  fi
}

build() {
    cmake -B build -S "QtOpenGuidance-git" \
        -DCMAKE_BUILD_TYPE='None' \
        -DCMAKE_INSTALL_PREFIX='/usr' \
        -Wno-dev
    make -C build
}

package() {
  cd "$srcdir/QtOpenGuidance-git/build"
  make DESTDIR="$pkgdir/" install
}
