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

Use CMake_. On x86-64 \*nix systems, the CMake release build command is

.. code:: bash

   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j

For Windows, the CMake release build command is

.. code:: shell

   cmake -S . -B build_windows -G Ninja -DCMAKE_BUILD_TYPE=Release && ^
   cmake --build build_windows -j

Here the bitness of the build artifacts depends on the command prompt used,
i.e. the "normal" Developer Command Prompt will produce 32-bit objects while
the x64 Native Tools Command prompt will produce 64-bit objects.

Building unit tests
-------------------

To build the unit tests, `Google Test`_ is required. The CMake build will look
for an existing Google Test installation, and if not found, will print a
message and skip building tests. You can build Google Test yourself and then
add to the CMake configure command

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
