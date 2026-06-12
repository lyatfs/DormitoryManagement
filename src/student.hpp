#pragma once

#include "config.hpp"
#include "libs/algorithms.hpp"
#include "libs/vector.hpp"
#include <optional>

/*
────────────────────────────────────────────────────────────────────────────────
Find student
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  find student by studentId
 * @param  studentId: id of student to find
 * @return index of student in studentsList if found, std::nullopt otherwise
 */
std::optional<size_t> findStudent(const std::string& studentId) {
    auto it = binarySearch(studentsList, studentId,
                           [](const Student& student, const std::string& id) {
                               return student.id < id;
                           });

    if (it != studentsList.end() && it->id == studentId) {
        return it - studentsList.begin();
    }
    return std::nullopt;
}

/*
────────────────────────────────────────────────────────────────────────────────
Student management
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  add student to the studentsList
 * @param  newStudent: student to add
 */
void addStudent(const Student& newStudent) {
    auto it = binarySearch(studentsList, newStudent.id,
                           [](const Student& student, const std::string& id) {
                               return student.id < id;
                           });

    if (it != studentsList.end() && it->id == newStudent.id) {
        return;
    }

    studentsList.insert(it - studentsList.begin(), newStudent);
}

/**
 * @brief  remove student from the studentsList
 * @param  studentId: id of student to remove
 */
void removeStudent(const std::string& studentId) {
    auto studentIndex = findStudent(studentId);

    if (!studentIndex.has_value()) {
        return;
    }

    studentsList.erase_at(studentIndex.value());
}

/**
 * @brief  update student information
 * @param  studentId    : id of student to update
 * @param  newClass     : new class of student
 * @param  newIsPriority: new priority status of student
 * @param  newPhone     : new phone number of student
 * @param  newEmail     : new email of student
 */
void updateStudent(const std::string& studentId, std::string newClass,
                   bool newIsPriority, std::string newPhone,
                   std::string newEmail) {
    auto studentIndex = findStudent(studentId);

    if (!studentIndex.has_value()) {
        return;
    }

    Student& student     = studentsList[studentIndex.value()];
    student.studentClass = newClass;
    student.isPriority   = newIsPriority;
    student.phone        = newPhone;
    student.email        = newEmail;
}
