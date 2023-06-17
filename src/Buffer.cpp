//
// Created by wuyifei0305 on 23-5-20.
//

#include "Buffer.h"
#include <cerrno>
#include <sys/uio.h>
#include <unistd.h>

void Buffer::make_space(std::size_t len) {
    if(writable_bytes()+prependable_bytes() < len + kCheapPrepend)
        buffer_.resize(writer_index+len);
    else{
        std::size_t readable = readable_bytes();
        std::copy(buffer_begin()+reader_index, buffer_begin()+writer_index, buffer_begin()+kCheapPrepend);
        reader_index = kCheapPrepend;
        writer_index = reader_index + readable;
    }
}

void Buffer::append(const char *data, std::size_t len) {
    ensure_writeable_bytes(len);
    std::copy(data, data+len, begin_write());
    writer_index += len;
}

void Buffer::retrieve(std::size_t len) {
    if (len < readable_bytes())
        reader_index += len; // 应用只读取了readable缓冲区数据的一部分，就是len，还剩下reader_index += len -> writer_index
    else   // len == readable_bytes()
        retrieve_all();
}

std::string Buffer::retrieve_as_string(std::size_t len) {
    std::string result{peek(), peek()+len};
    retrieve(len); // 上面一句把缓冲区中可读的数据，已经读取出来，这里肯定要对缓冲区进行复位操作
    return result;
}

ssize_t Buffer::read_fd(int fd, int *save_errno) {
    char extrabuf[65536]{0}; // 64k
    size_t extrabuf_len = sizeof(extrabuf);
    iovec buf_vec[2];
    const std::size_t writeable = writable_bytes();
    buf_vec[0].iov_base = &*buffer_begin()+writer_index;
    buf_vec[0].iov_len = writeable;
    buf_vec[1].iov_base = extrabuf;
    buf_vec[1].iov_len = extrabuf_len;
    const int iovcnt = (writeable < extrabuf_len)?2:1;
    const ssize_t n = ::readv(fd, buf_vec, iovcnt);
    if(n<0)
        *save_errno = errno;
    else if(n <= writeable)
        writer_index += n;
    else {
        writer_index = buffer_.size();
        append(extrabuf, static_cast<std::size_t>(n) - writeable);
    }
    return n;
}


ssize_t Buffer::write_fd(int fd, int *save_errno) {
    ssize_t n = ::write(fd, &*peek(), readable_bytes());
    if(n<0)
        *save_errno = errno;
    return n;
}

