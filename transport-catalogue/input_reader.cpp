#include "input_reader.h"
#include "geo.h"
#include "transport_catalogue.h"
#include "stat_reader.h"

#include <algorithm>
using namespace std::string_literals;

std::vector<std::string> parsing::RouteBus(std::string& request, size_t stop_title, char flag) {
	std::vector<std::string> route_bus;
	while (!request.empty())
	{
		if (request.find(" > "s) != request.npos || request.find(" - "s) != request.npos)
		{
			if (flag == '>')
			{
				stop_title = request.find(" > "s);
				std::string stop_name(request.begin(), request.begin() + stop_title);
				route_bus.push_back(std::move(stop_name));
				request.erase(request.begin(), request.begin() + stop_title + 3);
			}
			else if (flag == '-')
			{
				stop_title = request.find(" - "s);
				std::string stop_name(request.begin(), request.begin() + stop_title);
				route_bus.push_back(std::move(stop_name));
				request.erase(request.begin(), request.begin() + stop_title + 3);
			}
		}
		else
		{
			std::string stop_name(request.begin(), request.end());
			route_bus.push_back(std::move(stop_name));
			request.erase(request.begin(), request.end());
		}
	}
	return route_bus;
}

request::Bus parsing::RequestBus(std::string& request) {
	request::Bus bus_request;
	request.erase(remove_if(request.begin(), request.begin() + request.find_first_not_of(' '), [](char& c) {return c == ' '; })); //удаляем пробелы в начале строки
	auto bus_title_end = request.find(": "s); // конец названия автобуса

	std::string name(std::make_move_iterator(request.begin()), std::make_move_iterator(request.begin() + bus_title_end)); //выделяем название
	bus_request.number = std::move(name);

	request.erase(request.begin(), request.begin() + bus_title_end + 2); // удаляем название из запроса
	auto stop_title_end = request.find(" > "s); // конец названия остановки

	// определяем тип маршрута по флагу
	if (stop_title_end != request.npos)
	{
		bus_request.flag = '>';
	}
	else
	{
		stop_title_end = request.find(" - "s);
		bus_request.flag = '-';
	}

	bus_request.stops = std::move(parsing::RouteBus(request, stop_title_end, bus_request.flag)); // разбиваем маршрут по остановкам и добавляем в запрос

	return bus_request;
}

std::unordered_map<std::string, uint32_t> parsing::DistanseNextStopForStop(std::string& request) {
	std::unordered_map<std::string, uint32_t> result_distance_next_stop;
	result_distance_next_stop.reserve(100);
	while (!request.empty())
	{
		auto chapter_distance_stop = request.find("m to "s);
		uint32_t distance = 0;
		if (chapter_distance_stop != request.npos) {
			std::string str_distance(std::make_move_iterator(request.begin()), std::make_move_iterator(request.begin() + chapter_distance_stop));
			distance = std::move((atoi(str_distance.c_str())));
			request.erase(request.begin(), request.begin() + chapter_distance_stop + 5);
		}
		auto stop_title = request.find(", "s);

		if (stop_title != request.npos)
		{
			std::string title(std::make_move_iterator(request.begin()), std::make_move_iterator(request.begin() + stop_title));
			result_distance_next_stop[std::move(title)] = distance;
			request.erase(request.begin(), request.begin() + stop_title + 2);
		}
		else
		{
			std::string title(std::make_move_iterator(request.begin()), std::make_move_iterator(request.end()));
			result_distance_next_stop[std::move(title)] = distance;
			request.erase(request.begin(), request.end());
		}
	}
	return result_distance_next_stop;
}

coordinates::Coordinates parsing::CoordinatesStop(size_t middle_coord, std::string::iterator coord_begin, std::string::iterator coord_end) {
	coordinates::Coordinates stop_coordinates;
	std::string lat(std::make_move_iterator(coord_begin), std::make_move_iterator(coord_begin + middle_coord)); //широта
	stop_coordinates.lat = std::move(atof(lat.c_str()));

	std::string lng(std::make_move_iterator(coord_begin + middle_coord + 2), std::make_move_iterator(coord_end)); //долгота
	stop_coordinates.lng = std::move(atof(lng.c_str()));
	return stop_coordinates;
}

request::Stop parsing::RequestStop(std::string& request) {
	request::Stop stop_request;
	request.erase(remove_if(request.begin(), request.begin() + request.find_first_not_of(' '), [](char& c) {return c == ' '; })); //удаляем пробелы в начале строки
	auto stop_title_end = request.find(": "s); // конец названия остановки 
	std::string name(std::make_move_iterator(request.begin()), std::make_move_iterator(request.begin() + stop_title_end)); // выделяем название
	stop_request.title = std::move(name);
	request.erase(request.begin(), request.begin() + stop_title_end + 2); // удаляем название из запроса
	auto coord_med = request.find(','); // раздел координат 
	auto coord_end = std::find(request.begin() + coord_med + 1, request.end(), ','); // конец координат
	stop_request.coord = std::move(parsing::CoordinatesStop(coord_med, request.begin(), coord_end));

	if (coord_end != request.end()) {
		request.erase(request.begin(), coord_end + 2);
		stop_request.distance_next_stop = std::move(parsing::DistanseNextStopForStop(request));
	}
	else
	{
		stop_request.distance_next_stop = {};
	}
	return stop_request;
}

void ReadRequest(std::istream& input, TransportCatalogue& catalogue) {
	int count_request_base;
	input >> count_request_base;

	std::vector<request::Stop> request_base_stop;
	std::vector<request::Bus> request_base_bus;
	std::string request;
	std::string type_request_base;

	while (count_request_base != 0) {
		input >> type_request_base;
		std::getline(input, request);

		if (type_request_base == "Bus"s) {

			request_base_bus.push_back(std::move(parsing::RequestBus(request)));
		}
		else {
			request_base_stop.push_back(std::move(parsing::RequestStop(request)));
		}
		count_request_base--;
	}
	//формируем базу маршрутов и остановок
	upload_request::Bus(request_base_bus, catalogue);
	upload_request::Stop(request_base_stop, catalogue);

	//запросы на поиск
	int count_request_bus_information;
	input >> count_request_bus_information;
	std::string request_for_search;
	std::string type_request_search;

	while (count_request_bus_information != 0) {
		input >> type_request_search;
		std::getline(input, request_for_search);
		request_for_search.erase(remove_if(request_for_search.begin(), request_for_search.begin() + request_for_search.find_first_not_of(' '), [](char& c) {return c == ' '; })); //удаляем пробелы в начале строки
		if (type_request_search == "Bus"s)
		{
			auto inf = catalogue.SearchBus(request_for_search);
			print::BusInformation(inf);
			count_request_bus_information--;
		}
		else
		{
			auto inf = catalogue.SearchStop(request_for_search);
			print::StopInformation(inf);
			count_request_bus_information--;
		}
	}
}
