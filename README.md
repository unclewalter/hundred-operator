# Hundred Operator
Randomized 100 operator FM synth.

Using the experimental proposed standard C++ audio library.

This is a pretty whacky experiment that I've implemented in various ways over the years. It is an FM synth with one hundred operators Each operator's gain and frequency are randomly generated with exponental distributions. Frequencies range from DC up to 20kHz so some operators can function as LFOs. This can lead to very complex modulations between operators. 

The values are dumped out via stdout in CSV format if any particular value sets sound interesting.

## Compiling


Generate makefiles
`cmake .`

Compile
`make hundred_operator`
