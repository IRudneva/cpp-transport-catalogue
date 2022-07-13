#pragma once
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <deque>
#include <string_view>
#include <set>
#include <optional>

#include "geo.h"

namespace information {
	struct Route
	{
		std::string_view bus_num;
		size_t total_stops = 0;
		size_t uni_stops = 0;
		std::pair<double, double> all_lenght;
	};

	struct Stop
	{
		bool isfind = false;
		std::string_view stop_title;
		std::set<std::string_view> buses;
	};
}

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

class TransportCatalogue
{
public:
	
	TransportCatalogue() = default;

	void AddStop(std::string& stop, coordinates::Coordinates& coordinstes, std::unordered_map<std::string, uint32_t>& distance_next_stop);

	void AddBus(std::string& bus, std::vector<std::string>& stops, char flag);

	void CreateCatalogue(std::vector<request::Stop>& stop, std::vector<request::Bus>& bus);

	information::Route SearchBus(std::string& bus);

	information::Stop SearchStop(std::string& stop);

	std::unordered_map<std::string_view, std::unordered_map<std::string_view, uint32_t>>& GetNextStops();

	const std::optional<uint32_t> GetLength(const std::string_view from, const std::string_view to) const;

private:

	struct Bus
	{
		std::string_view number;
		std::vector<std::string_view> route;
		std::vector<std::string_view> uni_stops;
	};

	struct Stop
	{
		std::string_view title;
		coordinates::Coordinates coordinates;
	};

	std::unordered_map<std::string_view, Bus> buses_;
	std::unordered_map<std::string_view, Stop> stops_;
	std::unordered_map<std::string_view, std::set<std::string_view>> buses_for_stop_;
	std::unordered_map<std::string_view, std::unordered_map<std::string_view, uint32_t>> next_stops_for_stop_;
	std::unordered_map < std::string_view, std::pair<double, double>> lenght_for_all_buses_;
	//храним string
	std::deque<std::string> data_bus_;
	std::deque<std::string> data_stop_;
	std::deque<std::string> all_stops_;

	void SetDistance(const std::string_view current_stop, std::unordered_map<std::string, uint32_t>& distance_next_stop);

	void SetBusesForAllStops();

	std::vector<std::string_view> CreateLoopRoute(std::vector<std::string>& route) const;

	std::vector<std::string_view> CreateTwoWayRoute(std::vector<std::string>& route) const;

	std::vector<std::string_view> GetUniqueStops(const std::vector<std::string_view>& stops) const;

	std::pair<double, double> GetAllLenght(std::vector<std::string_view>& total_stops) const;
};
