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
		std::string indsType{ "PolyVertIds" };
		std::string vecType{ "IVec" };
		std::string type{ "Refinement" };

		std::string operator()(Int _size, const std::vector<HexVertsU>& _children) const;

	};

}

