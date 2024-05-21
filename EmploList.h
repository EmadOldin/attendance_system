#ifndef EMPLOLIST_H
#define EMPLOLIST_H
#include"Employee.h"
#include<map>

class EmploList
{
	std::map<std::string, Employee*> EL;
public:
	void add(std::string, std::string);
	void card_insert(std::string);
	void status(std::string id = " ");


};
#endif