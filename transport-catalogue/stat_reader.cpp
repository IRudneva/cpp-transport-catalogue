#include "stat_reader.h"

#include <iostream>

using namespace std::string_literals;

void print::BusInformation(std::ostream& out, information::Route& route) {
	if (route.total_stops == 0) {
		out << "Bus "s << std::string(route.bus_num) << ": not found"s << std::endl;
	}
	else {
		out << "Bus "s << std::string(route.bus_num) << ": "s << route.total_stops
			<< " stops on route, "s << route.uni_stops << " unique stops, "s;
		out << route.all_lenght.first << " route length, "s << route.all_lenght.second << " curvature"s << std::endl;
	}
}

void print::StopInformation(std::ostream& out, information::Stop& stop) {
	if (!stop.isfind)
	{
		out << "Stop "s << std::string(stop.stop_title) << ": not found"s << std::endl;
	}
	else
	{
		if (stop.buses.size() == 0)
		{
			out << "Stop "s << std::string(stop.stop_title) << ": no buses"s << std::endl;
		}
		else
		{
			out << "Stop "s << std::string(stop.stop_title) << ": buses";
			for (const auto& bus : stop.buses)
			{
				out << " "s << std::string(bus);
			}
			out << std::endl;
		}
	}
}