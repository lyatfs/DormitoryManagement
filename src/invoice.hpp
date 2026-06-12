#pragma once

#include "config.hpp"
#include "libs/algorithms.hpp"
#include "libs/vector.hpp"
#include <optional>
#include <string>

/*
────────────────────────────────────────────────────────────────────────────────
Generate id
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  generate a new unique ID for invoice
 * @return new invoice ID
 */
std::string generateInvoiceId() {
    size_t maxIdx = 0;
    if (serviceInvoicesList.empty()) {
        maxIdx = 1;
    } else {
        auto lastInvoice = serviceInvoicesList.back();
        maxIdx           = std::stoull(lastInvoice.id.substr(2));
    }

    int    digitNumber = 0;
    size_t nextIdx     = maxIdx + 1;
    while (nextIdx > 0) {
        nextIdx /= 10;
        digitNumber++;
    }
    std::string newId = "SI";
    for (int i = 0; i < 8 - digitNumber; i++) {
        newId += "0";
    }
    newId += std::to_string(maxIdx + 1);

    return newId;
}

/*
────────────────────────────────────────────────────────────────────────────────
Calculate service fee
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  calculate electricity fee
 * @param  oldIndex: old electricity index
 * @param  newIndex: new electricity index
 * @return electricity fee
 */
double electricityFee(double oldIndex, double newIndex) {
    double used = newIndex - oldIndex;
    if (used <= 0)
        return 0;

    double total = 0;
    if (used <= ELECTRICITY_TIER[0]) {
        total = used * ELECTRICITY_RATE[0];
    } else if (used <= ELECTRICITY_TIER[1]) {
        total = ELECTRICITY_TIER[0] * ELECTRICITY_RATE[0] +
                (used - ELECTRICITY_TIER[0]) * ELECTRICITY_RATE[1];
    } else if (used <= ELECTRICITY_TIER[2]) {
        total =
            ELECTRICITY_TIER[0] * ELECTRICITY_RATE[0] +
            (ELECTRICITY_TIER[1] - ELECTRICITY_TIER[0]) * ELECTRICITY_RATE[1] +
            (used - ELECTRICITY_TIER[1]) * ELECTRICITY_RATE[2];
    } else {
        total =
            ELECTRICITY_TIER[0] * ELECTRICITY_RATE[0] +
            (ELECTRICITY_TIER[1] - ELECTRICITY_TIER[0]) * ELECTRICITY_RATE[1] +
            (ELECTRICITY_TIER[2] - ELECTRICITY_TIER[1]) * ELECTRICITY_RATE[2] +
            (used - ELECTRICITY_TIER[2]) * ELECTRICITY_RATE[3];
    }
    return total;
}

/**
 * @brief  calculate water fee
 * @param  oldIndex: old water index
 * @param  newIndex: new water index
 * @return water fee
 */
double waterFee(double oldIndex, double newIndex) {
    if (newIndex < oldIndex)
        return 0;
    return (newIndex - oldIndex) * WATER_RATE;
}

/*
────────────────────────────────────────────────────────────────────────────────
Find service invoice
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  find invoice by invoiceId
 * @param  invoiceId: id of invoice to find
 * @return index of invoice in serviceInvoicesList if found, std::nullopt
 * otherwise
 */
std::optional<size_t> findInvoice(const std::string& invoiceId) {
    auto it = binarySearch(serviceInvoicesList, invoiceId,
                           [](const ServiceInvoice& invoice, const std::string& id) {
                               return invoice.id < id;
                           });

    if (it != serviceInvoicesList.end() && it->id == invoiceId) {
        return it - serviceInvoicesList.begin();
    }
    return std::nullopt;
}

/**
 * @brief  find invoice of room on (month, year)
 * @param  roomId   : ID of the room
 * @param  month    : month of the invoice
 * @param  year     : year of the invoice
 * @return index of invoice in serviceInvoicesList if found, std::nullopt
 * otherwise
 */
std::optional<size_t> findInvoice(const std::string& roomId, int& month,
                                  int& year) {
    for (auto& invoice : serviceInvoicesList) {
        if (invoice.roomID == roomId && invoice.month == month &&
            invoice.year == year) {
            return &invoice - serviceInvoicesList.begin();
        }
    }
    return std::nullopt;
}

std::optional<size_t> findLastInvoice(const std::string& roomId) {
    for (auto it = serviceInvoicesList.end() - 1;
         it != serviceInvoicesList.begin(); it--) {
        if (it->roomID == roomId) {
            return it - serviceInvoicesList.begin();
        }
    }
    return std::nullopt;
}

/*
────────────────────────────────────────────────────────────────────────────────
Service invoice management
────────────────────────────────────────────────────────────────────────────────
 */

/**
 * @brief  create new invoice and add to serviceInvoicesList
 * @param  roomId              : ID of the room
 * @param  month               : month of the invoice
 * @param  year                : year of the invoice
 * @param  newElectricityIndex : new electricity index
 * @param  newWaterIndex       : new water index
 */
void createInvoice(const std::string& roomId, int month, int year,
                   double newElectricityIndex, double newWaterIndex) {
    if (findInvoice(roomId, month, year).has_value()) {
        return;
    }

    ServiceInvoice newInvoice;

    newInvoice.id     = generateInvoiceId();
    newInvoice.roomID = roomId;
    newInvoice.month  = month;
    newInvoice.year   = year;

    auto lastInvoiceIndex = findLastInvoice(roomId);
    if (lastInvoiceIndex.has_value()) {
        auto& lastInvoice = serviceInvoicesList[lastInvoiceIndex.value()];

        newInvoice.oldElectricityIndex = lastInvoice.newElectricityIndex;
        newInvoice.oldWaterIndex       = lastInvoice.newWaterIndex;
    } else {
        newInvoice.oldElectricityIndex = 0;
        newInvoice.oldWaterIndex       = 0;
    }
    newInvoice.newElectricityIndex = newElectricityIndex;
    newInvoice.newWaterIndex       = newWaterIndex;
    newInvoice.totalAmount =
        electricityFee(newInvoice.oldElectricityIndex, newElectricityIndex) +
        waterFee(newInvoice.oldWaterIndex, newWaterIndex);

    newInvoice.isPaid = false;

    serviceInvoicesList.push_back(newInvoice);
}

/**
 * @brief  update payment status of an invoice
 * @param  invoiceId : id of the invoice to update
 * @param  status    : new payment status
 */
void updatePaymentStatus(const std::string& invoiceId, const bool& status) {
    auto invoiceIndex = findInvoice(invoiceId);
    if (!invoiceIndex.has_value()) {
        return;
    }
    serviceInvoicesList[invoiceIndex.value()].isPaid = status;
}