#pragma once
#include "transport_catalogue.h"

using namespace std::string_literals;

namespace print {
	void BusInformation(information::Route& s);

	void StopInformation(information::Stop& s);
}