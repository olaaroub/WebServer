#include "Methods.hpp"

int Methods::check_location(ServerConfigs serverconfigs, std::string uri)
{
    long max_length = -1;
    int right_index = -1;
    for (unsigned long i = 0; i < serverconfigs.locations.size() ; i++)
    {
        std::string path = serverconfigs.locations.at(i).path;
        if (uri.find(path) == 0)
        {
            if ((long)path.length() > max_length)
            {
                max_length = path.length();
                right_index = i;
            }

        }
    }
    return right_index;
}

bool Methods::check_method(std::string method , std::vector<std::string> allowed_methods)
{
    for (unsigned long i = 0; i < allowed_methods.size(); i++)
    {
        if (method == allowed_methods.at(i))
            return true;
    }
    return false;
}


Methods::Methods(ServerConfigs serverconfigs, Request request)
{
    location = check_location(serverconfigs, request.RequestLine.get_url());
    if (location == -1)
        return ;
    method = check_method(request.RequestLine.get_method(), serverconfigs.locations.at(location).allowed_methods);
    if (method == false)
        return ;

}