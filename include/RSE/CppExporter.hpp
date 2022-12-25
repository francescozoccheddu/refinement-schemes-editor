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
		std::string indsType{ "PolyVertData<I>" };
		std::string indsListType{ "std::vector<PolyVertData<I>>" };
		std::string vertType{ "IVec" };
		std::string vertListType{ "std::vector<IVec>" };
		std::string type{ "Refinement" };
		std::array<std::size_t, 8> polyIndices{ 0,2,3,1,4,6,7,5 };

		std::string operator()(Int _size, const std::vector<HexVertsU>& _children) const;

	};

}

