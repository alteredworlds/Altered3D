libavi
------

A library for playing back compressed audio and video AVI files.


Building
--------

Building consists of two stages, building the DLL with MSVC and building the test exe with mingw32

run 

nmake -f libavi.mak

then run

make -j test.mak
