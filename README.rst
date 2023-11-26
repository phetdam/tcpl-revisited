.. README.rst

tcpl-revisited
==============

Exercises from *The C Programming Language*, 2nd ed.

A long belated return to the book that first introduced me to programming.

About
-----

TBA. I wanted to pay homage to my programming roots, as my first language was
C, while also using my current knowledge of modern [#]_ C, CMake_, and
cross-platform development to holistically scratch my C itch.

.. [#] By modern C, I mean C11+, although I've never used ``_Generic`` before.

Building from source
--------------------

CMake_ >=3.21 is required to build from source on all platforms.

\*nix
~~~~~

Building is easy with the provided ``build.sh`` build script. For usage, type

.. code:: bash

   ./build.sh --help

To build release binaries for this project, simply use the command

.. code:: bash

   ./build.sh -c Release

Simply typing ``./build.sh`` will build unoptimized binaries with debug symbols.

By default, the ``pdcpl`` support library is built as a shared library,
requiring that ``PDCPL_DLL`` be defined during compilation. To explicitly
request that ``pdcpl`` be built as a static library, one should specify
``-DBUILD_SHARED_LIBS=0``. E.g. to build release binaries with ``pdcpl`` built
as a static library, one can use the command

.. code:: bash

   ./build.sh -c Release -Ca -DBUILD_SHARED_LIBS=0

Windows
~~~~~~~

Building is easy with the provided ``build.bat`` build script. For usage, type

.. code:: shell

   build --help

To build release binaries for this project, simple use the command

.. code:: shell

   build -c Release

Simply typing ``build`` will build unoptimized binaries and the program
database with debugging info. You can specify the target architecture using
the ``-a`` flag, e.g. to build 64-bit release binaries instead of the default
32-bit ones, use

.. code:: shell

   build -a x64 -c Release

Currently, the Visual Studio toolset used will be whichever is the default.

By default, the ``pdcpl`` support library is built as a shared library,
requiring that ``PDCPL_DLL`` be defined during compilation. To explicitly
request that ``pdcpl`` be built as a static library, one should specify
``-DBUILD_SHARED_LIBS=0``. E.g. to build 32-bit release binaries with ``pdcpl``
built as a static library, one can use the command

.. code:: shell

   build -c Release -Ca "-DBUILD_SHARED_LIBS=0"

The extra double quotes are needed to prevent the ``=`` from confusing CMD.

Building unit tests
-------------------

To build the unit tests, `Google Test`_ is required. Somewhat going against
Google's own recommendations, the CMake build will look for an existing Google
Test installation, and if not found, will print a message and skip building
tests. You can build Google Test yourself and then add to the CMake configure
command

.. code:: bash

   -DGTEST_ROOT=/path/to/googletest

The ``/path/to/googletest`` directory should have ``include``, ``lib``, and on
Windows, ``bin``. It's recommended to just build Google Test as a static
library using CMake, and on Windows, ``-Dgtest_force_shared_crt=1`` must be
specified to force linking against the shared VC++ C runtime. Note that on
Windows, you should have both debug and release builds for Google Test, as by
default, debug builds cause Google Test libraries to link against the debug
VC++ C runtime, release builds cause linking against the release VC++ C
runtime [#]_.

For those who aren't aware that there are multiple ABI-incompatible C runtime
libraries on Windows, please read `the Microsoft article on them`__. Lots of
annoying build and runtime issues can happen if you link against the wrong
VC++ runtime, as unlike on \*nix systems, the article notes that

   Every executable image (EXE or DLL) can have its own statically linked CRT,
   or can dynamically link to a CRT.

.. [#] Thus foiled was my attempt to set ``CMAKE_MSVC_RUNTIME_LIBRARY`` to
   ``MultiThreadedDLL`` in CMake, which would mean that all Windows builds
   would link against the release VC++ C runtime, i.e.
   ``VCRUNTIME[toolset].dll``, not the debug C runtime, i.e.
   ``VCRUNTIME[toolset]D.dll``. In a real, large software project, dependency
   management would be simplified on Windows, as there would be no need to
   vendor separate debug DLLs that need the debug VC++ C runtime.

.. __: https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features

.. _CMake: https://cmake.org/cmake/help/latest/

.. _Google Test: http://google.github.io/googletest/

Building ``pdcpl_bcdp``
-----------------------

The ``pdcpl_bcdp`` C++ library implements a rudimentary parser for ANSI C
declarations using Flex_ 2.6.4 to generate its lexer and Bison_ 3.8.2 to
generate the underlying parser implementation. ``pdcpl_bcdp`` follows the basic
framework of using a user-defined driver class to wrap the Bison parser as
described in the `complete C++ example section`__ in the Bison documentation
but goes a step further to use PIMPL as a way of isolating the Bison types from
user code. Besides providing ABI stability, PIMPL also enables appropriate use
of ``__declspec(dllexport)`` and ``__declspec(dllimport)`` on Windows or
exporting symbols when ``-fvisibility=hidden`` is specified during compilation.

When building via CMake using the ``build.sh`` or ``build.bat`` scripts, this
library is built only if both Flex and Bison are detected on the system. Like
the main ``pdcpl`` support library, it is built shared by default unless
``-DBUILD_SHARED_LIBS=0`` is specified to the build script, and when built as
shared, requires ``PDCPL_DLL`` to be defined during compilation of user code.

WIP

.. __: https://www.gnu.org/software/bison/manual/html_node/
   A-Complete-C_002b_002b-Example.html

.. _Flex: https://westes.github.io/flex/manual/

.. _Bison: https://www.gnu.org/software/bison/manual/html_node/index.html
