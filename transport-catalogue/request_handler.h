#pragma once

#include <unordered_set>    
#include <string_view>
#include <optional>             
#include <map>       

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

namespace detail
{
	std::pair<std::string_view, std::string_view> Split(std::string_view, char, int count = 1);
	std::string_view Lstrip(std::string_view);
	std::string_view Rstrip(std::string_view);
	std::string_view TrimString(std::string_view);
}

namespace transport_catalogue
{

	class RequestHandler {
	public:
		RequestHandler(const TransportCatalogue& transport_catalogue, map_renderer::MapRenderer& map_render) :
			transport_catalogue_(transport_catalogue)
			, map_render_(map_render) {
		}

		const std::optional<RouteStat*> GetRouteInfo(const std::string_view& bus_name) const;

		const std::optional<StopStat*> GetBusesForStop(const std::string_view& stop_name) const;

		svg::Document GetMapRender() const;

		std::optional<const router::RouteData> GetOptimalRoute(router::TransportRouter& tr, const std::string_view& from, const std::string_view& to) const;

	private:
		const TransportCatalogue& transport_catalogue_;
		map_renderer::MapRenderer& map_render_;
	};
}