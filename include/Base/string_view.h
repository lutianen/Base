///
/// @brief This file declares the string_view class.
///

#pragma once

#include <cassert>  // assert
#include <cstring>  // strlen
#include <string>   // string

namespace Lute {

#if __cplusplus >= 201703L
#include <string_view>

using string_view = std::string_view;

/// C++ 11
#else

class string_view {
public:
    static const size_t npos = static_cast<size_t>(-1);

    /* We provide non-explicit singleton constructors so users can pass
     in a "const char*" or a "string" wherever a "string_view" is
     expected. */
    string_view() : ptr_(nullptr), len_(0) {}

    /// @note It's not undefined in std::string_view.
    string_view(const std::string& str) : ptr_(str.data()), len_(str.size()) {}

    string_view(const char* str) : ptr_(str), len_(::strlen(ptr_)) {}

    string_view(const char* offset, int len) : ptr_(offset), len_(len) {}

    string_view(const unsigned char* str)
        : ptr_(reinterpret_cast<const char*>(str)), len_(::strlen(ptr_)) {}

    string_view(const string_view&) = default;

    string_view& operator=(const string_view&) = default;

    // data() may return a pointer to a buffer with embedded NULs, and the
    // returned buffer may or may not be null terminated.  Therefore it is
    // typically a mistake to pass data() to a routine that expects a NUL
    // terminated string.  Use "as_string().c_str()" if you really need to do
    // this.  Or better yet, change your routine so it does not rely on NUL
    // termination.
    const char* data() const { return ptr_; }
    size_t size() const { return len_; }
    size_t length() const { return len_; }
    size_t max_size() const {
        return (npos - sizeof(size_t) - sizeof(void*)) / sizeof(char) / 4;
    }
    bool empty() const { return len_ == 0; }

    const char* begin() const { return ptr_; }
    const char* end() const { return ptr_ + len_; }
    const char* cbegin() const { return ptr_; }
    const char* cend() const { return ptr_ + len_; }

    ///
    /// @brief clear data of string_view.
    /// @note It's not undefined in std::string_view.
    ///
    void clear() {
        ptr_ = nullptr;
        len_ = 0;
    }

    ///
    /// @brief Set buffer to string_view.
    /// @note It's not undefined in std::string_view.
    ///
    void set(const char* buffer, size_t len) {
        ptr_ = buffer;
        len_ = len;
    }

    ///
    /// @brief Set buffer to string_view.
    /// @note It's not undefined in std::string_view.
    ///
    void set(const char* str) {
        ptr_ = str;
        len_ = ::strlen(str);
    }

    ///
    /// @brief Set buffer to string_view.
    /// @note It's not undefined in std::string_view.
    ///
    void set(const void* buffer, size_t len) {
        ptr_ = reinterpret_cast<const char*>(buffer);
        len_ = len;
    }

    const char& operator[](size_t pos) const {
        assert(pos < len_);
        return ptr_[pos];
    }
    const char& at(size_t pos) const {
        assert(pos < len_);
        return ptr_[pos];
    }

    const char& front() const {
        assert(len_ > 0);
        return ptr_[0];
    }

    const char& back() const {
        assert(len_ > 0);
        return ptr_[len_ - 1];
    }

    void remove_prefix(size_t n) {
        assert(len_ >= n);
        ptr_ += n;
        len_ -= n;
    }

    void remove_suffix(size_t n) { len_ -= n; }

    void swap(string_view& other) {
        auto tmp = *this;
        *this = other;
        other = tmp;
    }

    string_view substr(size_t pos = 0, size_t n = npos) const {
        assert(pos <= len_);
        const size_t rlen = std::min(n, len_ - pos);
        return string_view(ptr_ + pos, rlen);
    }

    bool operator==(const string_view& x) const {
        return ((len_ == x.len_) &&
                (::memcmp(ptr_, x.ptr_, static_cast<size_t>(len_)) == 0));
    }
    bool operator!=(const string_view& x) const { return !(*this == x); }

#define STRING_VIEW_BINARY_PREDICATE(cmp, auxcmp)                      \
    bool operator cmp(const string_view& x) const {                    \
        int r = ::memcmp(ptr_, x.ptr_,                                 \
                         len_ < x.len_ ? static_cast<size_t>(len_)     \
                                       : static_cast<size_t>(x.len_)); \
        return ((r auxcmp 0) || ((r == 0) && (len_ cmp x.len_)));      \
    }

    STRING_VIEW_BINARY_PREDICATE(<, <);
    STRING_VIEW_BINARY_PREDICATE(<=, <);
    STRING_VIEW_BINARY_PREDICATE(>=, >);
    STRING_VIEW_BINARY_PREDICATE(>, >);
#undef STRING_VIEW_BINARY_PREDICATE

    int compare(const string_view& x) const {
        int r = ::memcmp(ptr_, x.ptr_,
                         len_ < x.len_ ? static_cast<size_t>(len_)
                                       : static_cast<size_t>(x.len_));
        if (r == 0) {
            if (len_ < x.len_)
                r = -1;
            else if (len_ > x.len_)
                r = +1;
        }
        return r;
    }

    /// @note It came out after C++20.
    bool starts_with(const string_view& x) const {
        return ((len_ >= x.len_) && (::memcmp(ptr_, x.ptr_, x.len_) == 0));
    }
    /// @note It came out after C++20.
    bool starts_with(char x) const { return !empty() && front() == x; }
    /// @note It came out after C++20.
    bool starts_with(const char* x) const {
        return starts_with(string_view(x));
    }

    /// @note It came out after C++20.
    bool ends_with(const string_view& x) const {
        return (len_ >= x.len_) &&
               (::memcmp(ptr_ + len_ - x.len_, x.ptr_, x.len_) == 0);
    }
    /// @note It came out after C++20.
    bool ends_with(char x) const { return !empty() && back() == x; }
    /// @note It came out after C++20.
    bool ends_with(const char* x) const { return ends_with(string_view(x)); }

    ///
    /// @brief Convert to std::string
    /// @note It's not undefined in std::string_view
    ///
    std::string as_string() const { return std::string(data(), size()); }
    ///
    /// @brief Copy data to target
    /// @note It's not undefined in std::string_view
    /// @param target target string
    ///
    void CopyToString(std::string* target) const { target->assign(ptr_, len_); }

private:
    /// The pointer to data
    const char* ptr_;
    /// length
    size_t len_;
};
#endif

}  // namespace Lute
