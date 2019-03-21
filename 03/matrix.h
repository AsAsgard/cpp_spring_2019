#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <stdexcept>

template<typename T>
class MatrixT {

    class VectorT {
        std::vector<T> _line;
    public:
        T& operator[](int pos) { return _line.at(pos);}
        const T& operator[](int pos) const { return _line.at(pos);}

        void resize(size_t size, T value = T()) { _line.resize(size, value);}

        typename std::vector<T>::iterator begin() { return _line.begin();}
        typename std::vector<T>::const_iterator begin() const { return _line.begin();}
        typename std::vector<T>::iterator end() { return _line.end();}
        typename std::vector<T>::const_iterator end() const { return _line.end();}

        bool operator==(const VectorT& other) const { return this->_line == other._line;}
        bool operator!=(const VectorT& other) const { return !(*this == other);}
    };

public:
    using value_type = T;

    explicit MatrixT(int rows, int cols)
        : _rows(rows)
        , _cols(cols)
    {
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

    int getRows() const { return _rows;}
    int getColumns() const { return _cols;}

    bool operator==(const MatrixT& other) const
    {
        if (this == &other) return true;
        if (this->_rows != other._rows ||
            this->_cols != other._cols) return false;

        for (int i = 0; i < _rows; ++i)
        {
            if ((*this)[i] != other[i]) return false;
        }
        return true;
    }
    bool operator!=(const MatrixT& other) const { return !(*this == other);}

    const MatrixT& operator*=(T value)
    {
        for (auto& line : _matrix) {
            for (auto& element : line) {
                element *= value;
            }
        }
        return *this;
    }

    MatrixT operator*(T value) const
    {
        MatrixT result = MatrixT(_rows, _cols);
        for (int i = 0; i != _rows; ++i)
        {
            for (int j = 0; j != _cols; ++j)
            {
                result[i][j] = (*this)[i][j] * value;
            }
        }
        return result;
    }

    MatrixT::VectorT& operator[](int pos) { return _matrix.at(pos);}
    const MatrixT::VectorT& operator[](int pos) const { return _matrix.at(pos);}

private:
    std::vector<VectorT> _matrix;
    int _rows;
    int _cols;
};

template<typename T>
MatrixT<T> operator*(T value, const MatrixT<T>& matrix)
{
    return matrix * value;
}

using Matrix = MatrixT<int>;

#endif // MATRIX_H
