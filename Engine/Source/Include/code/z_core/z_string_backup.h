#ifndef Z_STRING_H_
#define Z_STRING_H_

#include"internal/z_drive.h"

#include"z_object.h"

namespace zengine {
namespace internal {

/*
    String template class.
*/
template<typename _CharType>
requires kIsChar<_CharType>
class TString :public ZObject {
public:
    /*
        The temp string type, used for the string combining.The operator+ always
        return this.
    */
    class ZTempString :public TString<_CharType> {
    public:
        NODISCARD FORCEINLINE ZTempString& operator+(const _CharType* string) {
            return reinterpret_cast<ZTempString&>(TString<_CharType>::Append(string));
        }
        NODISCARD FORCEINLINE ZTempString& operator+(const TString& string) {
            return reinterpret_cast<ZTempString&>(TString<_CharType>::Append(string));
        }
        NODISCARD FORCEINLINE ZTempString& operator+(const ZTempString& string) {
            return reinterpret_cast<ZTempString&>(TString<_CharType>::Append(string));
        }
    };

    //TODO(Johnasd4): The iterators.

    FORCEINLINE TString() noexcept : SuperType_() {}
    FORCEINLINE TString(const _CharType* string);
    FORCEINLINE TString(const TString& string) : SuperType_(string) {}
    FORCEINLINE TString(TString&& string): SuperType_(std::forward<ZString>(string)) {}

    TString& operator=(const _CharType* string) noexcept;
    FORCEINLINE TString& operator=(const TString& string) { 
        return reinterpret_cast<TString&>(SuperType_::operator=(string));
    }
    FORCEINLINE TString& operator=(TString&& string) { 
        return reinterpret_cast<TString&>(SuperType_::operator=(std::forward<TString>(string)));
    }
    FORCEINLINE TString& operator=(const ZTempString& string) {
        return reinterpret_cast<TString&>(SuperType_::operator=(string));
    }

    NODISCARD FORCEINLINE _CharType& operator()(const IndexType index) { return SuperType_::operator()(index); }
    NODISCARD FORCEINLINE const _CharType& operator()(const IndexType index) const { 
        return SuperType_::operator()(index); 
    }

    NODISCARD FORCEINLINE ZTempString operator+(const _CharType* string) const;
    NODISCARD FORCEINLINE ZTempString operator+(const TString& string) const;
    NODISCARD FORCEINLINE ZTempString operator+(const ZTempString& string) const;

    FORCEINLINE ~TString() {}

    NODISCARD const _CharType* char_string() const noexcept;
    NODISCARD FORCEINLINE const IndexType& size() const { return SuperType_::size(); }
    NODISCARD FORCEINLINE const IndexType& length() const { return SuperType_::size(); }
    NODISCARD FORCEINLINE const IndexType& capacity() const { return SuperType_::capacity(); }

    FORCEINLINE Void set_size(const IndexType need_size) { SuperType_::set_size(need_size); }
    FORCEINLINE Void set_length(const IndexType need_length) noexcept { SuperType_::set_size(need_length); }
    FORCEINLINE Void set_capacity(const IndexType need_capacity) noexcept { SuperType_::set_capacity(need_capacity); }

    FORCEINLINE const Bool IfEmpty() { return SuperType_::IfEmpty(); }

    TString& Append(const _CharType* string) noexcept;
    TString& Append(const TString& string) noexcept;
    TString& Append(const ZTempString& string) noexcept;

    FORCEINLINE Void Clear() { SuperType_::Clear(); }

