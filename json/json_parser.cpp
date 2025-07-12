#include "json_parser.hpp"
#include <cctype>
#include <stdexcept>

//                                                  ---- Lexer Implementation ----
Lexer::Lexer(const std::string& input) : input(input), position(0) {}

void Lexer::skipWhitespace() {
    while (position < input.length() && std::isspace(input[position])){
        ++position;
    }
}

Token Lexer::readString() {
    // Skip the opening quote
    position++;
    size_t start = position;
    while (position < input.length() && input[position] != '"'){
        // In this simple impl, we dont handle escaped quotes \"
        position++;
    }
    if (position >= input.length()){
        throw std::runtime_error("Unterminated string");
    }
    std::string result = input.substr(start, position-start);
    // Move beyond the closing quote
    position++;
    return {TokenType::STRING, result};
}

Token Lexer::readNumber() {
    size_t start = position;
    while (position < input.length() && (std::isdigit(input[position]) || input[position] == '.'
    || input[position] == '-' || input[position] == 'e' || input[position] == 'E' || input[position] == '+' )){
        ++position;
    }
    std::string result = input.substr(start, position-start);
    return {TokenType::NUMBER, result};
}

Token Lexer::readKeyword() {
    size_t start = position;
    while (position < input.length() && isalpha(input[position])){
        ++position;
    }
    std::string value = input.substr(start, position-start);
    if (value == "true" || value == "false"){
        return {TokenType::BOOLEAN, value};
    }
    if (value == "null"){
        return {TokenType::NULL_T, value};
    }
    return {TokenType::UNK, value};
}

Token Lexer::getNextToken() {
    // skip the initial whitespace
    skipWhitespace();

    if (position >= input.length()){
        return {TokenType::EOF_T, ""};
    }

    char currentChar = input[position];

    if (currentChar == '{'){ position++; return {TokenType::LBRACE, "{"};}
    if (currentChar == '}'){ position++; return {TokenType::RBRACE, "}"};}
    if (currentChar == '['){ position++; return {TokenType::LBRACKET, "["};}
    if (currentChar == ']'){ position++; return {TokenType::RBRACKET, "]"};}
    if (currentChar == ','){ position++; return {TokenType::COMMA, ","};}
    if (currentChar == ':'){ position++; return {TokenType::COLON, ":"};}

    if (currentChar == '"'){ return readString();}
    if (std::isdigit(currentChar) || currentChar == '-'){ return readNumber();}
    if (isalpha(currentChar)){ return readKeyword();}
    // std::string(count, char)
    return {TokenType::UNK, std::string(1, currentChar)};
}

//                                                  ---- Parser Implementation ----
Parser::Parser(Lexer& lexer) : lexer(lexer), currentToken(lexer.getNextToken()) {}

void Parser::expect(TokenType type) {
    if (currentToken.type == type){
        currentToken = lexer.getNextToken();
    } else {
        throw std::runtime_error("Unexpected token: expected one type, got another");
    }
}

JsonValue Parser::parse(){
    JsonValue result = parseValue();
    if (currentToken.type != TokenType::EOF_T){
        throw std::runtime_error("unexpected characters at the end of file");
    }
    return result;
}

JsonValue Parser::parseValue(){
    switch (currentToken.type){
        case TokenType::LBRACE:
            return parseObject();
        case TokenType::LBRACKET:
            return parseArray();
        case TokenType::STRING:
            return parseString();
        case TokenType::NUMBER:
            return parseNumber();
        case TokenType::BOOLEAN:
        case TokenType::NULL_T:
            return parseKeyword();
        default:
            throw std::runtime_error("unexpected token when parsing value");
    }
}

JsonValue Parser::parseObject(){
    expect(TokenType::LBRACE);
    JsonObject obj;

    if (currentToken.type == TokenType::RBRACE){
        expect(TokenType::RBRACE);
        return JsonValue(obj);
    }

    while (true) {
        if (currentToken.type != TokenType::STRING){
            throw std::runtime_error("expected string key");
        }
        std::string key = currentToken.value;
        expect(TokenType::STRING);
        expect(TokenType::COLON);
        obj[key] = parseValue();
        if (currentToken.type == TokenType::RBRACE){
            break;
        }
        expect(TokenType::COMMA);
    }
    expect(TokenType::RBRACE);
    return JsonValue(obj);
}

JsonValue Parser::parseArray(){
    expect(TokenType::LBRACKET);
    JsonArray arr;

    if (currentToken.type == TokenType::RBRACKET){
        expect(TokenType::RBRACKET);
        return JsonValue(arr);
    }

    while (true){
        arr.push_back(parseValue());
        if (currentToken.type == TokenType::RBRACKET){
            break;
        }
        expect(TokenType::COMMA);
    }
    expect(TokenType::RBRACKET);
    return JsonValue(arr);
}

JsonValue Parser::parseString(){
    std::string result = currentToken.value;
    expect(TokenType::STRING);
    return JsonValue(result);
}

JsonValue Parser::parseNumber(){
    // NOTE: std::stod converts string to double
    std::string result = currentToken.value;
    expect(TokenType::NUMBER);
    return JsonValue(std::stod(result));
}

JsonValue Parser::parseKeyword(){
    if (currentToken.type == TokenType::BOOLEAN){
        bool result = (currentToken.value == "true");
        expect(TokenType::BOOLEAN);
        return JsonValue(result);
    }
    if (currentToken.type == TokenType::NULL_T){
        expect(TokenType::NULL_T);
        return JsonValue(nullptr);
    }
    throw std::runtime_error("unexpected token when parsing keyword");
}
