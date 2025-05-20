#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>

#include "sqlite3.h"

#include "Swapify.cc"


class SQLiteODBC {
protected:
    // class variables
    SString dbName;
    bool isConnected;

	sqlite3* db;
	sqlite3_stmt* stmt_handle;
	const char* stmt_leftover;

    // private methods
    void populateTable_Product() {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << "Populate Product\n";

        string stmt = "INSERT INTO Product VALUES(?, ?, ?)";
        int rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "A", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1001);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "A", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1002);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "A", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1003);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "A", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 2004);
        sqlite3_bind_text(stmt_handle, 3, "laptop", 6, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "A", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 2005);
        sqlite3_bind_text(stmt_handle, 3, "laptop", 6, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "A", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 2006);
        sqlite3_bind_text(stmt_handle, 3, "laptop", 6, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "B", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1004);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "B", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1005);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "B", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1006);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "B", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 2007);
        sqlite3_bind_text(stmt_handle, 3, "laptop", 6, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "C", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1007);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "D", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1008);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "D", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1009);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "D", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1010);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "D", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 3004);
        sqlite3_bind_text(stmt_handle, 3, "printer", 7, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "D", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 3005);
        sqlite3_bind_text(stmt_handle, 3, "printer", 7, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "E", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1011);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "E", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1012);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "E", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 1013);
        sqlite3_bind_text(stmt_handle, 3, "pc", 2, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "E", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 2001);
        sqlite3_bind_text(stmt_handle, 3, "laptop", 6, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "E", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 2002);
        sqlite3_bind_text(stmt_handle, 3, "laptop", 6, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "E", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 2003);
        sqlite3_bind_text(stmt_handle, 3, "laptop", 6, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "E", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 3001);
        sqlite3_bind_text(stmt_handle, 3, "printer", 7, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "E", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 3002);
        sqlite3_bind_text(stmt_handle, 3, "printer", 7, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "E", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 3003);
        sqlite3_bind_text(stmt_handle, 3, "printer", 7, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "F", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 2008);
        sqlite3_bind_text(stmt_handle, 3, "laptop", 6, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "F", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 2009);
        sqlite3_bind_text(stmt_handle, 3, "laptop", 6, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "G", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 2010);
        sqlite3_bind_text(stmt_handle, 3, "laptop", 6, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "H", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 3006);
        sqlite3_bind_text(stmt_handle, 3, "printer", 7, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_reset(stmt_handle);
        sqlite3_clear_bindings(stmt_handle);
        sqlite3_bind_text(stmt_handle, 1, "H", 1, 0);
        sqlite3_bind_int(stmt_handle, 2, 3007);
        sqlite3_bind_text(stmt_handle, 3, "printer", 7, 0);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_finalize(stmt_handle);

        cout << "success\n";
        cout << "++++++++++++++++++++++++++++++++++\n";
    }

    void populateTable_PC() {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << "Populate PC\n";

        SInt m = 1001, r = 1024, h = 250, p = 2114; SDouble s = 2.66;
        insert_PC(m, s, r, h, p);
        m = 1002; s = 2.10; r = 512; h = 250; p = 995;
        insert_PC(m, s, r, h, p);
        m = 1003; s = 1.42; r = 512; h = 80; p = 478;
        insert_PC(m, s, r, h, p);
        m = 1004; s = 2.80; r = 1024; h = 250; p = 649;
        insert_PC(m, s, r, h, p);
        m = 1005; s = 3.20; r = 512; h = 250; p = 630;
        insert_PC(m, s, r, h, p);
        m = 1006; s = 3.20; r = 1024; h = 320; p = 1049;
        insert_PC(m, s, r, h, p);
        m = 1007; s = 2.20; r = 1024; h = 200; p = 510;
        insert_PC(m, s, r, h, p);
        m = 1008; s = 2.20; r = 2048; h = 250; p = 770;
        insert_PC(m, s, r, h, p);
        m = 1009; s = 2.00; r = 1024; h = 250; p = 650;
        insert_PC(m, s, r, h, p);
        m = 1010; s = 2.80; r = 2048; h = 300; p = 770;
        insert_PC(m, s, r, h, p);
        m = 1011; s = 1.86; r = 2048; h = 160; p = 959;
        insert_PC(m, s, r, h, p);
        m = 1012; s = 2.80; r = 1024; h = 160; p = 649;
        insert_PC(m, s, r, h, p);
        m = 1013; s = 3.06; r = 512; h = 80; p = 529;
        insert_PC(m, s, r, h, p);

        cout << "success\n";
        cout << "++++++++++++++++++++++++++++++++++\n";
    }

    void insert_PC(SInt& _model, SDouble& _speed, SInt& _ram, SInt& _hd, SInt& _price) {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << "Insert PC\n";

        string stmt = "\
            INSERT INTO PC(model, speed, ram, hd, price) \
                VALUES(?, ?, ?, ?, ?)";
        int rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_bind_int(stmt_handle, 1, _model);
        sqlite3_bind_double(stmt_handle, 2, _speed);
        sqlite3_bind_int(stmt_handle, 3, _ram);
        sqlite3_bind_int(stmt_handle, 4, _hd);
        sqlite3_bind_int(stmt_handle, 5, _price);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
        sqlite3_finalize(stmt_handle);

        cout << "success\n";
        cout << "++++++++++++++++++++++++++++++++++\n";

    }

    void populateTable_Laptop() {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << "Populate Laptop\n";

        SInt m = 2001, r = 2048, h = 240, p = 3673; SDouble s = 2.00, sc = 20.1;
        insert_Laptop(m, s, r, h, sc, p);
        m = 2002, s = 1.73, r = 1024, h = 80, sc = 17.0, p = 949;
        insert_Laptop(m, s, r, h, sc, p);
        m = 2003, s = 1.80, r = 512, h = 60, sc = 15.4, p = 549;
        insert_Laptop(m, s, r, h, sc, p);
        m = 2004, s = 2.00, r = 512, h = 60, sc = 13.3, p = 1150;
        insert_Laptop(m, s, r, h, sc, p);
        m = 2005, s = 2.16, r = 1024, h = 120, sc = 17.0, p = 2500;
        insert_Laptop(m, s, r, h, sc, p);
        m = 2006, s = 2.00, r = 2048, h = 80, sc = 15.4, p = 1700;
        insert_Laptop(m, s, r, h, sc, p);
        m = 2007, s = 1.83, r = 1024, h = 120, sc = 13.3, p = 1429;
        insert_Laptop(m, s, r, h, sc, p);
        m = 2008, s = 1.60, r = 1024, h = 100, sc = 15.4, p = 900;
        insert_Laptop(m, s, r, h, sc, p);
        m = 2009, s = 1.60, r = 512, h = 80, sc = 14.1, p = 680;
        insert_Laptop(m, s, r, h, sc, p);
        m = 2010, s = 2.00, r = 2048, h = 160, sc = 15.4, p = 2300;
        insert_Laptop(m, s, r, h, sc, p);

        cout << "success\n";
        cout << "++++++++++++++++++++++++++++++++++\n";
    }

    void insert_Laptop(SInt& _model, SDouble& _speed, SInt& _ram,
        SInt& _hd, SDouble& _screen, SInt& _price) {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << "Insert Laptop\n";
            
        string stmt = "INSERT INTO Laptop VALUES(?, ?, ?, ?, ?, ?)";
        int rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_bind_int(stmt_handle, 1, _model);
        sqlite3_bind_double(stmt_handle, 2, _speed);
        sqlite3_bind_int(stmt_handle, 3, _ram);
        sqlite3_bind_int(stmt_handle, 4, _hd);
        sqlite3_bind_double(stmt_handle, 5, _screen);
        sqlite3_bind_int(stmt_handle, 6, _price);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
        sqlite3_finalize(stmt_handle);

        cout << "success\n";
        cout << "++++++++++++++++++++++++++++++++++\n";
    }

    void populateTable_Printer() {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << "Populate Printer\n";

        SInt m = 3001, c = 1, p = 99; SString t = string("ink-jet");
        insert_Printer(m, c, t, p);
        m = 3002, c = 0, t = string("laser"), p = 239;
        insert_Printer(m, c, t, p);
        m = 3003, c = 1, t = string("laser"), p = 899;
        insert_Printer(m, c, t, p);
        m = 3004, c = 1, t = string("ink-jet"), p = 120;
        insert_Printer(m, c, t, p);
        m = 3005, c = 0, t = string("laser"), p = 120;
        insert_Printer(m, c, t, p);
        m = 3006, c = 1, t = string("ink-jet"), p = 100;
        insert_Printer(m, c, t, p);
        m = 3007, c = 1, t = string("laser"), p = 200;
        insert_Printer(m, c, t, p);

        cout << "success\n";
        cout << "++++++++++++++++++++++++++++++++++\n";
    }

    void insert_Printer(SInt& _model, SInt& _color, SString& _type, SInt& _price) {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << "Insert Printer\n";
            
        string stmt = "\
            INSERT INTO Printer(model, color, type, price) \
                VALUES(?, ?, ?, ?)";
        int rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        sqlite3_bind_int(stmt_handle, 1, _model);
        sqlite3_bind_int(stmt_handle, 2, _color);
        string st = _type;
        sqlite3_bind_text(stmt_handle, 3, st.c_str(), st.length(), 0);
        sqlite3_bind_int(stmt_handle, 4, _price);
        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
        sqlite3_finalize(stmt_handle);

        cout << "success\n";
        cout << "++++++++++++++++++++++++++++++++++\n";
    }

