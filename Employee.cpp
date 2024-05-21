#include "Employee.h"

Employee::Employee(std::string name, std::string id) :id(id), username(name)
{
	std::cout << "The employee added successfuly" << std::endl;
}

void Employee::Start() {
	start = time(NULL);//millis
	isPresent = true;
}

void Employee::End()
{
	end = time(NULL);//millis
	manHour = end - start;
	isPresent = false;

}

bool Employee::status()
{
	//you can add message function here:)
	return isPresent;
}

void Employee::info()
{
	std::cout << "Username: " << username << std::endl;
	std::cout << "Man Hour: " << end - start << std::endl;
}
