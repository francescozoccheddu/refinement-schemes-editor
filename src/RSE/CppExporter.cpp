#include <RSE/CppExporter.hpp>

#include <sstream>
#include <utility>
#include <unordered_map>
#include <vector>

namespace RSE
{

	struct ExportData final
	{
		Int size{};
		std::vector<IVec3> verts{};
		std::vector<std::array<Int, 8>> polys{};
	};

	struct VertHasher final
	{

		size_t operator()(const IVec3& _vec) const
		{
			return _vec.x() * 100 + _vec.y() * 10 + _vec.z();
		}

	};

	ExportData prepare(Int _size, const std::vector<HexVertsU>& _children)
	{
		ExportData data{};
		data.size = _size;
		std::unordered_map<IVec3, Int, VertHasher> vertMap{};
		for (const HexVertsU& child : _children)
		{
			std::array<Int, 8> mappedChild;
			std::size_t i{};
			for (const IVec3& vert : child)
			{
				const auto it{ vertMap.find(vert) };
				if (it != vertMap.end())
				{
					mappedChild[i++] = it->second;
				}
				else
				{
					mappedChild[i++] = data.verts.size();
					vertMap.emplace_hint(it, vert, data.verts.size());
					data.verts.push_back(vert);
				}
			}
			data.polys.push_back(mappedChild);
		}
		return data;
	}

	std::string CppExporter::operator()(Int _size, const std::vector<HexVertsU>& _children) const
	{
		const ExportData data{ prepare(_size, _children) };
		std::ostringstream ss{};
		if (constant)
		{
			ss << "const ";
		}
		ss << type << " " << name << " {\n";
		ss << "\t" << _size << ",\n";
		ss << "\t" << vecListType << "{\n";
		for (const IVec3& vert : data.verts)
		{
			ss << "\t\t" << vecType << "{" << vert.x() << ", " << vert.y() << ", " << vert.z() << "},\n";
		}
		ss << "\t},\n";
		ss << "\t{\n";
		for (const std::array<Int, 8>&poly : data.polys)
		{
			ss << "\t\t" << indsType << "{";
			for (const std::size_t index : polyIndices)
			{
				ss << poly[index] << ", ";
			}
			ss << "},\n";
		}
		ss << "\t}\n";
		ss << "};";
		return ss.str();
	}

}