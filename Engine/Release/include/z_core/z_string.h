#ifndef Z_STRING_H_
#define Z_STRING_H_

#include"internal/drive.h"

#include"internal/z_container_base.h"

namespace zengine {
namespace internal {

/*
    Tests if the type is a kind of char.
*/
template<typename CharType>
concept kIsChar = std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>;

/*
    The string type.
*/
template<typename CharType>
requires kIsChar<CharType>
class ZString :public ZContainerBase<CharType, false> {
public:
    /*
        The temp string type, used for the string combining.The operator+ always
        return this.
    */
    class ZTempString :public ZString<CharType> {
    public:
        FORCEINLINE ZTempString& operator+(const CharType* string) { 
            return reinterpret_cast<ZTempString&>(ZString<CharType>::Append(string));
        }
        FORCEINLINE ZTempString& operator+(const ZString& string) {
            return reinterpret_cast<ZTempString&>(ZString<CharType>::Append(string));
        }
        FORCEINLINE ZTempString& operator+(const ZTempString& string) {
            return reinterpret_cast<ZTempString&>(ZString<CharType>::Append(string));
        }
    };

    //TODO(Johnasd4): The iterators.

    FORCEINLINE ZString() noexcept : SuperType() {}
    FORCEINLINE ZString(const CharType* string);
    FORCEINLINE ZString(const ZString& string) : SuperType(string) {}
    FORCEINLINE ZString(ZString&& string): SuperType(std::forward<ZString>(string)) {}

    ZString& operator=(const CharType* string) noexcept;
    FORCEINLINE ZString& operator=(const ZString& string) { 
        return reinterpret_cast<ZString&>(SuperType::operator=(string));
    }
    FORCEINLINE ZString& operator=(ZString&& string) { 
        return reinterpret_cast<ZString&>(SuperType::operator=(std::forward<ZString>(string)));
    }
    FORCEINLINE ZString& operator=(const ZTempString& string) {
        return reinterpret_cast<ZString&>(SuperType::operator=(string));
    }

    FORCEINLINE CharType& operator()(const IndexType index) { return SuperType::operator()(index); }
    FORCEINLINE const CharType& operator()(const IndexType index) const { return SuperType::operator()(index); }

    FORCEINLINE ZTempString operator+(const CharType* string) const;
    FORCEINLINE ZTempString operator+(const ZString& string) const;
    FORCEINLINE ZTempString operator+(const ZTempString& string) const;

    FORCEINLINE ~ZString() {}

    const CharType* char_string() const noexcept;
    FORCEINLINE const IndexType& size() const { return SuperType::size(); }
    FORCEINLINE const IndexType& length() const { return SuperType::size(); }
    FORCEINLINE const IndexType& capacity() const { return SuperType::capacity(); }

    FORCEINLINE Void set_size(const IndexType need_size) { SuperType::set_size(need_size); }
    FORCEINLINE Void set_length(const IndexType need_length) noexcept { SuperType::set_size(need_length); }
    FORCEINLINE Void set_capacity(const IndexType need_capacity) noexcept { SuperType::set_capacity(need_capacity); }

    FORCEINLINE const Bool IfEmpty() { return SuperType::IfEmpty(); }

    ZString& Append(const CharType* string) noexcept;
    ZString& Append(const ZString& string) noexcept;
    ZString& Append(const ZTempString& string) noexcept;

    FORCEINLINE Void Clear() { SuperType::Clear(); }

