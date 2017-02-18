#include "Room.h"
#include "clingo.h"
#include <iostream>
// Constructores
Room::Room(){


}

Room::Room(int h, int v) : dimh(h), dimv(v) {};

Room::Room(std::vector<std::unique_ptr<Wall>>& wlist){

for(auto& w : wlist)
	VWalls.push_back(std::move(w));

wlist.erase(wlist.cbegin(),wlist.cend());
}

Room::Room(std::vector<std::unique_ptr<Wall>>& wlist, std::vector<std::unique_ptr<Worldobject>>& olist){

for(auto& w : wlist)
	VWalls.push_back(std::move(w));

wlist.erase(wlist.cbegin(),wlist.cend());

for(auto& o : olist)
	VObjects.push_back(std::move(o));

olist.erase(olist.cbegin(),olist.cend());

}

void Room::setdimh(int h) { dimh=h;}

void Room::setdimv(int v) { dimv=v;}

int Room::getdimh() const {return dimh;}

int Room::getdimv() const {return dimv;}


// Función principal para lograr un placement
// Funciones auxiliares
void Room::addNewObject(std::unique_ptr<Worldobject>& o){
	VObjects.push_back(std::move(o));

}

void Room::removeAllObjects(){
	VObjects.erase(VObjects.cbegin(),VObjects.cend());
}

void Room::addNewWall(std::unique_ptr<Wall>&  w){
	VWalls.push_back(std::move(w));
}

void Room::removeAllWalls(){

	VWalls.erase(VWalls.cbegin(),VWalls.cend());
}
bool Room::on_statement (clingo_ast_statement_t const *stm, clingo_program_builder *b) {

if (!clingo_program_builder_add(b, stm))
	throw std::runtime_error {"Clingo error: adding statement to program"};
/*
std::cout<<"Stm type: "<<stm->type<<std::endl;
std::cout<<"Stm location: "<<stm->location.begin_line<<" "<<stm->location.end_line<<" "<<stm->location.begin_column<<" "<<stm->location.end_column<<" "<<stm->location.begin_file<<" "<<stm->location.end_file<<std::endl;
if(stm->type==0){
std::cout<<"Rule size: "<<(stm->rule)->size<<std::endl;
std::cout<<"Head type: "<<(stm->rule)->head.type<<std::endl;
std::cout<<"Body: "<<(stm->rule)->body<<std::endl;
std::cout<<"Head literal symbol type: "<<(stm->rule)->head.literal->type<<std::endl;
}
std::cout<<"After adding program sentence"<<std::endl;
*/
return true;

}
void Room::placeObjects(){


clingo_control_t *ctl = nullptr;
clingo_part_t parts[] ={{"base",NULL,0}};
clingo_solve_result_bitset_t solve_ret;

// First: unplacement of all objects in room
for(auto& o: VObjects)
	o->setunplaced(); 
// Second: prepare clingo control
if(!clingo_control_new(cl_argv,cl_argc,NULL,NULL,20, &ctl) !=0) 
	throw std::runtime_error {"Clingo error: creation of control failed"};

/*
if(!clingo_control_program_builder(ctl,&data.builder))
	throw std::runtime_error {"Clingo error: creation of data builder failed"};

if (!clingo_program_builder_begin(data.builder))
	throw std::runtime_error {"Clingo error: stating program builder"};

i*/

//if(!clingo_control_add(ctl,"base",NULL,0,Room::baseprogram))
//	throw std::runtime_error {"Clingo error: adding base program"};

//----------------------------------------------------------------------------------------
// Build the new symbols.
// Refill de symbols container with ground predicates.


Room::VCSymbols.clear();
addsymbolfromroom();
for(auto& obj: Room::VObjects)
	addsymbolfromobject(obj);	
for(auto& wall: Room::VWalls)
	addsymbolfromwall(wall);

//--------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------
// Introduce new symbols. using abstract symbol tree (ast)
// ------------------------------------------------------------------------------------

// Create program builder
clingo_program_builder_t *builder;
if(!clingo_control_program_builder(ctl,&builder))
	throw std::runtime_error {"Clingo error: creating program builder"};

// -------------------------------------------------------------------------------------
// The AST is based on a hierarchy of structures
// In this case: term->literal->head->rule->statement
// -------------------------------------------------------------------------------------


clingo_location_t location;
location.begin_line = location.end_line = 1;
location.begin_column = location.end_column = 1;
location.begin_file = location.end_file = "<string>";

clingo_ast_rule rule;
clingo_ast_head_literal_t head;
clingo_ast_body_literal_t* body;
clingo_ast_literal_t literal,literal1;
clingo_ast_statement stm;

// ----------------------------------------------------------------------------------
// We start program builiding
if(!clingo_program_builder_begin(builder))
	throw std::runtime_error {"Clingo error: starting program building"};

// Read the AST of the program
  if (!clingo_parse_program(baseprogram, (clingo_ast_callback_t*)on_statement, builder, NULL, NULL, 20)) 
	throw std::runtime_error {"Clingo error: parsing program"};
// Loop through symbols.
for(auto sym : VCSymbols){

	// Show symbol string for debugging purposes
	/*
	size_t nsize;
	if(!clingo_symbol_to_string_size(sym,&nsize))
		throw std::runtime_error {"Clingo error: getting symbol size"};
	std::cout<<"Symbol size: "<<nsize<<std::endl;
	char *strsym;
	if(!(strsym=(char*)malloc(sizeof(char)*nsize)))
		std::cout<<"Error allocating"<<std::endl;
	if(!clingo_symbol_to_string(sym,strsym,nsize))
		throw std::runtime_error {"Clingo error: getting symbol string"};
	std::cout<<"Symbol string: "<<strsym<<std::endl;
	free(strsym);
	*/

	// We build the AST hierarchy from term to sentence
	clingo_ast_term_t term;
	term.location=location;
	term.type=clingo_ast_term_type_symbol;
	term.symbol = sym;


	literal.location=location;
	literal.type=clingo_ast_literal_type_symbolic;
	literal.sign=clingo_ast_sign_none;
	literal.symbol= &term;
	

	head.location=location;
	head.type=clingo_ast_head_literal_type_literal;
	head.literal=&literal;

	rule.head=head;
	rule.body=nullptr;
	rule.size=0;

	stm.location=location;
	stm.type= clingo_ast_statement_type_rule;
	stm.rule= &rule;

	if(!clingo_program_builder_add(builder,&stm))
		throw std::runtime_error {"Clingo error: adding statement"};
	

} // End loop over room generated symbols

if(!clingo_program_builder_end(builder))
	throw std::runtime_error {"Clingo error: ending program building"};
if(!clingo_control_ground(ctl,parts,1,NULL,NULL))
	throw std::runtime_error {"Clingo error: grounding base program"};

if(!clingo_control_solve(ctl,on_model,this,NULL,0,&solve_ret))
	throw std::runtime_error {"Clingo error: solving grounded model"};


if(ctl) {clingo_control_free(ctl);}


}

