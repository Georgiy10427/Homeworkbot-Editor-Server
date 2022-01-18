#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <iostream>
#include <string>
#include <random>
#include <dispatcher.hpp>

using namespace std;

string generate_token(int minLenght, int maxLenght)
{
    random_device dev;
    mt19937 rng(dev());

    uniform_int_distribution<mt19937::result_type> number_dist(minLenght, maxLenght);
    uniform_int_distribution<mt19937::result_type> char_dist(97, 122);
    uniform_int_distribution<mt19937::result_type> type_dist(0, 1);

    int token_lenght = number_dist(rng);
    string token;

    for(int i = 0; i < token_lenght; i++)
    {
        if(type_dist(rng) == 0)
        {
            token = token + to_string(number_dist(rng));
        } else {
            char c = char_dist(rng);
            token += c;
        }
    }
    return token;
}

void check_tokens(Dispatcher dp)
{
    auto tokens = dp.get_storage().get_all<Token>();
    if(tokens.size() == 0)
    {
        Token t;
        t.body = generate_token(13, 15);
        dp.get_storage().insert(t);
    }
}

#endif // TOKEN_HPP
