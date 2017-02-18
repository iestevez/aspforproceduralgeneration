#include "Worldobject.h"
#include <iostream>
#include <string>

	Worldobject::Worldobject(int id):	id(id),
					    dimx(Defaultdim),
					    dimy(Defaultdim) {};
	void Worldobject::settowall(Wall::Name w) {towall=w;}; // Set towall constraint
	void Worldobject::setdim(std::initializer_list<int> l){
		std::initializer_list<int>::iterator it = l.begin();
		dimx=*(it++);
		dimy=*(it++);
		height=*it;
		
	}; // Set dimx and dimy with list
	void Worldobject::setplacement(int x, int y, Rotation rot) {

			x=x;
			y=y;
			rotation=rot;
			placement=true;
	}; // Set placement: x, y, and rotation
	void Worldobject::setunplaced() {
		placement=false;
	};
	Worldobject::Placement Worldobject::getplacement() const {
		Placement pl;
		pl.x=x;
		pl.y=y;
		pl.rot=rotation;
		return pl;

	}	

void Worldobject::print() const{

std::cout<<"Id: "<<id<<" Dimx: "<<dimx<<" Dimy: "<<dimy<<" Height: "<<height<<std::endl;
std::cout<<"To wall: "<<Wall::OrientationStrings[static_cast<int>(towall)]<<" Rotation: "<<RotationStrings[static_cast<int>(rotation)]<<std::endl;
std::string splacement =((placement==true)? "True" : "False");
std::cout<<"Placement: "<<splacement<<std::endl;
if(placement)
	std::cout<<"Pos x: "<<x<<" Pos y: "<<y<<std::endl;

}

int Worldobject::getid() const{

return id;

}

int Worldobject::getdimx() const {
return dimx;
}

int Worldobject::getdimy() const {
return dimy;
}

int Worldobject::getheight() const {
return height;
}

Wall::Name Worldobject::gettowall() const {
return towall;
}

const char* Worldobject::RotationStrings[5]={"Undefined","a","b","c","d"};

Worldobject::Rotation Worldobject::getrotationfromstring(std::string srot){
	Worldobject::Rotation ret=Worldobject::Rotation::none;
	if(srot=="a")
		ret=Worldobject::Rotation::a;
	else if(srot=="b")
		ret=Worldobject::Rotation::b;
	else if(srot=="c")
		ret=Worldobject::Rotation::c;
	else if(srot=="d")
		ret=Worldobject::Rotation::d;
	return ret;
}
	
