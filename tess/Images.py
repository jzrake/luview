import os

NumberToDraw = 800000
DensityCut = 1.0
DensityWidth = 0.99
Delaunay = 1
CUT = .1
WIDTH = 10.0
varC = 1
POINTSIZE = 1
loggy = 0
x = 0
y = 0
z = 10.0
angleX = 0
angleY = 0
angleZ = 0
COLORBAR = 4
VALEDIT = 0 #scale colormap?

i = 0

for i in range(50,51):
      #  angleY = i*2
	filename = './bins/output' + str('%03d' % i) + '.bin'
	os.system("./vbin %s 1 %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s" % (filename, 
	NumberToDraw,DensityCut,DensityWidth,Delaunay,CUT,WIDTH,varC,
	POINTSIZE,loggy, x, y, z, angleX, angleY, angleZ, COLORBAR, VALEDIT))
	os.system("convert test.ppm image" + str('%03d' % i ) + ".png")
	os.system("rm test.ppm")
	print i
#NOW ROTATE AROUND A SINGLE FINAL IMAGE
iFinal = i
for i in range(0,50):
        angleX = (i*2)
        filename = './bins/output' + str('%03d' % iFinal ) + '.bin'
        os.system("./vbin %s 1 %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s" % (filename,
        NumberToDraw,DensityCut,DensityWidth,Delaunay,CUT,WIDTH,varC,
        POINTSIZE,loggy, x, y, z, angleX, angleY, angleZ, COLORBAR, VALEDIT))
	os.system("convert test.ppm image" + str('%03d' % (i+iFinal) ) + ".png")
	os.system("rm test.ppm")
	print i


