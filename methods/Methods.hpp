#ifndef METHODS_HPP
#define METHODS_HPP

#include "Delete.hpp"
#include "Get.hpp"
#include "Post.hpp"
#include <iostream>

class Methods
{
    private:
        Post post;
        Get get;
        Delete Delete;
    public:
        void response(std::string MethodName) {}
};


#endif