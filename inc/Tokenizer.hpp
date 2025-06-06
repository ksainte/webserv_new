#ifndef TOKEN_HPP
# define TOKEN_HPP
#include <string>
#include <list>
#define ARGS 0
#define INPUT 1
#define SYNTAX 2
#define BRACKETS 3 
#define ORDER 4
#define BAD_SEMICOLONS 5
#define BAD_LOCATION 6
#define BAD_DIRECTIVE 7
#define DIRECTIVE_INCOMPLETE 8
#define NO_LISTEN 9
#define NOT_SERVER 10
#define NOT_STRING 11
#define ONLY_ONE 12
#define ONLY_TWO 13
#define NO_STRING 14
#define LOCATION_STRUCT 15
#define INVALID_METHOD 16
#define DBL_METHOD 17


// typedef enum s_token_type
// {
// 	SERVER,
// 	LOCATION,
// 	DIRECTIVE,
// 	LBRACE,
// 	STRING,
// 	RBRACE,
// 	SEMICOLON,
// 	QUOTE,
// 	ERROR,
// 	SINGLE_QUOTE,
// 	NUMBER,
// 	END,
// 	INVALID
// }							t_type;
//
// typedef struct s_node
// {
// 	t_type					type;
// 	std::string				value;
// 	s_node(t_type _type, std::string _value):type(_type), value(_value){};
// }							t_node;

struct Token;

class Tokenizer
{
		std::list<Token> _tokens_list;
		int				  _len;
		int 			  printError(int err);

	public:
		explicit Tokenizer(std::string file);
		~Tokenizer();

		int ft_tokenize(std::string s1);
		void ft_push_token(std::string s1);
		int ft_check_basic_syntax();
		int ft_check_directives(std::list<Token>::iterator &it);
		int ft_check_server_blocks();
		int ft_compare_with_table(std::string value, std::list<Token>::iterator &it, int flag_location_block);
		int ft_valid_values_after_directive(std::list<Token>::iterator &it, std::string t1_value);
		int ft_is_location_valid(std::list<Token>::iterator it);
		const std::list<Token> &ft_get_token_list() const;
	
};

#endif