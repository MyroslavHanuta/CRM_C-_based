/**
 * @file database_control.cpp
 * @brief SQLite CRUD implementation for all company entities.
 *
 * Each public method follows the same three-step pattern:
 *   1. Prepare the pre-compiled SQL statement.
 *   2. Bind typed parameters (prevents SQL injection).
 *   3. Execute, finalise, and return a success indicator or result vector.
 *
 * Passing id = 0 to any getAllXxx() method returns every row in that table
 * because the SELECT statements use the clause "WHERE id = ? OR ? = 0".
 */

#include "database_control.h"

#include <iostream>

using namespace std;

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

DatabaseControl::DatabaseControl(const char* dbPath) {
    // sqlite3_open returns SQLITE_OK (0) on success
    int rc = sqlite3_open(dbPath, &db);

    if (rc != SQLITE_OK) {
        cerr << "[DB] ERROR: Cannot open database '" << dbPath
             << "': " << sqlite3_errmsg(db) << "\n";
        db = nullptr; // mark connection as invalid
    } else {
        cout << "[DB] Opened: " << dbPath << "\n";
        // Enable foreign key enforcement — SQLite disables it by default
        sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    }
}

DatabaseControl::~DatabaseControl() {
    if (db) {
        sqlite3_close(db);
        cout << "[DB] Connection closed.\n";
    }
}

// ---------------------------------------------------------------------------
// Package
// ---------------------------------------------------------------------------

bool DatabaseControl::insertPackage(const char* name, int price, const char* contain) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_INSERT_PACKAGE, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    // Bind: 1=name, 2=price, 3=contain
    sqlite3_bind_text(stmt, 1, name,    -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 2, price);
    sqlite3_bind_text(stmt, 3, contain, -1, SQLITE_TRANSIENT);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

bool DatabaseControl::removePackage(int id) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_REMOVE_PACKAGE, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

vector<Package> DatabaseControl::getAllPackages(int id) const {
    vector<Package> result;
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_SELECT_PACKAGE, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    // Bind id twice: once for "WHERE Package_ID = ?", once for "OR ? = 0"
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Package p;
        p.id      = sqlite3_column_int (stmt, 0);
        p.name    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.price   = sqlite3_column_int (stmt, 2);
        p.contain = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        result.push_back(p);
    }

    sqlite3_finalize(stmt);
    return result;
}

// ---------------------------------------------------------------------------
// Employee
// ---------------------------------------------------------------------------

bool DatabaseControl::insertEmployee(const char* name, const char* email,
                                     const char* phone, const char* address,
                                     const char* role) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_INSERT_EMPLOYEE, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    // Bind: 1=name, 2=email, 3=phone, 4=address, 5=role
    sqlite3_bind_text(stmt, 1, name,    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, email,   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, phone,   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, address, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, role,    -1, SQLITE_TRANSIENT);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

bool DatabaseControl::removeEmployee(int id) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_REMOVE_EMPLOYEE, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

vector<Employee> DatabaseControl::getAllEmployees(int id) const {
    vector<Employee> result;
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_SELECT_EMPLOYEE, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Employee e;
        e.id      = sqlite3_column_int (stmt, 0);
        e.name    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        e.email   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        e.phone   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        e.address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        e.role    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        result.push_back(e);
    }

    sqlite3_finalize(stmt);
    return result;
}

// ---------------------------------------------------------------------------
// Invoice
// ---------------------------------------------------------------------------

bool DatabaseControl::insertInvoice(const char* date, int id) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_INSERT_INVOICE, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    // Bind: 1=date, 2=Package_ID
    sqlite3_bind_text(stmt, 1, date, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 2, id);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

bool DatabaseControl::removeInvoice(int id) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_REMOVE_INVOICE, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

vector<Invoice> DatabaseControl::getAllInvoices(int id) const {
    vector<Invoice> result;
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_SELECT_INVOICE, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Invoice i;
        i.I_id = sqlite3_column_int (stmt, 0);
        i.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        i.P_id = sqlite3_column_int (stmt, 2);
        result.push_back(i);
    }

    sqlite3_finalize(stmt);
    return result;
}

// ---------------------------------------------------------------------------
// Client
// ---------------------------------------------------------------------------

bool DatabaseControl::insertClient(const char* name, const char* address, int id) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_INSERT_CLIENT, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    // Bind: 1=name, 2=address, 3=Invoice_ID
    sqlite3_bind_text(stmt, 1, name,    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, address, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 3, id);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

