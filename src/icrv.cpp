
#include <cmath>
#include "luview.hpp"
extern "C" {
#define LUNUM_API_NOCOMPLEX
#include "numarray.h"
#include "lunum.h"
}



IntegralCurve::IntegralCurve() : DataSource()
{

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

  GLfloat r0[3] = { 0,0,0 };
  double r[3];
  double r1[3], r2[3], r3[3], r4[3];
  double k1[3], k2[3], k3[3], k4[3];

  double ds = 1e-2, s = 0.0, s1 = 10.0;
  std::memcpy(r, r0, 3*sizeof(double));

  std::vector<double> points;
  while (s < s1) {
    points.push_back(r[0]);
    points.push_back(r[1]);
    points.push_back(r[2]);

    {
      std::vector<double> v1 = __cpu_transform->call(r, 3);
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
