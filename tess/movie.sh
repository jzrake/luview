#mencoder -nosound mf://*.png -mf w=800:h=371:type=png:fps=10 -ovc lavc -lavcopts vcodec=mpeg4:vbitrate=2160000:mbd=2:keyint=132:v4mv:vqmin=3:lumi_mask=0.07:dark_mask=0.2:mpeg_quant:scplx_mask=0.1:tcplx_mask=0.1:naq -o ARGH.avi


ffmpeg -r 12 -sameq -i image%03d.png AAAABLARG.mp4
