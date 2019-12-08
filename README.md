# Speaker-classification-verification
Speaker recognition, classification and verification system, based on the MFCC cepstral coefficients and the use of the Gaussian Mixture Models (GMM).

The program - implemented in C ++ - basically consists of four parts: 
- Extraction of the speaker's characteristics (LPC, LPCC or MFCC)
- Use of an estimation and evaluation algorithm of the GMM probabilistic model based on the extracted characteristics
- Speaker classification by using a Bayesian classifier
- Speaker verification.

Finally, a Deep Learning algorithm based on a multilayer perceptron (artificial multilayer neural network) for speaker classification has been developed and implemented, studying the influence of different parameters - activation function, number of units per layer, leraning rate. ..- on its behaviour and precision.
