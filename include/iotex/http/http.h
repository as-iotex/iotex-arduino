#ifndef HTTP_H
#define HTTP_H

#include <memory>
#include <string>

namespace Iotex
{
    class IHTTP 
    {
        protected:
        IHTTP() = default;

        public:
        virtual ~IHTTP() {}

        virtual std::string get(const char* request) = 0;
        virtual std::string post(const char* request, const char* body) = 0;
    };

    class AbstractHTTP : public IHTTP 
    {
        protected:
            AbstractHTTP() = default;
            AbstractHTTP(AbstractHTTP&&) = delete;
            AbstractHTTP& operator=(AbstractHTTP&&) = delete;
            AbstractHTTP& operator=(const AbstractHTTP& other) = default;

        public:
            virtual ~AbstractHTTP(){};
    };

    std::unique_ptr<IHTTP> makeHTTP();
    
}  // namespace Iotex

#endif
