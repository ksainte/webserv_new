#include "../../inc/Tokenizer.hpp"
#include <iostream>
#include <fstream>

#include "../../inc/Logger.hpp"
#include "../../inc/constants/ErrorMessages.hpp"
#include "../../inc/types/TokenType.hpp"

int Tokenizer::printError(int err)
{
    if (err == INPUT)
        std::cerr << "\nError: could not open input file\n";
    if (err == BAD_SEMICOLONS)
        std::cerr << "\nSyntax Error: Misuse of Semicolons!\n";
    if (err == BRACKETS)
        std::cerr << "\nSyntax Error: Misuse of Brackets!\n";
    if (err == ORDER)
        std::cerr << "\nSyntax Error: Wrong Token Order!\n";
    if (err == BAD_LOCATION)
        std::cerr << "\nSyntax Error: Wrong Token after Location!\n";
    if (err == BAD_DIRECTIVE)
        std::cerr << "\nSyntax Error: String is not a valid Directive!\n";
    if (err == DIRECTIVE_INCOMPLETE)
        std::cerr << "\nSyntax Error: The Directive is Incomplete!\n";
    if (err == NO_LISTEN)
        std::cerr << "\nSyntax Error: There is no Listen directive in one of the Server Block(s)!\n";
    if (err == NOT_SERVER)
        std::cerr << "\nSyntax Error: A Server Block must start with server Header!\n";
    if (err == NOT_STRING)
        std::cerr << "\nSyntax Error: A Directive can only contain Strings!\n";
    if (err == ONLY_ONE)
        std::cerr << "\nSyntax Error: This Directive can only hold ONE STRING!\n";
    if (err == ONLY_TWO)
        std::cerr << "\nSyntax Error: This Directive can only hold TWO STRINGS!\n";
    if (err == NO_STRING)
        std::cerr << "\nSyntax Error: This Directive must as least hold ONE STRING!\n";
    if (err == LOCATION_STRUCT)
        std::cerr << "\nSyntax Error: A Location can only be followed by One String!!\n";
    if (err == INVALID_METHOD)
        std::cerr << "\nSyntax Error: The allowed methods are GET, POST, and DELETE!\n";
    if (err == DBL_METHOD)
        std::cerr << "\nSyntax Error: There is a doublure in the type of methods!\n";
    if (err == CONFIG_EMPTY)
        std::cerr << "\nError: Config File is empty!\n";
    return (0);
}

std::string	ft_get_value(std::string s1)
{
    int i;

    i = 0;
    if (s1[i] == '{' || s1[i] == '}' || s1[i] == ';')
    {
        s1 = s1.substr(0, 1);
        return (s1);
    }
    while(s1[i])
    {
        if ((s1[i] <= 12 && s1[i] >= 9) || s1[i] == 32 || s1[i] == '{' || s1[i] == '}' || s1[i] == ';')
            break;
        i++;
    }
    s1 = s1.substr(0, i);
    return(s1);
}

Token::TokenType	ft_get_type_token(std::string s1)
{
	if (s1[0] == '{')
		return (Token::LBRACE);
    if (s1[0] == '}')
		return (Token::RBRACE);
    if (s1[0] == ';')
		return (Token::SEMICOLON);
    if (s1.compare("location") == 0)
		return (Token::LOCATION);
    if (s1.compare("server") == 0)
		return (Token::SERVER);
	return (Token::STRING);
}

void Tokenizer::ft_push_token(std::string s1)
{
    Token::TokenType type;
    std::string value;
    std::list<Token>::iterator it;

    value = ft_get_value(s1);
    _len = value.length();
    type = ft_get_type_token(value);
    _tokens_list.push_back(Token(type, value));
}

int ft_check_brackets(Token::TokenType type, int &left_brackets)
{       
    if (type == Token::LBRACE)
    {
        left_brackets++;
        if (left_brackets > 2)
            return (0);
    }
    if (type == Token::RBRACE)
    {
        left_brackets--;
        if (left_brackets < 0)
            return (0);
    }
    return (1);
}

