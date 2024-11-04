
#include "A1_S8_20230129_20230120.h"
#include<bits/stdc++.h>

using namespace std;

//some used functions

bool isValidHex(const string& hex) {
    if (hex.empty()) return false;

    for (char c : hex) {
        if (!isxdigit(c)) {
            return false;
        }
    }
    return true;
}

int convertHexToDecimal(string hex) {
    if (!isValidHex(hex)) {
        throw invalid_argument("Invalid hexadecimal value: " + hex);
    }
    return stoi(hex, nullptr, 16);
}

string convertDecimalToHex(int x) {
    ostringstream con;
    con << hex << uppercase << (x & 0xFF);
    string hexStr = con.str();
    return hexStr.size() < 2 ? "0" + hexStr : hexStr;
}

//Exception class

HaltException::HaltException(const string& message) : runtime_error(message) {}

//Memory class

Memory::Memory()
{
    for (int i = 0; i < 256; i++) {
            cells[i] = "00";
        }
}

void Memory::setM(int address, const string& val) {
        if (cells.find(address) != cells.end()) {
            cells[address] = val;
        } else {
            throw out_of_range("Memory address out of range");
        }
    }

string Memory::get(int address) const {
        if (cells.find(address) == cells.end() || cells.find(address + 1) == cells.end()) {
            throw out_of_range("Memory address out of range");
        }
        string ir = cells.at(address) + cells.at(address + 1);
        return ir;
    }

string Memory::getM(int address) const {
        if (cells.find(address) == cells.end()) {
            throw out_of_range("Memory address out of range");
        }
        return cells.at(address);
    }

void Memory::displayM(){
        for(int i=0 ; i < 256;i++){
            cout << cells[i] << " ";
        if( i ==15 || i==31 ||i==47 ||i==63 ||i==79|| i==95|| i==111 || i==127
        || i==143 || i==159 ||i==175 ||i==191 || i==207 ||i==223 ||i==239|| i==255){
        cout<<"\n";
        }
        }
    }

bool Memory::isValidHex(const string& hex) {
    if (hex.empty()) return false;

    for (char c : hex) {
        if (!isxdigit(c)) {
            return false;
        }
    }
    return true;
}

bool Memory::getInput(int initialCounter) {
    ifstream file1;
    string filename;
    do {
        cout << "Enter the name of the first file: ";
        cin >> filename;
        file1.open(filename);
        if (!file1.is_open())
            cout << "Error opening the file \n";
    } while (!file1.is_open());

    string word;
    while (file1 >> word) {
        if (!isValidHex(word) || word.length() > 4) {
            cout << "Invalid hex value in file: " << word << "\n";
            file1.close();
            return false;
        }
        if (initialCounter < 256) {
            cells[initialCounter++] = word.substr(0, 2);
            if (initialCounter < 256 && word.length() > 2) {
                cells[initialCounter++] = word.substr(2, 2);
            }
        } else {
            cout << "Warning: Exceeded memory limit while reading from file.\n";
            break;
        }
    }
    if(cells[initialCounter-2]+cells[initialCounter-1]!="C000"){
        cells[initialCounter]="C0";
    }
    file1.close();
    return initialCounter <= 256;
}

//Register class

Register::Register(const string& value) : value(value) {}

void Register::setRegister(const string& newValue) {
        value = newValue;
    }

string Register::getRegister() const {
        return value;
    }

// Instructions class

void Instructions::set(const string& instruction) {
        this->instruction = instruction;
    }

string Instructions::get() const {
        return instruction;
    }

//Machine class

Machine::Machine(int initialCounter): pC(initialCounter), commandCount(0)
{
    for (int i = 0; i < 16; i++) {
            registers[i] = Register();  // Initialize registers
        }

}

void Machine::readFile(){
        if(!mem.getInput(pC)){
            throw runtime_error("Failed to read file. Too many instructions.");
        }
    }

