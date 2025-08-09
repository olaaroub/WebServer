
#include "Bureaucrat.hpp"


int main()
{

    try {
    Bureaucrat b1(5,"b1");
    std::cout << b1 << std::endl;
    b1.increment_grade();
    std::cout << b1 << std::endl;
    b1.increment_grade();
    std::cout << b1 << std::endl;
    b1.increment_grade();
    std::cout << b1 << std::endl;
    b1.increment_grade();
    std::cout << b1 << std::endl;
    b1.increment_grade();
    std::cout << b1 << std::endl;
    } catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    // -------- constructer error --- //
    try
    { Bureaucrat b2(156, "b2");} 
    catch(std::exception &e)
    { std::cout << e.what() << std::endl;}
  
}

