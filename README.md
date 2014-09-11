FractalWavelets
===============

2D fractal Haar wavelet transformation for image compression or robust steganography/watermarking

[Haar wavelets](http://en.wikipedia.org/wiki/Haar_wavelet) provide a simple linear transform in which coefficients describe differences of average values of corresponding regions. In contrast e.g. to Fouier transorm (at least O(n*log(n)), it can be calculated in linear time.

While this transform is usually performed in 1D, it can be taken to 2D by changing the set of shifts defining neighboring regions. This implementaton uses set of vectors from [complex base numeral systems](http://en.wikipedia.org/wiki/Complex_base_systems) for n=2 and different D, getting [dragon curve](http://en.wikipedia.org/wiki/Dragon_curve)-like blocks ("fraxels"). It fits the largest possible block to the size of picture and leaves the rest of the image unchanged. It uses library for bmp which requires 24-bit colors ( http://bitmap.codeplex.com/ ).

These coefficients should be quantized for image compression purpose, or slightly modified for steganography purpose. The current implementation only trims high frequency coefficients (set to zero) to get intuition about obtained block structure.

Here are [examples of result pictures](https://dl.dropboxusercontent.com/u/12405967/fravelets.png), [article](http://arxiv.org/abs/0712.1309) and [demonstration](http://demonstrations.wolfram.com/NumberSystemsUsingAComplexBase/)about complex base numeral systems.

Jarek Duda, September 2014
