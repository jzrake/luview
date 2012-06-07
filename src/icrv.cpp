
#include <cmath>
#include "luview.hpp"
extern "C" {
#define LUNUM_API_NOCOMPLEX
#include "numarray.h"
#include "lunum.h"
}



IntegralCurve::IntegralCurve() : DataSource()
{
  rstart[0] = 0.0;
  rstart[1] = 0.0;
  rstart[2] = 0.0;
}
void IntegralCurve::set_starting_point(const double *r0)
{
  rstart[0] = r0[0];
  rstart[1] = r0[1];
  rstart[2] = r0[2];
}

void IntegralCurve::__init_lua_objects()
{
  hold(__output_ds["scalars"] = create<DataSource>(__lua_state));
  hold(__output_ds["vertices"] = create<DataSource>(__lua_state));
  __output_ds["scalars"]->set_input(this);
  __output_ds["vertices"]->set_input(this);
}
void IntegralCurve::__refresh_cpu()
{
  if (__cpu_transform == NULL) return;

  double r[3] = {rstart[0], rstart[1], rstart[2]};
  double r1[3], r2[3], r3[3], r4[3];
  double k1[3], k2[3], k3[3], k4[3];

  double ds = 4e-2, s = 0.0, s1 = 50;
  std::vector<double> points;

  while (s < s1) {
    points.push_back(r[0]);
    points.push_back(r[1]);
    points.push_back(r[2]);

    {
      std::vector<double> v1 = __cpu_transform->call(r, 3);
      if (v1.size() < 3) break;
      double v = sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
      if (fabs(v) < 1e-14) v = 1e-14;

      k1[0] = ds * v1[0]/v;
      k1[1] = ds * v1[1]/v;
      k1[2] = ds * v1[2]/v;

      r1[0] = r[0] + 0.5*k1[0];
      r1[1] = r[1] + 0.5*k1[1];
      r1[2] = r[2] + 0.5*k1[2];
    }
    {
      std::vector<double> v1 = __cpu_transform->call(r1, 3);
      if (v1.size() < 3) break;
      double v = sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
      if (fabs(v) < 1e-14) v = 1e-14;

      k2[0] = 0.5*ds * v1[0]/v;
      k2[1] = 0.5*ds * v1[1]/v;
      k2[2] = 0.5*ds * v1[2]/v;

      r2[0] = r[0] + 0.5*k2[0];
      r2[1] = r[1] + 0.5*k2[1];
      r2[2] = r[2] + 0.5*k2[2];
    }
    {
      std::vector<double> v1 = __cpu_transform->call(r2, 3);
      if (v1.size() < 3) break;
      double v = sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
      if (fabs(v) < 1e-14) v = 1e-14;

      k3[0] = 0.5*ds * v1[0]/v;
      k3[1] = 0.5*ds * v1[1]/v;
      k3[2] = 0.5*ds * v1[2]/v;

      r3[0] = r[0] + k3[0];
      r3[1] = r[1] + k3[1];
      r3[2] = r[2] + k3[2];
    }
    {
      std::vector<double> v1 = __cpu_transform->call(r3, 3);
      if (v1.size() < 3) break;
      double v = sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
      if (fabs(v) < 1e-14) v = 1e-14;

      k4[0] = ds * v1[0]/v;
      k4[1] = ds * v1[1]/v;
      k4[2] = ds * v1[2]/v;

      r4[0] = r[0] + k4[0];
      r4[1] = r[1] + k4[1];
      r4[2] = r[2] + k4[2];
    }

    r[0] += (1./6.)*(k1[0] + 2*k2[0] + 2*k3[0] + k4[0]);
    r[1] += (1./6.)*(k1[1] + 2*k2[1] + 2*k3[1] + k4[1]);
    r[2] += (1./6.)*(k1[2] + 2*k2[2] + 2*k3[2] + k4[2]);

    s += ds;
  }

  std::vector<GLfloat> fpoints(points.begin(), points.end());
  int N[2] = { fpoints.size()/3, 3 };
  this->set_data(&fpoints[0], N, 2);
}




IntegralCurve::LuaInstanceMethod
IntegralCurve::__getattr__(std::string &method_name)
{
  AttributeMap attr;
  attr["set_starting_point"] = _set_starting_point_;
  RETURN_ATTR_OR_CALL_SUPER(DataSource);
}
int IntegralCurve::_set_starting_point_(lua_State *L)
{
  IntegralCurve *self = checkarg<IntegralCurve>(L, 1);
  double x[3];
  x[0] = luaL_checknumber(L, 2);
  x[1] = luaL_checknumber(L, 3);
  x[2] = luaL_checknumber(L, 4);
  self->set_starting_point(x);
  return 0;
}
