
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include "luview.hpp"



NbodySimulation::NbodySimulation() :
  NumberOfParticles(600),
  TimeStep(4e-6),
  particles(NULL)
{
  init_particles();
}
NbodySimulation::~NbodySimulation()
{
  if (particles) free(particles); 
}
void NbodySimulation::advance()
{
  MoveParticlesRK2(particles, NumberOfParticles, TimeStep);
  refresh_output();
}
void NbodySimulation::__init_lua_objects()
{
  output_points = create<PointsSource>(__lua_state);
  hold(output_points);
  refresh_output();
}
NbodySimulation::LuaInstanceMethod
NbodySimulation::__getattr__(std::string &method_name)
{
  AttributeMap attr;
  attr["advance"] = _advance_;
  attr["get_output"] = _get_output_;
  RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
}
int NbodySimulation::_advance_(lua_State *L)
{
  NbodySimulation *self = checkarg<NbodySimulation>(L, 1);
  self->advance();
  return 0;
}
void NbodySimulation::refresh_output()
{
  double *points = new double[NumberOfParticles*3];
  for (int i=0; i<NumberOfParticles; ++i) {
    struct MassiveParticle *p = particles + i;
    points[3*i + 0] = p->x[0];
    points[3*i + 1] = p->x[1];
    points[3*i + 2] = p->x[2];
  }
  output_points->set_points(points, NumberOfParticles, 3);
  delete [] points;
}
int NbodySimulation::_get_output_(lua_State *L)
{
  NbodySimulation *self = checkarg<NbodySimulation>(L, 1);
  self->retrieve(self->output_points);
  return 1;
}

void NbodySimulation::init_particles()
{
  const int N = NumberOfParticles;
  const double M = 1e7;

  particles = (MassiveParticle*) realloc(particles, N*sizeof(MassiveParticle));

  particles[0].m    = M;
  particles[0].x[0] = 0.0;
  particles[0].x[1] = 0.0;
  particles[0].x[2] = 0.0;
  particles[0].v[0] = 0.0;
  particles[0].v[1] = 0.0;
  particles[0].v[2] = 0.0;

  for (int i=1; i<N; ++i) {
    struct MassiveParticle *p = particles + i;

    const double r = RandomDouble(0.5, 1.0);
    const double t = RandomDouble(0.0, 2*M_PI);
    const double v = sqrt(M/r);

    p->id = i;
    p->m = RandomDouble(0.1, 10.0);

    p->x[0] = r*cos(t);//1+RandomDouble(-0.1, 0.1);//r*cos(t);
    p->x[1] = RandomDouble(-0.1, 0.1);
    p->x[2] = r*sin(t);//1+RandomDouble(-0.1, 0.1);//r*sin(t);

    //    double r = sqrt(p->x[0]*p->x[0] + p->x[1]*p->x[1] + p->x[2]*p->x[2]);
    //    double v = sqrt(M/r);

    p->v[0] = RandomDouble(-0.1, 0.1) + 0.9*v*p->x[2]/r;
    p->v[1] = RandomDouble(-0.1, 0.1);
    p->v[2] = RandomDouble(-0.1, 0.1) - 0.9*v*p->x[0]/r;
  }
}



void NbodySimulation::MoveParticlesFwE(struct MassiveParticle *P0, int N, double dt)
{
  ComputeForces(P0, N);

  for (int i=0; i<N; ++i) {
    for (int m=0; m<3; ++m) {
      P0[i].x[m] = P0[i].x[m] + P0[i].v[m]*1.0*dt;
      P0[i].v[m] = P0[i].v[m] + P0[i].a[m]*1.0*dt;
    }
  }
}

void NbodySimulation::MoveParticlesRK2(struct MassiveParticle *P0, int N, double dt)
{
  struct MassiveParticle *P1 =
    (struct MassiveParticle*) malloc(N*sizeof(struct MassiveParticle));
  memcpy(P1, P0, N*sizeof(struct MassiveParticle));

  ComputeForces(P0, N);

  for (int i=0; i<N; ++i) {
    for (int m=0; m<3; ++m) {
      P1[i].x[m] = P0[i].x[m] + P0[i].v[m]*0.5*dt;
      P1[i].v[m] = P0[i].v[m] + P0[i].a[m]*0.5*dt;
    }
  }
  ComputeForces(P1, N);

  for (int i=0; i<N; ++i) {

    for (int m=0; m<3; ++m) {
      P0[i].x[m] = P0[i].x[m] + P1[i].v[m]*dt;
      P0[i].v[m] = P0[i].v[m] + P1[i].a[m]*dt;
    }
  }

  free(P1);
}


