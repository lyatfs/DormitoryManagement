#pragma once

#include "config.hpp"
#include "libs/algorithms.hpp"
#include "libs/vector.hpp"
#include <new>
#include <optional>

/*
────────────────────────────────────────────────────────────────────────────────
Find room
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  find room by roomId
 * @param  roomId: id of room to find
 * @return index of room in roomsList if found, std::nullopt otherwise
 */
std::optional<size_t> findRoom(const std::string& roomId) {
    auto it = binarySearch(
        roomsList, roomId,
        [](const Room& room, const std::string& id) { return room.id < id; });

    if (it != roomsList.end() && it->id == roomId) {
        return it - roomsList.begin();
    }
    return std::nullopt;
}

/**
 * @brief  find student in a specific room
 * @param  room       : the room to search in
 * @param  studentId  : id of student to find
 * @return index of student in room.students if found, std::nullopt otherwise
 */
std::optional<size_t> findStudentInRoom(const Room&        room,
                                        const std::string& studentId) {
    auto it =
        binarySearch(room.students, studentId,
                     [](const std::string& id, const std::string& studentId) {
                         return id < studentId;
                     });

    if (it != room.students.end() && *it == studentId) {
        return it - room.students.begin();
    }
    return std::nullopt;
}

/*
────────────────────────────────────────────────────────────────────────────────
Room management
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  add room to the roomsList
 * @param  roomId   : id of the room to add
 * @param  roomType : type of the room to add
 * @param  roomPrice: price of the room to add
 */
void addRoom(const std::string& roomId, int roomType, int roomPrice) {
    auto roomIndex = findRoom(roomId);
    if (roomIndex.has_value()) {
        return;
    }
    if (roomType != 4 && roomType != 6 && roomType != 8) {
        return;
    }

    Room newRoom;
    newRoom.id    = roomId;
    newRoom.type  = roomType;
    newRoom.price = roomPrice;

    roomsList.insert(roomIndex.value(), newRoom);
}

/**
 * @brief  remove room from the roomsList
 * @param  roomId: id of the room to remove
 */
void removeRoom(const std::string& roomId) {
    auto roomIndex = findRoom(roomId);

    if (!roomIndex.has_value()) {
        return;
    }
    if (roomsList[roomIndex.value()].students.empty() == false) {
        return;
    }

    roomsList.erase_at(roomIndex.value());
}

/**
 * @brief  update room information
 * @param  roomId  : id of the room to update
 * @param  newType : new type of the room
 * @param  newPrice: new price of the room
 */
void updateRoom(const std::string& roomId, int newType, int newPrice) {
    auto roomIndex = findRoom(roomId);

    if (!roomIndex.has_value()) {
        return;
    }
    if (newType != 4 && newType != 6 && newType != 8) {
        return;
    }
    if (roomsList[roomIndex.value()].currentStudents() > newType) {
        return;
    }

    roomsList[roomIndex.value()].type  = newType;
    roomsList[roomIndex.value()].price = newPrice;
}

/**
 * @brief  add student to a specific room
 * @param  roomId   : id of the room to add student to
 * @param  studentId: id of the student to add
 */
void addStudentToRoom(const std::string& roomId, const std::string& studentId) {
    auto roomIndex = findRoom(roomId);

    if (!roomIndex.has_value()) {
        return;
    }
    if (roomsList[*roomIndex].hasAvailableSlot() == false) {
        return;
    }

    auto studentIndex = findStudentInRoom(roomsList[*roomIndex], studentId);
    if (studentIndex.has_value()) {
        return;
    }

    roomsList[*roomIndex].students.insert(*studentIndex, studentId);
}

/**
 * @brief  remove student from a specific room
 * @param  roomId   : id of the room to remove student from
 * @param  studentId: id of the student to remove
 */
void removeStudentFromRoom(const std::string& roomId,
                           const std::string& studentId) {
    auto roomIndex = findRoom(roomId);
    if (!roomIndex.has_value()) {
        return;
    }

    auto studentIndex =
        findStudentInRoom(roomsList[roomIndex.value()], studentId);
    if (!studentIndex.has_value()) {
        return;
    }

    roomsList[roomIndex.value()].students.erase_at(studentIndex.value());
}
