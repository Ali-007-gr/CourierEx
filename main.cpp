#include <iostream>
#include <fstream>
#include <string>
using namespace std;

/* ===================== Utilities (no <sstream>, no stoi/to_string) ===================== */

// Trim trailing '\r' if any (Windows line endings safety)
string trimCR(string s) {
    if (!s.empty() && s[s.size()-1] == '\r') s.erase(s.size()-1);
    return s;
}

// Split a CSV line into fields (no quotes support; simple comma-split)
int splitCSV(const string &line, string parts[], int maxParts) {
    int count = 0;
    size_t start = 0;
    while (count < maxParts - 1) {
        size_t pos = line.find(',', start);
        if (pos == string::npos) break;
        parts[count++] = line.substr(start, pos - start);
        start = pos + 1;
    }
    // last part (or entire line if no comma)
    parts[count++] = line.substr(start);
    // trim CR on the last part (if present)
    parts[count-1] = trimCR(parts[count-1]);
    return count;
}

// String -> int (non-negative). Returns -1 if invalid.
int strToInt(const string &s) {
    if (s.empty()) return -1;
    int val = 0;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] < '0' || s[i] > '9') return -1;
        val = val * 10 + (s[i] - '0');
    }
    return val;
}

// int -> string (non-negative)
string intToStr(int x) {
    if (x == 0) return "0";
    string r = "";
    while (x > 0) {
        char d = char('0' + (x % 10));
        r = d + r;
        x /= 10;
    }
    return r;
}

/* ===================== Person Base ===================== */

class Person {
protected:
    string name;
    string username;
    string password;
    string email;
    string role; // "user" or "admin"

    bool emailOK(const string &e) {
        return (e.find('@') != string::npos);
    }

    bool usernameTaken(const string &u) {
        ifstream fin("users.txt");
        string line, fields[5];
        while (getline(fin, line)) {
            if (line.empty()) continue;
            int n = splitCSV(line, fields, 5); // username,password,email,role,name
            if (n >= 4) {
                if (fields[0] == u) {
                    return true;
                }
            }
        }
        return false;
    }

public:
    Person() { name=""; username=""; password=""; email=""; role=""; }

    void setName(const string &n) {
        if (n != "") name = n;
        else cout << "Name cannot be empty.\n";
    }

    void setUsername(const string &u) {
        if (u == "") { cout << "Username cannot be empty.\n"; return; }
        if (usernameTaken(u)) cout << "Username already taken.\n";
        else username = u;
    }

    void setPassword(const string &p) {
        if (p.size() >= 6) password = p;
        else cout << "Password must be at least 6 characters.\n";
    }

    void setEmail(const string &e) {
        if (emailOK(e)) email = e;
        else cout << "Invalid email.\n";
    }

    string getName() { return name; }
    string getUsername() { return username; }
    string getEmail() { return email; }
    string getRole() { return role; }

    // Signup for normal users (admins are typically pre-seeded)
    void signupUser() {
        string n, u, p, e;
        cout << "Enter full name: ";
        cin.ignore();
        getline(cin, n);
        setName(n);

        cout << "Enter email: ";
        getline(cin, e);
        setEmail(e);

        cout << "Enter username: ";
        getline(cin, u);
        setUsername(u);

        cout << "Enter password (>=6): ";
        getline(cin, p);
        setPassword(p);

        if (name != "" && email != "" && username != "" && password != "") {
            ofstream fout("users.txt", ios::app);
            // username,password,email,role,name
            fout << username << "," << password << "," << email << ",user," << name << "\n";
            fout.close();
            cout << "Signup successful!\n";
        } else {
            cout << "Signup failed. Try again.\n";
        }
    }

