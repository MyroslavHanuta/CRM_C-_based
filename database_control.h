#ifndef ASSIGNMENT_DATABASE_CONTROL_H
#define ASSIGNMENT_DATABASE_CONTROL_H

#include "sqlite3.h"

#include <string>
#include <vector>

/* =========================================================================
 * BASE CLASSES
 * ========================================================================= */

/**
 * Base for any entity that has a human-readable name and address.
 *
 * Declares a pure-virtual getDisplayInfo() to force each derived type to
 * provide its own formatted string representation (polymorphism).
 */
class ContactEntity {
public:
    std::string name;
    std::string address;

    virtual ~ContactEntity() = default;

    /**
     * Returns a human-readable description of this entity.
     * Must be implemented by every concrete derived class.
     */
    virtual std::string getDisplayInfo() const = 0;
};

/**
 * Base for any entity that carries a numeric primary key.
 */
class Identifiable {
public:
    int id = 0;
    virtual ~Identifiable() = default;
};

/**
 * Base for financial documents (Invoice, Receipt) that share a
 * date and a monetary amount.
 *
 * getSummary() is virtual so derived types can extend the description.
 */
class FinancialDocument {
public:
    std::string date;
    int amount = 0;

    virtual ~FinancialDocument() = default;

    /**
     * Returns a formatted summary of this financial document.
     * Derived classes should call FinancialDocument::getSummary() and
     * prepend their own identifier.
     */
    virtual std::string getSummary() const {
        return "Amount: " + std::to_string(amount) + " GBP | Date: " + date;
    }
};

/* =========================================================================
 * PDF DATA-TRANSFER OBJECTS
 * Flat structs used to carry joined query results for PDF generation.
 * They inherit FinancialDocument so they gain date/amount and getSummary().
 * ========================================================================= */

/**
 * All data required to render an Invoice PDF.
 *
 * Populated by DatabaseControl::getInvoicePDF() via a JOIN across
 * Invoice, Client, and Package tables.
 */
struct InvoicePDF : public FinancialDocument {
    int         I_id           = 0;  ///< Invoice primary key
    std::string client_name;
    std::string client_address;
    std::string package_name;

    /// Overrides FinancialDocument::getSummary() to prepend invoice info.
    std::string getSummary() const override {
        return "Invoice #" + std::to_string(I_id)
             + " | " + package_name
             + " | " + FinancialDocument::getSummary();
    }
};

/**
 * All data required to render a Receipt PDF.
 *
 * Populated by DatabaseControl::getReceiptPDF() via JOINs across
 * Payment, Client, Invoice, Package, Appointment, and Employee.
 */
struct ReceiptPDF : public FinancialDocument {
    int         P_id             = 0;  ///< Payment primary key
    std::string payment_date;
    std::string client_name;
    std::string client_address;
    std::string appointment_date; ///< 'N/A' when no appointment exists
    std::string employee_name;    ///< 'N/A' when no appointment exists
    std::string package_name;

    /// Overrides FinancialDocument::getSummary() to prepend receipt info.
    std::string getSummary() const override {
        return "Receipt #" + std::to_string(P_id)
             + " | " + package_name
             + " | Paid: " + std::to_string(amount) + " GBP";
    }
};

/* =========================================================================
 * ENTITY STRUCTS
 * Each struct maps directly to a database table row.
 * ========================================================================= */

/**
 * Represents a service package offered by the company.
 * Maps to the Package table (Package_ID, Package_name, Package_price, Package_contain).
 */
struct Package : public Identifiable {
    std::string name;
    int         price   = 0;
    std::string contain; ///< Description of what is included

    /// Returns a concise display string for this package.
    std::string getDisplayInfo() const {
        return "Package[" + std::to_string(id) + "]: "
             + name + " - £" + std::to_string(price);
    }
};

/**
 * Middle base class combining ContactEntity and Identifiable.
 *
 * Both Employee and Client share name, address, and an integer ID,
 * so Person centralises that logic and satisfies ContactEntity's
 * pure virtual by providing a default implementation.
 */
class Person : public ContactEntity, public Identifiable {
public:
    std::string getDisplayInfo() const override {
        return "Person[" + std::to_string(id) + "]: " + name;
    }
};

/**
 * Represents a company employee.
 * Maps to the Employee table.
 */
struct Employee : public Person {
    std::string email;
    std::string phone;
    std::string role;

