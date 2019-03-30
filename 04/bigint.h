#ifndef BIGINT_H
#define BIGINT_H

#include "custom_vector.h"
#include <cstdint>
#include <ostream>

class BigInt {
    enum class Sign {
        plus,
        minus
    };

    static BigInt& plus_assign_impl(BigInt& lhs, const BigInt& rhs);
    static BigInt& minus_assign_impl(BigInt& lhs, const BigInt& rhs);

    friend std::ostream& operator<<(std::ostream& out, const BigInt& element);

public:
    BigInt(int64_t value=0);
    BigInt(const BigInt& other) : sign_(other.sign_), data_(other.data_) {}
    BigInt(BigInt&& other) : sign_(other.sign_), data_(std::move(other.data_)) {}
    BigInt& operator=(int64_t value);
    BigInt& operator=(const BigInt& other)
    {
        if (this != &other) {
            sign_ = other.sign_;
            data_ = other.data_;
        }
        return *this;
    }
    BigInt& operator=(BigInt&& other)
    {
        if (this != &other) {
            sign_ = other.sign_;
            data_ = std::move(other.data_);
            other.data_.PushFront(0);
        }
        return *this;
    }
    ~BigInt() {}

    BigInt operator-() const;

    BigInt& operator+=(const BigInt& other);
    BigInt& operator-=(const BigInt& other);

    BigInt& operator+=(int64_t value) { return *this += BigInt(value);}
    BigInt& operator-=(int64_t value) { return *this -= BigInt(value);}

    BigInt operator+(const BigInt& other) const { BigInt result(*this); return result += other;}
    BigInt operator-(const BigInt& other) const { BigInt result(*this); return result -= other;}

    BigInt operator+(int64_t value) const { BigInt result(*this); return result += value;}
    BigInt operator-(int64_t value) const { BigInt result(*this); return result -= value;}

    bool operator<(const BigInt& other) const;
    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const { return !(*this == other);}
    bool operator<=(const BigInt& other) const { return (*this == other || *this < other);}
    bool operator>=(const BigInt& other) const { return !(*this < other);}
    bool operator>(const BigInt& other) const  { return !(*this <= other);}

    bool operator<(int64_t value) const  { return *this < BigInt(value);}
    bool operator==(int64_t value) const { return *this == BigInt(value);}
    bool operator!=(int64_t value) const { return !(*this == value);}
    bool operator<=(int64_t value) const { return *this <= BigInt(value);}
    bool operator>=(int64_t value) const { return !(*this < value);}
    bool operator>(int64_t value) const  { return !(*this <= value);}

    static BigInt abs(const BigInt& element) { return (element>=0) ? element: -element;}

private:
    Sign sign_;
    ReversedCustomVector<uint8_t> data_;
};

std::ostream& operator<<(std::ostream& out, const BigInt& element);

BigInt operator+(int64_t value, const BigInt& element);
BigInt operator-(int64_t value, const BigInt& element);

bool operator<(int64_t lhs, const BigInt& rhs);
bool operator==(int64_t lhs, const BigInt& rhs);
bool operator!=(int64_t lhs, const BigInt& rhs);
bool operator<=(int64_t lhs, const BigInt& rhs);
bool operator>=(int64_t lhs, const BigInt& rhs);
bool operator>(int64_t lhs, const BigInt& rhs);

#endif // BIGINT_H
