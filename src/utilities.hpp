#pragma once

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

struct Date {
    int day   = 1;
    int month = 1;
    int year  = 1956;
    // Chuyen date sang mot gia tri de so sanh
    int comparableValue() const { return year * 10000 + month * 100 + day; }
    // Toan tu so sanh
    bool operator==(const Date& other) const {
        return comparableValue() == other.comparableValue();
    }
    bool operator!=(const Date& other) const { return !(*this == other); }
    bool operator<(const Date& other) const {
        return comparableValue() < other.comparableValue();
    }
    bool operator<=(const Date& other) const { return !(other < *this); }
    bool operator>(const Date& other) const { return other < *this; }
    bool operator>=(const Date& other) const { return !(*this < other); }
};
// Lay ngay hom nay theo local time tren may tinh
Date todayDate() {
    std::time_t now = std::time(nullptr);
    std::tm     localTime{};

#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
// chuyen thanh gio local tren may
    return Date{localTime.tm_mday, localTime.tm_mon + 1,
                localTime.tm_year + 1900};
}

int dateToComparableValue(const Date& date) { return date.comparableValue(); }
// Chuyen chuoi ngay thang sang kieu Date
Date parseDate(const std::string& dateText) {
    char separator = (dateText.find('/') != std::string::npos) ? '/' : '-';

    std::stringstream ss(dateText);
    std::string       first;
    std::string       second;
    std::string       third;

    std::getline(ss, first, separator);
    std::getline(ss, second, separator);
    std::getline(ss, third, separator);

    if (first.size() == 4) {
        return Date{std::stoi(third), std::stoi(second), std::stoi(first)};
    }
    return Date{std::stoi(first), std::stoi(second), std::stoi(third)};
}

std::string formatDate(const Date& date) {
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(4) << date.year << "-"
       << std::setw(2) << date.month << "-" << std::setw(2) << date.day;
    return ss.str();
}
// Kiem tra ngay co chua het han hay khong
bool isDateNotExpired(const Date& date) { return todayDate() <= date; }

bool isDateNotExpired(const std::string& dateText) {
    return isDateNotExpired(parseDate(dateText));
}
