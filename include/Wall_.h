#include <list>

#ifndef DEF_WALL_CLASS
#define DEF_WALL_CLASS

class Wall {

enum class Name {none,a,b,c,d};
Name orientation;
int length;
std::vector<Maxarea_t> VMaxareas;
public:
	typedef int* Maxarea_t;
	Wall(Name ori, int l);
	Wall(Name ori, int l, std::vector<Maxarea_t>& v);
	Wall(Name orio, int l, std::initializer_list<std::list<int>> ma_data);
	~Wall();
	void addnewarea(Maxarea_t a);
	void set_length(int l);
	void set_orientation(Wall::Name ori);
	void Wall::delallareas();
	int Wall::get_length() const;
	Name get_orientation() const;
	void print() const;



	static const char* OrientationStrings[];
};
#endif //DEF_WALL_CLASS
