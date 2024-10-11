#include <map>
#include <string>
#include <iostream>

int main() {
  std::map<std::string, double> identifier_map;

  // Function to add or update the map
  auto addOrUpdate = [&identifier_map](const std::string& id, double value) {
    identifier_map[id] = value;
  };

  // Adding and updating elements
  addOrUpdate("user1", 1.0);
  addOrUpdate("user2", 2.0);
  addOrUpdate("user3", 3.0);

  // Updating an existing element
  addOrUpdate("user2", 4.0);
  identifier_map["user5"] = 5.0;

  // Displaying the elements in the map
  for (const auto& pair : identifier_map) {
    std::cout << "Identifier: " << pair.first << ", Value: " << pair.second << '\n';
  }

  return 0;
}
