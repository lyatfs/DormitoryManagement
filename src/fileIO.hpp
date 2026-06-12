#pragma once

#include "config.hpp"
#include "libs/vector.hpp"
#include <fstream>
#include <sstream>
#include <string>

/*
─────────────────────────────────────────────────────────────────────────────
Convert file line into record
─────────────────────────────────────────────────────────────────────────────
*/
base::Vector<std::string> parseRecord(const std::string& line) {
    std::stringstream ss(line);
    std::string       token;

    base::Vector<std::string> record;

    while (std::getline(ss, token, ',')) {
        record.push_back(token);
    }

    return record;
}

/*
─────────────────────────────────────────────────────────────────────────────
Students I/O
─────────────────────────────────────────────────────────────────────────────
*/
void loadStudents() {
    std::ifstream fin{std::string(STUDENTS_FILES)};

    std::string line;
    while (std::getline(fin, line)) {
        base::Vector<std::string> record = parseRecord(line);

        Student student;

        student.id           = record[0];
        student.name         = record[1];
        student.studentClass = record[2];
        student.isPriority   = (record[3] == "true");
        student.phone        = record[4];
        student.email        = record[5];

        studentsList.push_back(student);
    }

    fin.close();
}

void saveStudents() {
    std::ofstream fout{std::string(STUDENTS_FILES)};

    bool isFirstline = true;
    for (const auto& student : studentsList) {
        if (isFirstline) {
            isFirstline = false;
        } else {
            fout << "\n";
        }

        fout << student.id << "," 
             << student.name << "," 
             << student.studentClass << "," 
             << (student.isPriority ? "true" : "false") << ","
             << student.phone << "," 
             << student.email;
    }

    fout.flush();
    fout.close();
}

/*
─────────────────────────────────────────────────────────────────────────────
Rooms I/O
─────────────────────────────────────────────────────────────────────────────
*/
void loadRooms() {
    std::ifstream fin{std::string(ROOMS_FILE)};

    std::string line;
    while (std::getline(fin, line)) {
        base::Vector<std::string> record = parseRecord(line);

        Room room;

        room.id    = record[0];
        room.type  = std::stoi(record[1]);
        room.price = std::stod(record[2]);

        std::stringstream ss(record[3]);
        std::string       token;
        while (std::getline(ss, token, '|')) {
            room.students.push_back(token);
        }

        roomsList.push_back(room);
    }

    fin.close();
}

void saveRooms() {
    std::ofstream fout{std::string(ROOMS_FILE)};

    bool isFirstline = true;
    for (const auto& room : roomsList) {
        if (isFirstline) {
            isFirstline = false;
        } else {
            fout << "\n";
        }

        fout << room.id << ","
             << room.type << ","
             << room.price << ",";
        for (size_t i = 0; i < room.students.size(); ++i) {
            fout << room.students[i];
            if (i < room.students.size() - 1) {
                fout << "|";
            }
        }
    }

    fout.flush();
    fout.close();
}

/*
─────────────────────────────────────────────────────────────────────────────
Contracts I/O
─────────────────────────────────────────────────────────────────────────────
*/
void loadContracts() {
    std::ifstream fin{std::string(CONTRACTS_FILE)};

    std::string line;
    while (std::getline(fin, line)) {
        base::Vector<std::string> record = parseRecord(line);

        Contract contract;

        contract.id        = record[0];
        contract.studentId = record[1];
        contract.roomId    = record[2];
        contract.startDate = record[3];
        contract.endDate   = record[4];
        contract.isActive  = (record[5] == "true");

        contractsList.push_back(contract);
    }

    fin.close();
}

void saveContracts() {
    std::ofstream fout{std::string(CONTRACTS_FILE)};

    bool isFirstline = true;
    for (const auto& contract : contractsList) {
        if (isFirstline) {
            isFirstline = false;
        } else {
            fout << "\n";
        }

        fout << contract.id << ","
             << contract.studentId << ","
             << contract.roomId << ","
             << contract.startDate << ","
             << contract.endDate << ","
             << (contract.isActive ? "true" : "false");
    }

    fout.flush();
    fout.close();
}

/*
─────────────────────────────────────────────────────────────────────────────
Bills I/O
─────────────────────────────────────────────────────────────────────────────
*/
void loadServiceInvoices() {
    std::ifstream fin{std::string(BILLS_FILE)};

    std::string line;
    while (std::getline(fin, line)) {
        base::Vector<std::string> record = parseRecord(line);

        ServiceInvoice invoice;

        invoice.id                  = record[0];
        invoice.roomID              = record[1];
        invoice.month               = std::stoi(record[2]);
        invoice.year                = std::stoi(record[3]);
        invoice.oldElectricityIndex = std::stod(record[4]);
        invoice.newElectricityIndex = std::stod(record[5]);
        invoice.oldWaterIndex       = std::stod(record[6]);
        invoice.newWaterIndex       = std::stod(record[7]);
        invoice.totalAmount         = std::stod(record[8]);
        invoice.isPaid              = (record[9] == "true");

        serviceInvoicesList.push_back(invoice);
    }

    fin.close();
}

void saveServiceInvoices() {
    std::ofstream fout{std::string(BILLS_FILE)};

    bool isFirstline = true;
    for (const auto& invoice : serviceInvoicesList) {
        if (isFirstline) {
            isFirstline = false;
        } else {
            fout << "\n";
        }

        fout << invoice.id << "," 
             << invoice.roomID << ","
             << invoice.month << ","
             << invoice.year << "," 
             << invoice.oldElectricityIndex << ","
             << invoice.newElectricityIndex << "," 
             << invoice.oldWaterIndex << ","
             << invoice.newWaterIndex << "," 
             << invoice.totalAmount << ","
             << (invoice.isPaid ? "true" : "false");
    }

    fout.flush();
    fout.close();
}
