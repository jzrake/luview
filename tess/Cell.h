#ifndef CELLULAR
#define CELLULAR

#define NUMVAR 10 //Number of varibles to load in

struct Cell{
   double x[3];
   std::list<struct Cell *> myFriends;
   double v[NUMVAR];
   bool Bc;
   double color[4];
};

#endif