bool Room::on_model(clingo_model_t* model, void* object, bool* goon){

bool ret=true;
*goon = true;
std::cout<<"Modelo encontrado"<<std::endl;
clingo_symbol_t *atoms = NULL; //Symbols to be retrieved
size_t atoms_n; //Number of retrieved symbols
// First: get how many symbols
if (!clingo_model_symbols_size(model, clingo_show_type_shown, &atoms_n))
	throw std::runtime_error {"Clingo error: obtaining the number of atoms"};
// Second: get space for the symobls
try{
	atoms = new clingo_symbol_t [atoms_n];
}
catch (std::bad_alloc& exception)
{
	throw; //rethrow
}
// Third: retrieving symbols
if (!clingo_model_symbols(model, clingo_show_type_shown, atoms, atoms_n))
	throw std::runtime_error {"Clingo error: retrieving symbols from obtained model"};
// Create signature uf function placement
clingo_signature_t sig_placement;
if(!clingo_signature_create("placement",4,true,&sig_placement))
	throw std::runtime_error {"Clingo error: creating signature for placement predicate"};

size_t str_n=0;
char* str=nullptr;
clingo_symbol_t const *it, *ie;
for (it = atoms,  ie = atoms + atoms_n; it != ie; ++it) {
size_t n;
char *str_new;
// determine size of the string representation of the next symbol in the model
 if (!clingo_symbol_to_string_size(*it, &n)) 
	throw std::runtime_error {"Clingo error: getting symbol string size"};
if(str_n<n){
	if (!(str_new = (char*)realloc(str, sizeof(*str) * n)))
		throw std::runtime_error {"Error getting memory for string"};

str=str_new;
str_n=n;
}
if (!clingo_symbol_to_string(*it, str, n))
	throw std::runtime_error {"Clingo error: assigning symbol to string"};

std::cout<<"Symbol: "<<str<<std::endl;
clingo_symbol_t stype;
if(!(stype=clingo_symbol_type(*it)))
	throw std::runtime_error {"Clingo error: getting symbol type"};

if(stype==clingo_symbol_type_function){

const char* sname;


if(!clingo_symbol_name(*it,&sname))
	throw std::runtime_error {"Clingo error: getting symbol name"};
clingo_signature_t symbolsignature;
if(!clingo_signature_create(sname,4,true,&symbolsignature))
	throw std::runtime_error {"Clingo error: creating expected signature"};

if(clingo_signature_is_equal_to(sig_placement,symbolsignature)){
	std::cout<<"Es un placement!"<<std::endl;
	clingo_symbol_t const *args;
	size_t nargs;
	if(!clingo_symbol_arguments(*it,&args,&nargs))
		throw std::runtime_error{"Clingo error: getting symbol arguments"};
	int objid;
	if(!clingo_symbol_number(*args,&objid))
		throw std::runtime_error{"Clingo error: getting obj id from symbol"};
	Wall::Name wall;
	char const *swall;
	if(!clingo_symbol_name(*(args+1), &swall))
		throw std::runtime_error{"Clingo error: getting placement wall from symbol"};
	std::string strwall(swall);
	wall=Wall::getwallnamefromstring(strwall);
 	int objpos;
	if(!clingo_symbol_number(*(args+2), &objpos))
		throw std::runtime_error{"Clingo error: getting obj y from symbol"};
	Worldobject::Rotation objrot;
	char const *srot;
	if(!clingo_symbol_name(*(args+3),&srot))
		throw std::runtime_error{"Clingo error: getting obj rot from symbol"};
	std::string strrot(srot);
	objrot=Worldobject::getrotationfromstring(strrot);
	std::cout<<"Object: "<<objid<<" Wall "<<strwall<<" POS "<<objpos<<" ROT "<<strrot<<std::endl;
	for(auto& obj :((Room *)object)->VObjects){
		if(obj->getid()==objid){
			int newx = ((Room *)object)->getxfromwallrelative(obj,wall,objpos,objrot);
			int newy = ((Room *)object)->getyfromwallrelative(obj,wall,objpos,objrot);
			obj->setplacement(newx,newy,objrot);		
			break;
		}
	}
	
}


}


}
delete atoms;

return ret;

}

