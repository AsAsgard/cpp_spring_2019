#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <iostream>
#include <iomanip>
#include <vector>

enum class Error
{
    NoError,
    CorruptedArchive
};


class SerializerGlobals {
protected:
    static constexpr char Separator = ' ';

    bool _boolalphaActive = true;

    explicit SerializerGlobals(bool boolalphaActive) : _boolalphaActive(boolalphaActive) {}

public:
    void setBoolalpha(bool checked) { _boolalphaActive = checked;}
};


class Serializer : public SerializerGlobals
{
public:
    explicit Serializer(std::ostream& out, bool boolalphaActive = true)
        : SerializerGlobals(boolalphaActive), _out(out)
    {
    }

    template <class T>
    Error save(T& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&&... args)
    {
        _out << std::noboolalpha;
        return process(std::forward<ArgsT>(args)...);
    }

private:
    std::ostream& _out;

    template<typename T, typename... Args>
    Error process(T&& first, Args&&... args)
    {
        if constexpr (std::is_same_v<std::remove_reference_t<T>, bool>) {
            if (_boolalphaActive) _out << std::boolalpha;
        }
        _out << first << Separator;
        _out << std::noboolalpha;
        return process(std::forward<Args>(args)...);
    }

    template<typename T>
    Error process(T&& last)
    {
        if constexpr (std::is_same_v<std::remove_reference_t<T>, bool>) {
            if (_boolalphaActive) _out << std::boolalpha;
        }
        _out << last << Separator;
        _out << std::noboolalpha;
        return Error::NoError;
    }
};


class Deserializer : public SerializerGlobals {

    struct delim : std::ctype<char> {

        delim(std::size_t refs = 0) : ctype(make_table(), false, refs) {}

        static const mask* make_table()
        {
            static std::vector<mask> v(classic_table(), classic_table() + table_size);
            v[' '] &= ~space;
            v[Separator] |= space;
            return &v[0];
        }
    };

public:
    explicit Deserializer(std::istream& in, bool boolalphaActive = true)
        : SerializerGlobals(boolalphaActive), _in(in)
    {
    }

    template <class T>
    Error load(T& object)
    {
        T tmp_obj;
        Error err = tmp_obj.serialize(*this);
        if (err == Error::NoError) {
            object = std::move(tmp_obj);
        }
        return err;
    }

    template<typename... Args>
    Error operator()(Args&&... args)
    {
        _in.imbue(std::locale(_in.getloc(), new delim));  // удаление delim происходит в деструкторе std::locale
        Error err = process(std::forward<Args>(args)...);
        _in.imbue(std::locale());
        return err;
    }

private:
    std::istream& _in;

    template<typename T, typename... Args>
    Error process(T&& first, Args&&... args)
    {
        if constexpr (std::is_unsigned_v<std::remove_reference_t<T>>) {
            while (_in.peek() == Separator) { _in.get();}
            if (_in.peek() == '-') return Error::CorruptedArchive;
        }
        if constexpr (std::is_same_v<std::remove_reference_t<T>, bool>) {
            if (_boolalphaActive) _in >> std::boolalpha;
        }

        _in >> first;

        _in >> std::noboolalpha;
        if (_in && _in.peek()==Separator) {
            return process(std::forward<Args>(args)...);
        } else {
            return Error::CorruptedArchive;
        }
    }

    template<typename T>
    Error process(T&& last)
    {
        if constexpr (std::is_unsigned_v<std::remove_reference_t<T>>) {
            while (_in.peek() == Separator) { _in.get();}
            if (_in.peek() == '-') return Error::CorruptedArchive;
        }
        if constexpr (std::is_same_v<std::remove_reference_t<T>, bool>) {
            if (_boolalphaActive) _in >> std::boolalpha;
        }

        _in >> last;

        _in >> std::noboolalpha;
        while (_in.peek() == Separator) { _in.get();}
        if (_in.eof()) {
            return Error::NoError;
        } else {
            return Error::CorruptedArchive;
        }
    }
};

#endif // SERIALIZER_H
