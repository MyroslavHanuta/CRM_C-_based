#include <string> // create a string
#include <vector> // create a vector
#include "hashing.h" // uploading hashing for passwords
#include "sqlite3.h" // library to use sql
#include "database_control.h" // custom library for database control
#include <ftxui/dom/elements.hpp> // building blocks for rendering UI
#include <ftxui/component/screen_interactive.hpp> // interactive screen
#include <ftxui/component/component.hpp> // for buttons and checkboxes
#include <ftxui/dom/table.hpp> // for tables
#include <regex>
#include <fstream>
#include <chrono>
#include <format>
#include <hpdf.h>


// std::cout << "\033[2J\033[H"; clear terminal

using namespace std;
using namespace ftxui;

/* --------------------------------------------------------------------- */

void GenerateInvoice(int invoice_id, const std::string& date,
                     const std::string& client_name, const std::string& client_address,
                     const std::string& package_name, int amount) {

    HPDF_Doc pdf = HPDF_New(NULL, NULL);
    HPDF_Page page = HPDF_AddPage(pdf);

    // set page size to A4
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

    HPDF_Font font_bold   = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
    HPDF_Font font_normal = HPDF_GetFont(pdf, "Helvetica", NULL);

    /* ----- COMPANY HEADER ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 24);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 780, "Falcon Company");
    HPDF_Page_EndText(page);

    HPDF_Page_SetFontAndSize(page, font_normal, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 760, "123 Business Street, City");
    HPDF_Page_EndText(page);

    /* ----- DIVIDER LINE ----- */
    HPDF_Page_SetLineWidth(page, 1);
    HPDF_Page_MoveTo(page, 50, 745);
    HPDF_Page_LineTo(page, 545, 745);
    HPDF_Page_Stroke(page);

    /* ----- INVOICE TITLE ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 18);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 720, ("INVOICE #" + std::to_string(invoice_id)).c_str());
    HPDF_Page_EndText(page);

    /* ----- DATE ----- */
    HPDF_Page_SetFontAndSize(page, font_normal, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 400, 720, ("Date: " + date).c_str());
    HPDF_Page_EndText(page);

    /* ----- CLIENT INFO ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 690, "Bill To:");
    HPDF_Page_EndText(page);

    HPDF_Page_SetFontAndSize(page, font_normal, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 673, client_name.c_str());
    HPDF_Page_EndText(page);

    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 656, client_address.c_str());
    HPDF_Page_EndText(page);

    /* ----- TABLE HEADER ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50,  620, "Package");
    HPDF_Page_TextOut(page, 400, 620, "Amount");
    HPDF_Page_EndText(page);

    // table header line
    HPDF_Page_MoveTo(page, 50, 615);
    HPDF_Page_LineTo(page, 545, 615);
    HPDF_Page_Stroke(page);

    /* ----- TABLE ROW ----- */
    HPDF_Page_SetFontAndSize(page, font_normal, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50,  598, package_name.c_str());
    HPDF_Page_TextOut(page, 400, 598, (std::to_string(amount) + " GBP").c_str());
    HPDF_Page_EndText(page);

    // table bottom line
    HPDF_Page_MoveTo(page, 50, 590);
    HPDF_Page_LineTo(page, 545, 590);
    HPDF_Page_Stroke(page);

    /* ----- TOTAL ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 14);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 350, 565, "TOTAL:");
    HPDF_Page_TextOut(page, 430, 565, (std::to_string(amount) + " GBP").c_str());
    HPDF_Page_EndText(page);

    /* ----- FOOTER ----- */
    HPDF_Page_SetFontAndSize(page, font_normal, 10);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 50, "Thank you for your business!");
    HPDF_Page_EndText(page);

    /* ----- SAVE ----- */
    std::string filename = "invoice_" + std::to_string(invoice_id) + ".pdf";
    HPDF_SaveToFile(pdf, filename.c_str());
    HPDF_Free(pdf);
}

void GenerateReceipt(const ReceiptPDF& r) {
    HPDF_Doc pdf = HPDF_New(NULL, NULL);
    HPDF_Page page = HPDF_AddPage(pdf);
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

    HPDF_Font font_bold   = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
    HPDF_Font font_normal = HPDF_GetFont(pdf, "Helvetica", NULL);

    /* ----- HEADER ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 24);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 780, "Falcon Company");
    HPDF_Page_EndText(page);

    HPDF_Page_SetFontAndSize(page, font_normal, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 760, "123 Business Street, City");
    HPDF_Page_EndText(page);

    /* ----- DIVIDER ----- */
    HPDF_Page_SetLineWidth(page, 1);
    HPDF_Page_MoveTo(page, 50, 745);
    HPDF_Page_LineTo(page, 545, 745);
    HPDF_Page_Stroke(page);

    /* ----- RECEIPT TITLE ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 18);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 720, ("RECEIPT #" + std::to_string(r.P_id)).c_str());
    HPDF_Page_EndText(page);

    HPDF_Page_SetFontAndSize(page, font_normal, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 400, 720, ("Date: " + r.payment_date).c_str());
    HPDF_Page_EndText(page);

    /* ----- CLIENT INFO ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 690, "Received From:");
    HPDF_Page_EndText(page);

    HPDF_Page_SetFontAndSize(page, font_normal, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 673, r.client_name.c_str());
    HPDF_Page_EndText(page);

    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 656, r.client_address.c_str());
    HPDF_Page_EndText(page);

    /* ----- APPOINTMENT DETAILS ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 625, "Appointment Details:");
    HPDF_Page_EndText(page);

    HPDF_Page_SetFontAndSize(page, font_normal, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 608, ("Date: " + r.appointment_date).c_str());
    HPDF_Page_EndText(page);

    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 591, ("Employee: " + r.employee_name).c_str());
    HPDF_Page_EndText(page);

    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 574, ("Package: " + r.package_name).c_str());
    HPDF_Page_EndText(page);

    /* ----- TABLE HEADER ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50,  540, "Description");
    HPDF_Page_TextOut(page, 400, 540, "Amount Paid");
    HPDF_Page_EndText(page);

    HPDF_Page_MoveTo(page, 50, 535);
    HPDF_Page_LineTo(page, 545, 535);
    HPDF_Page_Stroke(page);

    /* ----- TABLE ROW ----- */
    HPDF_Page_SetFontAndSize(page, font_normal, 12);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50,  518, ("Payment for " + r.package_name).c_str());
    HPDF_Page_TextOut(page, 400, 518, (std::to_string(r.amount) + " GBP").c_str());
    HPDF_Page_EndText(page);

    HPDF_Page_MoveTo(page, 50, 510);
    HPDF_Page_LineTo(page, 545, 510);
    HPDF_Page_Stroke(page);

    /* ----- TOTAL ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 14);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 350, 485, "TOTAL PAID:");
    HPDF_Page_TextOut(page, 450, 485, (std::to_string(r.amount) + " GBP").c_str());
    HPDF_Page_EndText(page);

    /* ----- PAID STAMP ----- */
    HPDF_Page_SetFontAndSize(page, font_bold, 36);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 180, 380, "PAID");
    HPDF_Page_EndText(page);

    /* ----- FOOTER ----- */
    HPDF_Page_SetFontAndSize(page, font_normal, 10);
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, 50, "Thank you for your payment!");
    HPDF_Page_EndText(page);

    /* ----- SAVE ----- */
    std::string filename = "receipt_" + std::to_string(r.P_id) + ".pdf";
    HPDF_SaveToFile(pdf, filename.c_str());
    HPDF_Free(pdf);
}

bool isValidDate (const string& date) {
    regex date_pattern(R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$)");
    if (!regex_match(date, date_pattern)) return false;

    istringstream ss(date);
    chrono::year_month_day ymd;
    ss >> chrono::parse("%Y-%m-%d", ymd);
    return !ss.fail() && ymd.ok();
};

