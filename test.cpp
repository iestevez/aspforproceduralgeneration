#include "Room.h"
#include "Worldobject.h"
#include "Wall.h"
#include <memory>
#include <iostream>

typedef std::unique_ptr<Wall>  UWall;
typedef std::unique_ptr<Worldobject>  UWorldobject;

int main(){


UWall pwalla = UWall(new Wall(Wall::Name::a,100));
UWall pwallb = UWall(new Wall(Wall::Name::b,100));
UWall pwallc = UWall(new Wall(Wall::Name::c,100));
UWall pwalld = UWall(new Wall(Wall::Name::d,100));



int marea1[3]={2,5,3};
int marea2[3]={7,10,10};
int marea3[3]={2,8,0};
int marea4[3]={7,10,10};

pwalla->addnewarea(marea1);
pwalla->addnewarea(marea2);
pwallc->addnewarea(marea3);
pwalld->addnewarea(marea4);

UWorldobject pwobj1=UWorldobject(new Worldobject(1));
UWorldobject pwobj2=UWorldobject(new Worldobject(2));
UWorldobject pwobj3=UWorldobject(new Worldobject(3));

pwobj1->setdim({5,2,3});
pwobj2->setdim({2,15,4});
pwobj3->setdim({10,5,4});

pwobj1->settowall(Wall::Name::c);
pwobj2->settowall(Wall::Name::b);


std::vector<UWall> vwall;
vwall.push_back(std::move(pwalla));
vwall.push_back(std::move(pwallb));
vwall.push_back(std::move(pwallc));
vwall.push_back(std::move(pwalld));

std::vector<UWorldobject> vobjects;
vobjects.push_back(std::move(pwobj1));
vobjects.push_back(std::move(pwobj2));
vobjects.push_back(std::move(pwobj3));

Room myroom(vwall,vobjects);
myroom.setdimh(16);
myroom.setdimv(16);
myroom.placeObjects();
myroom.printWalls();
myroom.printObjects();


return 0;

}
