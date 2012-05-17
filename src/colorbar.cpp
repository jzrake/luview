

#include <math.h>
#include "luview.hpp"
#include "pyplotcm.h"

static void get_rgb(double val, double *rp ,double *gp, double *bp, int COLORBAR);

ColormapCollection::ColormapCollection() : cmap_id(0), var_index(0) { }

ColormapCollection::LuaInstanceMethod ColormapCollection::__getattr__
(std::string &method_name)
{
  AttributeMap attr;
  attr["set_component"] = _set_component_;
  attr["set_colormap"] = _set_cmap_;
  attr["next_colormap"] = _next_colormap_;
  attr["prev_colormap"] = _prev_colormap_;
  RETURN_ATTR_OR_CALL_SUPER(CallbackFunction);
}
int ColormapCollection::_set_cmap_(lua_State *L)
{
  ColormapCollection *self = checkarg<ColormapCollection>(L, 1);
  self->cmap_id = luaL_checkinteger(L, 2);
  printf("using cmap %d\n", self->cmap_id);
  return 0;
}
int ColormapCollection::_set_component_(lua_State *L)
{
  ColormapCollection *self = checkarg<ColormapCollection>(L, 1);
  self->var_index = luaL_checkinteger(L, 2);
  printf("mapping colors over component %d\n", self->var_index);
  return 0;
}
int ColormapCollection::_next_colormap_(lua_State *L)
{
  ColormapCollection *self = checkarg<ColormapCollection>(L, 1);
  self->next_colormap();
  return 0;
}
int ColormapCollection::_prev_colormap_(lua_State *L)
{
  ColormapCollection *self = checkarg<ColormapCollection>(L, 1);
  self->prev_colormap();
  return 0;
}



// -----------------------------------------------------------------------------
//  List of choices:
//
//  0. Original
//  1. Standard
//  2. Copper
//  3. Roy G. Biv
//  4. Electric
//  5. Blank
// -----------------------------------------------------------------------------
void TessColormaps::next_colormap()
{
  if (++cmap_id == 5) cmap_id = 0;
}
void TessColormaps::prev_colormap()
{
  if (--cmap_id ==-1) cmap_id = 4;
}
std::vector<double> TessColormaps::call_priv(double *x, int narg)
{
  double z = 0.0;
  std::vector<double> ret(4);
  if (var_index < narg) {
    z = x[var_index];
  }
  ret[3] = 1.0; // alpha
  get_rgb(z, &ret[0], &ret[1], &ret[2], cmap_id);
  return ret;
}
void MatplotlibColormaps::next_colormap()
{
  const char **names = pyplot_colors_get_names();
  if (names[++cmap_id] == NULL) {
    cmap_id = 0;
  }
  printf("set colormap to matplotlib:%s\n", names[cmap_id]);
}
void MatplotlibColormaps::prev_colormap()
{
  const char **names = pyplot_colors_get_names();
  if (--cmap_id == -1) {
    cmap_id = pyplot_colors_get_num_tables()-1;
  }
  printf("set colormap to matplotlib:%s\n", names[cmap_id]);
}
std::vector<double> MatplotlibColormaps::call_priv(double *x, int narg)
{
  double z = 0.0;

  if (var_index < narg) {
    z = x[var_index];
  }

  const char **names = pyplot_colors_get_names();
  const float *data = pyplot_colors_get_lookup_table(names[cmap_id]);

  if (data == NULL) {
    printf("no cmap %s!\n", names[cmap_id]);
  }

  int n = z * 255.0;
  std::vector<double> ret(&data[4*n], &data[4*(n+1)]);
  return ret;
}


void get_rgb(double val, double *rp, double *gp, double *bp, int COLORBAR)
{
  double rrr,ggg,bbb;
  if( COLORBAR == 0 ){
    double nexp = 8.0;
    rrr = exp(-nexp*pow(val-5./6.,2.0)) + .25*exp(-nexp*pow(val+1./6.,2.0));
    ggg = exp(-nexp*pow(val-3./6.,2.0));
    bbb = exp(-nexp*pow(val-1./6.,2.0)) + .25*exp(-nexp*pow(val-7./6.,2.0));
  }else if(COLORBAR == 1){
    if( val < .1 ){
      bbb = 4.*(val+.15);
      ggg = 0.0;
      rrr = 0.0;
    }else if( val < .35){
      bbb = 1.0;
      ggg = 4.*(val-.1);
      rrr = 0.0;
    }else if( val < .6 ){
      bbb = 4.*(.6-val);
      ggg = 1.;
      rrr = 4.*(val-.35);
    }else if( val < .85){
      bbb = 0.0;
      ggg = 4.*(.85-val);
      rrr = 1.;
    }else{
      bbb = 0.0;
      ggg = 0.0;
      rrr = 4.*(1.1-val);
    }
  }else if(COLORBAR == 2){
    rrr = 2.*val;
    ggg = 1.2*val;
    bbb = .8*val;
  }else if(COLORBAR == 3){
    double gam = .8;
    double Amp;
    double r0,g0,b0;
    double hi,lo,x1,x2,x3,x4;
    hi = .8;
    lo = .1;
    if( val > hi ) Amp = .3 + .7*(1.-val)/(1.-hi);
    else if( val < lo ) Amp = .3 + .7*(val)/(lo);
    else Amp = 1.0;

    x1 = .5;
    x2 = .325;
    x3 = .15;
    x4 = 0.;

    if( val > x1 )      r0 = 1.;
    else if( val > x2 ) r0 = (val-x2)/(x1-x2);
    else if( val > x3 ) r0 = 0.;
    else if( val > x4 ) r0 = (val-x3)/(x4-x3);
    else                r0 = 1.;

    x1 = .6625;
    x2 = .5;
    x3 = .275;
    x4 = .15;

    if( val > x1 )      g0 = 0.;
    else if( val > x2 ) g0 = (val-x1)/(x2-x1);
    else if( val > x3 ) g0 = 1.;
    else if( val > x4 ) g0 = (val-x4)/(x3-x4);
    else                g0 = 0.;

    x1 = .325;
    x2 = .275;

    if( val > x1 )      b0 = 0.;
    else if( val > x2 ) b0 = (val-x1)/(x2-x1);
    else                b0 = 1.;

    rrr = pow(Amp*r0,gam);
    ggg = pow(Amp*g0,gam);
    bbb = pow(Amp*b0,gam);
  }else if(COLORBAR == 4){
    if( val < .1 ){
      bbb = 4.*(val+.125);
      ggg = 0.0;
      rrr = 0.0;
    }else if( val < .375){
      bbb = 1.0;
      ggg = 4.*(val-.125);
      rrr = 0.0;
    }else if( val < .625 ){
      bbb = 4.*(.625-val);
      rrr = 4.*(val-.375);
      ggg = bbb;
      if( rrr > bbb ) ggg = rrr;
    }else if( val < .875){
      bbb = 0.0;
      ggg = 4.*(.875-val);
      rrr = 1.;
    }else{
      bbb = 0.0;
      ggg = 0.0;
      rrr = 4.*(1.125-val);
    }
  }else if(COLORBAR == 5){
    rrr = val;
    ggg = val;
    bbb = val;
  }
  else if (COLORBAR == 6) {
    double a = 50.0;
    double b = 2.0;
    rrr = exp(-a*pow(val-0.3, b));
    ggg = exp(-a*pow(val-0.5, b));
    bbb = exp(-a*pow(val-0.7, b));

  }else{
    rrr = 1.0;
    ggg = 1.0;
    bbb = 1.0;
  }

  *rp = rrr;
  *gp = ggg;
  *bp = bbb;
}

