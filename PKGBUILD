pkgname=qtopenguidance-git # '-bzr', '-git', '-hg' or '-svn'
pkgver=git
pkgrel=1
pkgdesc="GPS based guidance for farming equipment"
arch=('x86_64' 'armv7h' 'aarch64')
url="https://github.com/eringerli/QtOpenGuidance"
license=('GPL')
groups=()
depends=('qt5-3d' 'gmp' 'mpfr' 'kddockwidgets-eringerli-git' 'libspnav' 'boost' )
makedepends=('git')
provides=("${pkgname}")
conflicts=("${pkgname}")
replaces=()
backup=()
options=()
install=
source=('QtOpenGuidance-git::git+https://github.com/eringerli/QtOpenGuidance'
        'https://github.com/CGAL/cgal/releases/download/releases%2FCGAL-5.0.2/CGAL-5.0.2.tar.xz')
noextract=()
sha256sums=('SKIP'
            'bb3594ba390735404f0972ece301f369b1ff12646ad25e48056b4d49c976e1fa')

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
  if [ ! -L "CGAL-5.0.2" ]; then
    ln -s "$srcdir/CGAL-5.0.2"
  fi
}

build() {
  cd "$srcdir/QtOpenGuidance-git"
  mkdir -p build

  cd build
  qmake -o Makefile ../QtOpenGuidance.pro -before PREFIX="$pkgdir/usr" -after CONFIG-=ccache
  make
}

package() {
  cd "$srcdir/QtOpenGuidance-git/build"
  make DESTDIR="$pkgdir/" install
}