    //TODO(Johnasd4):Other functions, such as find(), rfind(), cut(), replace(), 
    //split(), erase(), substring(), compare(), >, >=, <, <=, !=, ==
     
protected:
    using SuperType = internal::ZContainerBase<CharType, false>;
};

template<typename CharType>
requires kIsChar<CharType>
FORCEINLINE ZString<CharType>::ZTempString operator+(const CharType* dst_string, 
                                                       const ZString<CharType>& src_string) {
    typename ZString<CharType>::ZTempString temp_string(dst_string);
    return temp_string.Append(src_string);
}
template<typename CharType>
requires kIsChar<CharType>
FORCEINLINE ZString<CharType>::ZTempString operator+(const CharType* dst_string,
                                                       const typename ZString<CharType>::ZTempString& src_string) {
    typename ZString<CharType>::ZTempString temp_string(dst_string);
    return temp_string.Append(src_string);
}
template<typename CharType>
requires kIsChar<CharType>
FORCEINLINE ZString<CharType>::ZTempString ZString<CharType>::operator+(const CharType* string) const {
    ZString::ZTempString temp_string(*this);
    temp_string.Append(string);
    return temp_string;
}
template<typename CharType>
requires kIsChar<CharType>
FORCEINLINE ZString<CharType>::ZTempString ZString<CharType>::operator+(const ZString& string) const {
    ZString::ZTempString temp_string(*this);
    temp_string.Append(string);
    return temp_string;
}
template<typename CharType>
requires kIsChar<CharType>
FORCEINLINE ZString<CharType>::ZTempString ZString<CharType>::operator+(const ZTempString& string) const {
    ZString::ZTempString temp_string(*this);
    temp_string.Append(string);
    return temp_string;
}

template<>
FORCEINLINE ZString<CChar>::ZString(const CChar* string) 
        : SuperType(string, static_cast<IndexType>(strlen(string))) {}
template<>
FORCEINLINE ZString<TChar>::ZString(const TChar* string) 
        : SuperType(string, static_cast<IndexType>(wcslen(string))) {}

template<typename CharType>
requires kIsChar<CharType>
ZString<CharType>& ZString<CharType>::operator=(const CharType* string) noexcept {
    IndexType string_length = static_cast<IndexType>(strlen(string));
    SuperType::set_size(string_length);
    memcpy(reinterpret_cast<Address>(const_cast<CharType*>(SuperType::data_ptr())), 
           reinterpret_cast<Address>(const_cast<CharType*>(string)), 
           static_cast<SizeType>(string_length) * sizeof(SizeType));
}

template<typename CharType>
requires kIsChar<CharType>
const CharType* ZString<CharType>::char_string() const noexcept {
    //Adds \0 to the end of the stirng, makes sure the container is big enough.
    if (SuperType::capacity() <= SuperType::size()) {
        (const_cast<ZString*>(this))->SuperType::set_capacity(SuperType::capacity() + 1);
    }
    (const_cast<ZString*>(this))->SuperType::operator()(SuperType::size()) = '\0';
    return SuperType::data_ptr(); 
}

template<>
ZString<CChar>& ZString<CChar>::Append(const CChar* string) noexcept {
    IndexType add_string_length = static_cast<IndexType>(strlen(string));
    IndexType crrent_string_length = SuperType::size();
    SuperType::change_size(add_string_length);
    memcpy(reinterpret_cast<Address>(&SuperType::operator()(crrent_string_length)),
           reinterpret_cast<Address>(const_cast<CChar*>(string)), 
           static_cast<SizeType>(add_string_length) * sizeof(CChar));
    return *this;
}

template<>
ZString<TChar>& ZString<TChar>::Append(const TChar* string) noexcept {
    IndexType add_string_length = static_cast<IndexType>(wcslen(string));
    IndexType crrent_string_length = SuperType::size();
    SuperType::change_size(add_string_length);
    memcpy(reinterpret_cast<Address>(&SuperType::operator()(crrent_string_length)),
           reinterpret_cast<Address>(const_cast<TChar*>(string)),
           static_cast<SizeType>(add_string_length) * sizeof(TChar));
    return *this;
}

template<typename CharType>
requires kIsChar<CharType>
ZString<CharType>& ZString<CharType>::Append(const ZString& string) noexcept {
    IndexType crrent_string_length = SuperType::size();
    SuperType::change_size(string.SuperType::size());
    memcpy(reinterpret_cast<Address>(&SuperType::operator()(crrent_string_length)),
           reinterpret_cast<Address>(const_cast<SizeType*>(string.SuperType::data_ptr())),
           static_cast<SizeType>(string.SuperType::size()) * sizeof(CharType));
    return *this;
}

template<typename CharType>
requires kIsChar<CharType>
ZString<CharType>& ZString<CharType>::Append(const ZTempString& string) noexcept {
    IndexType crrent_string_length = SuperType::size();
    SuperType::change_size(string.SuperType::size());
    memcpy(reinterpret_cast<Address>(&SuperType::operator()(crrent_string_length)),
           reinterpret_cast<Address>(const_cast<SizeType*>(string.SuperType::data_ptr())),
           static_cast<SizeType>(string.SuperType::size()) * sizeof(CharType));
    return *this;
}

}//internal

using ZCString = internal::ZString<CChar>;
using ZTString = internal::ZString<TChar>;

}//zengine

#endif // !Z_STRING_H_
