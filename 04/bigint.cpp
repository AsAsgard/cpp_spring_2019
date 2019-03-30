#include "bigint.h"

BigInt::BigInt(int64_t value) : sign_(Sign::plus)
{
    if (value < 0)
    {
        this->sign_ = Sign::minus;
        value = std::abs(value);
    }
    do {
        this->data_.PushFront(value % 10);
    } while((value = value/10) != 0);
}

BigInt& BigInt::operator=(int64_t value)
{
    BigInt result(value);
    *this = std::move(result);
    return *this;
}

BigInt& BigInt::plus_assign_impl(BigInt& lhs, const BigInt& rhs)
{
    auto lit = lhs.data_.rbegin();
    auto rit = rhs.data_.rbegin();
    uint8_t tmpResult = 0;
    for (; lit != lhs.data_.rend() && rit != rhs.data_.rend(); ++lit, ++rit) {
        tmpResult += *lit + *rit;
        *lit = tmpResult % 10;
        tmpResult = (tmpResult >= 10) ? 1 : 0;
    }
    if (lit == lhs.data_.rend() && rit == rhs.data_.rend()) {
        if (tmpResult == 1) lhs.data_.PushFront(tmpResult);
    } else if (lit == lhs.data_.rend()) {
        while (rit != rhs.data_.rend()) {
            tmpResult += *rit;
            lhs.data_.PushFront(tmpResult % 10);
            tmpResult = (tmpResult >= 10) ? 1 : 0;
            ++rit;
        }
        if (tmpResult != 0) lhs.data_.PushFront(tmpResult);
    } else {
        while (tmpResult != 0 && lit != lhs.data_.rend()) {
            tmpResult += *lit;
            *lit = tmpResult % 10;
            tmpResult = (tmpResult >= 10) ? 1 : 0;
            ++lit;
        }
        if (tmpResult != 0) lhs.data_.PushFront(tmpResult);
    }
    return lhs;
}

BigInt& BigInt::minus_assign_impl(BigInt& lhs, const BigInt& rhs)
{
    if (abs(lhs) == abs(rhs)) {
        lhs = BigInt(0);
        return lhs;
    }
    BigInt bigger;
    const BigInt * smaller_ptr = nullptr;
    if (abs(lhs) > abs(rhs)) {
        bigger = lhs;
        smaller_ptr = &rhs;
    } else {
        bigger = rhs;
        bigger.sign_ = (lhs.sign_ == Sign::plus) ? Sign::minus : Sign::plus;
        smaller_ptr = &lhs;
    }
    const BigInt& smaller = *smaller_ptr;
    auto bit = bigger.data_.rbegin();
    auto sit = smaller.data_.rbegin();
    int8_t tmpResult = 0;
    for (; bit != bigger.data_.rend() && sit != smaller.data_.rend(); ++bit, ++sit) {
        tmpResult += *bit - *sit;
        *bit = (tmpResult >= 0) ? tmpResult : tmpResult + 10;
        tmpResult = (tmpResult >= 0) ? 0 : -1;
    }
    while (tmpResult != 0 && bit != bigger.data_.rend()) {
        tmpResult += *bit;
        *bit = (tmpResult >= 0) ? tmpResult : tmpResult + 10;
        tmpResult = (tmpResult >= 0) ? 0 : -1;
        ++bit;
    }
    for (auto it = bigger.data_.begin(); it != bigger.data_.end() && *it == 0; ++it) {
        bigger.data_.PopFront();
    }
    lhs = std::move(bigger);
    return lhs;
}

BigInt& BigInt::operator+=(const BigInt& other)
{
    if (this->sign_ != other.sign_) {
        return minus_assign_impl(*this, other);
    } else {
        return plus_assign_impl(*this, other);
    }
}
BigInt& BigInt::operator-=(const BigInt& other)
{
    if (this->sign_ == other.sign_) {
        return minus_assign_impl(*this, other);
    } else {
        return plus_assign_impl(*this, other);
    }
}

bool BigInt::operator<(const BigInt& other) const
{
    if (this->sign_ != other.sign_) {
        if (this->sign_ == Sign::plus) {
            return false;
        } else {
            return true;
        }
    }
    bool signCheck = (this->sign_ == Sign::plus);
    bool valueCheck = false;
    if (this->data_.Size() > other.data_.Size()) {
         valueCheck = false;
    } else if (this->data_.Size() < other.data_.Size()) {
         valueCheck = true;
    } else {
        for (size_t i = 0; i < this->data_.Size(); ++i)
        {
            if (this->data_[i] < other.data_[i]) {
                valueCheck = true;
                break;
            } else if (this->data_[i] > other.data_[i]) {
                valueCheck = false;
                break;
            }
            if (i == this->data_.Size() - 1) valueCheck = !signCheck;
        }
    }
    return (signCheck == valueCheck);
}
bool BigInt::operator==(const BigInt& other) const
{
    if (this->data_.Size() != other.data_.Size() || this->sign_ != other.sign_) {
        return false;
    } else {
        for (size_t i = 0; i < this->data_.Size(); ++i)
        {
            if (this->data_[i] != other.data_[i]) return false;
        }
        return true;
    }
}

BigInt BigInt::operator-() const
{
    BigInt result(*this);
    result.sign_ = (result.sign_ == Sign::minus || result == 0)
                  ? Sign::plus
                  : Sign::minus;
    return result;
}

std::ostream& operator<<(std::ostream& out, const BigInt& element)
{
    if (element.sign_ == BigInt::Sign::minus) {
        out << '-';
    }
    for (unsigned digit : element.data_) {
        out << digit;
    }
    return out;
}

BigInt operator+(int64_t value, const BigInt& element) { return element + value;}
BigInt operator-(int64_t value, const BigInt& element) { return -(element - value);}

bool operator<(int64_t lhs, const BigInt& rhs)  { return rhs >= lhs;}
bool operator==(int64_t lhs, const BigInt& rhs) { return rhs == lhs;}
bool operator!=(int64_t lhs, const BigInt& rhs) { return rhs != lhs;}
bool operator<=(int64_t lhs, const BigInt& rhs) { return rhs > lhs;}
bool operator>=(int64_t lhs, const BigInt& rhs) { return rhs < lhs;}
bool operator>(int64_t lhs, const BigInt& rhs)  { return rhs <= lhs;}
