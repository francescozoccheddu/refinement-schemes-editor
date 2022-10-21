#include <RSE/CppExporter.hpp>

#include <sstream>

namespace RSE
{

	std::string CppExporter::operator()(const std::vector<HexVerts>& _children) const
	{
		std::ostringstream ss{};
		if (constant)
		{
			ss << "const ";
		}
		ss << type << " " << name << " {{\n";
		for (const HexVerts& child : _children)
		{
			ss << "\t" << childType << "{ ";
			for (const RVec3 vert : child)
			{
				ss << vecType << "{" << vert.x() << ", " << vert.y() << ", " << vert.z() << "}, ";
			}
			ss << " },\n";
		}
		ss << "}};";
		return ss.str();
	}

}