void NbodySimulation::MoveParticlesRK4(struct MassiveParticle *P0, int N, double dt)
{
  struct MassiveParticle *P1 =
    (struct MassiveParticle*) malloc(N*sizeof(struct MassiveParticle));
  struct MassiveParticle *P2 =
    (struct MassiveParticle*) malloc(N*sizeof(struct MassiveParticle));
  struct MassiveParticle *P3 =
    (struct MassiveParticle*) malloc(N*sizeof(struct MassiveParticle));

  memcpy(P1, P0, N*sizeof(struct MassiveParticle));
  memcpy(P2, P0, N*sizeof(struct MassiveParticle));
  memcpy(P3, P0, N*sizeof(struct MassiveParticle));

  ComputeForces(P0, N);

  for (int i=0; i<N; ++i) {
    for (int m=0; m<3; ++m) {
      P1[i].x[m] = P0[i].x[m] + P0[i].v[m]*0.5*dt;
      P1[i].v[m] = P0[i].v[m] + P0[i].a[m]*0.5*dt;
    }
  }
  ComputeForces(P1, N);

  for (int i=0; i<N; ++i) {
    for (int m=0; m<3; ++m) {
      P2[i].x[m] = P0[i].x[m] + P1[i].v[m]*0.5*dt;
      P2[i].v[m] = P0[i].v[m] + P1[i].a[m]*0.5*dt;
    }
  }
  ComputeForces(P2, N);

  for (int i=0; i<N; ++i) {
    for (int m=0; m<3; ++m) {
      P3[i].x[m] = P0[i].x[m] + P1[i].v[m]*1.0*dt;
      P3[i].v[m] = P0[i].v[m] + P1[i].a[m]*1.0*dt;
    }
  }
  ComputeForces(P3, N);

  for (int i=0; i<N; ++i) {
    for (int m=0; m<3; ++m) {
      P0[i].x[m] += dt*(P0[i].v[m] + 2*P1[i].v[m] + 2*P2[i].v[m] + P3[i].v[m])/6;
      P0[i].v[m] += dt*(P0[i].a[m] + 2*P1[i].a[m] + 2*P2[i].a[m] + P3[i].a[m])/6;
    }
  }

  free(P1);
  free(P2);
  free(P3);
}


void NbodySimulation::ComputeForces(struct MassiveParticle *P0, int N)
{
  for (int i=0; i<N; ++i) {
    for (int m=0; m<3; ++m) {
      P0[i].a[m] = 0.0;
    }
  }

  for (int i=0; i<N; ++i) {
    for (int j=0; j<i; ++j) {

      struct MassiveParticle *p0 = P0 + i;
      struct MassiveParticle *p1 = P0 + j;

      const double *x0 = p0->x;
      const double *x1 = p1->x;

      const double R[3] = { x1[0]-x0[0], x1[1]-x0[1], x1[2]-x0[2] };
      const double r = sqrt(R[0]*R[0] + R[1]*R[1] + R[2]*R[2]);

      p0->a[0] += p1->m * R[0] / (r*r*r);
      p0->a[1] += p1->m * R[1] / (r*r*r);
      p0->a[2] += p1->m * R[2] / (r*r*r);

      p1->a[0] -= p0->m * R[0] / (r*r*r); // make funny bug here by using p1->m instead
      p1->a[1] -= p0->m * R[1] / (r*r*r);
      p1->a[2] -= p0->m * R[2] / (r*r*r);
    }
  }
}

void NbodySimulation::ComputeForces2(struct MassiveParticle *P0, int N)
{

  for (int i=0; i<N; ++i) {

    P0[i].a[0] = 0.0;
    P0[i].a[1] = 0.0;
    P0[i].a[2] = 0.0;

    for (int j=0; j<N; ++j) {

      if (i==j) continue;

      struct MassiveParticle *p0 = P0 + i;
      struct MassiveParticle *p1 = P0 + j;

      const double *x0 = p0->x;
      const double *x1 = p1->x;

      const double R[3] = { x1[0]-x0[0], x1[1]-x0[1], x1[2]-x0[2] };
      const double r = sqrt(R[0]*R[0] + R[1]*R[1] + R[2]*R[2]);

      p0->a[0] += p1->m * R[0] / (r*r*r);
      p0->a[1] += p1->m * R[1] / (r*r*r);
      p0->a[2] += p1->m * R[2] / (r*r*r);

    }
  }
}

double NbodySimulation::RandomDouble(double a, double b)
{
  return a + (b-a) * rand() / RAND_MAX;
}

