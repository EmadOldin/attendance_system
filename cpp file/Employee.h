#ifndef EMPLOYEE_H
#define EMPLOYEE_H
#include<iostream>
#include<ctime>
class Employee
{
	std::string username;
	const std::string id;
	bool isPresent = false;
	time_t start;
	time_t end;
	time_t manHour;
	
public:
	Employee(std::string, std::string);
	void Start();
	void End();
	bool status();
	void info();
	
};

#endif