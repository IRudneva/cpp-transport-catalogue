#include "transport_catalogue.h"
#include <iostream>
#include <numeric>
#include <execution>

using namespace std::string_literals;

void TransportCatalogue::AddStop(std::string& stop, coordinates::Coordinates& coordinstes, std::unordered_map<std::string, uint32_t>& distance_next_stop) {
	if (stops_.find(stop) != stops_.end()) //проверяем на уникальность в stops_;
	{
		return;
	}
	all_stops_.push_back(stop);
	data_stop_.push_back(std::move(stop)); //добавление в data_stop_;
	std::string_view new_stop = data_stop_.back(); //получили sring_view для только что добавленрго значения в data_stop_
	Stop stop_inf; //формируем информацию об остановке
	stop_inf.title = new_stop;
	stop_inf.coordinates = std::move(coordinstes);
	stops_[new_stop] = std::move(stop_inf); //добавлем в базу для поиска
	SetDistance(stop_inf.title, distance_next_stop);
}

void TransportCatalogue::AddBus(std::string& bus, std::vector<std::string>& stops, char flag) {
	if (buses_.find(bus) != buses_.end()) //проверяем на уникальность в buses_;
	{
		return;
	}
	//проверяем тип маршрута
	if (flag == '>') // кольцевой
	{
		data_bus_.push_back(std::move(bus)); // добавляем в data_bus_;
		std::string_view new_bus = data_bus_.back(); //получили sring_view для только что добавленрго значения в data_bus_;
		Bus bus_inf; //формируем информацию о маршруте
		bus_inf.number = std::move(new_bus);
		bus_inf.route = std::move(CreateLoopRoute(stops)); // формируем маршрут
		bus_inf.uni_stops = std::move(GetUniqueStops(bus_inf.route)); //формируем уникальные остановки
		buses_[bus_inf.number] = std::move(bus_inf); // добавляем в базу
	}
	else if (flag == '-') // в две стороны
	{
		data_bus_.push_back(std::move(bus)); // добавляем в data_bus_;
		std::string_view new_bus = data_bus_.back(); //получили sring_view для только что добавленрго значения в data_bus_;
		Bus bus_inf; //формируем информацию о маршруте
		bus_inf.number = std::move(new_bus);
		bus_inf.route = std::move(CreateTwoWayRoute(stops)); // формируем маршрут
		bus_inf.uni_stops = std::move(GetUniqueStops(bus_inf.route)); //формируем уникальные остановки
		buses_[bus_inf.number] = std::move(bus_inf); // добавляем в базу
	}
	else
	{
		return;
	}
}

void TransportCatalogue::CreateCatalogue(std::vector<request::Stop>& stop, std::vector<request::Bus>& bus) {

	for (auto& title_stop : stop) {
		AddStop(title_stop.title, title_stop.coord, title_stop.distance_next_stop);
	}

	for (auto& title_bus : bus) {
		AddBus(title_bus.number, title_bus.stops, title_bus.flag);
	}

	SetBusesForAllStops();

	for (const auto& bus : data_bus_)
	{
		lenght_for_all_buses_[bus] = std::move(GetAllLenght(buses_.at(bus).route));
	}
}

information::Route TransportCatalogue::SearchBus(std::string& bus) {
	information::Route information_result;
	if (buses_.find(bus) == buses_.end())
	{
		information_result.bus_num = bus;
		information_result.total_stops = 0;
		return information_result;
	}

	information_result.bus_num = *(find(data_bus_.begin(), data_bus_.end(), bus));
	information_result.total_stops = buses_.at(bus).route.size();
	information_result.uni_stops = buses_.at(bus).uni_stops.size();
	information_result.all_lenght = lenght_for_all_buses_.at(bus); ////////////
	return information_result;
}

information::Stop TransportCatalogue::SearchStop(std::string& stop) {
	information::Stop information_result;
	if (stops_.find(stop) == stops_.end())
	{
		information_result.isfind = false;
		information_result.stop_title = stop;
		information_result.buses.clear();
		return information_result;
	}

	information_result.isfind = true;
	information_result.stop_title = *(find(data_stop_.begin(), data_stop_.end(), stop));
	information_result.buses = buses_for_stop_[information_result.stop_title];
	return information_result;
}

std::unordered_map<std::string_view, std::unordered_map<std::string_view, uint32_t>>& TransportCatalogue::GetNextStops() {
	return next_stops_for_stop_;
}

