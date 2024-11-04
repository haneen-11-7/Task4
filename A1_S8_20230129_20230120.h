#ifndef LASTMACHINE_H
#define LASTMACHINE_H


#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;

class HaltException : public runtime_error {
public:
    HaltException(const string& message);
};

class Memory {
protected:
    map<int, string> cells;
public:
    Memory();
    void setM(int address, const string& val);
    string get(int address) const;
    string getM(int address) const;
    void displayM();
    bool isValidHex(const string& hex);
    bool getInput(int initialCounter);
};

class Register {
private:
    string value;
public:
    Register(const string& value = "00");
    void setRegister(const string& newValue);
    string getRegister() const;
};

class Instructions {
private:
    string instruction;
public:
    void set(const string& instruction);
    string get() const;
};

bool isValidHex(const string& hex);
int convertHexToDecimal(string hex);
string convertDecimalToHex(int x);

class Machine {
private:
    Instructions ir;
    int pC;
    Memory mem;
    map<int, Register> registers;
    int commandCount;
public:
    Machine(int initialCounter);
    void readFile();
    void setRegisterValue(int num, string value);
    string getRegisterValue(int num);
    void display();
    void fetch();
    void decode();
    int number_of_commands_till_HALT() const;
};

void menu(Machine& mc);

#endif // MACHINEE
