#pragma once

#include <memory>

#include "domain.h"    
#include "transport_catalogue.h"
#include "router.h"

namespace router
{
	struct RouterSettings
	{
		int bus_velocity = 0;
		int bus_wait_time = 0;
	};

	struct RouteItem
	{
		std::string edge_name;
		int span_count = 0;
		double time = 0.0;
		graph::EdgeType type;
	};

	struct RouteData
	{
		double total_time = 0.0;
		std::vector<RouteItem> items;
	};


	class TransportRouter
	{
	public:
		TransportRouter(transport_catalogue::TransportCatalogue&);

		void ApplyRouterSettings(RouterSettings&);
		const RouterSettings& GetRouterSettings() const;
		std::optional<const RouteData> CalculateRoute(const std::string_view, const std::string_view);

	private:
	
		const RouteData BuildOptimalRoute(const graph::Router<double>::RouteInfo&) const;

		void BuildGraph();

		void FillGraphStops(const std::vector<const transport_catalogue::Stop*>);

		void FillGraphRoutes(const std::deque<const transport_catalogue::Route*>);

		RouterSettings settings_;
		transport_catalogue::TransportCatalogue& tc_;

		graph::DirectedWeightedGraph<double> dw_graph_;
		std::unique_ptr<graph::Router<double>> router_ = nullptr;
		std::unordered_map<std::string_view, size_t> vertexes_wait_;
		std::unordered_map<std::string_view, size_t> vertexes_travel_;

	};
}