const std::optional<uint32_t> TransportCatalogue::GetLength(const std::string_view from, const std::string_view to) const {
	auto lenght = next_stops_for_stop_.at(from).find(to);
	if (lenght == next_stops_for_stop_.at(from).end())
	{
		lenght = next_stops_for_stop_.at(to).find(from);
		if (lenght == next_stops_for_stop_.at(to).end())
		{
			return std::nullopt;
		}
	}
	return lenght->second;
}

void TransportCatalogue::SetDistance(const std::string_view current_stop, std::unordered_map<std::string, uint32_t>& distance_next_stop) {
	for (const auto&[stop, dist] : distance_next_stop) {
		std::string_view next;
		auto it_find_stop = std::find(all_stops_.begin(), all_stops_.end(), stop);
		if (it_find_stop == all_stops_.end())
		{
			all_stops_.push_back(stop);
			next = all_stops_.back();
		}
		else
		{
			next = *it_find_stop;
		}

		next_stops_for_stop_[current_stop][next] = dist;
	}

	auto equal_route = std::find(all_stops_.begin(), all_stops_.end(), current_stop);
	if (distance_next_stop.empty() && equal_route != all_stops_.end())
	{
		next_stops_for_stop_[current_stop][*equal_route] = next_stops_for_stop_[*equal_route][current_stop];
	}
}

void TransportCatalogue::SetBusesForAllStops() {
	for (const auto& stop : data_stop_)
	{
		for (const auto& [bus, info] : buses_)
		{
			auto bus_for_stop = std::find(info.uni_stops.begin(), info.uni_stops.end(), stop);
			if (bus_for_stop != info.uni_stops.end())
			{
				buses_for_stop_[stop].insert(bus);

			}
		}

		if (buses_for_stop_[stop].empty())
		{
			buses_for_stop_[stop] = {};
		}
	}
}

std::vector<std::string_view> TransportCatalogue::CreateLoopRoute(std::vector<std::string>& route) const {
	std::vector<std::string_view> route_loop(route.end() - route.begin());
	transform(std::execution::par, route.begin(), route.end(), route_loop.begin(), [](std::string_view el) {return el; });
	return route_loop;
}

std::vector<std::string_view> TransportCatalogue::CreateTwoWayRoute(std::vector<std::string>& route) const {
	std::vector<std::string_view> route_two_way;
	route_two_way.reserve(route.end() - route.begin() + (route.end() - 1 - route.begin()));
	std::transform(route.begin(), route.end(), std::back_insert_iterator(route_two_way), [](std::string_view stop) {return stop; });
	std::vector<std::string_view> back_route_two_way(route.begin(), route.end() - 1);
	std::reverse(back_route_two_way.begin(), back_route_two_way.end());
	std::transform(back_route_two_way.begin(), back_route_two_way.end(), std::back_insert_iterator(route_two_way), [](std::string_view stop) {return std::move(stop); });
	return route_two_way;
}

std::vector<std::string_view> TransportCatalogue::GetUniqueStops(const std::vector<std::string_view>& stops) const {
	std::unordered_set<std::string_view> unique_stops_set(stops.begin(), stops.end());
	std::vector<std::string_view> unique_stops_vec(std::make_move_iterator(unique_stops_set.begin()), std::make_move_iterator(unique_stops_set.end()));
	return unique_stops_vec;
}

std::pair<double, double> TransportCatalogue::GetAllLenght(std::vector<std::string_view>& total_stops) const {
	double lenght = 0.0;
	double curvature = 0.0;
	for (auto stop1 = total_stops.begin(), stop2 = stop1 + 1; stop2 != total_stops.end(); ++stop1, ++stop2) {
		std::optional<uint32_t> lenght_ = GetLength(*stop1, *stop2);
		if (lenght_ != std::nullopt) {
			lenght += static_cast<double>(*lenght_);
		}
	}

	std::vector<coordinates::Coordinates> total_coord;
	total_coord.reserve(total_stops.size());

	for (const auto& stop : total_stops)
	{
		total_coord.push_back(stops_.at(stop).coordinates);
	}

	for (auto stop1 = total_coord.begin(), stop2 = stop1 + 1; stop2 != total_coord.end(); ++stop1, ++stop2) {

		curvature += coordinates::ComputeDistance(*stop1, *stop2);
	}
	curvature = static_cast<double>(lenght) / curvature;
	return { lenght, curvature };
}