int Room::getxfromwallrelative(std::unique_ptr<Worldobject>& obj,Wall::Name w, int pos, Worldobject::Rotation rot){
int ret=0;
switch(w){

	case Wall::Name::a:
		ret=pos;
		break;
	case Wall::Name::b:
		ret=0;
		break;
	case Wall::Name::c:
		ret=pos;
		break;
	case Wall::Name::d:
		if(rot==Worldobject::Rotation::a || rot==Worldobject::Rotation::c)
			ret=(VWalls.at(0)->get_length())-(obj->getdimx());
		else
			ret=(VWalls.at(0)->get_length())-(obj->getdimy());
		break;
	default:
		ret=pos;
}

return ret;

}


int Room::getyfromwallrelative(std::unique_ptr<Worldobject>& obj,Wall::Name w, int pos, Worldobject::Rotation rot){
int ret=0;
switch(w){

	case Wall::Name::a:
		ret=0;
		break;
	case Wall::Name::b:
		ret=pos;
		break;
	case Wall::Name::c:
		if(rot==Worldobject::Rotation::a || rot==Worldobject::Rotation::c)
			ret=(VWalls.at(1)->get_length())-(obj->getdimy());
		else
			ret=(VWalls.at(1)->get_length())-(obj->getdimx());
		break;
	default:
		ret=0;
}

return ret;

}

int  Room::placeObjectAt(int id, int x,int y, Worldobject::Rotation rot){

int ret=0;
for(auto& obj : VObjects){

if(obj->getid() == id){
	obj->setplacement(x,y,rot);
	ret=1;
	break;

}
}
return ret;

}

