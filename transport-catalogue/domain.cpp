#include "domain.h"

namespace transport_catalogue {

	Stop::Stop(Stop* stop_ptr) :
		name(stop_ptr->name),
		coordinates(stop_ptr->coordinates) {
	}

	Stop::Stop(const std::string_view stop_name, const double lat, const double lng) :
		name(stop_name),
		coordinates(geo::Coordinates{ lat,lng }) {
	}

	Route::Route(Route* route_ptr) :
		route_name(route_ptr->route_name),
		stops(route_ptr->stops),
		unique_stops_qty(route_ptr->unique_stops_qty),
		geo_route_length(route_ptr->geo_route_length),
		meters_route_length(route_ptr->meters_route_length),
		curvature(route_ptr->curvature),
		is_circular(route_ptr->is_circular) {
	}

	std::size_t PairPointersHasher::operator()(const std::pair<const Stop*, const Stop*> pair_of_pointers) const noexcept
	{
		auto ptr1 = static_cast<const void*>(pair_of_pointers.first);
		auto ptr2 = static_cast<const void*>(pair_of_pointers.second);
		return hasher_(ptr1) * 37 + hasher_(ptr2);
	}
}