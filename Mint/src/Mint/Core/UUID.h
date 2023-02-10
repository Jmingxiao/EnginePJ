#pragma once


MT_NAMESPACE_BEGIN

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};

MT_NAMESPACE_END

namespace std {
	template <typename T> struct hash;

	template<>
	struct hash<Mint::UUID>
	{
		std::size_t operator()(const Mint::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};
}