void Room::setwalllengthsfromdimensions(){
Wall::Name orientation=Wall::Name::none;
for(auto& w : VWalls){
	orientation=w->get_orientation();		
	if(orientation == Wall::Name::a)
		w->set_length(dimh);
	else if(orientation == Wall::Name::b)
		w->set_length(dimv);
	else if(orientation == Wall::Name::c)
		w->set_length(dimh);
	else if(orientation == Wall::Name::d)
		w->set_length(dimv);
}


}
void Room::addsymbolfromobject(std::unique_ptr<Worldobject>& obj){

int id=obj->getid();
int dimx=obj->getdimx();
int dimy=obj->getdimy();
int height=obj->getheight();
Wall::Name wall=obj->gettowall();

clingo_symbol_t sid,sdimx,sdimy,sheight,swall;
clingo_symbol_create_number(id,&sid);
clingo_symbol_create_number(dimx,&sdimx);
clingo_symbol_create_number(dimy,&sdimy);
clingo_symbol_create_number(height,&sheight);

std::string strwall(Wall::OrientationStrings[(int)wall]);

if(!clingo_symbol_create_id(strwall.c_str(), true, &swall))
	throw std::runtime_error {"Clingo error: creating symbol id for wall"};


clingo_symbol_t sobject,ssize,stowall;
clingo_symbol_t sobject_args[1], ssize_args[4],stowall_args[2];
sobject_args[0]=sid;
ssize_args[0]=sid; ssize_args[1]=sdimx; ssize_args[2]=sdimy; ssize_args[3]=sheight;
stowall_args[0]=sid;
stowall_args[1]=swall;

if(!clingo_symbol_create_function("object",sobject_args,1,true,&sobject))
	throw std::runtime_error {"Clingo error: creating object predicate"};
//std::unique_ptr<clingo_symbol_t> uptr_sobject = std::unique_ptr<clingo_symbol_t>(sobject);
Room::VCSymbols.push_back(sobject);
if(!clingo_symbol_create_function("size",ssize_args,4,true,&ssize))
	throw std::runtime_error {"Clingo error: creating size predicate"};
Room::VCSymbols.push_back(ssize);
if(!clingo_symbol_create_function("towall",stowall_args,2,true,&stowall))
	throw std::runtime_error {"Clingo error: creating towall predicate"};
Room::VCSymbols.push_back(stowall);



}

void Room::addsymbolfromwall(std::unique_ptr<Wall>& wall){

Wall::Name name=wall->get_orientation();
std::string strname = std::string(Wall::OrientationStrings[(int)name]);
clingo_symbol_t sname;
if(!clingo_symbol_create_id(strname.c_str(),true, &sname))
	throw std::runtime_error {"Clingo error: creating symbol name for wall"};
clingo_symbol_t swall;
clingo_symbol_t swall_args[1];
swall_args[0]=sname;
if(!clingo_symbol_create_function("wall",swall_args,1,true, &swall))
	throw std::runtime_error {"Clingo error: creating wall predicate"};
VCSymbols.push_back(swall);
for(auto ma : wall->VMaxareas){
	int start,end,height;
	start = *ma;	
	end = *(ma+1);
	height = *(ma +2);
	clingo_symbol_t sstart,send,sheight;
	clingo_symbol_create_number(start,&sstart);
	clingo_symbol_create_number(end,&send);
	clingo_symbol_create_number(height,&sheight);
	clingo_symbol_t smaxheight_args[4];
	smaxheight_args[0]=sname;
	smaxheight_args[1]=sstart;
	smaxheight_args[2]=send;
	smaxheight_args[3]=sheight;
	clingo_symbol_t smaxheight;
	if(!(clingo_symbol_create_function("maxheight",smaxheight_args,4,true,&smaxheight)))
		throw std::runtime_error {"Clingo error: createing function maxheight"};
	VCSymbols.push_back(smaxheight);
}




}

void Room::addsymbolfromroom(){

	clingo_symbol_t sdimh,sdimv;
	clingo_symbol_create_number(dimh,&sdimh);
	clingo_symbol_create_number(dimv,&sdimv);
	clingo_symbol_t sh,sv;
	if(!clingo_symbol_create_id("h",true,&sh))
		throw std::runtime_error {"Clingo error: create h symbol"};
	if(!clingo_symbol_create_id("v",true,&sv))
		throw std::runtime_error {"Clingo error: create v symbol"};
	clingo_symbol_t sdimroomh, sdimroomv, sdimroom_args[2];
	sdimroom_args[0]=sh;
	sdimroom_args[1]=sdimh;
	if(!clingo_symbol_create_function("dimroom", sdimroom_args,2,true,&sdimroomh))
		throw std::runtime_error {"Clingo error: creating dimroom predicate for h"};
	sdimroom_args[0]=sv;
	sdimroom_args[1]=sdimv;
	if(!clingo_symbol_create_function("dimroom", sdimroom_args,2,true,&sdimroomv))
		throw std::runtime_error {"Clingo error: creating dimroom predicate for v"};
	VCSymbols.push_back(sdimroomh);
	VCSymbols.push_back(sdimroomv);

}

