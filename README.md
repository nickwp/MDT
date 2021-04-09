# MDT
C++ Stand-alone library for Merging, Digitizing, and Triggering hits of photomultiplier tubes (PMTs) based on [WCSim](https://github.com/WCSim/WCSim). A Python interface is optionally provided, requiring pre-installation of [pybind11](https://pybind11.readthedocs.io/en/stable/installing.html).

The library provides C++ classes that manage three tasks:

 - Merging true hits (simulated photoelectrons by WCSim) of an events with those of any number of events
    - Intrinsic dark noise can be added during this stage
 - Digitizing true hits
    - Timing and charge of a digitized hit is simulated by a simple model used in WCSim
    - The library is capable of accomodating alternative models 
    - Parameters describing PMT charcteristics such as timing resolution can be varied
    - Digitized hits by PMT afterpusing can be added (not supported yet)
 - Triggering digitized hits
    - A simple algorithm based on coutung number of digitized hits falling in a sliding time winodw
    - Selectable trigger window and thershold
