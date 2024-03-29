#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include "domain.h"

namespace transport_catalogue {

// информация о маршруте
struct BusInfo {
    int stop_number;
    int unique_stop_number;
    int distance;
    double curvature;

    BusInfo();
};

struct HasherPair {
    size_t operator()(const std::pair<const domain::Stop*, const domain::Stop*>& p) const;
};

class TransportCatalogue {
public:
    TransportCatalogue() = default;

    // добавить остановку
    void AddStop(std::string_view name, geo_coord::Coordinates& coord);

    // добавить маршрут
    void AddBus(std::string_view name, std::string_view name_last_stop, bool is_roundtrip, const std::vector<std::string>& stops);

    // добавить информацию о маршруте
    void AddBusInfo(const domain::Bus* bus, const BusInfo& info);

    // поиск остановки по имени
    const domain::Stop* FindStop(std::string_view name) const;

    // поиск маршрута по имени
    const domain::Bus* FindBus(std::string_view name) const;

    // получение информации о маршруте
    const BusInfo GetBusInfo(const domain::Bus* bus);
    const BusInfo GetBusInfo(const std::string_view name);

    // получение информации о автобусах проходящих через остановку
    int GetBusesNumOnStop(const domain::Stop* stop) const;
    std::vector<const domain::Bus*> GetBusesOnStop(const domain::Stop* stop);

    // установить расстояние между остановок
    void SetDistance(const std::string& stop_from, const std::string& stop_to, int distance);

    // получить рассояние между остановок
    int GetDistance(const domain::Stop* stop_from, const domain::Stop* stop_to) const;

    // получить мапу для остановки: имя - указатель
    const std::unordered_map<std::string_view, const domain::Stop*>& GetStops() const;

    // получить мапу для маршрута: имя - указатель
    const std::unordered_map<std::string_view, const domain::Bus*>& GetBuses() const;

    // получить мапу для остановки: остановка - маршруты
    const std::unordered_map<const domain::Stop*, std::unordered_set<domain::Bus*>>& GetStopToBuses() const;

    // добавить расстояния для остановки
    void AddStopDistance(const std::string& stop_name, std::vector<std::pair<int, std::string>>& vct_distance);

    // получить мапу расстояний для остановок
    const std::unordered_map<std::string, std::vector<std::pair<int, std::string>>>& GetStopDistance();

private:
    // здесь живут все имена (сюда показывает string_view)
    std::deque<std::string> m_name_to_storage;

    // остановки
    std::deque<domain::Stop> m_stops;
    std::unordered_map<std::string_view, const domain::Stop*> m_name_to_stop;

    // маршруты
    std::deque<domain::Bus> m_buses;
    std::unordered_map<std::string_view, const domain::Bus*> m_name_to_bus;

    // расстояния между остановками
    std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, int, HasherPair> m_distance;

    // таблица для получения автобусов проходящих через остановку
    std::unordered_map<const domain::Stop*, std::unordered_set<domain::Bus*>> m_stop_to_bus;

    // таблица для получения информации о маршруте
    std::unordered_map<const domain::Bus*, BusInfo> m_bus_to_info;

    // таблица расстояний
    std::unordered_map<std::string, std::vector<std::pair<int, std::string>>> m_stop_to_dist;

    std::string_view GetName(std::string_view str);
};

} // namespace transport_catalogue