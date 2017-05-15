INCLUDEPATH += $$PWD/src
SRC_DIR = $$PWD

VERSION_MAJOR = 2
VERSION_MINOR = 0
VERSION_PATCH = 0
VERSION_EXTRA = alpha1
VERSION_ABI = "2.0"

# Disable all logging facilities and active heavy optimization code to speed up
# simulation. [default: off]
# DEFINES += VLE_FULL_OPTIMIZATION

# Enable debug log message. It slows simulation [default: ON]
DEFINES += VLE_DISABLE_DEBUG

unix {
  isEmpty(PREFIX) {
    PREFIX = $$(HOME)/usr
  }

  BINDIR = $$PREFIX/bin
  LIBDIR = $$PREFIX/lib
  LIBSDIR = $$PREFIX/lib/vle-$$VERSION_ABI
  DATADIR = $$PREFIX/share/vle-$$VERSION_ABI
  INCLUDEDIR = $$PREFIX/include/vle-$$VERSION_ABI
  MANPATH = $$PREFIX/share/man/man1
}
