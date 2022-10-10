# rmslib
Library to calculate True RMS value of a periodic signal

Given a stream of samples, this library attempts to calculate the rms value of a signal and related values
such as peak, frequency, power, and power factor with a focus on measuring power usage in a home microgrid.

## Dependencies
Uses the [Ewma](https://github.com/jonnieZG/EWMA) library to effect the result time constant.

## Goals
* Some level of platform independence.
* Speed and size to be secondary to readability and reusability.
* Explore some object oriented constructs of C++ in a real time application.

Altho this repository is structured following Arduino's library format, all Arduino specific calls have
been moved into the core application leaving the library clear of any arduino baggage. 
It is expected rmslib could easily be used on another platform by simply copying files where appropriate.

Data processing was separated into a real time update() for each sample, a background process() to handle
end of cycle calculations, and getter methods for final asynchronous processing.   

My first thought was to use the well tested [EmonLib](https://github.com/openenergymonitor/EmonLib) but 
because of my desire to explore how OO constructs can impact performance in a real time application i chose to
attempt my own. If you are looking for a library with a bit more history, you might give more weight to EmonLib.

## Use
At the core of the library are two classes, Scalar and Product, with a Scalar being a base unit such as amps
or volts, and Product being a container for two scalars resulting in a product such as watts.

### Setup
Instantiate a Scalar for each rms input and append to the adc queue for updating by that ISR.
Additionally instantiate a Product containing the two related scalars to be used by your application.

### ISR
Call the Scalar's update() method with the adc sample.

### Loop
Near the top of loop, call the process() method of each Product for end of cycle processing.
The application can then access Product to get its up-to-date properties as well as the properties of the Scalers.

See the nco example with a simulated sine, square, or triangle waveform to get started.

### Caveats
* Uses new throughout for memory allocation with no destructors.  The thought was the only time a scalar etc
would be destroyed was during reset.
* No overflow detection.  A simple way to estimate limits based on sample rate and adc bit depth would be helpful here.

## Platform
This library was developed on a Microchip AVR32DB32 using Spence Konde's [DxCore](https://github.com/SpenceKonde/DxCore)
with Arduino-CLI.  Testing was a simple comparison to a P3 Kill-A-Watt with a fridge load.

## To Do
* Add destructors for those who may want a more dynamic application.
* Factor out electrical names from product.  Product can be specialized by extending and property overriding.
* Add a menu to example.