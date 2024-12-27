#ifndef EVENT_H
#define EVENT_H

// 模板事件类
template <typename... Args>
class Event
{
public:
    // 定义回调函数类型
    typedef void (*Callback)(Args...);

    // 构造函数中指定回调
    Event(Callback callback = nullptr) : callback(callback) {}

    // 注册或更改回调函数
    void registerCallback(Callback callback)
    {
        this->callback = callback;
    }

    // 触发事件
    void trigger(Args... args)
    {
        if (callback)
        {
            callback(args...); // 调用回调函数
        }
    }

private:
    Callback callback; // 存储回调函数指针
};
#endif // EVENT_H