import subprocess
import os

os.system("mencoder -nosound mf://*.jpg -mf\
 w=800:h=371:type=jpg:fps=10 -ovc lavc -lavcopts\
 vcodec=mpeg4:vbitrate=2160000:mbd=2:keyint=132:v4mv:vqmin=3:lumi_mask=0.07:dark_mask=0.2:mpeg_quant:scplx_mask=0.1:tcplx_mask=0.1:naq -o ARGH.avi")

#command = ('mencoder', 'mf://*.jpg', '-mf',
#           'type=jpg:w=800:h=600:fps=8',
#           '-ovc', 'lavc', '-lavcopts',
#           'vcodec=mpeg4:vbitrate=2160000:mbd=2:keyint=132:v4mv:vqmin=3:lumi_mask=0.07:dark_mask=0.2:mpeg_quant:scplx_mask=0.1:tcplx_mask=0.1:naq', '-oac', 'copy', '-o', 'ARGH.avi')

#print "\n\nabout to execute:\n%s\n\n" % ' '.join(command)
#subprocess.check_call(command)

