#include "input_reader.h"
#include "transport_catalogue.h"

using namespace std;

int main() {
    TransportCatalogue transport_catalogue;
    ReadRequest(std::cin, transport_catalogue);
}