    // Role-safe login. requiredRole: "user" or "admin"
    bool login(const string &requiredRole) {
        string u, p;
        cout << "Enter username: ";
        cin >> u;
        cout << "Enter password: ";
        cin >> p;

        ifstream fin("users.txt");
        string line, fields[5];
        while (getline(fin, line)) {
            if (line.empty()) continue;
            int n = splitCSV(line, fields, 5); // username,password,email,role,name
            if (n >= 5) {
                string fu = fields[0];
                string fp = fields[1];
                string fe = fields[2];
                string fr = fields[3];
                string fn = fields[4];

                if (fu == u && fp == p && fr == requiredRole) {
                    username = fu; password = fp; email = fe; role = fr; name = fn;
                    cout << "Login successful as " << role << "!\n";
                    return true;
                }
            }
        }
        cout << "Invalid credentials or role mismatch.\n";
        return false;
    }

    void viewProfile() {
        cout << "\n--- Profile ---\n";
        cout << "Name: " << name << "\n";
        cout << "Username: " << username << "\n";
        cout << "Email: " << email << "\n";
        cout << "Role: " << role << "\n";
    }

    void changePassword() {
        string oldp, newp;
        cout << "Enter current password: ";
        cin >> oldp;
        if (oldp != password) {
            cout << "Wrong current password.\n";
            return;
        }
        cout << "Enter new password (>=6): ";
        cin >> newp;
        if (newp.size() < 6) {
            cout << "Too short.\n";
            return;
        }

        // Update users.txt
        ifstream fin("users.txt");
        ofstream temp("users.tmp");
        string line, fields[5];
        bool updated = false;

        while (getline(fin, line)) {
            if (line.empty()) { temp << "\n"; continue; }
            int n = splitCSV(line, fields, 5);
            if (n >= 5 && fields[0] == username) {
                fields[1] = newp; // update password
                temp << fields[0] << "," << fields[1] << "," << fields[2]
                     << "," << fields[3] << "," << fields[4] << "\n";
                updated = true;
            } else {
                temp << trimCR(line) << "\n";
            }
        }
        fin.close();
        temp.close();
        remove("users.txt");
        rename("users.tmp", "users.txt");

        if (updated) {
            password = newp;
            cout << "Password changed successfully.\n";
        } else {
            cout << "Could not update password.\n";
        }
    }
};

/* ===================== User ===================== */

class User : public Person {
public:
    // Book parcel: auto ID from parcel.txt last line; owner = username
    void bookParcel() {
        // Determine next ID
        ifstream fin("parcel.txt");
        string line;
        string lastID = "P1000"; // default start before first
        while (getline(fin, line)) {
            if (line.empty()) continue;
            // first field is ID up to first comma
            size_t pos = line.find(',');
            if (pos != string::npos) {
                string first = line.substr(0, pos);
                if (first.size() > 1 && first[0] == 'P') {
                    lastID = first; // keep updating; end = last line's ID
                }
            }
        }
        fin.close();

        // Extract numeric part and increment
        int lastNum = 1000;
        if (lastID.size() > 1 && lastID[0] == 'P') {
            string numPart = lastID.substr(1);
            int n = strToInt(numPart);
            if (n >= 0) lastNum = n;
        }
        int nextNum = lastNum + 1;
        string pid = "P" + intToStr(nextNum);

        // Collect details
        string rName, rContact, otype, pcity, dcity, paddr, daddr, invoice, date;
        cout << "Enter Receiver Name: ";
        cin.ignore();
        getline(cin, rName);
        cout << "Enter Contact Number (11 digits): ";
        getline(cin, rContact);
        cout << "Enter Order Type (Normal/Urgent): ";
        getline(cin, otype);
        cout << "Enter Pickup City: ";
        getline(cin, pcity);
        cout << "Enter Delivery City: ";
        getline(cin, dcity);
        cout << "Enter Pickup Address: ";
        getline(cin, paddr);
        cout << "Enter Delivery Address: ";
        getline(cin, daddr);
        cout << "Enter No. of Airway bill Invoice copies: ";
        getline(cin, invoice);
        cout << "Enter Order Date: ";
        getline(cin, date);

        // Default status = Pending
        string status = "Pending";

        // Save CSV:
        // id,ownerUsername,receiverName,contact,otype,pcity,dcity,paddr,daddr,invoice,date,status
        ofstream fout("parcel.txt", ios::app);
        fout << pid << "," << username << "," << rName << "," << rContact << ","
             << otype << "," << pcity << "," << dcity << ","
             << paddr << "," << daddr << "," << invoice << ","
             << date << "," << status << "\n";
        fout.close();

        cout << "Parcel booked successfully! Your parcel ID is: " << pid << "\n";
    }

