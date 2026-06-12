#pragma once

#include "libs/vector.hpp"
#include <string>
#include <string_view>

/*
────────────────────────────────────────────────────────────────────────────────
Data objects
────────────────────────────────────────────────────────────────────────────────
*/
struct Student {
    std::string id;
    std::string name;
    std::string studentClass;
    bool        isPriority;
    std::string phone;
    std::string email;
};

struct Room {
    std::string               id;
    int                       type;
    int                       price;
    base::Vector<std::string> students;

    bool hasAvailableSlot() const {
        return static_cast<int>(students.size()) < type;
    }

    size_t currentStudents() const { return students.size(); }
};

struct Contract {
    std::string id;
    std::string studentId;
    std::string roomId;
    std::string startDate;
    std::string endDate;
    bool        isActive;
};

struct ServiceInvoice {
    std::string id;
    std::string roomID;
    int         month;
    int         year;
    double      oldElectricityIndex;
    double      newElectricityIndex;
    double      oldWaterIndex;
    double      newWaterIndex;
    double      totalAmount;
    bool        isPaid;
};

/*
────────────────────────────────────────────────────────────────────────────────
Data lists
────────────────────────────────────────────────────────────────────────────────
*/
base::Vector<Student>        studentsList;
base::Vector<Room>           roomsList;
base::Vector<Contract>       contractsList;
base::Vector<ServiceInvoice> serviceInvoicesList;

/*
────────────────────────────────────────────────────────────────────────────────
Data files
────────────────────────────────────────────────────────────────────────────────
*/
constexpr std::string_view STUDENTS_FILES = "../data/Students.txt";
constexpr std::string_view ROOMS_FILE     = "../data/Rooms.txt";
constexpr std::string_view CONTRACTS_FILE = "../data/Contracts.txt";
constexpr std::string_view BILLS_FILE     = "../data/ServiceInvoices.txt";

/*
────────────────────────────────────────────────────────────────────────────────
Service rates
────────────────────────────────────────────────────────────────────────────────
*/
constexpr size_t ELECTRICITY_TIER[4] = {50, 100, 150, 200};
constexpr size_t ELECTRICITY_RATE[4] = {2500, 3000, 3600, 4000};
constexpr size_t WATER_RATE          = 10000;