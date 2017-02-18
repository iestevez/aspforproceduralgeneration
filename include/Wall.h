#include <list>
#include <vector>
#include <string>
#ifndef DEF_WALL_CLASS
#define DEF_WALL_CLASS

class Wall {

public:
	typedef int* Maxarea_t;	
	enum class Name {none=0,a=1,b=2,c=3,d=4};


	Wall(Name ori, int l);
	Wall(Name ori, int l, std::vector<Maxarea_t>& v);
	Wall(Name orio, int l, std::initializer_list<std::list<int>> ma_data);
	~Wall();
	void addnewarea(Maxarea_t a);
	void set_length(int l);
	void set_orientation(Wall::Name ori);
	void delallareas();
	int get_length() const;
	Name get_orientation() const;
	void print() const;



	static const char* OrientationStrings[];
	std::vector<Maxarea_t> VMaxareas;

	static Name getwallnamefromstring(std::string strwall);
private:

Name orientation;
int length;

};
#endif //DEF_WALL_CLASS
