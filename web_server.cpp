#include "libs/server_https.hpp"
#include "libs/crypto.hpp"
#include <future>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "dispatcher.hpp"
#include "token.hpp"

using namespace std;
using HttpsServer = SimpleWeb::Server<SimpleWeb::HTTPS>;

Dispatcher dp;

void run_server()
{
    HttpsServer server("certs/server.crt", "certs/server.key");
    server.config.port = 8080;
    
    check_tokens(dp);

    server.resource["/request"]["POST"] = [](shared_ptr<HttpsServer::Response> response,
                                             shared_ptr<HttpsServer::Request> request)
    {
        json query = json::parse(request->content);
        if (query["action"] == "get")
        {
            if(query["content-type"] == "notes")
            {
                if (query["number"] == "all")
                {
                    response->write(dp.get_notes());
                }
            }
        } else if(query["action"] == "get-by-id") {
            if(query["content-type"] == "note")
            {
                response->write(dp.get_note(query["id"]));
            }
        } else if (query["action"] == "edit") {
            if(query["content-type"] == "note")
            {
                response->write(dp.edit_note(query["id"], query["new_note"],
                                query["token"]));
            }
        }
        else if(query["action"] == "add")
        {
            if(query["content-type"] == "note")
            {
                response->write(dp.add_note(query["note_text"], query["token"]));
            }
        }
        else if(query["action"] == "delete")
        {
            if(query["content-type"] == "note")
            {
                response->write(dp.delete_note(query["id"], query["token"]));
            }
        }
        else if(query["action"] == "find")
        {
            if(query["content-type"] == "notes")
            {
                response->write(dp.find_note(query["key"]));
            }
        }

    };                

    server.on_error = [](shared_ptr<HttpsServer::Request>, 
                         const SimpleWeb::error_code & ec) 
    {
        // Handle error
        cout << "Server error " << ec.value() << ": " << ec.category().name() << endl;
    };

    // Start server
    promise<unsigned short> server_port;
    thread server_thread([&server, &server_port]() {
        server.start([&server_port](unsigned short port) {
            server_port.set_value(port);
        });
    });

    server_thread.join();
}

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    run_server();
    return 0;
}
