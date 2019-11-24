
class storaged_value
{
    typedef int size;

private:
    void *buf;
    size buf_size;

public:
    storaged_value();
    storaged_value(size _size);
    ~storaged_value();
};