#pragma once
#include "geo.h"

#include <string>
#include <vector>

namespace transport_catalogue
{
	struct Stop
	{
		Stop() = default;
		Stop(const std::string_view, const double, const double);
		Stop(Stop*);

		std::string name;
		geo::Coordinates coordinates{ 0L,0L };
	};

	struct Route
	{
		Route() = default;
		Route(Route*);

		std::string route_name;
		std::vector<Stop*> stops;
		size_t unique_stops_qty = 0U;
		double geo_route_length = 0L;
		size_t meters_route_length = 0U;
		double curvature = 0L;
		bool is_circular = false;
	};

	class PairPointersHasher
	{
	public:
		std::size_t operator()(const std::pair<const Stop*, const Stop*>) const noexcept;

	private:
		std::hash<const void*> hasher_;
	};

	struct RendererData
	{
		std::vector<geo::Coordinates> stop_coords;
		std::vector<std::string_view> stop_names;
		bool is_circular = false;
	};

}