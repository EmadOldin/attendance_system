#include "EmploList.h"

void EmploList::add(std::string id, std::string name)
{
	Employee* a = new Employee(id , name);
	this->EL[id] = a;
}

void EmploList::card_insert(std::string id)
{
	if (!(EL.find(id) != EL.end())) {
		if (!EL[id]->status())
			EL[id]->Start();
		else
			EL[id]->End();
	}
	else
		std::cout << "This ID not found" << std::endl;
}

void EmploList::status(std::string id)
{
	if (id == " ")//if you wana check all of your employees
	{
		for (const auto& a : this->EL) {
			a.second->status();
		}
	}
	else//for an exacte person
		EL[id]->status();

}
