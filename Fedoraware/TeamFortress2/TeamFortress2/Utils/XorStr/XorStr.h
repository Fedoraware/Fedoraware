#pragma once

#include <string>
#include <utility>

#pragma warning (push)
#pragma warning (disable : 6201)
#pragma warning (disable : 6294)

//I think this is originally by the OSH guy (kn4ck3r?)

//Basic compile time string encryption.
//I do not know who to credit for this
//I found this with no credits put anywhere, so I did some minor changes
//And used the time to learn how this exactly works
//This will help us remove the "naked" strings in our dll

//This included a check for _MSC_VER which is defined on a Microsoft C/C++ compiler
//We both use one, so I deleted that.
//I still kept this definition here, so it's easy to change or add back
#define FORCE_INLINE __forceinline

//compile time atoi, for XOR seed / key generation
namespace Const
{
	constexpr int Atoi(char c)
	{
		return c - '0';
	}
}

template<typename StringType, size_t nLenght>
class CBasicXorString
{
	using ValueType = typename StringType::value_type;
	static constexpr auto nRealLenght = (nLenght - 1);

public:
	constexpr FORCE_INLINE CBasicXorString(ValueType const (&str)[nLenght])
		: CBasicXorString(str, std::make_index_sequence<nRealLenght>()) { }

	//c_str operator
	//Decrypt the data if it's not decrypted already
	//And return as the type of string defined
	inline auto c_str() const
	{
		Decrypt();
		return m_Data;
	}

	//str operator
	//Decrypt the data if not decrypted already
	//And return as the type initialized
	inline auto str() const
	{
		Decrypt();
		return StringType(m_Data, m_Data + nRealLenght);
	}

	inline operator StringType() const
	{
		return str();
	}

private:

	//Our initializer which takes and encrypts the string
	template<size_t... nIndices>
	constexpr FORCE_INLINE CBasicXorString(ValueType const (&str)[nLenght], std::index_sequence<nIndices...>)
		: m_Data{ Encrypt(str[nIndices], nIndices)..., '\0' },
		m_bEncrypted(true) { }

	//Generate constant compile time seed to be used
	//Meaning the seed will change every time we compile
	//(Will take the current time and use that to generate the seed on compile time)
	//Of course ignoring the ":"'s present in displaying the time
	static constexpr auto XORKEY = static_cast<ValueType>
		(
			Const::Atoi(__TIME__[7]) +
			Const::Atoi(__TIME__[6]) * 10 +
			Const::Atoi(__TIME__[4]) * 60 +
			Const::Atoi(__TIME__[3]) * 600 +
			Const::Atoi(__TIME__[1]) * 3600 +
			Const::Atoi(__TIME__[0]) * 36000
			);

	//Encrypt the string
	//Simply XOR the string with generated key
	static FORCE_INLINE constexpr auto Encrypt(ValueType c, size_t i)
	{
		//size_t has to be casted as integer, as it can't be used in this operation as is.
		return static_cast<ValueType>(c ^ (XORKEY + static_cast<int>(i)));
	}

	//Decrypt the string
	//Undo the encryption
	inline void Decrypt() const
	{
		//Check if the initializer did it's job correctly, thus encrypting the string
		//And setting this member to true
		if (m_bEncrypted)
		{
			//If so, loop trough the "data" and use "Encrypt()" again to restore the string to it's original state
			for (size_t t = 0; t < nRealLenght; t++)
				m_Data[t] = Encrypt(m_Data[t], t);

			//Set the encrypted member to false, as we just finished decrypting it.
			m_bEncrypted = false;
		}
	}

	//Members that hold information about the string
	mutable ValueType m_Data[nLenght]; //String data
	mutable bool m_bEncrypted; //Is the string encrypted
};
//===========================================================================
template<size_t nLenght>
using XorStrA = CBasicXorString<std::string, nLenght>;
template<size_t nLenght>
using XorStrW = CBasicXorString<std::wstring, nLenght>;
template<size_t nLenght>
using XorStrU16 = CBasicXorString<std::u16string, nLenght>;
template<size_t nLenght>
using XorStrU32 = CBasicXorString<std::u32string, nLenght>;
//===========================================================================
template<typename StringType, size_t nLenght, size_t nLenght2>
inline auto operator==(const CBasicXorString<StringType, nLenght>& lhs, const CBasicXorString<StringType, nLenght2>& rhs)
{
	//Issue compile time error if lenghts wont match
	static_assert(nLenght == nLenght2, "[XorString operator==] Different lenghts!");
	return nLenght == nLenght2 && lhs.str() == rhs.str();
}
//===========================================================================
template<typename StringType, size_t nLenght>
inline auto operator==(const StringType& lhs, const CBasicXorString<StringType, nLenght>& rhs)
{
	return lhs.size() == nLenght && lhs == rhs.str();
}
//===========================================================================
template<typename StreamType, typename StringType, size_t nLenght>
inline auto& operator<<(StreamType& lhs, const CBasicXorString<StringType, nLenght>& rhs)
{
	lhs << rhs.c_str();
	return lhs;
}
//===========================================================================
template<typename StringType, size_t nLenght, size_t nLenght2>
inline auto operator+(const CBasicXorString<StringType, nLenght>& lhs, const CBasicXorString<StringType, nLenght2>& rhs)
{
	return lhs.str() + rhs.str();
}
//===========================================================================
template<typename StringType, size_t nLenght>
inline auto operator+(const StringType& lhs, const CBasicXorString<StringType, nLenght>& rhs)
{
	return lhs + rhs.str();
}
//===========================================================================
//Here we have the final wrappers for our string encrypting
//We probably don't even need char16/32, but I've kept them in just in case
//===========================================================================
template<size_t nLenght>
constexpr FORCE_INLINE auto XorStr(char const (&str)[nLenght])
{
	return XorStrA<nLenght>(str);
}
//===========================================================================
template<size_t nLenght>
constexpr FORCE_INLINE auto XorStr(wchar_t const (&str)[nLenght])
{
	return XorStrW<nLenght>(str);
}
//===========================================================================
template<size_t nLenght>
constexpr FORCE_INLINE auto XorStr(char16_t const (&str)[nLenght])
{
	return XorStrU16<nLenght>(str);
}
//===========================================================================
template<size_t nLenght>
constexpr FORCE_INLINE auto XorStr(char32_t const (&str)[nLenght])
{
	return XorStrU32<nLenght>(str);
}
//===========================================================================

#pragma warning (pop)