#include <json/json.h>
#include <iostream>
#include <fstream>
#include <streambuf>

std::string getJSON(const std::string& filename)
{
    std::ifstream file(filename);

    std::string str;
    file.seekg(0, std::ios::end);
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

    return str;

}

int main()
{
    std::string evictions = getJSON("eviction-notices.json");

    Json::Value root;
    Json::Reader reader;

    reader.parse(evictions, root);

    Json::Value data = root.get("data", "error");
    Json::Value first = data[0];

    std::vector<std::vector<Json::Value>> houses;
    for (unsigned int i = 0; i < data.size(); i++)
    {
        houses.push_back(std::vector<Json::Value>());
        for (auto it = data[i].begin(); it != data[i].end(); ++it)
        {
            houses[i].push_back(*it);
        }
    }

    std::cout << houses[4][9] << std::endl;
}