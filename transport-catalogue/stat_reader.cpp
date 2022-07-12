#include "stat_reader.h"

#include <iostream>

using namespace std::string_literals;

void print::BusInformation(information::Route& route) {
	if (route.total_stops == 0) {
		std::cout << "Bus "s << std::string(route.bus_num) << ": not found"s << std::endl;
	}
	else {
		std::cout << "Bus "s << std::string(route.bus_num) << ": "s << route.total_stops
			<< " stops on route, "s << route.uni_stops << " unique stops, "s;
		std::cout << route.all_lenght.first << " route length, "s << route.all_lenght.second << " curvature"s << std::endl;
	}
}

void print::StopInformation(information::Stop& stop) {
	if (!stop.isfind)
	{
		std::cout << "Stop "s << std::string(stop.stop_title) << ": not found"s << std::endl;
	}
	else
	{
		if (stop.buses.size() == 0)
		{
			std::cout << "Stop "s << std::string(stop.stop_title) << ": no buses"s << std::endl;
		}
		else
		{
			std::cout << "Stop "s << std::string(stop.stop_title) << ": buses";
			for (const auto& bus : stop.buses)
			{
				std::cout << " "s << std::string(bus);
			}
			std::cout << std::endl;
		}
	}
}