    /// Overrides Person::getDisplayInfo() to include the employee's role.
    std::string getDisplayInfo() const override {
        return "Employee[" + std::to_string(id) + "]: "
             + name + " (" + role + ")";
    }
};

/**
 * Represents a customer/client.
 * Maps to the Client table (Client_ID, Client_name, Client_address, Invoice_ID).
 */
struct Client : public Person {
    int C_id = 0;  ///< Client primary key (mirrors Identifiable::id for compatibility)
    int I_id = 0;  ///< Foreign key to Invoice

    /// Overrides Person::getDisplayInfo() to include the linked invoice.
    std::string getDisplayInfo() const override {
        return "Client[" + std::to_string(C_id) + "]: "
             + name + " | Invoice: " + std::to_string(I_id);
    }
};

/**
 * Represents a billing invoice.
 * Maps to the Invoice table (Invoice_ID, Invoice_date, Package_ID).
 */
struct Invoice : public FinancialDocument {
    int I_id = 0;  ///< Invoice primary key
    int P_id = 0;  ///< Foreign key to Package

    std::string getSummary() const override {
        return "Invoice[" + std::to_string(I_id)
             + "] | Package: " + std::to_string(P_id)
             + " | Date: " + date;
    }
};

/**
 * Represents a payment made by a client.
 * Maps to the Payment table (Payment_ID, Payment_date, Payment_amount, Client_ID).
 */
struct Payment : public FinancialDocument {
    int P_id = 0;  ///< Payment primary key
    int C_id = 0;  ///< Foreign key to Client

    std::string getSummary() const override {
        return "Payment[" + std::to_string(P_id)
             + "] | Amount: " + std::to_string(amount)
             + " GBP | Date: " + date;
    }
};

/**
 * Represents a scheduled appointment.
 * Maps to the Appointment table
 * (Appointment_ID, Appointment_date, Client_ID, Employee_ID, Payment_ID).
 */
struct Appointment : public Identifiable {
    int         A_id = 0;  ///< Appointment primary key (mirrors Identifiable::id)
    std::string date;
    int         C_id = 0;  ///< Foreign key to Client
    int         E_id = 0;  ///< Foreign key to Employee
    int         P_id = 0;  ///< Foreign key to Payment

    std::string getDisplayInfo() const {
        return "Appointment[" + std::to_string(A_id)
             + "] | Client: "   + std::to_string(C_id)
             + " | Employee: "  + std::to_string(E_id)
             + " | Date: "      + date;
    }
};

/* =========================================================================
 * DATABASE CONTROL
 * ========================================================================= */

/**
 * RAII wrapper around an SQLite3 database connection.
 *
 * Provides typed insert / remove / select methods for every entity table.
 * All SQL strings are stored as compile-time constants to avoid typos and
 * to keep the implementation methods short and readable.
 *
 * Usage:
 *   DatabaseControl db("company_database.sqlite");
 *   db.insertPackage("Gold Plan", 500, "All features");
 *   auto packages = db.getAllPackages(0); // 0 = fetch all
 */
class DatabaseControl {
private:
    sqlite3* db = nullptr; ///< Raw SQLite connection handle

    // -----------------------------------------------------------------------
    // SQL string constants — one set per table
    // -----------------------------------------------------------------------

    // Package table
    static constexpr const char* SQL_INSERT_PACKAGE =
        "INSERT INTO Package (Package_name, Package_price, Package_contain) VALUES (?, ?, ?);";
    static constexpr const char* SQL_REMOVE_PACKAGE =
        "DELETE FROM Package WHERE Package_ID = ?;";
    /// Passing id=0 returns all rows (the OR ? = 0 clause).
    static constexpr const char* SQL_SELECT_PACKAGE =
        "SELECT Package_ID, Package_name, Package_price, Package_contain "
        "FROM Package WHERE Package_ID = ? OR ? = 0;";

    // Employee table
    static constexpr const char* SQL_INSERT_EMPLOYEE =
        "INSERT INTO Employee "
        "(Employee_name, Employee_email, Employee_phoneNum, Employee_address, Employee_role) "
        "VALUES (?, ?, ?, ?, ?);";
    static constexpr const char* SQL_REMOVE_EMPLOYEE =
        "DELETE FROM Employee WHERE Employee_ID = ?;";
    static constexpr const char* SQL_SELECT_EMPLOYEE =
        "SELECT Employee_ID, Employee_name, Employee_email, "
        "Employee_phoneNum, Employee_address, Employee_role "
        "FROM Employee WHERE Employee_ID = ? OR ? = 0;";

