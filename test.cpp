#include <iostream>
#include <string>


int main()
{
    int a = 1;
    int b = 3;
    //cin >> a >> b; cin.ignore();
    int length = 5;
    //cin >> length; cin.ignore();
    std::string text = "HELLO";
    //getline(cin, text);
    int i = 0;
    std::string res;
    while(text[i] )
    {
     
	    std::cout << (char)(((((text[i] - 'A') * a) + b) % 29 + 'A'));
	   
        i++;
    }
}