    void trackParcel() {
        string qid;
        cout << "Enter Parcel ID (e.g., P1001): ";
        cin >> qid;

        ifstream fin("parcel.txt");
        string line, f[12];
        bool found = false;

        while (getline(fin, line)) {
            if (line.empty()) continue;
            int n = splitCSV(line, f, 12);
            if (n >= 12) {
                // f[0]=id, f[1]=owner, ..., f[11]=status
                if (f[0] == qid && f[1] == username) {
                    cout << "\n--- Parcel Details ---\n";
                    cout << "ID: " << f[0] << "\n";
                    cout << "Owner: " << f[1] << "\n";
                    cout << "Receiver: " << f[2] << "\n";
                    cout << "Contact: " << f[3] << "\n";
                    cout << "Type: " << f[4] << "\n";
                    cout << "Pickup City: " << f[5] << "\n";
                    cout << "Delivery City: " << f[6] << "\n";
                    cout << "Pickup Address: " << f[7] << "\n";
                    cout << "Delivery Address: " << f[8] << "\n";
                    cout << "Invoice Copies: " << f[9] << "\n";
                    cout << "Date: " << f[10] << "\n";
                    cout << "Status: " << f[11] << "\n";
                    found = true;
                }
            }
        }
        fin.close();

        if (!found) cout << "Parcel not found (or not owned by you).\n";
    }

    void viewHistory() {
        ifstream fin("parcel.txt");
        string line, f[12];
        bool any = false;
        cout << "\n--- Your Parcels ---\n";
        while (getline(fin, line)) {
            if (line.empty()) continue;
            int n = splitCSV(line, f, 12);
            if (n >= 12 && f[1] == username) {
                cout << "ID: " << f[0] << " | Receiver: " << f[2]
                     << " | Type: " << f[4] << " | Status: " << f[11] << "\n";
                any = true;
            }
        }
        fin.close();
        if (!any) cout << "No parcels found.\n";
    }

    void mainMenu() {
        int ch;
        do {
            cout << "\n--- User Menu ---\n";
            cout << "1. Book New Parcel\n";
            cout << "2. Track Parcel by ID\n";
            cout << "3. View Delivery History\n";
            cout << "4. View Profile\n";
            cout << "5. Change Password\n";
            cout << "6. Logout\n";
            cout << "Enter choice: ";
            cin >> ch;

            if (ch == 1) bookParcel();
            else if (ch == 2) trackParcel();
            else if (ch == 3) viewHistory();
            else if (ch == 4) viewProfile();
            else if (ch == 5) changePassword();
            else if (ch == 6) cout << "Logging out...\n";
            else cout << "Invalid choice.\n";
        } while (ch != 6);
    }
};

/* ===================== Admin ===================== */

class Admin : public Person {
public:
    void viewAllParcels() {
        ifstream fin("parcel.txt");
        string line, f[12];
        cout << "\n--- All Parcels ---\n";
        while (getline(fin, line)) {
            if (line.empty()) continue;
            int n = splitCSV(line, f, 12);
            if (n >= 12) {
                cout << "ID: " << f[0] << " | Owner: " << f[1]
                     << " | Receiver: " << f[2] << " | Type: " << f[4]
                     << " | Status: " << f[11] << "\n";
            }
        }
        fin.close();
    }