    // Invoice table
    static constexpr const char* SQL_INSERT_INVOICE =
        "INSERT INTO Invoice (Invoice_date, Package_ID) VALUES (?, ?);";
    static constexpr const char* SQL_REMOVE_INVOICE =
        "DELETE FROM Invoice WHERE Invoice_ID = ?;";
    static constexpr const char* SQL_SELECT_INVOICE =
        "SELECT Invoice_ID, Invoice_date, Package_ID "
        "FROM Invoice WHERE Invoice_ID = ? OR ? = 0;";

    // Client table
    static constexpr const char* SQL_INSERT_CLIENT =
        "INSERT INTO Client (Client_name, Client_address, Invoice_ID) VALUES (?, ?, ?);";
    static constexpr const char* SQL_REMOVE_CLIENT =
        "DELETE FROM Client WHERE Client_ID = ?;";
    static constexpr const char* SQL_SELECT_CLIENT =
        "SELECT Client_ID, Client_name, Client_address, Invoice_ID "
        "FROM Client WHERE Client_ID = ? OR ? = 0;";

    // Payment table
    static constexpr const char* SQL_INSERT_PAYMENT =
        "INSERT INTO Payment (Payment_date, Payment_amount, Client_ID) VALUES (?, ?, ?);";
    static constexpr const char* SQL_REMOVE_PAYMENT =
        "DELETE FROM Payment WHERE Payment_ID = ?;";
    static constexpr const char* SQL_SELECT_PAYMENT =
        "SELECT Payment_ID, Payment_date, Payment_amount, Client_ID "
        "FROM Payment WHERE Payment_ID = ? OR ? = 0;";

    // Appointment table
    static constexpr const char* SQL_INSERT_APPOINTMENT =
        "INSERT INTO Appointment (Appointment_date, Client_ID, Employee_ID, Payment_ID) VALUES (?, ?, ?, ?);";
    static constexpr const char* SQL_REMOVE_APPOINTMENT =
        "DELETE FROM Appointment WHERE Appointment_ID = ?;";
    static constexpr const char* SQL_SELECT_APPOINTMENT =
        "SELECT Appointment_ID, Appointment_date, Client_ID, Employee_ID, Payment_ID "
        "FROM Appointment WHERE Appointment_ID = ? OR ? = 0;";

    // PDF generation queries
    static constexpr const char* SQL_SELECT_INVOICE_PDF =
        "SELECT i.Invoice_ID, i.Invoice_date, "
        "c.Client_name, c.Client_address, "
        "p.Package_name, p.Package_price "
        "FROM Invoice i "
        "JOIN Client c ON c.Invoice_ID = i.Invoice_ID "
        "JOIN Package p ON p.Package_ID = i.Package_ID "
        "WHERE i.Invoice_ID = ?;";

    static constexpr const char* SQL_SELECT_RECEIPT_PDF =
        "SELECT pay.Payment_ID, pay.Payment_date, pay.Payment_amount, "
        "c.Client_name, c.Client_address, "
        "COALESCE(a.Appointment_date, 'N/A'), "
        "COALESCE(e.Employee_name, 'N/A'), "
        "COALESCE(p2.Package_name, 'N/A') "
        "FROM Payment pay "
        "JOIN Client c ON c.Client_ID = pay.Client_ID "
        "JOIN Invoice i ON i.Invoice_ID = c.Invoice_ID "
        "JOIN Package p2 ON p2.Package_ID = i.Package_ID "
        "LEFT JOIN Appointment a ON a.Client_ID = c.Client_ID "
        "LEFT JOIN Employee e ON e.Employee_ID = a.Employee_ID "
        "WHERE pay.Payment_ID = ? "
        "ORDER BY a.Appointment_ID DESC "
        "LIMIT 1;";

    /// Derives the payment amount directly from the package price — no manual entry needed.
    static constexpr const char* SQL_INSERT_PAYMENT_BY_INVOICE =
        "INSERT INTO Payment (Payment_date, Payment_amount, Client_ID) "
        "SELECT ?, p.Package_price, c.Client_ID "
        "FROM Client c "
        "JOIN Invoice i ON i.Invoice_ID = c.Invoice_ID "
        "JOIN Package p ON p.Package_ID = i.Package_ID "
        "WHERE i.Invoice_ID = ?;";

public:
    /**
     * Opens the SQLite database at the given path.
     * dbPath  Filesystem path to the .sqlite file.
     *                Must remain valid for the lifetime of this object.
     */
    explicit DatabaseControl(const char* dbPath);

