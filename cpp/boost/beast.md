## [Index](../README.md)

# Boost.Beast

- parse (read):
    - read from user buffer seq into message rep
    - read from network stream into message rep
- serialize (write):
    - write from message rep into network stream
    - write from message rep into user buffer seq

## HTTP

Interfaces for operating on HTTP messages are structured into several layers. The highest level provides ease of use, while lower levels provide progressively more control, options, and flexibility. At the lowest level customization points are provided, where user defined types can replace parts of the implementation. 

- User I/O with Message
    - Body:
        - BodyReader: Write to HTTP message body (parse: buffer sequence -> body rep)
        - BodyWriter: Write from HTTP message body to buffer sequences (serialize: body rep -> buffer sequence)
        - example:
            - `message::body_type::BodyReader`
                - construct with `header<isRequest, Fields>&` and `Body::value_type`
                - init
                - `std::size_t put(ConstBufferSequence const& buffers, error_code& ec)`
            - file body
                - BodyReader: write buffer sequence into file content
                - BodyWriter: file path -> buffer sequence of file content
    - With Body types provided by Beast, we typically don't need to interact with these two directly
        - https://www.boost.org/doc/libs/1_87_0/libs/beast/doc/html/beast/more_examples/send_child_process_output.html
- Network I/O with Message: layered streams below

### Layer 6: Message

- stream operation
    ```cpp
    async_write_some(stream, m);
    ```

### Layer 5: Serializer, Parser

- Non-trivial algorithms need to do more than send/receive entire messages at once, such as:
    - Send/receive the header first, and the body later.
        - https://www.boost.org/doc/libs/1_87_0/libs/beast/doc/html/beast/more_examples/expect_100_continue_client.html
    - Send/receive a message incrementally: bounded work in each I/O cycle.
    - ...
```cpp
/// Provides buffer oriented HTTP message serialization functionality.
template<
    bool isRequest,
    class Body,
    class Fields = fields
>
class serializer;
```

- stream operation
    ```cpp
    template<
        class SyncWriteStream,
        bool isRequest, class Body, class Fields>
    void
    send(
        SyncWriteStream& stream,
        message<isRequest, Body, Fields> const& m)
    {
        // Create the instance of serializer for the message
        serializer<isRequest, Body, Fields> sr{m};

        // Loop until the serializer is finished
        do
        {
            // This call guarantees it will make some
            // forward progress, or otherwise return an error.
            write_some(stream, sr);
        }
        while(! sr.is_done());
    }
    ```
- buffer-oriented serializing (invoked directly)

## WebSocket