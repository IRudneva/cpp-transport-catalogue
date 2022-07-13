#pragma once
#include "transport_catalogue.h"
#include "stat_reader.h"

#include <iostream>

using namespace std::string_literals;

namespace parsing {
	std::vector<std::string> RouteBus(std::string& request, size_t stop_title, char flag);

	request::Bus RequestBus(std::string& request);

	std::unordered_map<std::string, uint32_t> DistanseNextStopForStop(std::string& request);

	coordinates::Coordinates CoordinatesStop(size_t middle_coord, std::string::iterator coord_begin, std::string::iterator coord_end);

	request::Stop RequestStop(std::string& reques);
}

void ReadRequest(std::istream& input, TransportCatalogue& catalogue);
