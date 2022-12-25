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
		std::string vecListType{ "std::vector<IVec>" };
		std::string type{ "Refinement" };
		std::array<std::size_t, 8> polyIndices{ 0,1,3,2,4,5,7,6 };

		std::string operator()(Int _size, const std::vector<HexVertsU>& _children) const;

	};

}

