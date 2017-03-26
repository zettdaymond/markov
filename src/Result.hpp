#pragma once

#include <type_traits>
#include <cassert>
#include <algorithm>
#include <iostream>

template<typename T>
struct Ok {
    Ok(const T& val) : val(val) { }
    Ok(T&& val) : val(std::move(val)) { }

    T val;
};

template<typename E>
struct Err {
    Err(const E& val) : val(val) { }
    Err(E&& val) : val(std::move(val)) { }

    E val;
};

/**
 * \brief Algebraic type hightly inspired from Rust. Might be in two states :
 * - contains T
 * - contains E
 * Similar for Boost::Variant, but uses for error handling, instead of returning error codes.
 */
template<class T, class E>
class Result
{
    static constexpr size_t max_type_size = sizeof(T) > sizeof(E) ? sizeof(T) : sizeof(E);
    static constexpr size_t max_align_size = alignof(T) > alignof(E) ? alignof(T) : alignof(E);

    using storage_t =  typename std::aligned_storage<max_type_size, max_align_size>::type;

    static_assert(!std::is_same<E, void>::value, "'void' error type is not allowed");

public:
    constexpr Result() = delete;

    Result(const Result& old)
        : m_ok(old.m_ok)
    {
        if (m_ok) {
            new (&m_storage) T(*reinterpret_cast<const T*>(  &(old.m_storage) ));
        } else {
            new (&m_storage) E(*reinterpret_cast<const E*>(  &(old.m_storage) ));
        }
    }

    Result(Result && old)
        : m_ok(old.m_ok)
    {
        if (m_ok) {
            new (&m_storage) T(std::move(*reinterpret_cast<const T*>(  &(old.m_storage) )));
        } else {
            new (&m_storage) E(std::move(*reinterpret_cast<const E*>(  &(old.m_storage) )));
        }
    }

    Result & operator=(const Result&) = default;
    Result & operator=(Result &&) = default;

    constexpr const T* operator->() const
    {
        return &value();
    }

    T* operator->()
    {
        return &value();
    }

    constexpr const T& operator*() const
    {
        return value();
    }

    T & operator*()
    {
        return value();
    }

    constexpr explicit operator bool() const
    {
        return m_ok;
    }

    //---Setup variables----------------------
    constexpr Result(const Ok<T>& v)
    {
        new (&m_storage) T(v.val);
        m_ok = true;
    }

    constexpr Result(Ok<T> && v)
    {
        new (&m_storage) T(std::move(v.val));
        m_ok = true;
    }

    constexpr Result(const Err<E>& v)
    {
        new (&m_storage) E(v.val);
        m_ok = false;
    }

    constexpr Result(Err<E> && v)
    {
        new (&m_storage) E(std::move(v.val));
        m_ok = false;
    }

    constexpr auto operator=(const Ok<T>& v)
    {
        new (&m_storage) T(v.val);
        m_ok = true;
        return *this;
    }

    constexpr auto operator=(Ok<T> && v)
    {
        new (&m_storage) T(std::move(v.val));
        m_ok = true;
        return *this;
    }

    constexpr auto operator=(const Err<E>& v)
    {
        new (&m_storage) E(v.val);
        m_ok = false;
        return *this;
    }

    constexpr auto operator=(Err<E> && v)
    {
        new (&m_storage) E(std::move(v.val));
        m_ok = false;
        return *this;
    }

    //----------------------------------

    constexpr const T& value() const
    {
        assert(m_ok && "Trying to unwrap Result type without a value.");
        return *reinterpret_cast<T*>(&m_storage);
    }

    T& value()
    {
        assert(m_ok && "Trying to unwrap Result type without a value.");
        return *reinterpret_cast<T*>(&m_storage);
    }

    constexpr const E& err() const
    {
        assert(!m_ok && "Trying to get error in Result with value.");
        return *reinterpret_cast<E*>(&m_storage);
    }

    E& err()
    {
        assert(m_ok && "Trying to unwrap Result type without a value.");
        return *reinterpret_cast<E*>(&m_storage);
    }

    bool is_ok()
    {
        return m_ok;
    }

    bool is_err() {
        return !m_ok;
    }
private:
    storage_t m_storage;
    bool m_ok;
};
