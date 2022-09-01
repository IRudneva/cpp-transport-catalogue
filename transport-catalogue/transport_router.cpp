#include "transport_router.h"

namespace router
{
	TransportRouter::TransportRouter(transport_catalogue::TransportCatalogue& tc)
		: tc_(tc), dw_graph_(tc.GetAllStopsCount() * 2) {
	}

	void TransportRouter::ApplyRouterSettings(RouterSettings& settings)
	{
		settings_ = std::move(settings);
	}

	const RouterSettings& TransportRouter::GetRouterSettings() const
	{
		return settings_;
	}

	std::optional<const RouteData> TransportRouter::CalculateRoute(const std::string_view from, const std::string_view to)
	{
		if (!router_)
		{
			BuildGraph();
		}

		const auto stop_from = vertexes_wait_.at(from);
		const auto stop_to = vertexes_wait_.at(to);
		auto calculated_route = router_->BuildRoute(stop_from, stop_to);
		if (!calculated_route)
		{
			return std::nullopt;
		}

		return RouteData{ BuildOptimalRoute(calculated_route.value()) };
	}

	const RouteData TransportRouter::BuildOptimalRoute(const graph::Router<double>::RouteInfo& router_inf) const {
		RouteData result;

		for (const auto& element_id : router_inf.edges)
		{
			auto edge_details = dw_graph_.GetEdge(element_id);
			result.total_time += edge_details.weight;
			result.items.emplace_back(RouteItem{
				edge_details.edge_name,
				(edge_details.type == graph::EdgeType::TRAVEL) ? edge_details.span_count : 0,
				edge_details.weight,
				edge_details.type });
		}
		return result;
	}

	void TransportRouter::BuildGraph()
	{
		FillGraphStops(tc_.GetAllStopsPtr());

		FillGraphRoutes(tc_.GetAllRoutesPtr());

		router_ = std::make_unique<graph::Router<double>>(dw_graph_);
	}

	void TransportRouter::FillGraphStops(const std::vector<const transport_catalogue::Stop*> all_stops) {
		int vertex_id = 0;
		for (const auto& stop : all_stops)
		{
			vertexes_wait_.insert({ stop->name, vertex_id });
			vertexes_travel_.insert({ stop->name, ++vertex_id });
			dw_graph_.AddEdge({
					vertexes_wait_.at(stop->name),
					vertexes_travel_.at(stop->name),
					settings_.bus_wait_time * 1.0,
					stop->name,
					graph::EdgeType::WAIT,
					0
				});
			++vertex_id;
		}
	}

	void TransportRouter::FillGraphRoutes(const std::deque<const transport_catalogue::Route*> all_routes) {
		for (const auto& route : all_routes)
		{
			for (size_t it_from = 0; it_from < route->stops.size() - 1; ++it_from)
			{
				int span_count = 0;
				for (size_t it_to = it_from + 1; it_to < route->stops.size(); ++it_to)
				{
					double road_distance = 0.0;
					for (size_t it = it_from + 1; it <= it_to; ++it)
					{
						road_distance += static_cast<double>(tc_.GetDistance(route->stops[it - 1], route->stops[it]));
					}
					dw_graph_.AddEdge({
							vertexes_travel_.at(route->stops[it_from]->name),
							vertexes_wait_.at(route->stops[it_to]->name),
							road_distance / (settings_.bus_velocity * 1000.0 / 60.0),
							route->route_name,
							graph::EdgeType::TRAVEL,
							++span_count
						});
				}
			}
		}
	}
} //namespace router