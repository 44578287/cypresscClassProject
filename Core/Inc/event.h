#ifndef EVENT_H
#define EVENT_H

// Template Event Class
template <typename... Args>
class Event
{
public:
    // Defining Callback Function Types
    typedef void (*Callback)(Args...);

    // Specifying callbacks in constructors
    Event(Callback callback = nullptr) : callback(callback) {}

    // Registering or changing callback functions
    void registerCallback(Callback callback)
    {
        this->callback = callback;
    }

    // Trigger event
    void trigger(Args... args)
    {
        if (callback)
        {
            callback(args...); // Invoke the callback function
        }
    }

private:
    Callback callback; // Store the callback function pointer
};
#endif // EVENT_H