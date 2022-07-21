#ifndef __COROGEN__
#define __COROGEN__
#include <coroutine>
#include <exception>
#include <iostream>
#include <optional>
template <typename T>
class Generator
{
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    using value_type  = std::optional<T>;
    Generator(handle_type h)
        : handle_(h)
    {
        std::cout << __FUNCTION__ << "\n";
    }
    Generator(Generator&& other) noexcept
        : handle_(other.handle_)
    {
        std::cout << __FUNCTION__ << "\n";
        other.handle_ = nullptr;
    }

    // move operator = ...

    ~Generator()
    {
        std::cout << __FUNCTION__ << "\n";
        if (handle_)
            handle_.destroy();
    }
    Generator(const Generator&)            = delete;
    Generator& operator=(const Generator&) = delete;
    class promise_type
    {
    public:
        Generator get_return_object()
        {
            std::cout << __FUNCTION__ << "\n";
            return Generator(handle_type::from_promise(*this));
        }
        std::suspend_always initial_suspend()
        {
            std::cout << __FUNCTION__ << "\n";
            return {};
        }
        std::suspend_always final_suspend() noexcept
        {
            std::cout << __FUNCTION__ << "\n";
            return {};
        }
        void unhandled_exception()
        {
            std::cout << __FUNCTION__ << "\n";
            exit(1);
        }
        void return_void() { std::cout << __FUNCTION__ << "\n"; }
        auto yield_value(const T value)
        {
            std::cout << __FUNCTION__ << "\n";
            value_ = value;
            return std::suspend_always{};
        }

    private:
        friend class Generator;
        T value_;
    };

    class iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;

        iterator() = delete;
        iterator(Generator<T>* generator)
            : generator{ generator }
        {
            std::cout << "iterator: " << __FUNCTION__ << "\n";
        }

        iterator& operator++()
        {
            std::cout << "iterator: " << __FUNCTION__ << "\n";
            generator->handle_.resume();
            if (generator && generator->handle_.done())
                generator = nullptr;

            return *this;
        }

        bool operator==(const iterator&) const
        {
            std::cout << "iterator: " << __FUNCTION__ << "\n";
            return generator ? generator->handle_.promise().finished() : true;
        }
        bool operator!=(iterator other) const { return generator != other.generator; }
        T    operator*()
        {
            std::cout << "iterator: " << __FUNCTION__ << "\n";
            return generator->handle_.promise().value_;
        }

    private:
        Generator<T>* generator{};
    };
    T getValue()
    {
        std::cout << __FUNCTION__ << "\n";
        return handle_.promise().current_value();
    }
    bool next()
    {
        std::cout << __FUNCTION__ << "\n";
        handle_.resume();
        return !handle_.done();
    }
    iterator begin()
    {
        std::cout << __FUNCTION__ << "\n";
        return iterator(handle_.done() ? nullptr : this);
    }
    iterator end()
    {
        std::cout << __FUNCTION__ << "\n";
        return iterator(nullptr);
    }

private:
    friend class iterator;
    handle_type handle_;
};

#endif //__COROGEN__