    /**
     * Closes the SQLite connection.
     */
    ~DatabaseControl();

    // -----------------------------------------------------------------------
    // Package CRUD
    // -----------------------------------------------------------------------

    /** Inserts a new Package row.
     *  true on success, false on SQL error or duplicate name. */
    bool insertPackage(const char* name, int price, const char* contain);

    /** ""Deletes the Package with the given primary key.
     *  true if a row was deleted. */
    bool removePackage(int id);

    /** ""Fetches Package rows.
     *   id  Specific Package_ID to retrieve, or 0 to return all.
     *     Vector of matching Package objects. */
    std::vector<Package> getAllPackages(int id) const;

    // -----------------------------------------------------------------------
    // Employee CRUD
    // -----------------------------------------------------------------------

    /** ""Inserts a new Employee row. */
    bool insertEmployee(const char* name, const char* email,
                        const char* phone, const char* address,
                        const char* role);

    /** ""Deletes the Employee with the given primary key. */
    bool removeEmployee(int id);

    /** ""Fetches Employee rows (0 = all). */
    std::vector<Employee> getAllEmployees(int id) const;

    // -----------------------------------------------------------------------
    // Invoice CRUD
    // -----------------------------------------------------------------------

    /** ""Inserts a new Invoice row linked to a Package.
     *   date  Date string in YYYY-MM-DD format.
     *   id    Package_ID foreign key. */
    bool insertInvoice(const char* date, int id);

    /** ""Deletes the Invoice with the given primary key. */
    bool removeInvoice(int id);

    /** ""Fetches Invoice rows (0 = all). */
    std::vector<Invoice> getAllInvoices(int id) const;

    // -----------------------------------------------------------------------
    // Client CRUD
    // -----------------------------------------------------------------------

    /** ""Inserts a new Client row linked to an Invoice.
     *   id  Invoice_ID foreign key. */
    bool insertClient(const char* name, const char* address, int id);

    /** ""Deletes the Client with the given primary key. */
    bool removeClient(int id);

    /** ""Fetches Client rows (0 = all). */
    std::vector<Client> getAllClients(int id) const;

    // -----------------------------------------------------------------------
    // Payment CRUD
    // -----------------------------------------------------------------------

    /** ""Inserts a Payment row with an explicit amount.
     *   id  Client_ID foreign key. */
    bool insertPayment(const char* date, int amount, int id);

    /** ""Deletes the Payment with the given primary key. */
    bool removePayment(int id);

    /** ""Fetches Payment rows (0 = all). */
    std::vector<Payment> getAllPayments(int id) const;

    /**
     * ""Creates a Payment automatically from an Invoice.
     *
     * The payment amount is derived from the linked Package price,
     * avoiding the need for manual amount entry.
     *
     *  date        Payment date (YYYY-MM-DD).
     *  invoice_id  The Invoice to pay for.
     * true on success.
     */
    bool insertPaymentByInvoice(const char* date, int invoice_id);

    // -----------------------------------------------------------------------
    // Appointment CRUD
    // -----------------------------------------------------------------------

    /** ""Inserts a new Appointment row.
     *   C_id  Client_ID foreign key.
     *   E_id  Employee_ID foreign key.
     *   P_id  Payment_ID foreign key. */
    bool insertAppointment(const char* date, int C_id, int E_id, int P_id);

    /** ""Deletes the Appointment with the given primary key. */
    bool removeAppointment(int id);

    /** ""Fetches Appointment rows (0 = all). */
    std::vector<Appointment> getAllAppointments(int id) const;

    // -----------------------------------------------------------------------
    // PDF data retrieval
    // -----------------------------------------------------------------------

    /**
     * ""Returns all data needed to render an Invoice PDF.
     *
     * Executes a multi-table JOIN. If no matching invoice is found,
     * the returned InvoicePDF has I_id == 0.
     *
     *  invoice_id  The Invoice_ID to look up.
     */
    InvoicePDF getInvoicePDF(int invoice_id) const;

    /**
     * ""Returns all data needed to render a Receipt PDF.
     *
     * Executes a multi-table JOIN including the most recent Appointment
     * for the client (via ORDER BY + LIMIT 1).  If no matching payment
     * is found, the returned ReceiptPDF has P_id == 0.
     *
     *  payment_id  The Payment_ID to look up.
     */
    ReceiptPDF getReceiptPDF(int payment_id) const;
};

#endif // ASSIGNMENT_DATABASE_CONTROL_H