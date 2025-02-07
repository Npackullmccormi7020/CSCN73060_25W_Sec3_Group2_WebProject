#define CROW_MAIN

#include "crow_all.h"
#include <iostream>
#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

// Function to initialize the SQLite database
void initDatabase(sqlite3*& db) {
    const char* createTableSQL = R"SQL(
        CREATE TABLE IF NOT EXISTS appointments (
            id TEXT PRIMARY KEY,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            service TEXT NOT NULL,
            date TEXT NOT NULL,
            time TEXT NOT NULL
        );
    )SQL";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Error creating table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

// Function to serve static files
void sendFile(crow::response& res, const std::string& path, const std::string& contentType) {
    std::string fullPath = "/Shared/public/" + path;
    std::ifstream file(fullPath, std::ios::in);
    std::cout << "Attempting to serve file: " << fullPath << std::endl;
    if (file) {
        std::ostringstream content;
        content << file.rdbuf();
        file.close();
        res.set_header("Content-Type", contentType);
        res.write(content.str());
    }
    else {
        std::cerr << "File not found: " << fullPath << std::endl;
        res.code = 404;
        res.write("File not found");
    }
    res.end();
}

int main() {
    crow::SimpleApp app;
    sqlite3* db;

    // Open SQLite database
    if (sqlite3_open("appointments.db", &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }
    initDatabase(db);

    // Route to serve the index page
    CROW_ROUTE(app, "/")
        ([](const crow::request&, crow::response& res) {
        sendFile(res, "index.html", "text/html");
            });

    // Route to serve the booking page
    CROW_ROUTE(app, "/book")
        ([](const crow::request&, crow::response& res) {
        sendFile(res, "book.html", "text/html");
            });

    // Route to serve the manage appointments page
    CROW_ROUTE(app, "/manage")
        ([](const crow::request&, crow::response& res) {
        sendFile(res, "manage.html", "text/html");
            });

    // Route to serve CSS files
    CROW_ROUTE(app, "/styles.css")
        ([](const crow::request&, crow::response& res) {
        sendFile(res, "styles.css", "text/css");
            });

    // Route to serve JS files for booking
    CROW_ROUTE(app, "/scripts/book.js")
        ([](const crow::request&, crow::response& res) {
        sendFile(res, "scripts/book.js", "application/javascript");
            });

    // Route to serve JS files for managing
    CROW_ROUTE(app, "/scripts/manage.js")
        ([](const crow::request&, crow::response& res) {
        sendFile(res, "scripts/manage.js", "application/javascript");
            });

    // Route to handle appointment booking
    CROW_ROUTE(app, "/appointments").methods("POST"_method)([&](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            std::string id = body["id"];
            std::string first_name = body["first_name"];
            std::string last_name = body["last_name"];
            std::string service = body["service"];
            std::string date = body["date"];
            std::string time = body["time"];

            std::string sql = "INSERT INTO appointments (id, first_name, last_name, service, date, time) VALUES (?, ?, ?, ?, ?, ?);";
            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, first_name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, last_name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, service.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 5, date.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 6, time.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                sqlite3_finalize(stmt);
                return crow::response(500, "Failed to create appointment.");
            }

            sqlite3_finalize(stmt);
            return crow::response(201, "Appointment created successfully.");
        }
        catch (const std::exception& e) {
            return crow::response(400, "Invalid data: " + std::string(e.what()));
        }
        });

    // Route to get an appointment by ID
    CROW_ROUTE(app, "/appointments/<string>").methods("GET"_method)([&](const crow::request&, crow::response& res, const std::string& id) {
        std::string sql = "SELECT * FROM appointments WHERE LOWER(id) = LOWER(?);";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            json appointment = {
                {"id", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)))},
                {"first_name", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)))},
                {"last_name", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)))},
                {"service", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)))},
                {"date", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)))},
                {"time", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)))}
            };
            sqlite3_finalize(stmt);
            res.set_header("Content-Type", "application/json");
            res.write(appointment.dump());
        }
        else {
            sqlite3_finalize(stmt);
            res.code = 404;
            res.write("Appointment not found.");
        }
        res.end();
        });



    // Route to get an appointment by last name
    CROW_ROUTE(app, "/appointments").methods("GET"_method)([&](const crow::request& req, crow::response& res) {
        auto query_params = crow::query_string(req.url_params);
        const char* last_name_param = query_params.get("last_name");

        if (!last_name_param) {
            res.code = 400;
            res.write("Missing required parameter: last_name");
            res.end();
            return;
        }

        std::string last_name = last_name_param;
        std::string sql = "SELECT * FROM appointments WHERE LOWER(last_name) = LOWER(?);";

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, last_name.c_str(), -1, SQLITE_TRANSIENT);

        json appointments = json::array();
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            json appointment = {
                {"id", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)))},
                {"first_name", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)))},
                {"last_name", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)))},
                {"service", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)))},
                {"date", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)))},
                {"time", std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)))}
            };
            appointments.push_back(appointment);
        }

        sqlite3_finalize(stmt);

        if (appointments.empty()) {
            res.code = 404;
            res.write("No appointments found for the given last name.");
        }
        else {
            res.set_header("Content-Type", "application/json");
            res.write(appointments.dump());
        }

        res.end();
        });






    // Route to update an appointment
    CROW_ROUTE(app, "/appointments/<string>").methods("PUT"_method)([&](const crow::request& req, crow::response& res, const std::string& id) {
        auto body = json::parse(req.body);
        std::string sql = "UPDATE appointments SET service = ?, date = ?, time = ? WHERE id = ?";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, body["service"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, body["date"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, body["time"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, id.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            res.code = 200;
            res.write("Appointment updated successfully.");
        }
        else {
            sqlite3_finalize(stmt);
            res.code = 500;
            res.write("Failed to update appointment.");
        }
        res.end();
        });




    // Route to delete an appointment
    CROW_ROUTE(app, "/appointments/<string>").methods("DELETE"_method)([&](const crow::request&, crow::response& res, const std::string& id) {
        std::string sql = "DELETE FROM appointments WHERE id = ?";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            res.code = 204;  // No content
        }
        else {
            sqlite3_finalize(stmt);
            res.code = 500;
            res.write("Failed to delete appointment.");
        }
        res.end();
        });



    // Route to partially update an appointment
    CROW_ROUTE(app, "/appointments/<string>").methods("PATCH"_method)([&](const crow::request& req, crow::response& res, const std::string& id) {
        try {
            auto body = json::parse(req.body);

            // Debug print request body
            std::cout << "Update request for ID: " << id << std::endl;
            std::cout << "Received Data: " << body.dump() << std::endl;

            std::string sql = "UPDATE appointments SET service = ?, date = ?, time = ? WHERE id = ?";
            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
            sqlite3_bind_text(stmt, 1, body["service"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, body["date"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, body["time"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, id.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) == SQLITE_DONE) {
                sqlite3_finalize(stmt);
                res.code = 200;
                res.write("Appointment updated successfully.");
            }
            else {
                std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt);
                res.code = 500;
                res.write("Failed to update appointment.");
            }
        }
        catch (const std::exception& e) {
            std::cerr << "JSON Parse Error: " << e.what() << std::endl;
            res.code = 400;
            res.write("Invalid JSON format.");
        }
        res.end();
        });


    // Route to handle OPTIONS requests
    CROW_ROUTE(app, "/appointments/<string>").methods("OPTIONS"_method)([](const crow::request&, crow::response& res, const std::string&) {
        res.set_header("Allow", "GET, POST, PUT, PATCH, DELETE, OPTIONS");
        res.code = 204; // No Content
        res.end();
        });

    CROW_ROUTE(app, "/appointments").methods("OPTIONS"_method)([](const crow::request&, crow::response& res) {
        res.set_header("Allow", "GET, POST, OPTIONS");
        res.code = 204; // No Content
        res.end();
        });


    app.port(23500).multithreaded().run();
    sqlite3_close(db);

    return 0;
}
