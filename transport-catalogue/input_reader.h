#pragma once
#include "geo.h"
#include "transport_catalogue.h"
#include "stat_reader.h"

#include <iostream>

using namespace std::string_literals;

namespace request {
	struct Bus
	{
		char flag;
		std::string number;
		std::vector<std::string> stops;
	};

	struct Stop
	{
		std::string title;
		coordinates::Coordinates coord;
		std::unordered_map<std::string, uint32_t> distance_next_stop;
	};
}

namespace upload_request {
	template<typename Requests>
	void Stop(std::vector<Requests>& requests, TransportCatalogue& catalogue);

	template<typename Requests>
	void Bus(std::vector<Requests>& requests, TransportCatalogue& catalogue);
}

namespace parsing {
	std::vector<std::string> RouteBus(std::string& request, size_t stop_title, char flag);

	request::Bus RequestBus(std::string& request);

	std::unordered_map<std::string, uint32_t> DistanseNextStopForStop(std::string& request);

	coordinates::Coordinates CoordinatesStop(size_t middle_coord, std::string::iterator coord_begin, std::string::iterator coord_end);

	request::Stop RequestStop(std::string& reques);
}

void ReadRequest(std::istream& input, TransportCatalogue& catalogue);

template<typename Requests>
void upload_request::Stop(std::vector<Requests>& requests, TransportCatalogue& catalogue) {
	for (auto& e : requests) {
		catalogue.AddStop(e.title, e.coord, e.distance_next_stop);
	}
}

template<typename Requests>
void upload_request::Bus(std::vector<Requests>& requests, TransportCatalogue& catalogue) {
	for (auto& e : requests) {
		catalogue.AddBus(e.number, e.stops, e.flag);
	}
}
