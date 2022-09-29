# rmslib
Library to calculate RMS value of a signal

Given a periodic update of samples, this library attempts to calculate the rms value of a signal and related values such at peak, 
frequency, and power factor with a focus of measuring power consumption in a home microgrid.

## Goals
* Some level of platform independence.
* Speed and size to be secondary to readablility and reusability.
* Explore some object oriented constructs of C++ in a real time application.

Altho this repository is structured following Arduino's 1.6 library format, all Arduino specific calls have been moved into the core application 
leaving the library clear of any arduino baggage. 
It is expected rmslib could easily be dropped into another platform by simply copying files as appropriate.

The only real speed optimization was to manage floats by separating the processing into a real time update() call and a background process() call.
While all math in update() is integer and would most likely be called by an ADC ISR, any floats have been factored into process() and would be
called by a background loop along with any application processing and communication.  Further, any non real time processing was factored out of
process() and placed in their appropriate getter methods.

My first thought was to use the well tested [EmonLib](https://github.com/openenergymonitor/EmonLib) library but 
because of my desire to explore how OO constructs can impact performance in a real time application i chose to attempt my own.
If you are looking for a more hardened solution please give that repository your consideration.

## Use
The basis of the library is two classes, Scalar and Product, with a scalar being a base unit such as amps or volts, 
and a product being a derived unit such as power.
