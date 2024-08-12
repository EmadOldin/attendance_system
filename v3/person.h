#ifndef PERSON_H
#define PERSON_H
#include <iostream>
class Person
{
public:
  std::string username;
  std::string id;
  int hozor;
  unsigned long start;
  unsigned long stop;

  Person(const char *username, const char *id, int hozor = 0, unsigned long start = 0, unsigned long stop = 0)
  {
    this->username = std::string(username);
    this->id = std::string(id);
    this->hozor = hozor;
    this->start = start;
    this->stop = stop;
  }
};
#endif