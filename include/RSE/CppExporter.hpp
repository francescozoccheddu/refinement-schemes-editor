#pragma once

#include <RSE/types.hpp>
#include <string>
#include <vector>

namespace RSE
{

	struct CppExporter final
	{

		bool constant{ true };
		std::string name{ "generatedRefinement" };
		std::string type{ "Refinement" };

		std::string operator()(const std::vector<HexVerts>& _children) const;

	};

}

