#pragma once

#include "config.hpp"
#include "libs/algorithms.hpp"
#include "libs/vector.hpp"
#include "room.hpp"
#include <optional>
#include <string>

/*
────────────────────────────────────────────────────────────────────────────────
Generate contract ID
────────────────────────────────────────────────────────────────────────────────
*/
std::string generateContractId() {
    size_t maxIdx = 0;
    if (contractsList.empty()) {
        maxIdx = 1;
    } else {
        auto lastContract = contractsList.back();
        maxIdx            = std::stoull(lastContract.id.substr(2));
    }

    int    digitNumber = 0;
    size_t nextIdx     = maxIdx + 1;
    while (nextIdx > 0) {
        nextIdx /= 10;
        digitNumber++;
    }
    std::string newId = "CT";
    for (int i = 0; i < 8 - digitNumber; i++) {
        newId += "0";
    }
    newId += std::to_string(maxIdx + 1);

    return newId;
}

/*
────────────────────────────────────────────────────────────────────────────────
Find contract
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  find contract by contractId
 * @param  contractId: id of contract to find
 * @return index of contract in contractsList if found, std::nullopt otherwise
 */
std::optional<size_t> findContract(const std::string& contractId) {
    auto it = binarySearch(contractsList, contractId,
                           [](const Contract& contract, const std::string& id) {
                               return contract.id < id;
                           });

    if (it != contractsList.end() && it->id == contractId) {
        return it - contractsList.begin();
    }
    return std::nullopt;
}

/**
 * @brief  find active contract of a student
 * @param  studentId: id of student to find
 * @return index of active contract in contractsList if found, std::nullopt
 * otherwise
 */
std::optional<size_t>
findActiveContractOfStudent(const std::string& studentId) {
    for (auto& contract : contractsList) {
        if (contract.studentId == studentId && contract.isActive == true) {
            return &contract - contractsList.begin();
        }
    }
    return std::nullopt;
}

/*
────────────────────────────────────────────────────────────────────────────────
Contract management
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  add new contract to contractsList
 * @param  studentId    : id of student associated with the contract
 * @param  roomId       : id of room associated with the contract
 * @param  startDate    : start date of the contract
 * @param  endDate      : end date of the contract
 */
void addContract(const std::string& studentId, const std::string& roomId,
                 const std::string& startDate, const std::string& endDate) {
    if (findActiveContractOfStudent(studentId).has_value()) {
        return;
    }
    if (!findRoom(roomId).has_value()) {
        return;
    }

    Contract newContract;

    newContract.id        = generateContractId();
    newContract.studentId = studentId;
    newContract.roomId    = roomId;
    newContract.startDate = startDate;
    newContract.endDate   = endDate;
    newContract.isActive  = true;

    contractsList.push_back(newContract);
}

/**
 * @brief  remove contract from contractsList
 * @param  contractId: id of contract to remove
 */
void removeContract(const std::string& contractId) {
    auto contractIndex = findContract(contractId);

    if (!contractIndex.has_value()) {
        return;
    }
    if (contractsList[contractIndex.value()].isActive == true) {
        return;
    }

    contractsList.erase_at(contractIndex.value());
}

/**
 * @brief  extend end date of a contract
 * @param  contractId: id of contract to extend
 * @param  newEndDate: new end date of the contract
 */
void extendContract(const std::string& contractId,
                    const std::string& newEndDate) {
    auto contractIndex = findContract(contractId);

    if (!contractIndex.has_value()) {
        return;
    }
    if (contractsList[contractIndex.value()].isActive == false) {
        return;
    }

    contractsList[contractIndex.value()].endDate = newEndDate;
}

/**
 * @brief  terminate a contract
 * @param  contractId: id of contract to terminate
 */
void terminateContract(const std::string& contractId) {
    auto contractIndex = findContract(contractId);

    if (!contractIndex.has_value()) {
        return;
    }
    if (contractsList[contractIndex.value()].isActive == false) {
        return;
    }

    contractsList[contractIndex.value()].isActive = false;
}

/*
────────────────────────────────────────────────────────────────────────────────
Room assignment management
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  assign student to a room
 * @param  studentId    : id of student to assign
 * @param  roomId       : id of room to assign student to
 * @param  startDate    : start date of the assignment
 * @param  endDate      : end date of the assignment
 */
void registerRoom(const std::string& studentId, const std::string& roomId,
                  const std::string& startDate, const std::string& endDate) {
    if (findActiveContractOfStudent(studentId).has_value()) {
        return;
    }

    auto roomIndex = findRoom(roomId);
    if (!roomIndex.has_value()) {
        return;
    }
    if (roomsList[roomIndex.value()].hasAvailableSlot() == false) {
        return;
    }

    addStudentToRoom(roomId, studentId);
    addContract(studentId, roomId, startDate, endDate);
}

/**
 * @brief  transfer student to a different room
 * @param  studentId    : id of student to transfer
 * @param  newRoomId    : id of new room to transfer student to
 * @param  newStartDate : start date of the new contract
 * @param  newEndDate   : end date of the new contract
 */
void transferRoom(const std::string& studentId, const std::string& newRoomId,
                  const std::string& newStartDate,
                  const std::string& newEndDate) {

    auto oldContractIndex = findActiveContractOfStudent(studentId);
    if (!oldContractIndex.has_value()) {
        return;
    }
    if (contractsList[oldContractIndex.value()].isActive == false) {
        return;
    }

    auto newRoomIndex = findRoom(newRoomId);
    if (!newRoomIndex.has_value()) {
        return;
    }
    if (roomsList[newRoomIndex.value()].hasAvailableSlot() == false) {
        return;
    }

    std::string oldRoomId     = contractsList[oldContractIndex.value()].roomId;
    std::string oldContractId = contractsList[oldContractIndex.value()].id;

    removeStudentFromRoom(oldRoomId, studentId);
    terminateContract(oldContractId);
    registerRoom(studentId, newRoomId, newStartDate, newEndDate);
}

/**
 * @brief  checkout student from a room
 * @param  studentId: id of student to checkout
 */
void checkoutRoom(const std::string& studentId) {
    auto contractIndex = findActiveContractOfStudent(studentId);

    if (!contractIndex.has_value()) {
        return;
    }
    if (contractsList[contractIndex.value()].isActive == false) {
        return;
    }

    std::string contractId = contractsList[contractIndex.value()].id;
    std::string roomId     = contractsList[contractIndex.value()].roomId;

    removeStudentFromRoom(roomId, studentId);
    terminateContract(contractId);
}