void Room::printObjects(){

std::cout<<"------Objects-------"<<std::endl;

for(auto& o: VObjects)
	o->print();
	std::cout<<"------------------------"<<std::endl;
}

void Room::printWalls(){

std::cout<<"-----Walls--------"<<std::endl;

for(auto& w: VWalls)
	w->print();
	std::cout<<"------------------------------"<<std::endl;

}


const char* Room::cl_argv[3]={"--models","50"};
const int Room::cl_argc=3;
const char* Room::baseprogram=R"ZXSP(
%wall(a;b;c;d).

%  All wall lengths are equal. 
%dimroom((h;v),16).

% Wall restricted areas:
% maxheight/4: wall, start, end, max height
%maxheight(a,2,5,3).
%maxheight(a,7,10,0).
%maxheight(c,2,5,3).
%maxheight(c,7,10,0).

% We have also objects.
%object(1..3).

% Object dimension (all of them are cubes):
% size/4: object id, hor. length, ver length. height
%size(1,5,2,3).
%size(2,2,15,4).
%size(3,10,5,4).

% Object sides that shoud be oriented towards the wall.
% towall/2: ide object, side
%towall(1,c).
%towall(2,b).

% --------------------------------------------------------
% Second section: problem definition (auxiliary, generation and constraints).
% --------------------------------------------------------
% Auxiliary definitions 
% --------------------------------------------------------

% Auxiliary: object effective length
% Auxiliary: length of each wall.
lenw((a;c),L):-dimroom(h,L).
lenw((b;d),L):-W=(b;d),dimroom(v,L).
numobjects(X):-X=#max{O:object(O)}.

% We define rotations using the same names as walls
% rotation(W) means side a goes to occupy W orientation. Therefore rotation(a) means no rotation.
rotation(W):-wall(W).

% We define the rotation operator oprot(side, number of 90º rots counter clock-wise, new side at the position of the original side)
% This is equivalent to oprot(W,n,W1). If n=0 side a is towards wall
% If n=1 then side b is towards wall.
% If n=2 then side c is towards wall.
% If n=3 then side d is towards wall.
oprot(a,0,a). oprot(a,1,b). oprot(a,2,c). oprot(a,3,d).
oprot(b,0,b). oprot(b,1,c). oprot(b,2,d). oprot(b,3,a).
oprot(c,0,c). oprot(c,1,d). oprot(c,2,a). oprot(c,3,b).
oprot(d,0,d). oprot(d,1,a). oprot(d,2,b). oprot(d,3,c).

% Valid lengths for an object are dimx or dimy
validlong(O,L):-object(O),size(O,L,_,_).
validlong(O,L):-object(O),size(O,_,L,_). 
% Horizontal dimension is dimx
hordim(O,L):-size(O,L,_,_).
% Vertical dimension is dimy
verdim(O,L):-size(O,_,L,_).
% Dimension of a particular face in an object
dimface(O,F,L):-F=(a;c),hordim(O,L).
dimface(O,F,L):-F=(b;d),verdim(O,L).

% Effective area is the projection of object at the floor. 
% effectivearea/5. 1. Object 2,3. X1,Y1 coordinates. 4,5. X2,Y2 coordinates
% Depends on object dimensions and object placement.
% Notice that placement is defined as placement/4where 1. Object, 2. Wall, 3. Position at the wall. 4. Rotation.
effectivearea(O,X1,Y1,X2,Y2):-object(O),size(O,L,H,_),placement(O,a,OP,R),X1=OP,X2=(OP+L),Y1=0,Y2=H,#false:R=(b;d).
effectivearea(O,X1,Y1,X2,Y2):-object(O),size(O,L,H,_),placement(O,a,OP,R),X1=OP,X2=(OP+H),Y1=0,Y2=L,#false:R=(a;c).
effectivearea(O,X1,Y1,X2,Y2):-object(O),size(O,L,H,_),placement(O,b,OP,R),X1=0,X2=L,Y1=OP,Y2=(OP+H),#false:R=(b;d).
effectivearea(O,X1,Y1,X2,Y2):-object(O),size(O,L,H,_),placement(O,b,OP,R),X1=0,X2=H,Y1=OP,Y2=(OP+L),#false:R=(a;c).
effectivearea(O,X1,Y1,X2,Y2):-object(O),size(O,L,H,_),placement(O,c,OP,R),lenw(b,W),X1=OP,X2=(OP+L),Y1=W-H,Y2=W,#false:R=(b;d).
effectivearea(O,X1,Y1,X2,Y2):-object(O),size(O,L,H,_),placement(O,c,OP,R),lenw(b,W),X1=OP,X2=(OP+H),Y1=W-L,Y2=W,#false:R=(a;c).
effectivearea(O,X1,Y1,X2,Y2):-object(O),size(O,L,H,_),placement(O,d,OP,R),lenw(a,W),X1=W-L,X2=W,Y1=OP,Y2=(OP+H),#false:R=(b;d).
effectivearea(O,X1,Y1,X2,Y2):-object(O),size(O,L,H,_),placement(O,d,OP,R),lenw(a,W),X1=W-H,X2=W,Y1=OP,Y2=(OP+L),#false:R=(a;c).