int Tokenizer::ft_check_basic_syntax()
{
    std::list<Token>::iterator it;
    Token::TokenType t1;
    Token::TokenType t2;
    int left_brackets;

    left_brackets = 0;
    int size =  _tokens_list.size();
    if (!size)
        return (printError(CONFIG_EMPTY));
    std::cout << "mylist contains:\n";
    it = _tokens_list.begin();
    while (it != _tokens_list.end())
    {
        std::cout << "type is " << (*it).type;
        std::cout << "\nvalue is " << (*it).value;
        t2 = (*it).type;
        if (t2 == Token::LBRACE && !(ft_check_brackets(t2, left_brackets)))
            return (printError(BRACKETS));
        if (t2 == Token::RBRACE && !(ft_check_brackets(t2, left_brackets)))
            return (printError(BRACKETS));
        if (it != _tokens_list.begin())
        {
            --it;
            std::cout << "\nThe previous element is " << (*it).type << '\n';
            t1 = (*it).type;
            ++it;
            if (t1 == t2 && t1 == Token::SEMICOLON)
                return (printError(BAD_SEMICOLONS));
            if ((t1 == Token::SEMICOLON && t2 == Token::LBRACE) || ((t1 == Token::RBRACE || t1 == Token::LBRACE) && t2 == Token::SEMICOLON))
                return (printError(BAD_SEMICOLONS));
        }
        std::cout << "\nnext token:\n";
        it++;
        // std::cout << "left brackets are " <<left_brackets << "\n";
    }
    std::cout << "----------------";
    if (left_brackets == 0)
        return (1);
    return (printError(Token::LBRACE));
}

int Tokenizer::ft_tokenize(const std::string s1)
{
    std::string temp;

    int i;

    i = 0;
    _len = 0;
	while (s1[i])
	{
		if ((s1[i] <= 12 && s1[i] >= 9) || s1[i] == 32)
			i++;
		else
		{
            temp = s1.substr(i);
            if (temp[0] == '#')
                break;
			ft_push_token(temp);
		}
		i = i + _len;
		_len = 0;
	}
    return (1);
}

int Tokenizer::ft_compare_with_table(std::string value, std::list<Token>::iterator &it, int flag_location_block)
{
    const std::string server_block[10] = {"listen", "host", "port", "server_name", "error_page", "client_max_body_size", "return", "root", "index", "autoindex"};
    const std::string location_block[9] = {"error_page", "client_max_body_size", "method", "return", "root", "index", "autoindex", "cgi_pass", "cgi_params"};
    int i;

    i = 0;
    if (flag_location_block == 0)
    {
        while (i < 10)
        {
            if (value == server_block[i])
            {
                std::cout << "Matched directive in server block!\n";
                (*it).type = Token::DIRECTIVE;
                std::cout << "New type is " << (*it).type << "\n";
                return (1);
            }
            i++;
        }
    }
    else
    {
        while (i < 9)
        {
            if (value == location_block[i])
            {
                std::cout << "Matched directive in location block!\n";
                (*it).type = Token::DIRECTIVE;
                std::cout << "New type is " << (*it).type << "\n";
                return (1);
            }
            i++;
        }
    }
    if (flag_location_block == 0)
        std::cout << "\nSever block issue: " << (*it).value << "\n";
    else
        std::cout << "\nLocation block issue: " << (*it).value << "\n";
    return (0);
}

int Tokenizer::ft_valid_values_after_directive(std::list<Token>::iterator &it, std::string t1_value)
{
    const std::string one_string[7] = {"listen", "host", "port", "client_max_body_size", "root", "autoindex", "cgi_pass"};
    const std::string two_strings[1] = {"cgi_params"};
    const std::string valid_methods[3] = {"GET", "POST", "DELETE"};
    int i;
    int string_number;
    int get_flag;
    int post_flag;
    int delete_flag;

    get_flag = 0;
    post_flag = 0;
    delete_flag = 0;
    i = 0;
    string_number = 0;
    while (i < 7)
    {
        if (t1_value == one_string[i])
        {
            while ((*it).type != Token::SEMICOLON)
            {
                if ((*it).type == Token::STRING)
                {
                    string_number++;
                }//pas oublier que avec ca server et location passe pas!
                else//is not a string! par exemple location ou server ou autre
                    return (printError(NOT_STRING));//what is between directive and semicolon is not a string
                it++;
            }
            if (string_number != 1)
                return (printError(ONLY_ONE));
            return (1);
        }
        i++;
    }
    i = 0;
    while (i < 1)
    {
        if (t1_value == two_strings[i])
        {
            while ((*it).type != Token::SEMICOLON)
            {
                if ((*it).type == Token::STRING)
                {
                    string_number++;
                }
                else
                    return (printError(NOT_STRING));//what is between directive and semicolon is not a string
                it++;
            }
            if (string_number != 2)
                return (printError(ONLY_TWO));
            return (1);
        }
        i++;
    }
    if (t1_value == "method")
    {
        while ((*it).type != Token::SEMICOLON)
        {
            if ((*it).value == "GET")
                get_flag += 1;
            else if ((*it).value == "POST")
                post_flag += 1;
            else if ((*it).value == "DELETE")
                delete_flag += 1;
            else
                return (printError(INVALID_METHOD));//what is between directive and semicolon is not a string
            it++;
        }
        if (get_flag + post_flag + delete_flag == 0)
            return (printError(NO_STRING));
        if ((get_flag > 1 || post_flag > 1 || delete_flag > 1))
            return (printError(DBL_METHOD));
        return (1);
    }
    while ((*it).type != Token::SEMICOLON)
    {
        if ((*it).type == Token::STRING)
        {
            string_number++;
        }
        else//is not a string! par exemple location ou server ou autre
            return (printError(NOT_STRING));//what is between directive and semicolon is not a string
        it++;
    }
    if (string_number == 0)
        return (printError(NO_STRING));
    return (1);
}


