//     __ _____ _____ _____
//  __|  |   __|     |   | |  Simple JSON
// |  |  |__   |  |  | | | |  version 1.0.0
// |_____|_____|_____|_|___|
// Copyright (c) 2025, Muhammad Ahmed
#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <stdexcept>

enum class TokenType {
    LBRACE,   // {
    RBRACE,   // }
    LBRACKET, // [
    RBRACKET, // ]
    COMMA,    // ,
    COLON,    // :
    STRING,   // "..."
    NUMBER,   // 123.456
    BOOLEAN,  // true or false
    NULL_T,   // null
    EOF_T,    // end of file 
    UNK       // for errors
};

/**
 * @brief A struct that represents a token in the JSON stream
 * It has a type and a value
 */
struct Token {
    TokenType type;
    std::string value;
};

//                                                  ---- Lexer Class ----
class Lexer {
    private:
        const std::string& input;
        size_t position;
        
        void skipWhitespace();
        Token readString();
        Token readNumber();
        Token readKeyword();
    public:
        /**
        * @brief Constructor for the Lexer
        * @param input The input string to be parsed
        */
        Lexer(const std::string& input);

        /**
        * @brief Get the next token from the input stream
        */
        Token getNextToken();
};

//                                                  ---- JSON Value Class ----
class JsonValue;

// Custom Value Types
using JsonObject = std::map<std::string, JsonValue>;
using JsonArray  = std::vector<JsonValue>;
using Value = std::variant<std::nullptr_t, bool, double, std::string, JsonObject, JsonArray>;

/**
 * @brief A class that represents a JSON value
 * Value can be a string, number, boolean, null, object, or array
 */
class JsonValue {
    private:
        Value value;
    public:
        JsonValue(Value val = nullptr): value(std::move(val)) {}

        // Convenience methods to check type 
        bool isObject() const { return std::holds_alternative<JsonObject>(value);}
        bool isArray() const { return std::holds_alternative<JsonArray>(value);}
        bool isString() const { return std::holds_alternative<std::string>(value);}
        bool isNumber() const { return std::holds_alternative<double>(value);}
        bool isBool() const { return std::holds_alternative<bool>(value);}
        bool isNull() const { return std::holds_alternative<std::nullptr_t>(value);}

        // Convenience methods to get the value
        // Throws an exception if the type is not the expected one 
        // TODO: Error should be presented in a better way like giving a line number and column number
        const JsonObject& asObject() const { return std::get<JsonObject>(value);}
        const JsonArray& asArray() const { return std::get<JsonArray>(value);}
        const std::string& asString() const { return std::get<std::string>(value);}
        double asNumber() const { return std::get<double>(value);}
        bool asBool() const { return std::get<bool>(value);}
};

//                                                  ---- Parser Class ----
class Parser {
    private:
        // Helper methods to parse specific types (RECURSIVE DESCENT PARSER)
        JsonValue parseValue();
        JsonValue parseObject();
        JsonValue parseArray();
        JsonValue parseString();
        JsonValue parseNumber();
        // Keywords are true, false, null
        JsonValue parseKeyword();

        // Helper method to check and consume a token
        void expect(TokenType type);

        Lexer& lexer;
        Token currentToken;
    
    public:
        /**
         * @brief Constructor for the Parser
         * @param lexer The lexer to use for tokenization
         */
        Parser(Lexer& lexer);

        /**
         * @brief Main entry point to parse the JSON input
         * @return A JsonValue object representing the parsed JSON
         */
        JsonValue parse();
};

#endif // JSON_PARSER_HPP