% fitroom/1. Determines if object O at fitroom(O) fits at room. A placement is assumed for object O.
fitroom(O):-object(O),effectivearea(O,X1,Y1,X2,Y2),X1>=0,Y1>=0, lenw(a,La),lenw(b,Lb),X2<=La,Y2<=Lb.

% Determining overlaping
% ----------------------
% overlaphor(O1,O2) determines if there is an overlap in the horizontal dimensions between object O1 and object O2.
overlaphor(O1,O2):-effectivearea(O1,X11,_,X12,_),effectivearea(O2,X21,_,X22,_),X12>X21,X12<X22.
overlaphor(O1,O2):-effectivearea(O1,X11,_,X12,_),effectivearea(O2,X21,_,X22,_),X22>X11,X22<X12.
% overlapver(O1,O2) is the same as overlaphor(O1,O2) but in the vertical dimension.
overlapver(O1,O2):-effectivearea(O1,_,Y11,_,Y12),effectivearea(O2,_,Y21,_,Y22),Y12>Y21,Y12<Y22.
overlapver(O1,O2):-effectivearea(O1,_,Y11,_,Y12),effectivearea(O2,_,Y21,_,Y22),Y22>Y11,Y22<Y12.
% overlap(O1,O2) determines an overlap betweeb object O1 and object O2.
overlap(O1,O2):-O1!=O2,overlaphor(O1,O2),overlapver(O1,O2).
% ----------------------

% Determining which side of othe object is towards the wall.
% ----------------------
% facetowall(O,a) is true is object O has its "a" side towards wall.
facetowall(O,a):-placement(O,W,_,W1),wall(W),oprot(W,0,W1).
facetowall(O,b):-placement(O,W,_,W1),wall(W),oprot(W,3,W1).
facetowall(O,c):-placement(O,W,_,W1),wall(W),oprot(W,2,W1).
facetowall(O,d):-placement(O,W,_,W1),wall(W),oprot(W,1,W1).
% ----------------------

% Determining if object fit at wall
% --------------------------------
fitwall(O):-facetowall(O,F),placement(O,W,OP,_),lenw(W,L),dimface(O,F,LF),(OP+LF)<=L.
% --------------------------------

% ------------------------------------------------
% Generative part of model.
% ------------------------------------------------

% For each object we need one placement.
% placement/4. 1. Object, 2. Wall, 3. Position at wall, 4. Rotation (a,b,c,d).
{placement(O,W,OP,R):wall(W),lenw(W,L),OP=0..L,rotation(R)}<=1:-object(O).

% ---------------------------------------------------
% Constraints.
% ----------------------------------------------------

% If towall(O,W) then facetowall(O,W)
:-object(O),wall(W),placement(O,_,_,_),towall(O,W),not facetowall(O,W).
% Every object placement should make the object to fit at room
:-object(O),placement(O,_,_,_),not fitroom(O).
% Avoid overlaping of any two objects.
:-object(O1),object(O2), placement(O1,_,_,_), placement(O2,_,_,_),overlap(O1,O2).
% Use maxheight constraints
%:-object(O),facetowall(O,F),placement(O,W,OP,_),maxheight(W,S,E,H),dimface(O,F,LF), size(O,_,_,HO),overlap1d(OP,OP+LF,S,E),HO>O.

% Weak constraint.
% ----------------
% Try to place as many objects as possible.
#maximize{1@1,O: placement(O,_,_,_)}.
% -----------------
%:-placement(O,_,_,_). [ -1@1,O ]
%:-object(O),wall(W),rotation(R),lenw(W,L),not placement(O,W,OP,R).
%:-#count{placement(O,W,OP,R):object(O),wall(W),lenw(W,L),OP=0..L,rotation(R)}!=1.

#show numobjects/1.
#show effectivearea/5.
#show placement/4.
)ZXSP";
