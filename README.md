Virtual Laboratory Environment 2.1
==================================

| Linux | Windows |
| :---- | :---- |
[ ![Linux build status][1]][2] | [![Windows build status][3]][4] |

[1]: https://travis-ci.org/vle-forge/vle.svg?branch=master2.1
[2]: https://travis-ci.org/vle-forge/vle?branch=master2.1
[3]: https://ci.appveyor.com/api/projects/status/github/vle-forge/vle?branch=master2.1&svg=true
[4]: https://ci.appveyor.com/project/quesnel/vle?branch=master2.1

See AUTHORS and COPYRIGHT for the list of contributors.

VLE is an operational framework for multi-modeling, simulation and
analysis of complex dynamical systems. It addresses the reliability
issue by using recent developments in the theory of modeling and
simulation proposed by B. Zeigler with the Discrete Event System
Specification formalism (DEVS).

## Requirements

For the VFL API:

* expat (≥ 2.0)
* boost (≥ 1.47)
* cmake (≥ 3.5)
T c++ compiler (gcc ≥ 4.9, clang ≥ 3.3, intel icc (≥ 11.0)

For the MPI command line:

* Any MPI 2 library as OpenMpi, mpich

For the GUI:

* Qt5
* QCustomplot

## Getting the code

The source tree is currently hosted on Github and Sourceforge. To view
the repository online: https://github.com/vle-forge/vle The URL to
clone it:

```
cd $HOME/
git clone git://github.com/vle-forge/vle.git
```

Install dependencies (recent ubuntu/debian). Copy/paste the following lines:

```bash
apt-get install curl libexpat1-dev libboost-dev cmake pkg-config g++ \
        qttools5-dev qttools5-dev-tools qtbase5-dev qtbase5-dev-tools \
        qtchooser qt5-default libqcustomplot-dev \
        asciidoctor ruby-asciidoctor
```

Once you have met requirements, compiling and installing is simple. Copy/paste
the following lines:

```bash
cd vle
mkdir build
cd build
export QT_SELECT=qt5
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make
make install
```

## License

This software in GPLv3 or later. See the file COPYING. Some files are
under a different license. Check the headers for the copyright info.

## Usage

VLE is a multi-modeling environment to build model, to simulate and
analys them.  To more information, see the VLE website
(http://www.vle-projet.org).

To build a new package::

    vle-2.1 -P mypackage create

To configure, build a package::

    vle-2.1 -P mypackage configure build

To run vpz in a package::

    vle-2.1 -P mypackage file.vpz
