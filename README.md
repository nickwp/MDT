# MDT
C++ Stand-alone library for Merging, Digitizing, and Triggering PMT hits based on [WCSim](https://github.com/WCSim/WCSim). A Python interface is provided as an option, requiring installation of [pybind11](https://pybind11.readthedocs.io/en/stable/installing.html).

The library provides C++ classes that manage three tasks:

 - Merging true hits (simulated photoelectrons by WCSim) of an events with those of any number of events
 - Digitizing true hits
    - Timing and charge of a digitized hit is simulated by a simple model used in WCSim
    - The library is capable of accomodating alternative models 
 - Triggering digitized hits
    - A simple algorithm based on coutung number of digitized hits falling in a sliding time winodw
 
