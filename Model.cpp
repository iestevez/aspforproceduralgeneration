#include "Model.h"

clingo_part_t Model::parts[]={{ "base", NULL,0}};
 
Model::Model(std::string const pathtofile) {

setmodelfromfile(pathtofile);
setarguments(std::vector<std::string>());
}

Model::Model(std::string const pathtofile, std::vector<std::string> vargs) {

setmodelfromfile(pathtofile);
setarguments(vargs);
}

Model::Model(std::string const pathtofile, std::initializer_list<std::string> const largs) {
std::vector<std::string> vargs;
for(auto s : largs)
	vargs.push_back(s);
setmodelfromfile(pathtofile);
setarguments(vargs);
}




Model::~Model(){

delete chmodel;
char **bargv=argv;
for(int i=0;i<argc;i++)
	delete *(bargv++);
delete argv;

}

void Model::setmodelfromfile(std::string const pathtofile) {

std::ifstream fs(pathtofile.c_str(), std::ifstream::in);
std::string strmodel;

fs.seekg(0,std::ios::end);
int size=fs.tellg();
fs.seekg(0,std::ios::beg);

strmodel.assign((std::istreambuf_iterator<char>(fs)),(std::istreambuf_iterator<char>()));
chmodel= new char [size+1];

std::strcpy(chmodel, strmodel.c_str());


}

void Model::setarguments(std::vector<std::string> vargs){

int length=vargs.size();
argv=new char* [length];
argc=length;
char** bargv = argv;
for( std::string s : vargs){
	(*bargv)=new char [s.size()+1];
	std::strcpy(*bargv,s.c_str());
	bargv++;
}

}

void Model::creategroundmodel(){
if(!clingo_control_new(argv,argc,NULL,NULL,20,&ctl)!=0) throw std::runtime_error {"CLINGO ERROR: At control object creation"};
if(!clingo_control_program_builder(ctl, &builder)) throw std::runtime_error {"CLINGO ERROR: At builder object creation"};
if(!clingo_program_builder_begin(builder)) throw std::runtime_error {"CLINGO ERROR: Beginning program building"};
if(!clingo_parse_program(chmodel, (clingo_ast_callback_t*) Model::on_statement, NULL,NULL,NULL,20)) throw std::runtime_error {"CLINGO ERROR: Parsing program"};
if(!clingo_program_builder_end(builder)) throw std::runtime_error {"CLINGO ERROR: Ending program building"};
if(!clingo_control_ground(ctl,parts,1,NULL,NULL)) throw std::runtime_error {"CLINGO ERROR: Grounding program"};
}

clingo_control_t* Model::getcontrol(){

return ctl;

}

char* Model::getchmodel(){

return chmodel;
}

bool Model::on_statement(clingo_ast_statement_t const *stm){
return true;
}


