/*************************************************************************
    > File Name: Json.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-01-23 13:53
************************************************************************/
#include <iostream>
#include <variant>
#include <vector>
#include <map>
using namespace std;

struct JsonValue;
using JsonArray = vector<JsonValue>;
using JsonObject = map<string, JsonValue>;
struct JsonValue:
    variant<bool,
            long,
            double,
            std::string,
            nullptr_t,
            JsonArray,
            JsonObject> {
    using variant::variant;
};

int main(int argc, char** argv) {
    JsonValue value = JsonObject {
        { "widget", JsonObject {
            {"debug", false},
            {"window", JsonObject {
                {"title", "Sample Widget"},
                {"name", "main window"},
                {"width", 500},
                {"height", 500} }
            },
            {"image", JsonObject {
                {"src", "image/sun.png"},
                {"name", "sum"},
                {"offset", JsonArray{250, 250}},
                {"alignment", "center"} }
            }},
        }
    };
    return 0;
}
