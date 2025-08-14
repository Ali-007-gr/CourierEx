📦 Courier & Parcel Tracking System

A C++ console-based application for managing courier and parcel deliveries with separate User & Admin dashboards.
The project focuses on simplicity, efficiency, and clean code using only iostream, fstream, and string.

🚀 Features
👤 User Features

Sign Up / Login with input validation (email format, password length, username availability)

Book New Parcel (automatic Parcel ID starting from P1001)

Track Parcel by ID

View Delivery History

Manage Profile (view/update details, change/forgot password)

Logout

🛠️ Admin Features

Admin Login (separate from user login)

View All Parcels

Update Parcel Status

Search Parcel by ID

View Profile & Change Password

Logout

💾 Data Storage

All records stored in text files using comma-separated values (CSV style) for easy reading and parsing:

users.txt → Stores user credentials & profile details

admin.txt → Stores admin credentials & profile details

parcels.txt → Stores all parcel booking records

🛠️ Technologies Used

C++ (Standard Library)

iostream – input/output operations

fstream – file handling (read/write persistent storage)

string – string operations

📂 Project Structure
Courier-Parcel-Tracking-System/
│
├── main.cpp        # Main program file
├── users.txt       # User data storage
├── admin.txt       # Admin data storage
└── parcels.txt     # Parcel booking data storage

📋 How to Run

Clone the repository

git clone https://github.com/Ali-007-gr/CourierEx.git
cd CourierEx


Compile the code

g++ main.cpp -o parcel_system


Run the program

./parcel_system

🧩 Future Improvements

Add date & time stamps for bookings and status updates

Implement search by sender/receiver name

Add password encryption for better security

Improve UI with colored console output

📜 License

This project is open-source and available under the MIT License.
