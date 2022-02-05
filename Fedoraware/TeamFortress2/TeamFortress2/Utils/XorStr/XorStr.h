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

template<typename StringType, size_t nLength>
class CBasicXorString
{
	using ValueType = typename StringType::value_type;
	static constexpr auto nRealLength = (nLength - 1);

public:
	constexpr FORCE_INLINE CBasicXorString(ValueType const (&str)[nLength])
		: CBasicXorString(str, std::make_index_sequence<nRealLength>()) { }

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
		return StringType(m_Data, m_Data + nRealLength);
	}

	inline operator StringType() const
	{
		return str();
	}

private:

	//Our initializer which takes and encrypts the string
	template<size_t... nIndices>
	constexpr FORCE_INLINE CBasicXorString(ValueType const (&str)[nLength], std::index_sequence<nIndices...>)
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
			for (size_t t = 0; t < nRealLength; t++)
				m_Data[t] = Encrypt(m_Data[t], t);

			//Set the encrypted member to false, as we just finished decrypting it.
			m_bEncrypted = false;
		}
	}

	//Members that hold information about the string
	mutable ValueType m_Data[nLength]; //String data
	mutable bool m_bEncrypted; //Is the string encrypted
};
//===========================================================================
template<size_t nLength>
using XorStrA = CBasicXorString<std::string, nLength>;
template<size_t nLength>
using XorStrW = CBasicXorString<std::wstring, nLength>;
template<size_t nLength>
using XorStrU16 = CBasicXorString<std::u16string, nLength>;
template<size_t nLength>
using XorStrU32 = CBasicXorString<std::u32string, nLength>;
//===========================================================================
template<typename StringType, size_t nLength, size_t nLength2>
inline auto operator==(const CBasicXorString<StringType, nLength>& lhs, const CBasicXorString<StringType, nLength2>& rhs)
{
	//Issue compile time error if lengths wont match
	static_assert(nLength == nLength2, "[XorString operator==] Different lengths!");
	return nLength == nLength2 && lhs.str() == rhs.str();
}
//===========================================================================
template<typename StringType, size_t nLength>
inline auto operator==(const StringType& lhs, const CBasicXorString<StringType, nLength>& rhs)
{
	return lhs.size() == nLength && lhs == rhs.str();
}
//===========================================================================
template<typename StreamType, typename StringType, size_t nLength>
inline auto& operator<<(StreamType& lhs, const CBasicXorString<StringType, nLength>& rhs)
{
	lhs << rhs.c_str();
	return lhs;
}
//===========================================================================
template<typename StringType, size_t nLength, size_t nLength2>
inline auto operator+(const CBasicXorString<StringType, nLength>& lhs, const CBasicXorString<StringType, nLength2>& rhs)
{
	return lhs.str() + rhs.str();
}
//===========================================================================
template<typename StringType, size_t nLength>
inline auto operator+(const StringType& lhs, const CBasicXorString<StringType, nLength>& rhs)
{
	return lhs + rhs.str();
}
//===========================================================================
//Here we have the final wrappers for our string encrypting
//We probably don't even need char16/32, but I've kept them in just in case
//===========================================================================
template<size_t nLength>
constexpr FORCE_INLINE auto XorStr(char const (&str)[nLength])
{
	return XorStrA<nLength>(str);
}
//===========================================================================
template<size_t nLength>
constexpr FORCE_INLINE auto XorStr(wchar_t const (&str)[nLength])
{
	return XorStrW<nLength>(str);
}
//===========================================================================
template<size_t nLength>
constexpr FORCE_INLINE auto XorStr(char16_t const (&str)[nLength])
{
	return XorStrU16<nLength>(str);
}
//===========================================================================
template<size_t nLength>
constexpr FORCE_INLINE auto XorStr(char32_t const (&str)[nLength])
{
	return XorStrU32<nLength>(str);
}
//===========================================================================

#pragma warning (pop)