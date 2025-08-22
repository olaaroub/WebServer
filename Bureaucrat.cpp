#include "Bureaucrat.hpp"
#include "Form.hpp"

// Orthodox Canonical Form //
Bureaucrat::Bureaucrat(void) : name("default"), grade(150)
{
    std::cout << "Default constructer Called !" << std::endl;
}

Bureaucrat::Bureaucrat(int grade, std::string name) : name(name)
{
    if (grade < 1)
        throw GradeTooHighException();
    else if (grade > 150)
        throw GradeTooLowException();
    this->grade = grade;
    std::cout << "Parameterized Constructor Called !" << std::endl;
}

Bureaucrat::Bureaucrat(const Bureaucrat &other_bureaucrat) : name(other_bureaucrat.getName())
{
    std::cout << "Copy constructor Called !" << std::endl;
    this->grade = other_bureaucrat.getGrade();
}

Bureaucrat &Bureaucrat::operator=(const Bureaucrat &other_bureaucrat)
{
    std::cout << "Copy assignment operator Called !" << std::endl;
    if (this == &other_bureaucrat)
        return (*this);
    this->grade = other_bureaucrat.getGrade();
    return (*this);
}

Bureaucrat::~Bureaucrat()
{
    std::cout << "Destructer Called !" << std::endl;
}
// ---------- //

int Bureaucrat::getGrade() const
{
    return this->grade;
}

std::string Bureaucrat::getName() const
{
    return this->name;
}

void Bureaucrat::increment_grade()
{
    if (grade == 1)
        throw GradeTooHighException();
    this->grade--;
}

void Bureaucrat::decrement_grade()
{
    if (grade == 150)
        throw GradeTooLowException();
    this->grade++;
}


void Bureaucrat::signForm(Form &form)
{
    try {
    form.beSigned(*this);
    std::cout << this->getName() << " signed " << form.get_name() << std::endl;
    }catch (std::exception &e)
    {
        std::cout << this->getName() << " couldn’t sign " << form.get_name()
        << " because the bureaucrat’s grade is low than required grade ." << std::endl;
    }
   
    
}



std::ostream &operator<<(std::ostream &os, const Bureaucrat &bureaucrat)
{
    os << bureaucrat.getName() << ", bureaucrat grade " << bureaucrat.getGrade();
    return os;
}
