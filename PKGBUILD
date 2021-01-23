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
        'https://github.com/CGAL/cgal/releases/download/v5.2/CGAL-5.2.tar.xz')
noextract=()
sha256sums=('SKIP'
            '744c86edb6e020ab0238f95ffeb9cf8363d98cde17ebb897d3ea93dac4145923')

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
  if [ ! -L "CGAL-5.2" ]; then
    ln -s "$srcdir/CGAL-5.2"
  fi
}

build() {
    cmake -B build -S "QtOpenGuidance-git" \
        -DCMAKE_BUILD_TYPE='None' \
        -DCMAKE_INSTALL_PREFIX='/usr' \
        -Wno-dev \
        -DUSE_UNITY=true \
        -DUSE_CCACHE=true \
        -DUSE_PCH=false
    make -C build
}

package() {
  make -C "build" DESTDIR="$pkgdir" install
}
