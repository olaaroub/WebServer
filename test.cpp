#include <iostream>


class test{
    public : 
    test()
    {
        std::cout << "ddd" << std::endl;
        return ;
        std::cout << "eee" << std::endl;
    }
};

void tt(){
     test t;
    std::cout << "continue after ! " << std::endl;
}

int main()
{
   tt();
   std::cout << "reach end  " << std::endl;
}