public:
    // constructor & destructor
    SQLiteODBC(SString& _dbFile) : dbName(_dbFile), isConnected(false) {}
    
    virtual ~SQLiteODBC() {
        if (true == isConnected) {
            CloseConnection();
        }
    }

    // public interface
    void OpenConnection() {
        if (isConnected == false) {
            cout << "++++++++++++++++++++++++++++++++++\n";
            cout << "Open database: " << dbName << endl;

            int rc = sqlite3_open(string(dbName).c_str(), &db);
            if (rc != SQLITE_OK) {
                cout << sqlite3_errmsg(db) << endl;
                sqlite3_close(db);
                exit(1);
            }

            isConnected = true;

            cout << "success\n";
            cout << "++++++++++++++++++++++++++++++++++\n";
        }
    }

    void CloseConnection() {
        if (isConnected == true) {
            cout << "++++++++++++++++++++++++++++++++++\n";
            cout << "Close database: " << dbName << endl;

            sqlite3_close(db);
            isConnected = false;

            cout << "success\n";
            cout << "++++++++++++++++++++++++++++++++++\n";
        }
    }

    void CreateTables() {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << "Create tables\n";

        string stmt = "\
            CREATE TABLE IF NOT EXISTS Product (\
                maker CHAR(32),\
                model INTEGER PRIMARY KEY,\
                type VARCHAR(20) NOT NULL);";

        int rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
        sqlite3_finalize(stmt_handle);

        stmt = "\
            CREATE TABLE IF NOT EXISTS PC (\
                model INTEGER PRIMARY KEY,\
                speed FLOAT,\
                ram INTEGER,\
                hd INTEGER,\
                price DECIMAL(7,2) NOT NULL);";

        rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
        sqlite3_finalize(stmt_handle);

        stmt = "\
            CREATE TABLE IF NOT EXISTS Laptop (\
                model INTEGER PRIMARY KEY,\
                speed FLOAT,\
                ram INTEGER,\
                hd INTEGER,\
                screen DECIMAL(4,1),\
                price DECIMAL(7,2) NOT NULL);";

        rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
        sqlite3_finalize(stmt_handle);

        stmt = "\
            CREATE TABLE IF NOT EXISTS Printer (\
                model INTEGER PRIMARY KEY,\
                color BOOL,\
                type VARCHAR(30),\
                price decimal(7,2) NOT NULL);";

        rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
        sqlite3_finalize(stmt_handle);

        cout << "success\n";
        cout << "++++++++++++++++++++++++++++++++++\n";
    }

    void DropTables() {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << "Drop tables\n";

        string stmt = "DROP TABLE IF EXISTS Product;";
        int rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
        sqlite3_finalize(stmt_handle);

        stmt = "DROP TABLE IF EXISTS PC;";
        rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
        sqlite3_finalize(stmt_handle);

        stmt = "DROP TABLE IF EXISTS Laptop;";
        rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
        sqlite3_finalize(stmt_handle);

        stmt = "DROP TABLE IF EXISTS Printer;";
        rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        rc = sqlite3_step(stmt_handle);
        if (rc != SQLITE_DONE) {
            cout << "Cannot execute statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }
        sqlite3_finalize(stmt_handle);

        cout << "success\n";
        cout << "++++++++++++++++++++++++++++++++++\n";
    }

    void PopulateTables() {
        populateTable_Product();
        populateTable_PC();
        populateTable_Laptop();
        populateTable_Printer();
    }

    void PCsByMaker(SString& _maker) {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << "PCs by maker: " << _maker << endl;

        string stmt =
            "select P.model as model, PC.price as price \
            from Product P, PC \
            where P.model = PC.model AND \
            maker = ?";
        int rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        string st = _maker;
        sqlite3_bind_text(stmt_handle, 1, st.c_str(), st.length(), 0);

        cout << setw(10) << right << "model" << setw(10) << right << "price" << endl;
        cout << "-------------------------------\n";

        while (true) {
            rc = sqlite3_step(stmt_handle);
            if (rc == SQLITE_DONE) break;
            if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
                cout << "Cannot execute statment " << stmt << endl;
                cout << "The error is: " << sqlite3_errmsg(db) << endl;
                exit(1);
            }
            int model = sqlite3_column_int(stmt_handle, 0);
            int price = sqlite3_column_int(stmt_handle, 1);

            cout << setw(10) << right << model << setw(10) << right << price << endl;
        }
        sqlite3_finalize(stmt_handle);

        cout << "++++++++++++++++++++++++++++++++++\n";
    }

    void ProductByMaker(SString& _pType, SString& _maker) {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << _pType << " by maker: " << _maker << endl;

        string stmt =
            "select P.model as model, " +
            string(_pType)  + ".price as price " +
            "from Product P, " + string(_pType) +
            " where P.model = " + string(_pType) + ".model AND " +
            "maker = ?";
        int rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        string st = _maker;
        sqlite3_bind_text(stmt_handle, 1, st.c_str(), st.length(), 0);

        cout << setw(10) << right << "model" << setw(10) << right << "price" << endl;
        cout << "-------------------------------\n";

        while (true) {
            rc = sqlite3_step(stmt_handle);
            if (rc == SQLITE_DONE) break;
            if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
                cout << "Cannot execute statment " << stmt << endl;
                cout << "The error is: " << sqlite3_errmsg(db) << endl;
                exit(1);
            }
            int model = sqlite3_column_int(stmt_handle, 0);
            int price = sqlite3_column_int(stmt_handle, 1);

            cout << setw(10) << right << model << setw(10) << right << price << endl;
        }
        sqlite3_finalize(stmt_handle);

        cout << "++++++++++++++++++++++++++++++++++\n";
    }

    void AllProductsByMaker(SString& _maker) {
        cout << "++++++++++++++++++++++++++++++++++\n";
        cout << "Products by maker: " << _maker << endl;

        string stmt =
            "select P.model as model, P.type as type, PC.price as price \
            from Product P, PC \
            where P.model = PC.model AND \
            maker = ?";

        stmt += " UNION ";

        stmt +=
            "select P.model as model, P.type as type, L.price as price \
            from Product P, Laptop L \
            where P.model = L.model AND \
            maker = ?";
        
        stmt += " UNION ";

        stmt +=
            "select P.model as model, P.type as type, Pr.price as price \
            from Product P, Printer Pr \
            where P.model = Pr.model AND \
            maker = ?";

        int rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &stmt_handle, &stmt_leftover);
        if (rc != SQLITE_OK) {
            cout << "Cannot compile statment " << stmt << endl;
            cout << "The error is: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        string st = _maker;
        sqlite3_bind_text(stmt_handle, 1, st.c_str(), st.length(), 0);
        sqlite3_bind_text(stmt_handle, 2, st.c_str(), st.length(), 0);
        sqlite3_bind_text(stmt_handle, 3, st.c_str(), st.length(), 0);

        cout << setw(10) << right << "model" << setw(20) << right << "type" << setw(10) << right << "price" << endl;
        cout << "-------------------------------\n";

        while (true) {
            rc = sqlite3_step(stmt_handle);
            if (rc == SQLITE_DONE) break;
            if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
                cout << "Cannot execute statment " << stmt << endl;
                cout << "The error is: " << sqlite3_errmsg(db) << endl;
                exit(1);
            }
            int model = sqlite3_column_int(stmt_handle, 0);
    		string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt_handle, 1));
            int price = sqlite3_column_int(stmt_handle, 2);

            cout << setw(10) << right << model << setw(20) << right << type << setw(10) << right << price << endl;
        }
        sqlite3_finalize(stmt_handle);

        cout << "++++++++++++++++++++++++++++++++++\n";        
    }
};


int main (int argc, char* argv[]) {
	if (argc != 2) {
		cout << "Usage: main [sqlite_file]" << endl;
		return -1;
	}

	SString dbFile(argv[1]);
	SQLiteODBC sj(dbFile);
	
	sj.OpenConnection();

	sj.DropTables();
	sj.CreateTables();
	sj.PopulateTables();

	SString maker("A"), product("Laptop");

	sj.PCsByMaker(maker);
	sj.ProductByMaker(product, maker);
	sj.AllProductsByMaker(maker);

	sj.CloseConnection();

	return 0;
}