int Tokenizer::ft_is_location_valid(std::list<Token>::iterator it)
{
    int string_number;

    //it est sur location mtn
    string_number = 0;
    while ((*it).type != Token::LBRACE)
    {            
        if ((*it).type == Token::STRING)
        {
            string_number++;
        }//pas oublier que avec ca server et location passe pas!
        else//is not a string! par exemple location ou server ou autre
            return (printError(LOCATION_STRUCT));//what is between location and lbrace is not a string
        it++;
    }
    if (string_number != 1)
        return (printError(LOCATION_STRUCT));
    return (1);
}


//compare les strings qui sont que apres un SC, LB ou RB!
int Tokenizer::ft_check_directives(std::list<Token>::iterator &it)
{
    int flag_location_block;
    int flag_start_location_block;
    int flag_valid_server_block;
    Token::TokenType t1;
    Token::TokenType t2;
    std::string t1_value;
    std::string t2_value;

    flag_location_block = 0;
    flag_start_location_block = 0;
    flag_valid_server_block = 0;
    if (it != _tokens_list.end())
    {
        if ((*it).type != Token::SERVER)
            return (printError(NOT_SERVER));
        it++;
        if (it == _tokens_list.end() || (*it).type != Token::LBRACE)
            return (printError(ORDER));
        it++;
    }
    while (it != _tokens_list.end())//1 2 3 dir 4
    {
        std::cout << "\ntype is " << (*it).type;
        std::cout << "\nvalue is " << (*it).value;
        t2 = (*it).type;//current token
        it--;
        std::cout << "\nThe previous element is " << (*it).type << '\n';
        t1 = (*it).type;//previous token
        t1_value = (*it).value;
        it++;//current
        if (t1 == Token::DIRECTIVE)//we know its a string after a directive, but how many?
        {
            if (!(ft_valid_values_after_directive(it, t1_value)))
                return (0);
        }
        if (t2 == Token::STRING && (t1 == Token::SEMICOLON || t1 == Token::LBRACE || t1 == Token::RBRACE))//make a current string a directive
        {
            if (!(ft_compare_with_table((*it).value, it, flag_location_block)))
                return (printError(BAD_DIRECTIVE));
            if ((*it).value == "listen")
                flag_valid_server_block = 1;
        }
        if (t1 == Token::LOCATION)//peux pas skip l interieur de location
        {
            if (!(ft_is_location_valid(it)))
                return (0);
            //it du current donc de la string
            t2_value = (*it).value;
            it--;//sur la location
            (*it).value = t2_value;
            std::cout << "Previous Type :" << (*it).type << " has New Value :" << (*it).value << "\n";
            it++;//back on the string
            (*it).type = Token::INVALID;//make this string invalid! for later iteration!
            flag_location_block = 1;
        }
        if (flag_location_block == 1 && t2 == Token::LBRACE)
            flag_start_location_block = 1;        
        if (flag_start_location_block == 1 && t2 == Token::RBRACE)
            flag_location_block = 0;
        else if (flag_location_block == 0 && t2 == Token::RBRACE)//end of first server block
        {
            if (!(flag_valid_server_block))
                return (printError(NO_LISTEN));
            it++;// passer au server suivant ou sur null si fin!
            std::cout << "\n-----------Check new server block---------\n";
            if (!(ft_check_directives(it)))
                return (0);
            return (1);
        }
        it++;
    }
    return (1);
}

int Tokenizer::ft_check_server_blocks()
{
    std::list<Token>::iterator it;

    it = _tokens_list.begin();
    if (!(ft_check_directives(it)))
        return (0);
    return (1);
}

const std::list<Token> &Tokenizer::ft_get_token_list() const
{
    return (_tokens_list);
}


Tokenizer::Tokenizer(std::string file) : _len()
{
    std::ifstream fileIn;
    std::string buff;

    fileIn.open(file.c_str());
    if (!fileIn.good())
    {
        LOG_CRITICAL << ErrorMessages::E_OPEN_FILE;
        throw std::invalid_argument("");
    }
    while (getline(fileIn, buff))
        ft_tokenize(buff + '\n');

    if (!(ft_check_basic_syntax()))
    {
        fileIn.close();
        throw std::runtime_error("");
    }
    std::cout << "DIRECTIVES-------------\n";
    if (!(ft_check_server_blocks()))
    {
        fileIn.close();
        throw std::runtime_error("");
    }
    else
        std::cout << "\nok!\n";

    fileIn.close();
}

Tokenizer::~Tokenizer(){}