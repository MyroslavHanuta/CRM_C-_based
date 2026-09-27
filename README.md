# Falcon Company Management System

A terminal-based business management application written in C++. It manages packages, employees, clients, invoices, payments and appointments. Data is stored in SQLite, the interface is an interactive terminal UI built with FTXUI, and the app can export PDF invoices and receipts.

## Features

- **Role-based access control:** three roles, each with its own menu and permissions:
  - **Admin:** full access, including packages, payments and the control panel
  - **Owner:** employees, invoices, clients and appointments
  - **Staff:** invoices, clients and appointments
- **Login with hashed credentials:** usernames and passwords are stored as salted hashes, not plaintext
- **CRUD operations** (insert, delete, select) for all six entities
- **SQL injection protection:** every query uses prepared statements with bound parameters
- **Input validation:** regex checks for email, phone number (E.164-style) and dates (`YYYY-MM-DD`, validated as real calendar dates)
- **Referential integrity:** SQLite foreign key enforcement is enabled
- **PDF generation:** A4 invoices and receipts built with libharu
- **Scrollable tables** navigated with the arrow keys

## Tech Stack

| Component    | Library                                              |
|--------------|------------------------------------------------------|
| Language     | C++20 (`std::format`, `std::chrono::parse`)          |
| Database     | [SQLite3](https://www.sqlite.org/)                   |
| Terminal UI  | [FTXUI](https://github.com/ArthurSonzogni/FTXUI)     |
| PDF export   | [libharu](https://github.com/libharu/libharu)        |

## Project Structure

```
main.cpp               UI, input validation, PDF generation, login flow
database_control.cpp   SQLite CRUD layer (prepared statements)
hashing.cpp            Credential hashing and login check
```

## Build

Requires a C++20 compiler plus the SQLite3, FTXUI and libharu development libraries.

```bash
g++ -std=c++20 main.cpp database_control.cpp hashing.cpp -o falcon \
    -lftxui-component -lftxui-dom -lftxui-screen -lsqlite3 -lhpdf
./falcon
```

At runtime the app expects:
- `company_database.sqlite`: the database file
- `password.txt`: one user per line, in the format `<username_hash> <password_hash> <role>` (1 = Admin, 2 = Owner, 3 = Staff)

## Security Notes & Known Limitations

This is a learning project. These are the known weaknesses and the planned fixes:

- **DJB2 is not a password hash.** It is a fast, non-cryptographic hash, which makes stored hashes easy to brute-force. *Planned:* replace it with Argon2id or bcrypt.
- **Single hard-coded salt.** One salt shared by all users and compiled into the binary means identical passwords produce identical hashes. *Planned:* a random per-user salt (handled automatically by Argon2/bcrypt).
- **Credentials in a flat file.** `password.txt` should not be committed to version control. *Planned:* move the credentials into the database.
- **Role checks happen only in the UI layer.** *Planned:* enforce permissions in the database layer too.

## Author

Myroslav Hanuta, BSc Cyber Security student, University of Roehampton
