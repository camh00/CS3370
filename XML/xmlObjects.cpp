/*
Author: Cameron Hancock
2/28/2024
This program reads in data from an XML file and creates employee objects which are then stored in
a .bin file.
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>

using namespace std;

struct EmployeeRec {
    int id;
    char name[31];
    char address[26];
    char city[21];
    char state[21];
    char country[21];
    char phone[21];
    double salary;
};

class Employee {
private:
    string name;
    int id;
    string address;
    string city;
    string state;
    string country;
    string phone;
    double salary;

public:
    Employee(int _id = 0) : id(_id), salary(0.0) {}

    void display(ostream& os) const {
        os << "id: " << id << endl;
        os << "name: " << name << endl;
        os << "address: " << address << endl;
        os << "city: " << city << endl;
        os << "state: " << state << endl;
        os << "country: " << country << endl;
        os << "phone: " << phone << endl;
        os << "salary: " << salary << endl << endl;
    }

    // Display object data in xml format
    void toXML(ostream& os) const {
        os << "<Employee>" << endl;
        os << "    <Name>" << name << "</Name>" << endl;
        os << "    <ID>" << id << "</ID>" << endl;
        os << "    <Address>" << address << "</Address>" << endl;
        os << "    <City>" << city << "</City>" << endl;
        os << "    <State>" << state << "</State>" << endl;
        os << "    <Country>" << country << "</Country>" << endl;
        os << "    <Phone>" << phone << "</Phone>" << endl;
        os << "    <Salary>" << salary << "</Salary>" << endl;
        os << "</Employee>" << endl << endl;
    }

    // Getters for private member variables
    string getName() const {return name;}
    int getId() const {return id;}
    string getAddress() const {return address;}
    string getCity() const {return city;}
    string getState() const {return state;}
    string getCountry() const {return country;}
    string getPhone() const {return phone;}
    double getSalary() const {return salary;}

    // Setters for private member variables
    void setName(string newName) {name = newName;}
    void setID(int newID) {id = newID;}
    void setAddress(string newAddress) {address = newAddress;}
    void setCity(string newCity) {city = newCity;}
    void setState(string newState) {state = newState;}
    void setCountry(string newCountry) {country = newCountry;}
    void setPhone(string newPhone) {phone = newPhone;}
    void setSalary(double newSalary) {salary = newSalary;}

    // Write objects to file
    void write(ostream& os) const {
        EmployeeRec record;
        record.id = id;
        strncpy(record.name, name.c_str(), sizeof(record.name));
        strncpy(record.address, address.c_str(), sizeof(record.address));
        strncpy(record.city, city.c_str(), sizeof(record.city));
        strncpy(record.state, state.c_str(), sizeof(record.state));
        strncpy(record.country, country.c_str(), sizeof(record.country));
        strncpy(record.phone, phone.c_str(), sizeof(record.phone));
        record.salary = salary;

        os.write(reinterpret_cast<const char*>(&record), sizeof(EmployeeRec));
    }

    // Using write, Store oject data in file
    void store(iostream& ios) const {
        streampos pos = ios.tellp();
        ios.seekp(id * sizeof(EmployeeRec), ios.beg);
        write(ios);
        ios.seekp(pos);
    }

    // take the EmployeeRec struct and makes objects out of it
    static Employee* read(istream& is) {
        EmployeeRec record;
        is.read(reinterpret_cast<char*>(&record), sizeof(EmployeeRec));

        if (is.gcount() == 0)
            return nullptr;

        Employee* emp = new Employee(record.id);
        emp->name = record.name;
        emp->address = record.address;
        emp->city = record.city;
        emp->state = record.state;
        emp->country = record.country;
        emp->phone = record.phone;
        emp->salary = record.salary;

        return emp;
    }

    // Look for and retrieve employees based on their ID
    static Employee* retrieve(const vector<unique_ptr<Employee>>& employees, int empId) {
    for (const auto& emp : employees) {
            if (emp->getId() == empId) {
                return emp.get();
            }
        }
        return nullptr; // Employee with the specified ID not found
    }

    // Read in XML files and build employee objects Which are put into a vector
    static vector<Employee*> fromXML(istream& is) {
    string tag;
    string value;
    vector<Employee*> employees;
    int noClose = 0;
    int firstTag = 1;
    int bypass = 0;
    while (is) {
        // checks if no previous closing employee tag was provided
        if (noClose == 0) {
            getline(is, tag, '>');
            tag += '>';
        }
        noClose = 0;
        // Erase leading and trailing whitespaces and newlines
        tag.erase(0, tag.find_first_not_of(" \t\n"));
        tag.erase(tag.find_last_not_of(" \t\n") + 1);

        if (tag.empty()) // Skip empty tags
            continue;

        if (tag[0] == '/') // Handle closing tags
            continue;

        if (tag[tag.size() - 2] == '/' && tag[0] != '<') // Self-closing tag
            continue;

        // Convert tag to lowercase
        transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
        if (tag.find("employee") == string::npos && firstTag == 1) {
            bypass = 1;
        }
        firstTag = 0;

        if (tag.find("employee") != string::npos || bypass == 1) {
            Employee* emp = new Employee();
            while (true) {
                // Check if tag has two tags combined due to lack of closing '>'
                if (size(tag) > 10) {
                    tag = tag.substr(1);
                    tag = tag.substr(tag.find('<'));
                }
                else if (bypass == 1) {
                    bypass = 0;
                }
                else {
                    getline(is, tag, '>');
                    tag += '>';
                }
                // Erase leading and trailing whitespaces and newlines
                tag.erase(0, tag.find_first_not_of(" \t\n"));
                tag.erase(tag.find_last_not_of(" \t\n") + 1);
                
                // Convert tag to lowercase
                transform(tag.begin(), tag.end(), tag.begin(), ::tolower);

                if (tag.empty()) // Skip empty tags
                    continue;

                if (tag.find("</employee>") != string::npos) {
                    employees.push_back(emp);
                    break;
                }
                if (tag.find("<employee>") != string::npos) {
                    employees.push_back(emp);
                    noClose = 1;
                    break;
                }
                if (tag[0] == '/')
                    continue;
                getline(is, value, '<');
                if (value.empty())
                    break;

                // Fill in employee data
                if (tag.find("name") != string::npos)
                    emp->name = value;
                else if (tag.find("id") != string::npos)
                    emp->id = stoi(value);
                else if (tag.find("address") != string::npos)
                    emp->address = value;
                else if (tag.find("city") != string::npos || tag.find("village") != string::npos)
                    emp->city = value;
                else if (tag.find("state") != string::npos)
                    emp->state = value;
                else if (tag.find("country") != string::npos)
                    emp->country = value;
                else if (tag.find("phone") != string::npos)
                    emp->phone = value;
                else if (tag.find("salary") != string::npos)
                    emp->salary = stod(value);
                else
                    throw runtime_error("Invalid tag: " + tag);
            }
        }
    }

    return employees;
}




};

int main(int argc, char* argv[]) {
    // Verify that a file argument was provided
    if (argc < 2) {
        cerr << "Error: No input file provided" << endl;
        return 1;
    }

    // Search for file
    string inputFileName = argv[1];
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error: Unable to open input file: " << inputFileName << endl;
        return 1;
    }

    // Create a vector to fill with Employee objects
    vector<unique_ptr<Employee>> employees;

    // Read file and create Employee objects
    try {
        vector<Employee*> parsedEmployees = Employee::fromXML(inputFile);
        for (auto emp : parsedEmployees) {
            employees.push_back(unique_ptr<Employee>(emp));
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }

    // Display Employee data
    for (const auto& emp : employees) {
        emp->display(cout);
    }

    // write employees to employee.bin
    ofstream outputFile("employee.bin", ios::binary);
    for (const auto& emp : employees) {
        emp->write(outputFile);
    }
    outputFile.close();

    employees.clear();

    // Repopulate employees from employee.bin
    ifstream inputFileBin("employee.bin", ios::binary);
    try {
        while (true) {
            Employee* emp = Employee::read(inputFileBin);
            if (!emp)
                break;
            employees.push_back(unique_ptr<Employee>(emp));
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
    inputFileBin.close();

    for (const auto& emp : employees) {
        emp->toXML(cout);
    }

    // ifstream fileStream("employee.bin", ios::binary);
    Employee* retrieved = Employee::retrieve(employees, 12345);
    if (retrieved) {
        cout << "Found employee: " << endl;
        retrieved->setSalary(150000.0);
        cout << "new salaray set" << endl;
        fstream file("employee.bin", ios::in | ios::out | ios::binary);
        retrieved->store(file);
        file.close();
    }

    retrieved = Employee::retrieve(employees, 12345);
    if (retrieved) {
        cout << "Salary after update: " << retrieved->getSalary() << endl << endl;
    }
    employees.clear();

    // Create a new Employee object
    Employee* newEmployee = new Employee(99999);
    newEmployee->setName("Johnathan deer");
    newEmployee->setAddress("555 Fake Street");
    newEmployee->setCity("Central");
    newEmployee->setState("Utah");
    newEmployee->setCountry("USA");
    newEmployee->setPhone("555-555-5555");
    newEmployee->setSalary(80000.0);


    // Write new employee to .bin
    fstream file("employee.bin", ios::in | ios::out | ios::binary);
    newEmployee->store(file);
    file.close();
    cout << "new employee successfully stored" << endl;
    delete newEmployee;

    // Reopen file and repopulate employees
    file.open("employee.bin", ios::in | ios::out | ios::binary);
    file.seekg(0, ios::beg);

    try {
        while (true) {
            Employee* emp = Employee::read(file);
            if (!emp)
                break;
            employees.push_back(unique_ptr<Employee>(emp));
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
    inputFileBin.close();

    // Retrieve and display the new Employee object
    retrieved = Employee::retrieve(employees, 99999);
    if (retrieved) {
        cout << "Retrieved new Employee: " << endl;
        retrieved->display(cout);
        delete retrieved;
    }
    file.close();
    employees.clear();

    return 0;
}
