//
// Created by wuyifei0305 on 23-5-20.
//

#pragma once

#include "noncopyable.h"
#include <vector>
#include <string>
#include <algorithm>

class Buffer :noncopyable{
public:
    using vector_iter = std::vector<char>::iterator;
    static constexpr std::size_t kCheapPrepend{8};
    static constexpr std::size_t kInitialSize{1024};
    explicit Buffer(std::size_t initialSize=kInitialSize): reader_index(kCheapPrepend), writer_index(kCheapPrepend), buffer_(kCheapPrepend+initialSize){}
    inline std::size_t readable_bytes() const{
        return writer_index - reader_index;
    }
    inline std::size_t writable_bytes() const{
        return buffer_.size() - writer_index;
    }
    inline std::size_t prependable_bytes() const{
        return reader_index;
    }
    inline vector_iter peek() {
        return buffer_begin()+reader_index;
    }
    inline void retrieve_all(){
        writer_index = kCheapPrepend;
        reader_index = kCheapPrepend;
    }
    void retrieve(std::size_t len);
    std::string retrieve_as_string(std::size_t len);
    inline std::string retrieve_all_as_string(){
        return retrieve_as_string(readable_bytes()); // 应用可读取数据的长度
    }
    inline void ensure_writeable_bytes(std::size_t len){
        if (writable_bytes() < len)
            make_space(len); // 扩容函数
    }
    void append(const char* data, std::size_t len);
    inline vector_iter begin_write(){
        return buffer_begin() + writer_index;
    }
    ssize_t read_fd(int fd, int* save_errno);
    ssize_t write_fd(int fd, int* save_errno);
private:
    std::vector<char> buffer_;
    std::size_t reader_index;
    std::size_t writer_index;
    inline vector_iter buffer_begin(){
        return buffer_.begin();
    }
    void make_space(std::size_t);

};

