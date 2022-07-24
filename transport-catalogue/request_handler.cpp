#include "request_handler.h"

namespace detail
{
	std::pair<std::string_view, std::string_view> Split(std::string_view line, char by, int count)
	{
		size_t pos = line.find(by);
		for (int i = 1; (i < count) && (pos != line.npos); ++i)
		{
			pos = line.find(by, pos + 1);
		}
		std::string_view left = line.substr(0, pos);
		if (pos < line.size() && pos + 1 < line.size())
		{
			return { left, line.substr(pos + 1) };
		}
		else
		{
			return { left, std::string_view() };
		}
	}

	std::string_view Lstrip(std::string_view line)
	{
		while (!line.empty() && std::isspace(line[0]))
		{
			line.remove_prefix(1);
		}
		return line;
	}

	std::string_view Rstrip(std::string_view line)
	{
		size_t line_size = line.size();
		while (!line.empty() && std::isspace(line[line_size - 1]))
		{
			line.remove_suffix(1);
			--line_size;
		}
		return line;
	}

	std::string_view TrimString(std::string_view line)
	{
		return Rstrip(Lstrip(line));
	}
}

namespace transport_catalogue
{

	const std::optional<RouteStat*> RequestHandler::GetRouteInfo(const std::string_view& bus_name) const
	{
		return transport_catalogue_.GetRouteInfo(bus_name);
	}

	const std::optional<StopStat*> RequestHandler::GetBusesForStop(const std::string_view& stop_name) const
	{
		return transport_catalogue_.GetBusesForStop(stop_name);
	}

	svg::Document RequestHandler::GetMapRender() const
	{
		std::map<const std::string, transport_catalogue::RendererData> all_routes;

		transport_catalogue_.GetAllRoutes(all_routes);

		return map_render_.RenderMap(all_routes);
	}
}