bool DatabaseControl::removeClient(int id) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_REMOVE_CLIENT, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

vector<Client> DatabaseControl::getAllClients(int id) const {
    vector<Client> result;
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_SELECT_CLIENT, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Client c;
        c.C_id    = sqlite3_column_int (stmt, 0);
        c.id      = c.C_id; // keep Identifiable::id in sync
        c.name    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        c.address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        c.I_id    = sqlite3_column_int (stmt, 3);
        result.push_back(c);
    }

    sqlite3_finalize(stmt);
    return result;
}

// ---------------------------------------------------------------------------
// Payment
// ---------------------------------------------------------------------------

bool DatabaseControl::insertPayment(const char* date, int amount, int id) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_INSERT_PAYMENT, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    // Bind: 1=date, 2=amount, 3=Client_ID
    sqlite3_bind_text(stmt, 1, date,   -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 2, amount);
    sqlite3_bind_int (stmt, 3, id);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

bool DatabaseControl::removePayment(int id) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_REMOVE_PAYMENT, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

vector<Payment> DatabaseControl::getAllPayments(int id) const {
    vector<Payment> result;
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_SELECT_PAYMENT, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Payment p;
        p.P_id   = sqlite3_column_int (stmt, 0);
        p.date   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.amount = sqlite3_column_int (stmt, 2);
        p.C_id   = sqlite3_column_int (stmt, 3);
        result.push_back(p);
    }

    sqlite3_finalize(stmt);
    return result;
}

bool DatabaseControl::insertPaymentByInvoice(const char* date, int invoice_id) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_INSERT_PAYMENT_BY_INVOICE, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    // Bind: 1=date, 2=Invoice_ID
    // The SQL SELECT derives the amount and Client_ID from the joined tables
    sqlite3_bind_text(stmt, 1, date,       -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 2, invoice_id);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

// ---------------------------------------------------------------------------
// Appointment
// ---------------------------------------------------------------------------

bool DatabaseControl::insertAppointment(const char* date, int C_id, int E_id, int P_id) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_INSERT_APPOINTMENT, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    // Bind: 1=date, 2=Client_ID, 3=Employee_ID, 4=Payment_ID
    sqlite3_bind_text(stmt, 1, date, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 2, C_id);
    sqlite3_bind_int (stmt, 3, E_id);
    sqlite3_bind_int (stmt, 4, P_id);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

bool DatabaseControl::removeAppointment(int id) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_REMOVE_APPOINTMENT, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

vector<Appointment> DatabaseControl::getAllAppointments(int id) const {
    vector<Appointment> result;
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_SELECT_APPOINTMENT, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Appointment a;
        a.A_id = sqlite3_column_int (stmt, 0);
        a.id   = a.A_id; // keep Identifiable::id in sync
        a.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        a.C_id = sqlite3_column_int (stmt, 2);
        a.E_id = sqlite3_column_int (stmt, 3);
        a.P_id = sqlite3_column_int (stmt, 4);
        result.push_back(a);
    }

    sqlite3_finalize(stmt);
    return result;
}

// ---------------------------------------------------------------------------
// PDF data retrieval
// ---------------------------------------------------------------------------

InvoicePDF DatabaseControl::getInvoicePDF(int invoice_id) const {
    InvoicePDF result; // I_id defaults to 0; caller checks this for failure
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_SELECT_INVOICE_PDF, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_int(stmt, 1, invoice_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result.I_id           = sqlite3_column_int (stmt, 0);
        result.date           = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        result.client_name    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        result.client_address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        result.package_name   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        result.amount         = sqlite3_column_int (stmt, 5);
    }

    sqlite3_finalize(stmt);
    return result;
}

ReceiptPDF DatabaseControl::getReceiptPDF(int payment_id) const {
    ReceiptPDF result; // P_id defaults to 0; caller checks this for failure
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, SQL_SELECT_RECEIPT_PDF, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_int(stmt, 1, payment_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result.P_id             = sqlite3_column_int (stmt, 0);
        result.payment_date     = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        result.amount           = sqlite3_column_int (stmt, 2);
        result.client_name      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        result.client_address   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        result.appointment_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        result.employee_name    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        result.package_name     = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
    }

    sqlite3_finalize(stmt);
    return result;
}