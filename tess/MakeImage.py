import os

NumberToDraw = 15086
DensityCut = 1.0
DensityWidth = 0.9
Delaunay = 1
CUT = .1
WIDTH = .64
varC = 1
POINTSIZE = 1
loggy = 0
x = .1
y = -0.1
z = 1.1
angleX = -184
angleY = 702
COLORBAR = 3
PLOT_VALUE = 1
VALEDIT = 1

#SET NUMBER FOR FILE YOU WANT TO RECREATE HERE
i = 3
#
filename = 'output' + str('%03d' % i) + '.bin'
os.system("./vbin %s 1 %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s" % (filename,
NumberToDraw,DensityCut,DensityWidth,Delaunay,CUT,WIDTH,varC,
POINTSIZE,loggy, x, y, z, angleX, angleY, COLORBAR, PLOT_VALUE, VALEDIT))
os.system("rm " + str('%03d' % i) + "test.ppm")
os.system("convert test.ppm image"+ str('%03d' % i) + ".jpg")
os.system("rm test.ppm")

