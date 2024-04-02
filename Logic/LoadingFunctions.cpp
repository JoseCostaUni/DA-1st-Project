#include <memory>
#include "stdafx.h"
#include "LoadingFunctions.h"

std::unordered_set<DeliverySite> nodesToAdd;
std::vector<PumpingStations> edges;

/**
 * @brief Removes termination characters (e.g., '\r', '\n', '\0') from the given string.
 * @param str The string from which to remove termination characters.
 * @complexity O(n), where n is the length of the string.
 */
void Remove_terminations(std::string& str)
{
    size_t pos = str.find('\r');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    pos = str.find('\n');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    pos = str.find('\0');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
}

/**
 * @brief Loads cities data from a CSV file.
 * @complexity O(n*p), where n is the number of lines in the CSV file and p is the length of each line.
 */
void LoadCities() {

    std::ifstream file("LargeDataSet/Cities.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open the CSV file." << std::endl;
    }

    std::string line;
    getline(file, line);

    while (getline(file, line)) {

        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        std::istringstream lineStream(line);
        std::vector<std::string> tokens;
        std::string token;

        while (getline(lineStream, token, ',')) {
            tokens.push_back(token);
        }

        std::string name = tokens[0];
        std::string municipality;
        std::string code = tokens[2];
        int id = stoi(tokens[1]);

        int maxDelivery = 0;
        int demand = std::stoi(tokens[3]);

        Remove_terminations(tokens[4]);
        int population = std::stoi(tokens[4]);

        DeliverySite deliverySite(name, municipality, code, id, maxDelivery , demand , population , CITY);

        nodesToAdd.insert(deliverySite);
    }

    file.close();

}

/**
 * @brief Loads pipes data from a CSV file.
 * @complexity O(n*p), where n is the number of lines in the CSV file and p is the length of each line.
 */
void LoadPipes() {

    std::ifstream file("LargeDataSet/Pipes.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open the CSV file." << std::endl;
    }

    std::string line;
    getline(file, line);

    while (getline(file, line)) {

        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        std::istringstream lineStream(line);
        std::vector<std::string> tokens;
        std::string token;

        while (getline(lineStream, token, ',')) {
            tokens.push_back(token);
        }

        std::string servicePointA = tokens[0];
        std::string servicePointB = tokens[1];
        int capacity = stoi(tokens[2]);
        Remove_terminations(tokens[3]);
        bool direction = stoi(tokens[3]);

        PumpingStations pumpingStation(servicePointA, servicePointB, capacity,direction);

        edges.push_back(pumpingStation);
    }

    file.close();

}

/**
 * @brief Loads water reservoirs data from a CSV file.
 * @complexity O(n*p), where n is the number of lines in the CSV file and p is the length of each line.
 */
void LoadWaterReservoirs() {

    std::ifstream file("LargeDataSet/Reservoir.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open the CSV file." << std::endl;
    }

    std::string line;
    getline(file, line);

    while (getline(file, line)) {

        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        std::istringstream lineStream(line);
        std::vector<std::string> tokens;
        std::string token;

        while (getline(lineStream, token, ',')) {
            tokens.push_back(token);
        }

        Remove_terminations(tokens[4]);
        std::string name = tokens[0];
        std::string municipality = tokens[1];
        std::string code = tokens[3];
        int id = stoi(tokens[2]);

        int maxDelivery = stoi(tokens[4]);
        int demand = 0;
        int population = 0;
        //mandatory

        DeliverySite deliverySite(name, municipality, code, id, maxDelivery , demand , population , WATER_RESERVOIR);

        nodesToAdd.insert(deliverySite);
    }

    file.close();

    //
}

/**
 * @brief Loads fire stations data from a CSV file.
 * @complexity O(n*p), where n is the number of lines in the CSV file and p is the length of each line.
 */
void LoadFireStations()
{
    std::ifstream file("LargeDataSet/Stations.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open the CSV file." << std::endl;
    }

    std::string line;
    getline(file, line);

    while (getline(file, line)) {
        std::istringstream lineStream(line);
        std::vector<std::string> tokens;
        std::string token;

        while (getline(lineStream, token, ',')) {
            if(!token.empty())
                tokens.push_back(token);
        }

        if(!tokens.empty()){

            std::string name;
            std::string municipality;
            Remove_terminations(tokens[1]);
            std::string code = tokens[1];
            int id = stoi(tokens[0]);

            int maxDelivery = 0;
            int demand = 0;
            int population = 0;
            //mandatory

            DeliverySite deliverySite(name, municipality, code, id, maxDelivery , demand , population , FIRE_STATION);

            nodesToAdd.insert(deliverySite);
        }
    }

    file.close();
    //std::this_thread::sleep_for(std::chrono::seconds(5));
}

/**
 * @brief Creates a graph based on the loaded data.
 * @param g Pointer to the graph to be created.
 * @return True if the graph creation is successful, false otherwise.
 * @complexity O(m + n), where m is the number of edges (pipes) and n is the number of vertices (delivery sites).
 */
bool createGraph(Graph<DeliverySite>* g)
{

    for(const DeliverySite& deliverySite : nodesToAdd){
        if(!g->addVertex(deliverySite)){
            std::cerr << "Error adding vertex " << deliverySite.getCode();
            return false;
        }
    }

    auto a = edges.size();
    auto b = 0;
    for(const PumpingStations& pumpingStation : edges){
        DeliverySite deliverySiteA = DeliverySite((std::string) pumpingStation.getServicePointA());
        DeliverySite deliverySiteB = DeliverySite((std::string) pumpingStation.getServicePointB());

        if(!pumpingStation.getDirection()){
            if(!g->addBidirectionalEdge(deliverySiteA , deliverySiteB , pumpingStation.getCapacity())){
                std::cerr << "Error adding vertex";
                return false;
            }
            b++;
        }else{
            if(!g->addEdge(deliverySiteA , deliverySiteB , pumpingStation.getCapacity())){
                std::cerr << "Error adding edge";
                return false;
            }
        }

    }

    return true;
}