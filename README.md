# Hundred Operator
Randomized 100 operator FM synth.

Using the experimental proposed standard C++ audio library.

This is a pretty whacky experiment that I've implemented in various ways over the years. It is an FM synth with one hundred operators cascaded serially. Each operator's gain and frequency are randomly generated with exponental distributions. Frequencies range from DC up to 20kHz so some operators can function as LFOs. This can lead to very complex modulations between operators. 

The values are dumped out via stdout in CSV format if any particular value sets sound interesting.

## Compiling


Generate makefiles
`cmake .`

Compile
`make hundred_operator`

*OR*

In Visual Studio Code. Run the build task.

## Use

Simplest way to use it is invoke it directly in the terminal: `./hundred_operator`

By default it will run for a default duration of 5 seconds and have a frequency skew of 8.0 and gain skew of 1.5. It accepts three arguments, duration, frequency skew and gain skew. The higher the skew value, the more values are likely to gather in the lower range. If you specify a duration of 0, the program will keep running until manually exited.

e.g. `hundred-operator 0 1.2 3.0` - Runs indefinitely, frequency skew of 1.2 and gain skew of 3.0. Results in more much brighter output than the defaults, but the trade-off is that sounds lean mostly towards noise. Higher frequency skew values will result in more periodic sounds, however the trade-off is the trend towards lower frequencies results in much darker sounds. 

## TODO:

+ Implement some alterative distibution options, including bimodal distribution and changing distibution weighting depending on where in the signal chain an operator is.
+ Fix up the math around the frequency distribution. It sometimes overshoots 20kHz with certain skews values and so the resulting sine wave aliases and effectively messes with the distribution.
+ Implement a high pass filter on the way out to remove some of the extreme LF/DC offsets that can sometimes end up in the output.
+ Implement file input to read the CSV data.
