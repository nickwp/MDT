# MDT
C++ Stand-alone library for Merging, Digitizing, and Triggering hits of photomultiplier tube (PMT) based on [WCSim](https://github.com/WCSim/WCSim). A Python interface is optionally provided, requiring pre-installation of [pybind11](https://pybind11.readthedocs.io/en/stable/installing.html).

The library provides C++ classes that manage three tasks:

 - Merging true hits (simulated photoelectrons by WCSim) of an events with those of any number of events
    - Intrinsic dark noise of 
 - Digitizing true hits
    - Timing and charge of a digitized hit is simulated by a simple model used in WCSim
    - The library is capable of accomodating alternative models 
 - Triggering digitized hits
    - A simple algorithm based on coutung number of digitized hits falling in a sliding time winodw
 
# Installation
Currently, compilation is done by a traditional. Mordern compilation by CMake will be supported soon. 
The following describes the procedure using [compile.sh](https://github.com/akutsuR/MDT/blob/master/compile.sh)
