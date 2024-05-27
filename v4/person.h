class Person {
public:
  String username;  // Member variable for username
  String id;
  int hozor;
  unsigned long start;
  unsigned long stop;
  
  Person(const char* username, const char* id, int hozor, unsigned long start, unsigned long stop) {
    this->username = String(username);
    this->id = String(id);
    this->hozor = hozor;
    this->start = start;
    this->stop = stop;
  }
};