void Machine::setRegisterValue(int num, string value) {
        registers[num].setRegister(value);
    }

string Machine::getRegisterValue(int num)  {
        return registers.at(num).getRegister();
    }



void Machine::display()  {
    cout << "Memory :\n";
    mem.displayM();
    cout << "=======================================================================\n";
    cout << "Registers:\n";
    for (const auto& reg : registers) {
        cout << "Register " << reg.first << ": " << reg.second.getRegister() << "\n";
    }
}

void Machine::fetch() {
    cout << " PC: " << pC << " -> ";
    string instruction = mem.get(pC);
    if (!isValidHex(instruction) || instruction.length() != 4) {
        throw invalid_argument("Fetched instruction is not a valid hex code: " + instruction);
    }
    cout << "Fetched instruction: " << instruction << endl;
    ir.set(instruction);
    pC += 2;
}

void Machine::decode(){
        string instruction = ir.get();
        if (!isValidHex(instruction) || instruction.length() != 4) {
        throw invalid_argument("Decoded instruction is not valid hex: " + instruction);
    }
        commandCount++;
        switch (instruction[0]) {
            case '1': {
                int registerNumber = convertHexToDecimal(instruction.substr(1, 1));
                int memoryAddress = convertHexToDecimal(instruction.substr(2, 2));
                if (memoryAddress < 1 || memoryAddress >= 256) {
                    throw out_of_range("Memory address out of range");
                }
                string memValue = mem.getM(memoryAddress);
                setRegisterValue(registerNumber, memValue);
                break;
            }
            case '2': {  // LOAD immediate value to register
                int registerNumber = convertHexToDecimal(instruction.substr(1, 1));
                string immediateValue = instruction.substr(2, 2);
                setRegisterValue(registerNumber, immediateValue);
                break;
            }
            case '3': {  // STORE from register to memory
                int registerNumber = convertHexToDecimal(instruction.substr(1, 1));
                int memoryAddress = convertHexToDecimal(instruction.substr(2, 2));
                string regValue = getRegisterValue(registerNumber);
                mem.setM(memoryAddress, regValue);
                cout<<"Memory address 00 \n"<<"Current value: "<<regValue<<"\n";
                break;
            }
            case '4': {  // MOVE from one register to another
                int sourceRegister = convertHexToDecimal(instruction.substr(2, 1));
                int destRegister = convertHexToDecimal(instruction.substr(3, 1));
                string valueToMove = getRegisterValue(sourceRegister);
                setRegisterValue(destRegister, valueToMove);
                break;
            }
            case '5': {  // ADD two's complement values
                int destRegister = convertHexToDecimal(instruction.substr(1, 1));
                int srcRegister1 = convertHexToDecimal(instruction.substr(2, 1));
                int srcRegister2 = convertHexToDecimal(instruction.substr(3, 1));

                int value1 = stoi(getRegisterValue(srcRegister1), nullptr, 16);
                int value2 = stoi(getRegisterValue(srcRegister2), nullptr, 16);

                if (value1 > 127) value1 -= 256;
                if (value2 > 127) value2 -= 256;

                int result = (value1 + value2) & 0xFF;

                setRegisterValue(destRegister, convertDecimalToHex(result));
                break;
            }
            case 'B': {  // JUMP if register R is zero
                int registerNumber = convertHexToDecimal(instruction.substr(1, 1));
                int memoryAddress = convertHexToDecimal(instruction.substr(2, 2));
                if(memoryAddress < 0 || memoryAddress >= 256){
                    throw out_of_range("Memory address out of range");
                }
                string registerValue = getRegisterValue(registerNumber);
                string registerZeroValue = getRegisterValue(0);
                int regValue = convertHexToDecimal(registerValue);
                int regZeroValue = convertHexToDecimal(registerZeroValue);
                if(regValue == regZeroValue){
                    pC = memoryAddress;
                }
                break;
            }

            //Bonus instructions

            //OR
            case '7':
                {
                    int destination=convertHexToDecimal(instruction.substr(1,1));
                    int src1=convertHexToDecimal(instruction.substr(2,1));
                    int src2=convertHexToDecimal(instruction.substr(3,1));

                    int stored_src1=convertHexToDecimal(getRegisterValue(src1));
                    int stored_src2=convertHexToDecimal(getRegisterValue(src2));

                    int OR_result=stored_src1 | stored_src2;

                    setRegisterValue(destination,convertDecimalToHex(OR_result));

                    break;

                }

             //And

            case '8':
                {
                    int destination=convertHexToDecimal(instruction.substr(1,1));
                    int src1=convertHexToDecimal(instruction.substr(2,1));
                    int src2=convertHexToDecimal(instruction.substr(3,1));

                    int stored_src1=convertHexToDecimal(getRegisterValue(src1));
                    int stored_src2=convertHexToDecimal(getRegisterValue(src2));

                    int AND_result=stored_src1 & stored_src2;

                    setRegisterValue(destination,convertDecimalToHex(AND_result));

                    break;
                }

              //XOR

            case '9':
                {
                    int destination=convertHexToDecimal(instruction.substr(1,1));
                    int src1=convertHexToDecimal(instruction.substr(2,1));
                    int src2=convertHexToDecimal(instruction.substr(3,1));

                    int stored_src1=convertHexToDecimal(getRegisterValue(src1));
                    int stored_src2=convertHexToDecimal(getRegisterValue(src2));

                    int XOR_result=stored_src1 ^ stored_src2;

                    setRegisterValue(destination,convertDecimalToHex(XOR_result));

                    break;
                }

            //Rotate right

            case 'A':
                {
                    int myregister=convertHexToDecimal(instruction.substr(2,1));
                    int bits=convertHexToDecimal(instruction.substr(3,1));

                    int stored_register=convertHexToDecimal(getRegisterValue(myregister));

                    int rotated = (stored_register >> bits) | (stored_register << (16 - bits));

                    setRegisterValue(myregister,convertDecimalToHex(rotated));

                    break;
                }

            //Halt Execution

            case 'C': {  // HALT
                throw HaltException("Program has been halted.");
            }
            default: {
                break;
            }
        }
    }

    int Machine::number_of_commands_till_HALT() const {
        return commandCount;
    }

