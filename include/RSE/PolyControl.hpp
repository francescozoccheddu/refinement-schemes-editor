#pragma once

#include <RSE/types.hpp>
#include <type_traits>
#include <limits>
#include <string_view>
#include <optional>

namespace RSE
{

	namespace internal
	{

		template<bool TInt>
		class PolyControl final
		{

		public:

			using Value = std::conditional_t<TInt, Int, Real>;
			using Vert = Vec3<Value>;
			using Verts = PolyVertData<Vert>;

		private:

			static constexpr std::string_view c_cbVertsPrefix{ TInt ? "rse_polycontrol_int_verts\n" : "rse_polycontrol_real_verts\n" };
			static constexpr std::string_view c_cbVertPrefix{ TInt ? "rse_polycontrol_int_vert\n" : "rse_polycontrol_real_vert\n" };

			PolyVertData<unsigned char> m_ids;
			Verts m_verts;
			bool m_valid;
			std::optional<std::size_t> m_activeVert;
			bool m_vertSelection;

			void update();

		public:

			static Verts cubeVerts(Value _min, Value _max);

			static Verts cubeVerts(Value _size = Value{ 1 });

			PolyControl(const Verts& _verts, bool _vertSelection = false);

			static void copyVert(const Vert& _vert);

			static std::optional<Vert> pasteVert();

			static void copyVerts(const Verts& _verts);

			static std::optional<Verts> pasteVerts();

			static std::optional<PolyControl> paste();

			void copy() const;

			void setVerts(const Verts& _verts);

			void setActiveVert(std::optional<std::size_t> _index);

			void setVertSelection(bool _enabled);

			bool vertSelection() const;

			std::optional<std::size_t> activeVert() const;

			const Verts& verts() const;

			PolyVertData<std::size_t> firstOccurrenceIndices() const;

			bool valid() const;

			bool draw(Value _min, Value _max, std::optional<Verts>& _copiedVerts, std::optional<Vert>& _copiedVert);

			bool draw(Value _min, Value _max, const std::optional<Verts>& _copiedVerts, const std::optional<Vert>& _copiedVert);

			bool draw(Value _min = Value{ 0 }, Value _max = std::numeric_limits<Value>::max());

		};

	}

	using IPolyControl = internal::PolyControl<true>;

	using RPolyControl = internal::PolyControl<false>;

}

#define RSE_POLYCONTROL_TPP
#include <RSE/PolyControl.tpp>
#undef RSE_POLYCONTROL_TPP