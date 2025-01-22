#ifndef FUP_SYNC_WRITER_HPP
#define FUP_SYNC_WRITER_HPP
#include "common.hpp"

namespace fup
{
    class sync_writer
    {
    private:
        std::vector<uint8_t> data;
    public:
        sync_writer();
        ~sync_writer();
    };
    
    sync_writer::sync_writer()
    {
    }
    
    sync_writer::~sync_writer()
    {
    }
    
} // namespace fup


#endif