    //TODO(Johnasd4):Other functions, such as find(), rfind(), cut(), replace(), 
    //split(), erase(), substring(), compare(), >, >=, <, <=, !=, ==
     
protected:
    using SuperType_ = internal::ZContainerBase<CharType, false>;

};

template<typename CharType>
requires kIsChar<CharType>
NODISCARD FORCEINLINE TString<CharType>::ZTempString operator+(const CharType* dst_string,
                                                       const TString<CharType>& src_string) {
    typename TString<CharType>::ZTempString temp_string(dst_string);
    return temp_string.Append(src_string);
}
template<typename CharType>
requires kIsChar<CharType>
NODISCARD FORCEINLINE TString<CharType>::ZTempString operator+(const CharType* dst_string,
                                                       const typename TString<CharType>::ZTempString& src_string) {
    typename TString<CharType>::ZTempString temp_string(dst_string);
    return temp_string.Append(src_string);
}
template<typename CharType>
requires kIsChar<CharType>
NODISCARD FORCEINLINE TString<CharType>::ZTempString TString<CharType>::operator+(const CharType* string) const {
    TString::ZTempString temp_string(*this);
    temp_string.Append(string);
    return temp_string;
}
template<typename CharType>
requires kIsChar<CharType>
NODISCARD FORCEINLINE TString<CharType>::ZTempString TString<CharType>::operator+(const TString& string) const {
    TString::ZTempString temp_string(*this);
    temp_string.Append(string);
    return temp_string;
}
template<typename CharType>
requires kIsChar<CharType>
NODISCARD FORCEINLINE TString<CharType>::ZTempString TString<CharType>::operator+(const ZTempString& string) const {
    TString::ZTempString temp_string(*this);
    temp_string.Append(string);
    return temp_string;
}

template<>
FORCEINLINE ZString<CChar>::ZString(const CChar* string) 
        : SuperType_(string, static_cast<IndexType>(strlen(string))) {}
template<>
FORCEINLINE ZString<TChar>::ZString(const TChar* string) 
        : SuperType_(string, static_cast<IndexType>(wcslen(string))) {}

template<typename CharType>
requires kIsChar<CharType>
TString<CharType>& TString<CharType>::operator=(const CharType* string) noexcept {
    IndexType string_length = static_cast<IndexType>(strlen(string));
    SuperType_::set_size(string_length);
    memcpy(reinterpret_cast<Address>(const_cast<_CharType*>(SuperType_::data_ptr())), 
           reinterpret_cast<Address>(const_cast<_CharType*>(string)), 
           static_cast<SizeType>(string_length) * sizeof(SizeType));
}

template<typename CharType>
requires kIsChar<CharType>
NODISCARD const CharType* TString<CharType>::char_string() const noexcept {
    //Adds \0 to the end of the stirng, makes sure the container is big enough.
    if (SuperType_::capacity() <= SuperType_::size()) {
        (const_cast<TString*>(this))->SuperType_::set_capacity(SuperType_::capacity() + 1);
    }
    (const_cast<TString*>(this))->SuperType_::operator()(SuperType_::size()) = '\0';
    return SuperType_::data_ptr(); 
}

template<>
ZString<CChar>& ZString<CChar>::Append(const CChar* string) noexcept {
    IndexType add_string_length = static_cast<IndexType>(strlen(string));
    IndexType crrent_string_length = SuperType_::size();
    SuperType_::change_size(add_string_length);
    memcpy(reinterpret_cast<Address>(&SuperType_::operator()(crrent_string_length)),
           reinterpret_cast<Address>(const_cast<CChar*>(string)), 
           static_cast<SizeType>(add_string_length) * sizeof(CChar));
    return *this;
}

template<>
ZString<TChar>& ZString<TChar>::Append(const TChar* string) noexcept {
    IndexType add_string_length = static_cast<IndexType>(wcslen(string));
    IndexType crrent_string_length = SuperType_::size();
    SuperType_::change_size(add_string_length);
    memcpy(reinterpret_cast<Address>(&SuperType_::operator()(crrent_string_length)),
           reinterpret_cast<Address>(const_cast<TChar*>(string)),
           static_cast<SizeType>(add_string_length) * sizeof(TChar));
    return *this;
}

template<typename CharType>
requires kIsChar<CharType>
TString<CharType>& TString<CharType>::Append(const TString& string) noexcept {
    IndexType crrent_string_length = SuperType_::size();
    SuperType_::change_size(string.SuperType_::size());
    memcpy(reinterpret_cast<Address>(&SuperType_::operator()(crrent_string_length)),
           reinterpret_cast<Address>(const_cast<SizeType*>(string.SuperType_::data_ptr())),
           static_cast<SizeType>(string.SuperType_::size()) * sizeof(_CharType));
    return *this;
}

template<typename CharType>
requires kIsChar<CharType>
TString<CharType>& TString<CharType>::Append(const ZTempString& string) noexcept {
    IndexType crrent_string_length = SuperType_::size();
    SuperType_::change_size(string.SuperType_::size());
    memcpy(reinterpret_cast<Address>(&SuperType_::operator()(crrent_string_length)),
           reinterpret_cast<Address>(const_cast<SizeType*>(string.SuperType_::data_ptr())),
           static_cast<SizeType>(string.SuperType_::size()) * sizeof(_CharType));
    return *this;
}

}//internal

using ZCString = internal::ZString<CChar>;
using ZTString = internal::ZString<TChar>;

}//zengine

#endif // !Z_STRING_H_
