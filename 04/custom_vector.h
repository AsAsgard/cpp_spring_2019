#pragma once

#include <cstdlib>
#include <iterator>
#include <algorithm>

template <typename T>
class CustomVector {
public:
  CustomVector() : data_(nullptr), size_(0), capacity_(0) {}
  explicit CustomVector(size_t size) : data_(new T[size]), size_(size), capacity_(size) {}
  CustomVector(const CustomVector& other)
      : data_(nullptr)
      , size_(other.size_)
      , capacity_(other.capacity_)
  {
      if (other.data_) {
        data_ = new T[other.capacity_];
        std::copy(other.begin(), other.end(), begin());
      }
  }
  CustomVector(CustomVector&& other)
        : data_(other.data_)
        , size_(other.size_)
        , capacity_(other.capacity_)
  {
      other.data_ = nullptr;
      other.size_ = 0;
      other.capacity_ = 0;
  }
  ~CustomVector() { delete [] data_;}

  CustomVector& operator=(const CustomVector& other)
  {
      if (this != &other) {
          if (other.size_ <= capacity_) {
              if (other.size_ > 0) {
                  std::copy(other.begin(), other.end(), begin());
                  size_ = other.size_;
              } else {
                  delete [] data_;
                  data_ = nullptr;
                  size_ = 0;
                  capacity_ = 0;
              }
          } else {
              CustomVector tmp(other);
              std::swap(tmp.data_, data_);
              std::swap(tmp.size_, size_);
              std::swap(tmp.capacity_, capacity_);
          }
      }
      return *this;
  }
  CustomVector& operator=(CustomVector&& other)
  {
      if (this != &other) {
          delete [] data_;
          data_ = other.data_;
          size_ = other.size_;
          capacity_ = other.capacity_;

          other.data_ = nullptr;
          other.size_ = 0;
          other.capacity_ = 0;
      }
      return *this;
  }

  T& operator[](size_t index) { return data_[index];}
  const T& operator[](size_t index) const { return data_[index];}

  T* begin() { return data_;}
  T* end() { return data_ + size_;}
  const T* begin() const { return data_;}
  const T* end() const { return data_ + size_;}

  auto rbegin() { return std::make_reverse_iterator(end());}
  auto rend() { return std::make_reverse_iterator(begin());}
  auto rbegin() const { return std::make_reverse_iterator(end());}
  auto rend() const { return std::make_reverse_iterator(begin());}

  size_t Size() const { return size_;}
  size_t Capacity() const { return capacity_;}

  void PushBack(const T& value)
  {
      if (size_ == capacity_) {
          if (size_ == 0)
              ++capacity_;
          else
              capacity_ *= 2;
          T* new_data_ = new T[capacity_];
          if (size_ != 0)
              std::copy(data_, data_ + size_, new_data_);
          delete [] data_;
          data_ = new_data_;
      }
      data_[size_] = value;
      ++size_;
  }

  void PushBack(T&& value)
  {
      if (size_ == capacity_) {
          if (size_ == 0)
              ++capacity_;
          else
              capacity_ *= 2;
          T* new_data_ = new T[capacity_];
          if (size_ != 0)
              std::move(data_, data_ + size_, new_data_);
          delete [] data_;
          data_ = new_data_;
      }
      data_[size_] = std::move(value);
      ++size_;
  }

  void PopBack()
  {
      if (size_ > 0) {
          --size_;
      } else {
          throw std::underflow_error("");
      }
  }

private:

  T* data_;
  std::size_t size_;
  std::size_t capacity_;
};


template<typename T>
class ReversedCustomVector {
public:
    ReversedCustomVector() : data_() {}
    explicit ReversedCustomVector(size_t size) : data_(size) {}
    ReversedCustomVector(const ReversedCustomVector& other) : data_(other.data_) {}
    ReversedCustomVector(ReversedCustomVector&& other) : data_(std::move(other.data_)) {}
    ~ReversedCustomVector() {}

    ReversedCustomVector& operator=(const ReversedCustomVector& other) { if (this != &other) data_ = other.data_; return *this;}
    ReversedCustomVector& operator=(ReversedCustomVector&& other) { if (this != &other) data_ = std::move(other.data_); return *this;}

    T& operator[](size_t index) { return data_[data_.Size() - index - 1];}
    const T& operator[](size_t index) const { return data_[data_.Size() - index - 1];}

    auto begin() { return data_.rbegin();}
    auto end() { return data_.rend();}
    auto begin() const { return data_.rbegin();}
    auto end() const { return data_.rend();}

    T* rbegin() { return data_.begin();}
    T* rend() { return data_.end();}
    const T* rbegin() const { return data_.begin();}
    const T* rend() const { return data_.end();}

    size_t Size() const { return data_.Size();}
    size_t Capacity() const { return data_.Capacity();}

    void PushFront(const T& value)
    {
        data_.PushBack(value);
    }

    void PushFront(T&& value)
    {
        data_.PushBack(std::move(value));
    }

    void PopFront()
    {
        data_.PopBack();
    }

private:
    CustomVector<T> data_;
};