    void updateParcelStatus() {
        string qid, newStatus;
        cout << "Enter Parcel ID to update: ";
        cin >> qid;
        cout << "Enter New Status: ";
        cin.ignore();
        getline(cin, newStatus);

        ifstream fin("parcel.txt");
        ofstream temp("parcel.tmp");
        string line, f[12];
        bool updated = false;

        while (getline(fin, line)) {
            if (line.empty()) { temp << "\n"; continue; }
            int n = splitCSV(line, f, 12);
            if (n >= 12 && f[0] == qid) {
                f[11] = newStatus; // status is last field
                temp << f[0] << "," << f[1] << "," << f[2] << "," << f[3] << ","
                     << f[4] << "," << f[5] << "," << f[6] << "," << f[7] << ","
                     << f[8] << "," << f[9] << "," << f[10] << "," << f[11] << "\n";
                updated = true;
            } else {
                temp << trimCR(line) << "\n";
            }
        }
        fin.close();
        temp.close();
        remove("parcel.txt");
        rename("parcel.tmp", "parcel.txt");

        if (updated) cout << "Status updated.\n";
        else cout << "Parcel not found.\n";
    }

    void searchParcelByID() {
        string qid;
        cout << "Enter Parcel ID to search: ";
        cin >> qid;

        ifstream fin("parcel.txt");
        string line, f[12];
        bool found = false;

        while (getline(fin, line)) {
            if (line.empty()) continue;
            int n = splitCSV(line, f, 12);
            if (n >= 12 && f[0] == qid) {
                cout << "\n--- Parcel ---\n";
                cout << "ID: " << f[0] << "\n";
                cout << "Owner: " << f[1] << "\n";
                cout << "Receiver: " << f[2] << "\n";
                cout << "Contact: " << f[3] << "\n";
                cout << "Type: " << f[4] << "\n";
                cout << "Pickup City: " << f[5] << "\n";
                cout << "Delivery City: " << f[6] << "\n";
                cout << "Pickup Address: " << f[7] << "\n";
                cout << "Delivery Address: " << f[8] << "\n";
                cout << "Invoice Copies: " << f[9] << "\n";
                cout << "Date: " << f[10] << "\n";
                cout << "Status: " << f[11] << "\n";
                found = true;
            }
        }
        fin.close();

        if (!found) cout << "Parcel not found.\n";
    }

    void mainMenu() {
        int ch;
        do {
            cout << "\n--- Admin Menu ---\n";
            cout << "1. View All Parcels\n";
            cout << "2. Update Parcel Status\n";
            cout << "3. Search Parcel by ID\n";
            cout << "4. View Profile\n";
            cout << "5. Change Password\n";
            cout << "6. Logout\n";
            cout << "Enter choice: ";
            cin >> ch;

            if (ch == 1) viewAllParcels();
            else if (ch == 2) updateParcelStatus();
            else if (ch == 3) searchParcelByID();
            else if (ch == 4) viewProfile();
            else if (ch == 5) changePassword();
            else if (ch == 6) cout << "Logging out...\n";
            else cout << "Invalid choice.\n";
        } while (ch != 6);
    }
};

/* ===================== Seed a default admin (optional helper) ===================== */

void seedDefaultAdminIfMissing() {
    // Check if an admin exists; if not, create: admin, admin123, admin@mail.com, admin, "System Admin"
    ifstream fin("users.txt");
    string line, f[5];
    bool hasAdmin = false;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        int n = splitCSV(line, f, 5);
        if (n >= 4 && f[3] == "admin") { hasAdmin = true; break; }
    }
    fin.close();
    if (!hasAdmin) {
        ofstream fout("users.txt", ios::app);
        fout << "admin,admin123,admin@mail.com,admin,System Admin\n";
        fout.close();
        //cout << "(Seeded default admin: username=admin, password=admin123)\n";
    }
}

/* ===================== Main ===================== */

int main() {
    seedDefaultAdminIfMissing();

    int choice;
    do {
        cout << "\n=== Main Menu ===\n";
        cout << "1. User Signup\n";
        cout << "2. User Login\n";
        cout << "3. Admin Login\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            User u;
            u.signupUser();
        } else if (choice == 2) {
            User u;
            if (u.login("user")) {
                u.mainMenu();
            }
        } else if (choice == 3) {
            Admin a;
            if (a.login("admin")) {
                a.mainMenu();
            }
        } else if (choice == 4) {
            cout << "Goodbye!\n";
        } else {
            cout << "Invalid choice.\n";
        }
    } while (choice != 4);

    return 0;
}

