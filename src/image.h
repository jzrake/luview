
#ifndef __Image_HEADER__
#define __Image_HEADER__

void luview_write_ppm(const char *fname, const double *data, int cmap,
		      int Nx, int Ny, const double *range);
const float *luview_get_colormap(int cmap);

#endif // __Image_HEADER__
