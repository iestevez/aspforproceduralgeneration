#include "clingo.h"

#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <cstring>
#include <vector>

#ifndef DEF_MODEL_CLASS
#define DEF_MODEL_CLASS

class Model {


public:

Model(std::string const pathtofile);
Model(std::string const pathtofile, std::vector<std::string> vargs);
Model(std::string const pathtofile, std::initializer_list<std::string> const largs);
~Model();
clingo_control_t* getcontrol();
char* getchmodel();

private:
clingo_control_t *ctl = nullptr;
char* chmodel;
char** argv;
int argc;
clingo_program_builder_t *builder;
static clingo_part_t parts[];
// Private members:
void setmodelfromfile(std::string const pathtofile);
void setarguments(std::vector<std::string> vargs);
void creategroundmodel();
static bool on_statement(clingo_ast_statement_t const *stm);

};

#endif //DEF_MODEL_CLASS
