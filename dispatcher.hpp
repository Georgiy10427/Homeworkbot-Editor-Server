#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <iostream>
#include <string>
#include "libs/json.hpp"
#include "libs/sqlite_orm.h"
#include <boost/locale.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <map>

using namespace std;
using json = nlohmann::json;
using namespace sqlite_orm;

struct Note
{
    int id;
    std::string body;
};

struct Token
{
    std::string body;
};

string replace_str(string str, string substr, string match)
{
    size_t index = 0;
    while (true) {
         index = str.find(substr, index);
         if (index == std::string::npos) break;

         str.replace(index, match.length(), match);
         index += substr.length();
    }
    return str;
}

class Dispatcher
{
public:
    auto get_storage()
    {
        auto storage = make_storage("database.sqlite",
                                    make_table("posts",
                                               make_column("id", &Note::id, autoincrement(), primary_key()),
                                               make_column("body", &Note::body)),
                                    make_table("admin_tokens",
                                               make_column("body", &Token::body)));
        return storage;
    }

    string get_notes()
    {
        auto allNotes = get_storage().get_all<Note>();
        json response;
        response["action"] = "get";
        response["content-type"] = "posts";

        map <int, string, greater<int>> notes;
        notes.insert(notes.end(), notes.begin());
        for(auto &note : allNotes) {
            notes.insert(pair<int, string>(note.id, note.body));
        }

        response["notes"] = notes;

        return response.dump();
    }

    string get_note(int id)
    {
        json response;
        response["action"] = "get";
        response["content-type"] = "note";
        if(auto note = get_storage().get_no_throw<Note>(id)){
            response["note"] = note->body;
            response["message"] = "ok";
        } else {
            response["message"] = "not found";
        }
        return response.dump();
    }

    bool is_valid_token(string t)
    {
        auto allTokens = get_storage().get_all<Token>();
        for(auto &token : allTokens)
        {
            if (token.body == t)
            {
                return true;
            }
        }
        return false;
    }

    string edit_note(int id, string new_note, string token)
    {
        json response;
        if (!is_valid_token(token)){
            response["message"] = "access denied";
            return response.dump();
        }

        if(auto note = get_storage().get_no_throw<Note>(id)){
            Note updated_note;
            updated_note.id = note->id;
            updated_note.body = new_note;
            get_storage().update(updated_note);
            response["message"] = "ok";
        } else {
            response["message"] = "not found";
        }

        return response.dump();
    }

    string add_note(string note_text, string token)
    {
        json response;
        if (!is_valid_token(token)){
            response["message"] = "access denied";
            return response.dump();
        }
        Note note;
        note.body = note_text;
        note.id = -1;
        auto id = get_storage().insert(note);

        response["id"] = id;
        response["message"] = "ok";
        return response.dump();
    }

    string delete_note(int id, string token)
    {
        json response;
        if(!is_valid_token(token))
        {
            response["message"] = "access denied";
            return response.dump();
        }
        get_storage().remove<Note>(id);
        response["message"] = "ok";
        return response.dump();
    }

    string find_note(string key)
    {
        // Need to refactoring
        // Lower conversition not works
        key = boost::algorithm::to_lower_copy(key);
        auto allNotes = get_storage().get_all<Note>();
        map <int, string, greater<int>> foundNotes;

        for(auto &note : allNotes)
        {
            string note_body = boost::algorithm::to_lower_copy(note.body);
            size_t pos = note_body.find(key);
            if (pos != string::npos)
            {
                foundNotes.insert(pair<int, string>(note.id, note.body));
            }
        }

        json response;
        response["notes"] = foundNotes;
        return response.dump();
    }
};

#endif // DISPATCHER_HPP
