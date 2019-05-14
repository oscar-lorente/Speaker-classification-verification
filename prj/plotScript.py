#!/usr/bin/python3
import matplotlib.pyplot as plt
import pylab

plt.plotfile('ekis.txt')
plt.title("Audio signal - time domain")
plt.xlabel('Samples')
plt.ylabel('Amplitude')
plt.grid(True)

plt.plotfile('filePot.txt')
plt.title("Autocorrelation based power [dB]")
plt.xlabel('Samples')
plt.ylabel('Power [dB]')
plt.grid(True)

#plt.plotfile('filePotMedia.txt', newfig = False, linestyle='dashed', label = 'Average noise power')
#plt.legend()

plt.plotfile('fileR1.txt')
plt.title("Normalized r[1] of each frame")
plt.xlabel('Samples')
plt.ylabel('r[1]/r[0]')
plt.grid(True)

plt.plotfile('fileRLag.txt')
plt.title("Normalized rmax of each frame")
plt.xlabel('Samples')
plt.ylabel('r[lag]/r[0]')
plt.grid(True)
plt.show()


#plt.plotfile('autoCorr.txt')
#plt.title("Voiced frame autocorrelation - Center Clipping")
#plt.xlabel('Samples')
#plt.ylabel('r[sample]')
#plt.grid(True)
#plt.show()

#plt.plotfile('pitches.txt')
#plt.title("Detected pitches")
#plt.xlabel('Samples')
#plt.ylabel('Pitch')
#plt.grid(True)
#plt.show()

#plt.plotfile('cepstrum.txt')
#plt.title("Voiced frame cepstrum")
#plt.xlabel('Samples')
#plt.ylabel('c[sample]')
#plt.grid(True)
#plt.show()