/* Style for tables */

void ApplyTableStyle(ftxui::Table& table) {
    table.SelectAll().Border(LIGHT);
    table.SelectRow(0).Decorate(bold);
    table.SelectAll().SeparatorHorizontal(LIGHT);
    table.SelectAll().SeparatorVertical(LIGHT);
    table.SelectRow(0).Decorate(color(Color::Green));
}
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* table for Package */

Element RenderPackageTable(const vector<Package>& packages) {
    vector<vector<string>> dataset;

    dataset.push_back({" ID ", " Name ", " Price ", " Contain "});

    for (const auto& p : packages) {
        dataset.push_back({
            to_string(p.id),
            p.name,
            to_string(p.price),
            p.contain
        });
    }

    auto table = Table(dataset);
    ApplyTableStyle(table);
    return table.Render();
}
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* table for Employee */

Element RenderEmployeeTable(const vector<Employee>& employees) {
    vector<vector<string>> dataset;

    dataset.push_back({" ID ", " Name ", " Email ", " PhoneNum ", " Address ", " Role "});

    for (const auto& e : employees) {
        dataset.push_back({
            to_string(e.id),
            e.name,
            e.email,
            e.phone,
            e.address,
            e.role

        });
    }

    auto table = Table(dataset);
    ApplyTableStyle(table);
    return table.Render();
}
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* table for Invoice */

Element RenderInvoiceTable(const vector<Invoice>& invoices) {
    vector<vector<string>> dataset;

    dataset.push_back({" Invoice_ID ", " Date ", " Package_ID "});

    for (const auto& i : invoices) {
        dataset.push_back({
            to_string(i.I_id),
            i.date,
            to_string(i.P_id)
        });
    }

    auto table = Table(dataset);
    ApplyTableStyle(table);
    return table.Render();
}
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* table for Client */

Element RenderClientTable(const vector<Client>& clients) {
    vector<vector<string>> dataset;

    dataset.push_back({" Client_ID ", " Name ", " Address ", " Invoice_ID "});

    for (const auto& c : clients) {
        dataset.push_back({
            to_string(c.C_id),
            c.name,
            c.address,
            to_string(c.I_id)
        });
    }

    auto table = Table(dataset);
    ApplyTableStyle(table);
    return table.Render();
}
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* table for Payment */
Element RenderPaymentTable(const vector<Payment>& payments) {
    vector<vector<string>> dataset;

    dataset.push_back({" Payment_ID ", " Date ", " Amount ", " Client_ID "});

    for (const auto& p : payments) {
        dataset.push_back({
            to_string(p.P_id),
            p.date,
            to_string(p.amount),
            to_string(p.C_id)
        });
    }

    auto table = Table(dataset);
    ApplyTableStyle(table);
    return table.Render();
}
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* table for Appointment */

Element RenderAppointmentTable(const vector<Appointment>& appointments) {
    vector<vector<string>> dataset;

    dataset.push_back({" Appointment_ID ", " Date ", " Client_ID ", " Employee_ID ", " Payment_ID "});

    for (const auto& a : appointments) {
        dataset.push_back({
            to_string(a.A_id),
            a.date,
            to_string(a.C_id),
            to_string(a.E_id),
            to_string(a.P_id)
        });
    }

    auto table = Table(dataset);
    ApplyTableStyle(table);
    return table.Render();
}/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */

