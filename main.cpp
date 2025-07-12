#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include "haversine_formula.cpp"
#include "json/json_parser.hpp"

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()){
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int ArgCount, char **Args) {
    if (ArgCount != 2){
        std::cerr << "Usage: " << Args[0] << " <json_file>" << std::endl;
        return 1;
    }

    std::string jsonFile = Args[1];
    try {
        // 1. Read the JSON file
        std::string jsonData = readFile(jsonFile);
        std::cout << "---Parsing JSON DATA---" << std::endl;
        // 2. Lexer (Tokenizes the JSON data) 
        Lexer lexer(jsonData);
        // 3. Parser (Parses the JSON data)
        Parser parser(lexer);
        // 4. Parse the JSON data
        JsonValue parsedJSON = parser.parse();
        std::cout << "---JSON Parsed Successfully---" << std::endl;

        if (parsedJSON.isObject()){
            JsonObject root = parsedJSON.asObject();
            if (root.count("pairs") && root.at("pairs").isArray()){
                JsonArray pairs = root.at("pairs").asArray();
                std::cout << "Found " << pairs.size() << " pairs" << std::endl;
                double Sum = 0;
                double sumCoef = 1.0/(double)pairs.size();
                for (size_t i = 0; i < pairs.size(); ++i) {
                    JsonValue &pairValue = pairs[i];

                    if (pairValue.isObject()){
                        JsonObject pairObj = pairValue.asObject();

                        double x0 = pairObj.at("x0").asNumber();
                        double y0 = pairObj.at("y0").asNumber();
                        double x1 = pairObj.at("x1").asNumber();
                        double y1 = pairObj.at("y1").asNumber();

                        // 5. Compute Haversine distance
                        double EarthRadius = 6371.8;
                        double HaversineDistance = ReferenceHaversine(x0, y0, x1, y1, EarthRadius);

                        Sum += sumCoef * HaversineDistance;
                    }
                }
                std::cout << "Sum of Haversine distances: " << Sum << std::endl;
            }
        }
    } catch (const std::exception &err){
        std::cerr << "Error: " << err.what() << std::endl;
    }

    return 0;
}