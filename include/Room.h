#include <vector>
#include <memory>
#include "Wall.h"
#include "Worldobject.h"
#include "clingo.h"

#ifndef DEF_ROOM_CLASS
#define DEF_ROOM_CLASS
class Room{ 

std::vector<std::unique_ptr<Worldobject>> VObjects;
std::vector<std::unique_ptr<Wall>> VWalls; // Wall and Woldobject shoud be assignables
std::vector<clingo_symbol_t> VCSymbols;
int dimh=0;
int dimv=0;
public:
	Room();
	Room(int dimx, int dimy);
	Room(std::vector<std::unique_ptr<Wall>>& wlist);
	Room(std::vector<std::unique_ptr<Wall>>& wlist, std::vector<std::unique_ptr<Worldobject>>& olist);
	void addNewObject(std::unique_ptr<Worldobject>&   o);
	int placeObjectAt(int id, int x, int y, Worldobject::Rotation rot); 
	void addNewWall(std::unique_ptr<Wall>&  wall);
	void removeAllObjects();
	void removeAllWalls();
	void placeObjects();
	void printObjects();
	void printWalls();
	void setdimh(int h);
	void setdimv(int v);
	int  getdimh() const;
	int  getdimv() const;	
		
	static const char* cl_argv[];
	static const int cl_argc;
 	static const char* baseprogram;	
	int getxfromwallrelative(std::unique_ptr<Worldobject>& obj, Wall::Name w, int pos, Worldobject::Rotation rot); 
	int getyfromwallrelative(std::unique_ptr<Worldobject>& obj, Wall::Name w, int pos, Worldobject::Rotation rot); 
	void addsymbolfromobject(std::unique_ptr<Worldobject>& obj);
	void addsymbolfromwall(std::unique_ptr<Wall>& wall);
	void addsymbolfromroom();
	void prepare_backend(clingo_backend_t* backend);
private:
	static bool on_model(clingo_model_t* model, void* object, bool* goon);
	static bool on_statement(clingo_ast_statement_t const *stm, clingo_program_builder *b);
	void setwalllengthsfromdimensions();
	


};
#endif //DEF_ROOM_CLASS