int main() {
    // initialize db
    DatabaseControl myDB("company_database.sqlite");
    /* --------------------------------------------------------------------- */

    // create all vectors to store info for tables
    vector<Package> getAllPackages;
    vector<Employee> getAllEmployees;
    vector<Invoice> getAllInvoices;
    vector<Client> getAllClients;
    vector<Payment> getAllPayments;
    vector<Appointment> getAllAppointments;
    /* --------------------------------------------------------------------- */

    // use for interactive screen
    auto screen = ScreenInteractive::TerminalOutput();
    /* --------------------------------------------------------------------- */

    // counters for menu tables
    int menu_selected_staff = 0, menu_selected_admin = 0, menu_selected_owner = 0;
    int db_selected_package = 0, db_selected_invoice = 0, db_selected_client = 0, db_selected_payment = 0, db_selected_appointment = 0;
    /* --------------------------------------------------------------------- */

    // security index
    int security_index = 0;
    /* --------------------------------------------------------------------- */

    // used for scroll table
    int scroll_y_package = 0, scroll_y_employee = 0, scroll_y_invoice = 0, scroll_y_client = 0, scroll_y_payment = 0, scroll_y_appointment = 0;
    const int visible_rows = 10;
    /* --------------------------------------------------------------------- */

    // to regex check
    const regex email_pattern(R"((\w+)(\.{1}\w+)*@(\w+)(\.\w+)+)");
    const regex phone_pattern(R"(^\+\d{10,14}$)");
    /* --------------------------------------------------------------------- */

    // for password check
    string username, password;
    /* --------------------------------------------------------------------- */

    InvoicePDF data_for_Invoice;
    ReceiptPDF data_for_Receipt;

    // menu for staff
    vector<string> menu_entries_staff = {
        " Dashboard ",
        " Invoice ",
        " Client ",
        " Appointment ",
        " Log Out "
    };
    /* --------------------------------------------------------------------- */

    // menu for admin
    vector<string> menu_entries_admin = {
        " Dashboard ",
        " Package",
        " Employee ",
        " Invoice ",
        " Client ",
        " Payment ",
        " Appointment ",
        " Control ",
        " Log Out "
    };
    /* --------------------------------------------------------------------- */

    // menu for owner
    vector<string> menu_entries_owner = {
        " Dashboard ",
        " Employee ",
        " Invoice ",
        " Client ",
        " Appointment ",
        " Log Out "
    };
    /* --------------------------------------------------------------------- */

    // for second menu table control
    vector<string> db_control_entities = {
        " Insert ",
        " Delete ",
        " Select ",
    };
    vector<string> db_control_entities_invoice = {
        " Insert ",
        " Delete ",
        " Select ",
        " Generate Invoice ",
    };
    /* --------------------------------------------------------------------- */

    // used for menu to choose table
    auto menu_opt_staff = MenuOption::Vertical();
    menu_opt_staff.on_enter = [&] {  };
    /* --------------------------------------------------------------------- */

    // used for menu to choose table
    auto menu_opt_admin = MenuOption::Vertical();
    menu_opt_admin.on_enter = [&] {  };
    /* --------------------------------------------------------------------- */

    // used for menu to choose table
    auto menu_opt_owner = MenuOption::Vertical();
    menu_opt_owner.on_enter = [&] {  };
    /* --------------------------------------------------------------------- */

    // used for menu to select table option
    auto db_opt = MenuOption::Vertical();
    db_opt.on_enter = [&] {  };
    /* --------------------------------------------------------------------- */

    // menu
    Component sidebar_staff = Menu(&menu_entries_staff, &menu_selected_staff, menu_opt_staff); // menu for staff console
    Component sidebar_admin = Menu(&menu_entries_admin, &menu_selected_admin, menu_opt_admin); // menu for admin console
    Component sidebar_owner = Menu(&menu_entries_owner, &menu_selected_owner, menu_opt_owner); // menu for owner console
    /* --------------------------------------------------------------------- */



    // menu for table options
    Component db_control_package     = Menu(&db_control_entities, &db_selected_package, db_opt);
    Component db_control_invoice     = Menu(&db_control_entities, &db_selected_invoice, db_opt);
    Component db_control_client      = Menu(&db_control_entities, &db_selected_client, db_opt);
    Component db_control_payment     = Menu(&db_control_entities, &db_selected_payment, db_opt);
    Component db_control_appointment = Menu(&db_control_entities, &db_selected_appointment, db_opt);
    /* --------------------------------------------------------------------- */


    // variables to fill form
    string id_pack, id_e, id_i, id_c, id_pay, id_a, name, address, price, date, email, phonenum, role, contain, db_delete, db_select;
    string get_invoice_index = "";
    string get_receipt_index = "";
    string create_payment_index = "";
    /* --------------------------------------------------------------------- */

    // input field
    Component input_name_pack = Input(&name, "Enter Name"); // for package
    Component input_contain_pack = Input(&contain, "Enter Contain"); // for package
    Component input_price_pack = Input(&price, "Enter Price"); // for package




    Component input_date_pay = Input(&date, "Enter Date"); // for payment
    Component input_price_pay = Input(&price, "Enter Price"); // for payment
    Component input_id_c_pay = Input(&id_c, "Enter Client ID"); // for payment





    Component input_delete_pack = Input(&db_delete, "Enter ID");


    Component input_delete_pay = Input(&db_delete, "Enter ID");


    Component input_select_pack = Input(&db_select, "Enter ID or 0 for all"); // Use to get db index



    Component input_select_pay = Input(&db_select, "Enter ID or 0 for all"); // Use to get db index


    /* --------------------------------------------------------------------- */

    // dashboard render
    auto MakeDashboard = []() {
        return Renderer([] {
            return vbox({
                text("Welcome to Falcon company") | bold | color(Color::Green) | center,
                separator(),
                text("Please use menu at the left"),
            });
        });
    };
    /* --------------------------------------------------------------------- */

    /******************************************************************************************************************************************************/
    /*                                         PACKAGE                                                                                                    */
    /******************************************************************************************************************************************************/

    string msg_insert_package = "", msg_delete_package = "", msg_select_package = ""; // used to output a real time messages

    // button for package insert
    Component btn_package_insert = Button("Submit", [&] {
        if (!any_of(name.begin(), name.end(), ::isdigit) && !name.empty()) {
            if (all_of(price.begin(), price.end(), ::isdigit)) {
                if (!contain.empty()) {
                    if (myDB.insertPackage(name.c_str(), stoi(price), contain.c_str())) {
                        name = "";
                        price = "";
                        contain = "";
                        msg_insert_package = "Success";
                    } else {
                        msg_insert_package = "Item with this name already exist";
                    }
                } else { msg_insert_package = "Error, contain is empty"; }
            } else { msg_insert_package = "Error, price is incorrect"; }
        } else { msg_insert_package = "Error, name is incorrect"; }

    });
    /* --------------------------------------------------------------------- */

    // button for package delete
    Component btn_package_delete = Button("Submit", [&] {
        if (all_of(db_delete.begin(), db_delete.end(), ::isdigit ) && !db_delete.empty()) {
            if (myDB.removePackage(stoi(db_delete))) {
            db_delete = "";
            msg_delete_package = "Success";
            }else {
                msg_delete_package = "Nothing to delete";
            }
        } else { msg_delete_package = "Error, ID is empty or not a number"; }

    });
    /* --------------------------------------------------------------------- */

    // button for package select
    Component btn_package_select = Button("Submit", [&] {
        if (all_of(db_select.begin(), db_select.end(), ::isdigit ) && !db_select.empty()) {
            getAllPackages = myDB.getAllPackages(stoi(db_select));
            if (!getAllPackages.empty()) {
                msg_select_package = "Success";
                db_select = "";
            } else {
                msg_select_package = "No records found";
            }
        } else { msg_select_package = "Error, ID is empty or not a number"; }

    });
    /* --------------------------------------------------------------------- */

    // tab for insert package
    auto tab_insert_package = Renderer(
        Container::Vertical({
            input_name_pack,
            input_price_pack,
            input_contain_pack,
            btn_package_insert
        }),
        [&] {
        return vbox({
            text("Insert Package") | bold | color(Color::Green), separator(),
            text(msg_insert_package) | color(Color::Red), separator(),
            hbox({text("Name: "), input_name_pack ->Render()}),
            hbox({text("Price: "), input_price_pack ->Render()}),
            hbox({text("Contain: "), input_contain_pack ->Render()}), separator(),
            btn_package_insert ->Render(),
        }) ;
    });
    /* --------------------------------------------------------------------- */

    // tab for delete package
    auto tab_delete_package = Renderer(
        Container::Vertical({
            input_delete_pack,
            btn_package_delete
        }),
        [&] {
        return vbox({
            text("Delete Package") | bold | color(Color::Green), separator(),
            text(msg_delete_package) | color(Color::Red), separator(),
            hbox({text("Enter ID: "), input_delete_pack ->Render()}), separator(),
            btn_package_delete ->Render(),
        }) ;
    });
    /* --------------------------------------------------------------------- */

    // used to make table scrollable
    auto table_scrollable_package = Renderer([&] {
        vector<Package> visible_packages;
        int start = min(scroll_y_package, (int)getAllPackages.size());
        int end = min(start + visible_rows, (int)getAllPackages.size());

        for (int i = start; i < end; i++) {
            visible_packages.push_back(getAllPackages[i]);
        }

        return RenderPackageTable(visible_packages);
    });
    /* --------------------------------------------------------------------- */

    // tab  for select package
    auto tab_select_package = Renderer(
        Container::Vertical({
            input_select_pack,
            btn_package_select,
            table_scrollable_package
        }),
        [&] {
        return vbox({
            text("Select Package") | bold | color(Color::Green), separator(),
            text(msg_select_package) | color(Color::Red), separator(),
            hbox({text("Enter ID: "), input_select_pack ->Render()}), separator(),
            btn_package_select ->Render(), separator(),
           table_scrollable_package ->Render() | size(HEIGHT, EQUAL, 25),
        });
    });
    /* --------------------------------------------------------------------- */

    // menu for table option
    auto screen_package_tabs = Container::Tab({tab_insert_package, tab_delete_package, tab_select_package}, &db_selected_package);
    /* --------------------------------------------------------------------- */


    // create a container for screen
    auto screen_package_container = Container::Horizontal({
        db_control_package,
        screen_package_tabs,
    });
    /* --------------------------------------------------------------------- */

    // apply some good visuals
    auto screen_package_render = Renderer(screen_package_container,[&] {
        return hbox({
            db_control_package -> Render() | size(WIDTH, EQUAL, 15) | border ,
            screen_package_tabs -> Render() | flex  | border,
        });
    });
    /* --------------------------------------------------------------------- */

    // in event of keyboard presses do something
    auto screen_package_scroll = CatchEvent(screen_package_render, [&](Event event) {
        if (db_selected_package == 2) {
            if (event == Event::ArrowUp) {
                scroll_y_package = max(0, scroll_y_package -1);
                return true;
            }
            if (event == Event::ArrowDown) {
                scroll_y_package = min((int)getAllPackages.size() - visible_rows, scroll_y_package + 1);
                return true;
            }
        }
        return false;
    });
    /* --------------------------------------------------------------------- */

    /******************************************************************************************************************************************************/
    /******************************************************************************************************************************************************/



    /******************************************************************************************************************************************************/
    /*                                                  EMPLOYEE                                                                                          */
    /******************************************************************************************************************************************************/

    string msg_insert_employee = "", msg_delete_employee = "", msg_select_employee = ""; // used to output a real time messages

    auto MakeEmployeeScreen = [&] () -> Component {
        auto db_selected = make_shared<int>(0);
        auto db_control = Menu(&db_control_entities, db_selected.get(), db_opt);

        Component input_name_emp = Input(&name, "Enter Name"); // for employee
        Component input_email_emp = Input(&email, "Enter Email"); // for employee
        Component input_phonenum_emp = Input(&phonenum, "Enter Phone"); // for employee
        Component input_address_emp = Input(&address, "Enter Address"); // for employee
        Component input_role_emp = Input(&role, "Enter Role"); // for employee

        Component input_delete_emp = Input(&db_delete, "Enter ID");
        Component input_select_emp = Input(&db_select, "Enter ID or 0 for all"); // Use to get db index

        // button for employee insert
        Component btn_employee_insert = Button("Submit", [&] {
            if (!any_of(name.begin(), name.end(), ::isdigit) && !name.empty()) {
                if (regex_match(email, email_pattern)) {
                    if (regex_match(phonenum, phone_pattern)) {
                        if (!address.empty()) {
                            if (!role.empty() && !all_of(role.begin(), role.end(), ::isdigit)) {
                                if (myDB.insertEmployee(name.c_str(), email.c_str(), phonenum.c_str(), address.c_str(), role.c_str())) {
                                    name = "";
                                    email = "";
                                    phonenum = "";
                                    address = "";
                                    role = "";
                                    msg_insert_employee = "Success";
                                } else {msg_insert_employee = "Item with this name already exist";}
                            } else {msg_insert_employee = "Error, Role contain digit or it is empty";}
                        } else {msg_insert_employee = "Error, Address must not be empty";}
                    } else {msg_insert_employee = "Error, Phone number is incorrect";}
                } else { msg_insert_employee = "Error, Email is incorrect";}
            } else {msg_insert_employee = "Error, Name in incorrect or empty";}
        });
        /* --------------------------------------------------------------------- */

        // button for employee delete
        Component btn_employee_delete = Button("Submit", [&] {
            if (all_of(db_delete.begin(), db_delete.end(), ::isdigit ) && !db_delete.empty()) {
                if (myDB.removeEmployee(stoi(db_delete))) {
                db_delete = "";
                msg_delete_employee = "Success";
                }else {msg_delete_employee = "Nothing to delete";}
            } else { msg_delete_employee = "Error, ID is empty or not a number"; }

        });
        /* --------------------------------------------------------------------- */

        // button for employee select
        Component btn_employee_select = Button("Submit", [&] {
            if (all_of(db_select.begin(), db_select.end(), ::isdigit ) && !db_select.empty()) {
                getAllEmployees = myDB.getAllEmployees(stoi(db_select));
                if (!getAllEmployees.empty()) {
                    msg_select_employee = "Success";
                    db_select = "";
                } else {msg_select_employee = "Error";}
            } else { msg_select_employee = "Error, ID is empty or not a number"; }

        });
        /* --------------------------------------------------------------------- */

        // tab for insert employee
        auto tab_insert_employee = Renderer(
            Container::Vertical({
                input_name_emp,
                input_email_emp,
                input_phonenum_emp,
                input_address_emp,
                input_role_emp,
                btn_employee_insert
            }),
            [&, input_name_emp,input_email_emp, input_phonenum_emp, input_address_emp, input_role_emp, btn_employee_insert] {
            return vbox({
                text("Insert Employee") | bold | color(Color::Green), separator(),
                text(msg_insert_employee) | color(Color::Red), separator(),
                hbox({text("Name: "), input_name_emp -> Render()}),
                hbox({text("Email: "), input_email_emp -> Render()}),
                hbox({text("Phone Number: "), input_phonenum_emp -> Render()}),
                hbox({text("Address: "), input_address_emp -> Render()}),
                hbox({text("Role: "), input_role_emp -> Render()}), separator(),
                btn_employee_insert ->Render(),
            }) ;
        });
        /* --------------------------------------------------------------------- */

        // tab for delete package
        auto tab_delete_employee = Renderer(
            Container::Vertical({
                input_delete_emp,
                btn_employee_delete
            }),
            [&, input_delete_emp, btn_employee_delete] {
            return vbox({
                text("Delete Employee") | bold | color(Color::Green), separator(),
                text(msg_delete_employee) | color(Color::Red), separator(),
                hbox({text("Enter ID: "), input_delete_emp ->Render()}), separator(),
                btn_employee_delete ->Render(),
            }) ;
        });
        /* --------------------------------------------------------------------- */

        // used to make table scrollable
        auto table_scrollable_employee = Renderer([&] {
            vector<Employee> visible_employees;
            int start = min(scroll_y_employee, (int)getAllEmployees.size());
            int end = min(start + visible_rows, (int)getAllEmployees.size());

            for (int i = start; i < end; i++) {
                visible_employees.push_back(getAllEmployees[i]);
            }

            return RenderEmployeeTable(visible_employees);
        });
        /* --------------------------------------------------------------------- */

        // tab  for select package
        auto tab_select_employee = Renderer(
            Container::Vertical({
                input_select_emp,
                btn_employee_select,
                table_scrollable_employee
            }),
            [&, input_select_emp, btn_employee_select, table_scrollable_employee] {
            return vbox({
                text("Select Employee") | bold | color(Color::Green), separator(),
                text(msg_select_employee) | color(Color::Red), separator(),
                hbox({text("Enter ID: "), input_select_emp ->Render()}), separator(),
                btn_employee_select ->Render(), separator(),
               table_scrollable_employee ->Render() | size(HEIGHT, EQUAL, 25),
            });
        });
        /* --------------------------------------------------------------------- */

        // menu for table option
        auto screen_employee_tabs = Container::Tab({tab_insert_employee, tab_delete_employee, tab_select_employee}, db_selected.get());
        /* --------------------------------------------------------------------- */


        // create a container for screen
        auto screen_employee_container = Container::Horizontal({
            db_control,
            screen_employee_tabs,
        });
        /* --------------------------------------------------------------------- */

        // apply some good visuals
        auto screen_employee_render = Renderer(screen_employee_container,[&, db_control, screen_employee_tabs] {
            return hbox({
                db_control -> Render() | size(WIDTH, EQUAL, 15) | border ,
                screen_employee_tabs -> Render() | flex  | border,
            });
        });
        /* --------------------------------------------------------------------- */

        // in event of keyboard presses do something
        return CatchEvent(screen_employee_render, [&, db_selected](Event event) {
            if (*db_selected == 2) {
                if (event == Event::ArrowUp) {
                    scroll_y_employee = max(0, scroll_y_employee -1);
                    return true;
                }
                if (event == Event::ArrowDown) {
                    scroll_y_employee = min((int)getAllEmployees.size() - visible_rows, scroll_y_employee + 1);
                    return true;
                }
            }
            return false;
        });
        /* --------------------------------------------------------------------- */

    };

    /******************************************************************************************************************************************************/
    /******************************************************************************************************************************************************/



    /******************************************************************************************************************************************************/
    /*                                                  INVOICE                                                                                           */
    /******************************************************************************************************************************************************/

    string msg_insert_invoice = "", msg_delete_invoice = "", msg_select_invoice = "", msg_get_invoice = ""; // used to output a real time messages

    auto MakeInvoiceScreen = [&] () -> Component {
        auto db_selected = make_shared<int>(0);
        auto db_control = Menu(&db_control_entities_invoice, db_selected.get(), db_opt);

        Component input_date_inv = Input(&date, "Enter Date"); // for invoice
        Component input_id_pack_inv = Input(&id_pack, "Enter Package ID"); // for invoice

        Component input_get_invoice = Input(&get_invoice_index, "Enter Invoice ID");

        Component input_select_inv = Input(&db_select, "Enter ID or 0 for all"); // Use to get db index
        Component input_delete_inv = Input(&db_delete, "Enter ID");

        Component btn_get_invoice = Button("Submit", [&] {
            if (all_of(get_invoice_index.begin(), get_invoice_index.end(), ::isdigit) && !get_invoice_index.empty()) {
                data_for_Invoice = myDB.getInvoicePDF(stoi(get_invoice_index));
                GenerateInvoice(data_for_Invoice.I_id, data_for_Invoice.date, data_for_Invoice.client_name, data_for_Invoice.client_address, data_for_Invoice.package_name, data_for_Invoice.amount);
                msg_get_invoice = "Success";
            } else {msg_get_invoice = "Error, ID is not a digit or empty";}
        });

        // button for invoice insert
        Component btn_invoice_insert = Button("Submit", [&] {
            if (isValidDate(date)) {
                if (all_of(id_pack.begin(), id_pack.end(), ::isdigit) && !id_pack.empty()) {
                    if (myDB.insertInvoice(date.c_str(), stoi(id_pack))) {
                        date = "";
                        id_pack = "";
                        msg_insert_invoice = "Success";
                    } else {msg_insert_invoice = "Error, please enter valid package id";}
                }else {msg_insert_invoice = "Error, ID is not digit or empty";}
            } else {msg_insert_invoice = "Error, Invalid date";}
        });
        /* --------------------------------------------------------------------- */

        // button for invoice delete
        Component btn_invoice_delete = Button("Submit", [&] {
            if (all_of(db_delete.begin(), db_delete.end(), ::isdigit ) && !db_delete.empty()) {
                if (myDB.removeInvoice(stoi(db_delete))) {
                db_delete = "";
                msg_delete_invoice = "Success";
                }else {msg_delete_invoice = "Nothing to delete";}
            } else { msg_delete_invoice = "Error, ID is empty or not a number"; }

        });
        /* --------------------------------------------------------------------- */

        // button for invoice select
        Component btn_invoice_select = Button("Submit", [&] {
            if (all_of(db_select.begin(), db_select.end(), ::isdigit ) && !db_select.empty()) {
                getAllInvoices = myDB.getAllInvoices(stoi(db_select));
                if (!getAllInvoices.empty()) {
                    msg_select_invoice = "Success";
                    db_select = "";
                } else {msg_select_invoice = "Error";}
            } else { msg_select_invoice = "Error, ID is empty or not a number"; }

        });
        /* --------------------------------------------------------------------- */

        // tab for insert invoice
        auto tab_insert_invoice = Renderer(
            Container::Vertical({
                input_date_inv,
                input_id_pack_inv,
                btn_invoice_insert
            }),
            [&, input_date_inv, input_id_pack_inv, btn_invoice_insert] {
            return vbox({
                text("Insert Invoice") | bold | color(Color::Green), separator(),
                text(msg_insert_invoice) | color(Color::Red), separator(),
                hbox({text("Date: "), input_date_inv -> Render()}),
                hbox({text("Package ID: "), input_id_pack_inv -> Render()}), separator(),
                btn_invoice_insert ->Render(),
            }) ;
        });
        /* --------------------------------------------------------------------- */

        // tab for delete package
        auto tab_delete_invoice = Renderer(
            Container::Vertical({
                input_delete_inv,
                btn_invoice_delete
            }),
            [&, input_delete_inv, btn_invoice_delete] {
            return vbox({
                text("Delete Invoice") | bold | color(Color::Green), separator(),
                text(msg_delete_invoice) | color(Color::Red), separator(),
                hbox({text("Enter ID: "), input_delete_inv ->Render()}), separator(),
                btn_invoice_delete ->Render(),
            }) ;
        });
        /* --------------------------------------------------------------------- */

        // used to make table scrollable
        auto table_scrollable_invoice = Renderer([&] {
            vector<Invoice> visible_invoices;
            int start = min(scroll_y_invoice, (int)getAllInvoices.size());
            int end = min(start + visible_rows, (int)getAllInvoices.size());

            for (int i = start; i < end; i++) {
                visible_invoices.push_back(getAllInvoices[i]);
            }

            return RenderInvoiceTable(visible_invoices);
        });
        /* --------------------------------------------------------------------- */

        // tab  for select package
        auto tab_select_invoice = Renderer(
            Container::Vertical({
                input_select_inv,
                btn_invoice_select,
                table_scrollable_invoice
            }),
            [&, input_select_inv, btn_invoice_select, table_scrollable_invoice] {
            return vbox({
                text("Select Invoice") | bold | color(Color::Green), separator(),
                text(msg_select_invoice) | color(Color::Red), separator(),
                hbox({text("Enter ID: "), input_select_inv ->Render()}), separator(),
                btn_invoice_select ->Render(), separator(),
               table_scrollable_invoice ->Render() | size(HEIGHT, EQUAL, 25),
            });
        });
        /* --------------------------------------------------------------------- */

        // tab to generate invoice
        auto tab_generate_invoice = Renderer(
            Container::Vertical({
                input_get_invoice,
                btn_get_invoice,
            }), [&, input_get_invoice, btn_get_invoice] {
                return vbox({
                    text("Generate Invoice") | bold | color(Color::Green), separator(),
                    text(msg_get_invoice) | color(Color::Red), separator(),
                    hbox({text("Enter ID: "), input_get_invoice ->Render()}), separator(),
                    btn_get_invoice ->Render(), separator(),
                });
            });
        /* --------------------------------------------------------------------- */

        // menu for table option
        auto screen_invoice_tabs = Container::Tab({tab_insert_invoice, tab_delete_invoice, tab_select_invoice, tab_generate_invoice}, db_selected.get());
        /* --------------------------------------------------------------------- */


        // create a container for screen
        auto screen_invoice_container = Container::Horizontal({
            db_control,
            screen_invoice_tabs,
        });
        /* --------------------------------------------------------------------- */

        // apply some good visuals
        auto screen_invoice_render = Renderer(screen_invoice_container,[&, db_control, screen_invoice_tabs] {
            return hbox({
                db_control -> Render() | size(WIDTH, EQUAL, 20) | border ,
                screen_invoice_tabs -> Render() | flex  | border,
            });
        });
        /* --------------------------------------------------------------------- */

        // in event of keyboard presses do something
        return CatchEvent(screen_invoice_render, [&, db_selected](Event event) {
            if (*db_selected == 2) {
                if (event == Event::ArrowUp) {
                    scroll_y_invoice = max(0, scroll_y_invoice -1);
                    return true;
                }
                if (event == Event::ArrowDown) {
                    scroll_y_invoice = min((int)getAllInvoices.size() - visible_rows, scroll_y_invoice + 1);
                    return true;
                }
            }
            return false;
        });
        /* --------------------------------------------------------------------- */

    };

    /******************************************************************************************************************************************************/
    /******************************************************************************************************************************************************/

    /******************************************************************************************************************************************************/
    /*                                                  CLIENT                                                                                            */
    /******************************************************************************************************************************************************/

    string msg_insert_client = "", msg_delete_client = "", msg_select_client = ""; // used to output a real time messages

    auto MakeClientScreen = [&] () -> Component {
        auto db_selected = make_shared<int>(0);
        auto db_control = Menu(&db_control_entities, db_selected.get(), db_opt);

        Component input_name_cln = Input(&name, "Enter Name"); // for client
        Component input_address_cln = Input(&address, "Enter Address"); // for client
        Component input_id_i_cln = Input(&id_i, "Enter Invoice ID"); // for client

        Component input_delete_cln = Input(&db_delete, "Enter ID");
        Component input_select_cln = Input(&db_select, "Enter ID or 0 for all"); // Use to get db index

        // button for client insert
        Component btn_client_insert = Button("Submit", [&] {
            if (!any_of(name.begin(), name.end(), ::isdigit) && !name.empty()) {
                if (!address.empty()) {
                    if (all_of(id_i.begin(), id_i.end(), ::isdigit) && !id_i.empty()) {
                        if (myDB.insertClient(name.c_str(), address.c_str(), stoi(id_i))) {
                            name = "";
                            address = "";
                            id_i = "";
                            msg_insert_client = "Success";
                        } else {msg_insert_client = "Error, please enter valid invoice id";}
                    }else {msg_insert_client = "Error, ID in not digit or empty";}
                }else {msg_insert_client = "Error, Address is empty";}
            }else {msg_insert_client = "Error, Name is invalid or empty";}
        });
        /* --------------------------------------------------------------------- */

        // button for client delete
        Component btn_client_delete = Button("Submit", [&] {
            if (all_of(db_delete.begin(), db_delete.end(), ::isdigit ) && !db_delete.empty()) {
                if (myDB.removeClient(stoi(db_delete))) {
                db_delete = "";
                msg_delete_client = "Success";
                }else {msg_delete_client = "Nothing to delete";}
            } else { msg_delete_client = "Error, ID is empty or not a number"; }

        });
        /* --------------------------------------------------------------------- */

        // button for client select
        Component btn_client_select = Button("Submit", [&] {
            if (all_of(db_select.begin(), db_select.end(), ::isdigit ) && !db_select.empty()) {
                getAllClients = myDB.getAllClients(stoi(db_select));
                if (!getAllClients.empty()) {
                    msg_select_client = "Success";
                    db_select = "";
                } else {msg_select_client = "Error";}
            } else { msg_select_client = "Error, ID is empty or not a number"; }

        });
        /* --------------------------------------------------------------------- */

        // tab for insert client
        auto tab_insert_client = Renderer(
            Container::Vertical({
                input_name_cln,
                input_address_cln,
                input_id_i_cln,
                btn_client_insert,
            }),
            [&, input_name_cln,input_address_cln,input_id_i_cln,btn_client_insert] {
            return vbox({
                text("Insert Client") | bold | color(Color::Green), separator(),
                text(msg_insert_client) | color(Color::Red), separator(),
                hbox({text("Name: "), input_name_cln -> Render()}),
                hbox({text("Address: "), input_address_cln -> Render()}),
                hbox({text("Invoice ID: "), input_id_i_cln -> Render()}), separator(),
                btn_client_insert ->Render(),
            }) ;
        });
        /* --------------------------------------------------------------------- */

        // tab for delete package
        auto tab_delete_client = Renderer(
            Container::Vertical({
                input_delete_cln,
                btn_client_delete
            }),
            [&, input_delete_cln,btn_client_delete] {
            return vbox({
                text("Delete Client") | bold | color(Color::Green), separator(),
                text(msg_delete_client) | color(Color::Red), separator(),
                hbox({text("Enter ID: "), input_delete_cln ->Render()}), separator(),
                btn_client_delete ->Render(),
            }) ;
        });
        /* --------------------------------------------------------------------- */

        // used to make table scrollable
        auto table_scrollable_client = Renderer([&] {
            vector<Client> visible_clients;
            int start = min(scroll_y_client, (int)getAllClients.size());
            int end = min(start + visible_rows, (int)getAllClients.size());

            for (int i = start; i < end; i++) {
                visible_clients.push_back(getAllClients[i]);
            }

            return RenderClientTable(visible_clients);
        });
        /* --------------------------------------------------------------------- */

        // tab  for select package
        auto tab_select_client = Renderer(
            Container::Vertical({
                input_select_cln,
                btn_client_select,
                table_scrollable_client
            }),
            [&, input_select_cln,btn_client_select,table_scrollable_client] {
            return vbox({
                text("Select Client") | bold | color(Color::Green), separator(),
                text(msg_select_client) | color(Color::Red), separator(),
                hbox({text("Enter ID: "), input_select_cln ->Render()}), separator(),
                btn_client_select ->Render(), separator(),
               table_scrollable_client ->Render() | size(HEIGHT, EQUAL, 25),
            });
        });
        /* --------------------------------------------------------------------- */

        // menu for table option
        auto screen_client_tabs = Container::Tab({tab_insert_client, tab_delete_client, tab_select_client}, db_selected.get());
        /* --------------------------------------------------------------------- */


        // create a container for screen
        auto screen_client_container = Container::Horizontal({
            db_control,
            screen_client_tabs,
        });
        /* --------------------------------------------------------------------- */

        // apply some good visuals
        auto screen_client_render = Renderer(screen_client_container,[&, db_control, screen_client_tabs] {
            return hbox({
                db_control -> Render() | size(WIDTH, EQUAL, 15) | border ,
                screen_client_tabs -> Render() | flex  | border,
            });
        });
        /* --------------------------------------------------------------------- */

        // in event of keyboard presses do something
        return CatchEvent(screen_client_render, [&, db_selected](Event event) {
            if (*db_selected == 2) {
                if (event == Event::ArrowUp) {
                    scroll_y_client = max(0, scroll_y_client -1);
                    return true;
                }
                if (event == Event::ArrowDown) {
                    scroll_y_client = min((int)getAllClients.size() - visible_rows, scroll_y_client + 1);
                    return true;
                }
            }
            return false;
        });
        /* --------------------------------------------------------------------- */
    };
    /******************************************************************************************************************************************************/
    /******************************************************************************************************************************************************/

    /******************************************************************************************************************************************************/
    /*                                                  PAYMENT                                                                                           */
    /******************************************************************************************************************************************************/

    string msg_insert_payment = "", msg_delete_payment = "", msg_select_payment = ""; // used to output a real time messages

    // button for payment insert
    Component btn_payment_insert = Button("Submit", [&] {
        if (isValidDate(date)) {
            if (all_of(price.begin(), price.end(), ::isdigit) && !price.empty()) {
                if (all_of(id_c.begin(), id_c.end(), ::isdigit) && !id_c.empty() ) {
                    if (myDB.insertPayment(date.c_str(), stoi(price), stoi(id_c))) {
                        date = "";
                        price = "";
                        id_c = "";
                        msg_insert_payment = "Success";
                    } else {msg_insert_payment = "Error, please enter valid client id";}
                } else {msg_insert_payment = "Error, Client ID in invalid or empty";}
            }else {msg_insert_payment = "Error, Amount is invalir or empty";}
        }else {msg_insert_payment = "Error, Date is invalid or empty";}
    });
    /* --------------------------------------------------------------------- */

    // button for payment delete
    Component btn_payment_delete = Button("Submit", [&] {
        if (all_of(db_delete.begin(), db_delete.end(), ::isdigit ) && !db_delete.empty()) {
            if (myDB.removePayment(stoi(db_delete))) {
            db_delete = "";
            msg_delete_payment = "Success";
            }else {msg_delete_payment = "Nothing to delete";}
        } else { msg_delete_payment = "Error, ID is empty or not a number"; }

    });
    /* --------------------------------------------------------------------- */

    // button for payment select
    Component btn_payment_select = Button("Submit", [&] {
        if (all_of(db_select.begin(), db_select.end(), ::isdigit ) && !db_select.empty()) {
            getAllPayments = myDB.getAllPayments(stoi(db_select));
            if (!getAllPayments.empty()) {
                msg_select_payment = "Success";
                db_select = "";
            } else {msg_select_payment = "Error";}
        } else { msg_select_payment = "Error, ID is empty or not a number"; }

    });
    /* --------------------------------------------------------------------- */

    // tab for insert payment
    auto tab_insert_payment = Renderer(
        Container::Vertical({
            input_date_pay,
            input_price_pay,
            input_id_c_pay,
            btn_payment_insert,
        }),
        [&] {
        return vbox({
            text("Insert payment") | bold | color(Color::Green), separator(),
            text(msg_insert_payment) | color(Color::Red), separator(),
            hbox({text("Date: "), input_date_pay -> Render()}),
            hbox({text("Amount: "), input_price_pay -> Render()}),
            hbox({text("Client ID: "), input_id_c_pay -> Render()}), separator(),
            btn_payment_insert ->Render(),
        }) ;
    });
    /* --------------------------------------------------------------------- */

    // tab for delete package
    auto tab_delete_payment = Renderer(
        Container::Vertical({
            input_delete_pay,
            btn_payment_delete
        }),
        [&] {
        return vbox({
            text("Delete payment") | bold | color(Color::Green), separator(),
            text(msg_delete_payment) | color(Color::Red), separator(),
            hbox({text("Enter ID: "), input_delete_pay ->Render()}), separator(),
            btn_payment_delete ->Render(),
        }) ;
    });
    /* --------------------------------------------------------------------- */

    // used to make table scrollable
    auto table_scrollable_payment = Renderer([&] {
        vector<Payment> visible_payments;
        int start = min(scroll_y_payment, (int)getAllPayments.size());
        int end = min(start + visible_rows, (int)getAllPayments.size());

        for (int i = start; i < end; i++) {
            visible_payments.push_back(getAllPayments[i]);
        }

        return RenderPaymentTable(visible_payments);
    });
    /* --------------------------------------------------------------------- */

    // tab  for select package
    auto tab_select_payment = Renderer(
        Container::Vertical({
            input_select_pay,
            btn_payment_select,
            table_scrollable_payment
        }),
        [&] {
        return vbox({
            text("Select payment") | bold | color(Color::Green), separator(),
            text(msg_select_payment) | color(Color::Red), separator(),
            hbox({text("Enter ID: "), input_select_pay ->Render()}), separator(),
            btn_payment_select ->Render(), separator(),
           table_scrollable_payment ->Render() | size(HEIGHT, EQUAL, 25),
        });
    });
    /* --------------------------------------------------------------------- */

    // menu for table option
    auto screen_payment_tabs = Container::Tab({tab_insert_payment, tab_delete_payment, tab_select_payment}, &db_selected_payment);
    /* --------------------------------------------------------------------- */


    // create a container for screen
    auto screen_payment_container = Container::Horizontal({
        db_control_payment,
        screen_payment_tabs,
    });
    /* --------------------------------------------------------------------- */

    // apply some good visuals
    auto screen_payment_render = Renderer(screen_payment_container,[&] {
        return hbox({
            db_control_payment -> Render() | size(WIDTH, EQUAL, 15) | border ,
            screen_payment_tabs -> Render() | flex  | border,
        });
    });
    /* --------------------------------------------------------------------- */

    // in event of keyboard presses do something
    auto screen_payment_scroll = CatchEvent(screen_payment_render, [&](Event event) {
        if (db_selected_payment == 2) {
            if (event == Event::ArrowUp) {
                scroll_y_payment = max(0, scroll_y_payment -1);
                return true;
            }
            if (event == Event::ArrowDown) {
                scroll_y_payment = min((int)getAllPayments.size() - visible_rows, scroll_y_payment + 1);
                return true;
            }
        }
        return false;
    });
    /* --------------------------------------------------------------------- */

    /******************************************************************************************************************************************************/
    /******************************************************************************************************************************************************/



    /******************************************************************************************************************************************************/
    /*                                                  APPOINTMENT                                                                                           */
    /******************************************************************************************************************************************************/

    string msg_insert_appointment = "", msg_delete_appointment = "", msg_select_appointment = ""; // used to output a real time messages

    auto MakeAppointmentScreen = [&]() -> Component {
        auto db_selected = make_shared<int>(0);
        auto db_control = Menu(&db_control_entities, db_selected.get(), db_opt);

        Component input_date_app = Input(&date, "Enter Date"); // for appointment
        Component input_id_c_app = Input(&id_c, "Enter Client ID"); // for appointment
        Component input_id_e_app = Input(&id_e, "Enter Employee ID"); // for appointment
        Component input_id_pay_app = Input(&id_pay, "Enter Payment ID"); // for appointment

        Component input_delete_app = Input(&db_delete, "Enter ID");
        Component input_select_app = Input(&db_select, "Enter ID or 0 for all"); // Use to get db index

        // button for appointment insert
        Component btn_appointment_insert = Button("Submit", [&] {
            if (isValidDate(date)) {
                if (all_of(id_c.begin(), id_c.end(), ::isdigit) && !id_c.empty()) {
                    if (all_of(id_e.begin(), id_e.end(), ::isdigit) && !id_e.empty()) {
                        if (all_of(id_pay.begin(), id_pay.end(), ::isdigit) && !id_pay.empty()) {
                            if (myDB.insertAppointment(date.c_str(), stoi(id_c), stoi(id_e), stoi(id_pay))) {
                                date = "";
                                id_c = "";
                                id_e = "";
                                id_pay = "";
                                msg_insert_appointment = "Success";
                            } else {msg_insert_appointment = "Error, please check all ID to exist in table";}
                        } else {msg_insert_appointment = "Error, Payment ID is not a digit or empty";}
                    } else {msg_insert_appointment = "Error, Employee ID is not a digit or empty";}
                }else {msg_insert_appointment = "Error, Client ID in not a digit or empty";}
            } else {msg_insert_appointment = "Error, date is incorrect or empty";}


        });
        /* --------------------------------------------------------------------- */

        // button for appointment delete
        Component btn_appointment_delete = Button("Submit", [&] {
            if (all_of(db_delete.begin(), db_delete.end(), ::isdigit ) && !db_delete.empty()) {
                if (myDB.removeAppointment(stoi(db_delete))) {
                db_delete = "";
                msg_delete_appointment = "Success";
                }else {msg_delete_appointment = "Nothing to delete";}
            } else { msg_delete_appointment = "Error, ID is empty or not a number"; }

        });
        /* --------------------------------------------------------------------- */

        // button for appointment select
        Component btn_appointment_select = Button("Submit", [&] {
            if (all_of(db_select.begin(), db_select.end(), ::isdigit ) && !db_select.empty()) {
                getAllAppointments = myDB.getAllAppointments(stoi(db_select));
                if (!getAllAppointments.empty()) {
                    msg_select_appointment = "Success";
                    db_select = "";
                } else {msg_select_appointment = "Error";}
            } else { msg_select_appointment = "Error, ID is empty or not a number"; }

        });
        /* --------------------------------------------------------------------- */

        // tab for insert appointment
        auto tab_insert_appointment = Renderer(
            Container::Vertical({
                input_date_app,
                input_id_c_app,
                input_id_e_app,
                input_id_pay_app,
                btn_appointment_insert,
            }),
            [&, input_date_app, input_id_c_app, input_id_e_app, input_id_pay_app, btn_appointment_insert] {
            return vbox({
                text("Insert Appointment") | bold | color(Color::Green), separator(),
                text(msg_insert_appointment) | color(Color::Red), separator(),
                hbox({text("Date: "), input_date_app -> Render()}),
                hbox({text("Client ID: "), input_id_c_app -> Render()}),
                hbox({text("Employee ID: "), input_id_e_app -> Render()}),
                hbox({text("Payment ID: "), input_id_pay_app -> Render()}), separator (),
                btn_appointment_insert ->Render(),
            }) ;
        });
        /* --------------------------------------------------------------------- */

        // tab for delete package
        auto tab_delete_appointment = Renderer(
            Container::Vertical({
                input_delete_app,
                btn_appointment_delete
            }),
            [&, input_delete_app, btn_appointment_delete] {
            return vbox({
                text("Delete Appointment") | bold | color(Color::Green), separator(),
                text(msg_delete_appointment) | color(Color::Red), separator(),
                hbox({text("Enter ID: "), input_delete_app ->Render()}), separator(),
                btn_appointment_delete ->Render(),
            }) ;
        });
        /* --------------------------------------------------------------------- */

        // used to make table scrollable
        auto table_scrollable_appointment = Renderer([&] {
            vector<Appointment> visible_appointments;
            int start = min(scroll_y_appointment, (int)getAllAppointments.size());
            int end = min(start + visible_rows, (int)getAllAppointments.size());

            for (int i = start; i < end; i++) {
                visible_appointments.push_back(getAllAppointments[i]);
            }

            return RenderAppointmentTable(visible_appointments);
        });
        /* --------------------------------------------------------------------- */

        // tab  for select package
        auto tab_select_appointment = Renderer(
            Container::Vertical({
                input_select_app,
                btn_appointment_select,
                table_scrollable_appointment
            }),
            [&, input_select_app, btn_appointment_select, table_scrollable_appointment] {
            return vbox({
                text("Select Appointment") | bold | color(Color::Green), separator(),
                text(msg_select_appointment) | color(Color::Red), separator(),
                hbox({text("Enter ID: "), input_select_app ->Render()}), separator(),
                btn_appointment_select ->Render(), separator(),
               table_scrollable_appointment ->Render() | size(HEIGHT, EQUAL, 25),
            });
        });
        /* --------------------------------------------------------------------- */

        // menu for table option
        auto screen_appointment_tabs = Container::Tab({tab_insert_appointment, tab_delete_appointment, tab_select_appointment}, db_selected.get());
        /* --------------------------------------------------------------------- */


        // create a container for screen
        auto screen_appointment_container = Container::Horizontal({
            db_control,
            screen_appointment_tabs,
        });
        /* --------------------------------------------------------------------- */

        // apply some good visuals
        auto screen_appointment_render = Renderer(screen_appointment_container,[&, db_control, screen_appointment_tabs] {
            return hbox({
                db_control -> Render() | size(WIDTH, EQUAL, 15) | border,
                screen_appointment_tabs -> Render() | flex | border
            });
        });
        /* --------------------------------------------------------------------- */

        // in event of keyboard presses do something
        return CatchEvent(screen_appointment_render, [&, db_selected](Event event) {
            if (*db_selected == 2) {
                if (event == Event::ArrowUp) {
                    scroll_y_appointment = max(0, scroll_y_appointment -1);
                    return true;
                }
                if (event == Event::ArrowDown) {
                    scroll_y_appointment = min((int)getAllAppointments.size() - visible_rows, scroll_y_appointment + 1);
                    return true;
                }
            }
            return false;
        });
        /* --------------------------------------------------------------------- */
    };

    /******************************************************************************************************************************************************/
    /******************************************************************************************************************************************************/

    Component input_get_receipt = Input(&get_receipt_index, "Enter Payment ID");
    Component input_create_payment = Input(&create_payment_index, "Enter Invoice ID");

    string msg_control = "";

    Component btn_get_receipt = Button("Submit", [&] {
        if (all_of(get_receipt_index.begin(), get_receipt_index.end(), ::isdigit) && !get_receipt_index.empty()) {
                data_for_Receipt = myDB.getReceiptPDF(stoi(get_receipt_index));
                if (data_for_Receipt.P_id == 0) {
                msg_control = "Error, payment not found or no appointment linked";
                return;
                }
                GenerateReceipt(data_for_Receipt);
                msg_control = "Success made reciept";
            } else {msg_control = "Error, ID is not a digit or empty";}
    });

    Component btn_create_payment = Button("Submit", [&] {
        string current_date = format("{:%Y-%m-%d}", chrono::floor<chrono::days>(chrono::system_clock::now()));
        if (all_of(create_payment_index.begin(), create_payment_index.end(), ::isdigit) && !create_payment_index.empty()) {
                if (myDB.insertPaymentByInvoice(current_date.c_str(), stoi(create_payment_index))) {
                    msg_control = "Success create Payment";
                } else{msg_control = "Enter valid invoice ID";};

            } else {msg_control = "Error, ID is not a digit or empty";}
    });

    auto screen_control = Renderer(
        Container::Vertical({
            input_get_receipt,
            btn_get_receipt,
            input_create_payment,
            btn_create_payment,
        }), [&, input_get_receipt, btn_get_receipt, input_create_payment, btn_create_payment] {
            return vbox({
                text("Control Panel") | color(Color::Green), separator(),
                text(msg_control) | color(Color::Red), separator(),
                text("Generate Reciept") | color(Color::Green), separator(),
                hbox({text("Enter Payment ID: "), input_get_receipt ->Render()}), separator(),
                btn_get_receipt ->Render(), separator(),
                text("Create Payment") | color(Color::Green), separator(),
                hbox({text("Enter Invoice ID: "), input_create_payment -> Render()}), separator(),
                btn_create_payment -> Render(), separator(),
            });
        });

    auto MakeLogOut = [&]() {
        auto btn = Button("Log Out", [&] {
            security_index = 0;
            username = "";
            password = "";
        });
        return Renderer(Container::Vertical({btn}), [btn]() mutable {
            return btn->Render() | center;
        });
    };

    auto admin_dashboard = MakeDashboard();
    auto owner_dashboard = MakeDashboard();
    auto staff_dashboard = MakeDashboard();

    auto admin_logout = MakeLogOut();
    auto owner_logout = MakeLogOut();
    auto staff_logout = MakeLogOut();

    auto admin_employee = MakeEmployeeScreen();
    auto owner_employee = MakeEmployeeScreen();

    auto admin_invoice = MakeInvoiceScreen();
    auto owner_invoice = MakeInvoiceScreen();
    auto staff_invoice = MakeInvoiceScreen();

    auto admin_client = MakeClientScreen();
    auto owner_client = MakeClientScreen();
    auto staff_client = MakeClientScreen();

    auto admin_appointment = MakeAppointmentScreen();
    auto owner_appointment = MakeAppointmentScreen();
    auto staff_appointment = MakeAppointmentScreen();


    auto screen_admin_tabs = Container::Tab({admin_dashboard, screen_package_scroll, admin_employee, admin_invoice, admin_client, screen_payment_scroll, admin_appointment,screen_control, admin_logout}, &menu_selected_admin);
    auto screen_owner_tabs = Container::Tab({owner_dashboard, owner_employee, owner_invoice, owner_client, owner_appointment, owner_logout}, &menu_selected_owner);
    auto screen_staff_tabs = Container::Tab({staff_dashboard, staff_invoice, staff_client, staff_appointment, staff_logout}, &menu_selected_staff);


    auto screen_staff_container = Container::Horizontal({
        sidebar_staff | border,
        screen_staff_tabs | flex | border
    });


    auto screen_admin_container = Container::Horizontal({
        sidebar_admin | border,
        screen_admin_tabs | flex | border

    });


    auto screen_owner_container = Container::Horizontal({
        sidebar_owner | border,
        screen_owner_tabs | flex | border

    });


    InputOption pwd_opt;
    pwd_opt.password = true;

    Component input_username = Input(&username, "Username");
    Component input_password = Input(&password, "Password", pwd_opt);

    string msg_log_in = "";

    Component btn_log_in = Button("Submit", [&] {

        int result = CompareHash(username, password);


        if (result == 1) {
            security_index = 1;
        }
        else if (result == 2) {
            security_index = 2;
        }
        else if (result == 3) {
            security_index = 3;
        }
        else {
            msg_log_in = "Invalid username or password";
        }
    });

    auto login_screen = Renderer(
        Container::Vertical({
            input_username,
            input_password,
            btn_log_in
        }), [&] {
           return vbox({
               text("Welcome to log in page") | bold | color(Color::Green) | center, separator(),
               text(msg_log_in) | color(Color::Red) | center, separator(),
               hbox({text("Username: "), input_username ->Render()}), separator(),
               hbox({text("Password: "), input_password->Render()}), separator(),
               btn_log_in ->Render(), separator(),
           }) | border | flex;
        });


    auto final3 = CatchEvent(
    Renderer([&] {
        if (security_index == 0) return login_screen->Render() | flex;
        if (security_index == 1) return screen_admin_container->Render() | flex;
        if (security_index == 2) return screen_owner_container->Render() | flex;
        if (security_index == 3) return screen_staff_container->Render() | flex;
        return login_screen->Render() | flex;
    }),
    [&](Event event) {
        if (security_index == 0) return login_screen->OnEvent(event);
        if (security_index == 1) return screen_admin_container->OnEvent(event);
        if (security_index == 2) return screen_owner_container->OnEvent(event);
        if (security_index == 3) return screen_staff_container->OnEvent(event);
        return login_screen->OnEvent(event);
    }
);

    screen.Loop(final3); // used to update screen

    return 0;
}