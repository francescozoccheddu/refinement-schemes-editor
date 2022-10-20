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
		class HexControl final
		{

		public:

			using Value = std::conditional_t<TInt, Int, Real>;
			using Vert = Vec3<Value>;
			using Verts = HexVertData<Vert>;

		private:

			static constexpr std::string_view c_cbVertsPrefix{ TInt ? "rse_HexControl_int_verts\n" : "rse_HexControl_real_verts\n" };
			static constexpr std::string_view c_cbVertPrefix{ TInt ? "rse_HexControl_int_vert\n" : "rse_HexControl_real_vert\n" };

			HexVertData<unsigned char> m_ids;
			Verts m_verts;
			bool m_valid;
			std::size_t m_activeVert;

			void update();

		public:

			static Verts cubeVerts(Value _min, Value _max);

			static Verts cubeVerts(Value _size = Value{ 1 });

			HexControl(const Verts& _verts, bool _vertSelection = false);

			static void copyVert(const Vert& _vert);

			static std::optional<Vert> pasteVert();

			static void copyVerts(const Verts& _verts);

			static std::optional<Verts> pasteVerts();

			static std::optional<HexControl> paste();

			void copy() const;

			void setVerts(const Verts& _verts);

			void setActiveVert(std::size_t _index);

			void setActiveVert(const Vert& _vert);

			std::size_t activeVert() const;

			const Verts& verts() const;

			HexVertData<std::size_t> firstOccurrenceIndices() const;

			bool valid() const;

			bool draw(bool _activeVertSel, Value _min, Value _max, std::optional<Verts>& _copiedVerts, std::optional<Vert>& _copiedVert);

			bool draw(bool _activeVertSel, Value _min, Value _max, const std::optional<Verts>& _copiedVerts, const std::optional<Vert>& _copiedVert);

			bool draw(bool _activeVertSel = true, Value _min = Value{ 0 }, Value _max = std::numeric_limits<Value>::max());

		};

	}

	using IHexControl = internal::HexControl<true>;

	using RHexControl = internal::HexControl<false>;

}

#define RSE_HEXCONTROL_TPP
#include <RSE/HexControl.tpp>
#undef RSE_HEXCONTROL_TPP