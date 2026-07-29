#pragma once

// 프로그램 전체에서 하나만 존재해야 하는 객체를 위한 기반 클래스입니다.
template <typename T>
class Singleton
{
public:
    static T& GetInstance()
    {
        static T instance;
        return instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton() = default;
    ~Singleton() = default;
};
