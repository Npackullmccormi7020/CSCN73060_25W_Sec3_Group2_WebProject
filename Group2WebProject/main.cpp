#define CROW_MAIN

#include "crow_all.h"
#include <iostream>
using namespace std;
using namespace crow;

void sendFile(response &res, string filename, string contentType)
{
	ifstream in("../public/" + filename, ifstream::in);
	if (in)
    {
        ostringstream contents;
        contents << in.rdbuf();
        in.close();
        res.set_header("Content-Type", contentType);
        res.write(contents.str());
    }
    else
    {
        res.code = 404;
        res.write("NotFound");
    }
    res.end();
}

void sendHtml(response &res, string filename)
{
    sendFile(res, filename + ".html", "text/html");
}

void sendImage(response &res, string filename)
{
    sendFile(res, "images/" + filename, "image/jpeg");
}

void sendStyle(response &res, string filename)
{
	sendFile(res, "styles/" + filename, "text/css");
}

void sendScript(response &res, string filename)
{
	sendFile(res, "scripts/" + filename, "text/javascript");
}


int main()
{
	crow::SimpleApp app;

	CROW_ROUTE(app, "/")
	([](const request& req, response& res) {
	sendHtml(res, "Index");
	});

	CROW_ROUTE(app, "/<string>")
	([](const request& req, response& res, string filename) {
	sendHtml(res, filename);
	});

	CROW_ROUTE(app, "/get_script/<string>")
	([](const request& req, response& res, string filename) {
	sendScript(res, filename);
	});

	CROW_ROUTE(app, "/get_style/<string>")
	([](const request& req, response& res, string filename) {
	sendStyle(res, filename);
	});

	CROW_ROUTE(app, "/get_image/<string>")
	([](const request& req, response& res, string filename) {
	sendImage(res, filename);
	});

	app.port(23500).multithreaded().run();
	return 1;
}