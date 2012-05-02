import os

NumberToDraw = 20086
DensityCut = 1.0
DensityWidth = 0.91
Delaunay = 1
CUT = .1
WIDTH = .64
varC = 1
POINTSIZE = 1
loggy = 0
x = .1
y = -0.1
z = 1.1
angleX = 0 
angleY = 0
COLORBAR = 3
PLOT_VALUE = 1
VALEDIT = 1
i = 0

for i in range(100):
        angleY = i*2
	filename = 'output' + str('%03d' % i) + '.bin'
	os.system("./vbin %s 1 %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s" % (filename, 
	NumberToDraw,DensityCut,DensityWidth,Delaunay,CUT,WIDTH,varC,
	POINTSIZE,loggy, x, y, z, angleX, angleY, COLORBAR, PLOT_VALUE, VALEDIT))
	os.system("cp test.ppm blarg" + str('%03d' % i) + ".ppm")
	iFinal = i
#NOW ROTATE AROUND A SINGLE FINAL IMAGE
i = 0
while i < 100:
        angleX = (i*2)
        angleY = iFinal*2
        filename = 'output' + str('%03d' % iFinal) + '.bin'
        os.system("./vbin %s 1 %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s" % (filename,
        NumberToDraw,DensityCut,DensityWidth,Delaunay,CUT,WIDTH,varC,
        POINTSIZE,loggy, x, y, z, angleX, angleY, COLORBAR, PLOT_VALUE, VALEDIT))
        os.system("cp test.ppm blarg" + str('%03d' % (i+iFinal)) + ".ppm")
	i+=1



os.system("rm test.ppm")

os.system("convert *.ppm image%03d.jpg") #mogrify -format jpg *.ppm")

os.system("rm *.ppm")