//Menu

void menu(Machine& mc) {
    int choice;
    for (;;) {
        cout << "Menu: \n";
        cout << "Enter number of your choice please: \n";
        cout << "1) Load program from file \n";
        cout << "2) Execute program \n";
        cout << "3) Display state \n";
        cout << "4) Exit \n";

        cin >> choice;
        if (choice == 4) {
            cout << "Thank you, bye";
            break;
        }
        else if (choice == 1) {
        int startAddress;
                cout << "Specify memory location for loading (0 to 255):\n";
                cin >> startAddress;
                while(startAddress < 0 || startAddress >= 256) {
                    cout << "Invalid memory location. Try again:\n";
                    cin >> startAddress;
                }
                mc = Machine(startAddress);
                try{
                    mc.readFile();
                } catch (const runtime_error& err){
                    cout << "Error: " << err.what() << "\n";
                    cout << "Try loading a file with fewer instructions.\n";
                }
        }
        else if (choice == 2) {
            try {
        for (int i = 0; i < 256; i += 2) {
            mc.fetch();
            mc.decode();
        }
    } catch (const HaltException& halt) {
        cout << "Halting: " << halt.what() << "\n";
        cout << "Executed " << mc.number_of_commands_till_HALT() << " commands before halting.\n";
    } catch (const invalid_argument& ia) {
        cout << "Error: " << ia.what() << "\n";
    } catch (const out_of_range& oor) {
        cout << "Error: " << oor.what() << "\n";
    }

        }

        else if (choice == 3) {
            mc.display();
        } else {
            cout << "Invalid choice, please try again\n";
        }
    }
}

