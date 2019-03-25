#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <stdexcept>
#include <utility>

template<typename T>
class MatrixT {

    class VectorT {
        std::vector<T> _line;
    public:
        T& operator[](size_t pos) { return _line.at(pos);}
        const T& operator[](size_t pos) const { return _line.at(pos);}

        void resize(size_t size, T value = T()) { _line.resize(size, value);}

        auto begin() { return _line.begin();}
        auto begin() const { return _line.begin();}
        auto end() { return _line.end();}
        auto end() const { return _line.end();}

        bool operator==(const VectorT& other) const { return this->_line == other._line;}
        bool operator!=(const VectorT& other) const { return !(*this == other);}
    };

public:
    using value_type = T;

    explicit MatrixT(size_t rows, size_t cols)
        : _rows(rows)
        , _cols(cols)
    {
        static_assert(std::is_arithmetic_v<T>, "The type should be arithmetic!");
        if (_rows >= 0 && _cols >= 0)
        {
            _matrix.resize(rows);
            for (auto& line : _matrix)
            {
                line.resize(cols);
            }
        } else {
            throw std::invalid_argument("Invalid rows or cols value in constructor of Matrix!");
        }
    }

    size_t getRows() const { return _rows;}
    size_t getColumns() const { return _cols;}

    bool operator==(const MatrixT& other) const
    {
        if (this == &other) return true;
        if (this->_rows != other._rows ||
            this->_cols != other._cols) return false;

        for (size_t i = 0; i < _rows; ++i)
        {
            if ((*this)[i] != other[i]) return false;
        }
        return true;
    }
    bool operator!=(const MatrixT& other) const { return !(*this == other);}

    MatrixT& operator*=(T value)
    {
        for (auto& line : this->_matrix) {
            for (auto& element : line) {
                element *= value;
            }
        }
        return *this;
    }

    MatrixT operator*(T value) const
    {
        MatrixT result = *this;
        result *= value;
        return result;
    }

    MatrixT::VectorT& operator[](size_t pos) { return _matrix.at(pos);}
    const MatrixT::VectorT& operator[](size_t pos) const { return _matrix.at(pos);}

private:
    std::vector<VectorT> _matrix;
    size_t _rows;
    size_t _cols;
};

template<typename T>
MatrixT<T> operator*(T value, const MatrixT<T>& matrix)
{
    return matrix * value;
}

using Matrix = MatrixT<int>;

#endif // MATRIX_H
