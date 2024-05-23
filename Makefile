NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98   
RM = rm -f
SRC = conf_server.cpp\
	  configfile.cpp\
	  location.cpp\
	  main.cpp\
	  Multiplexing.cpp\
	  request.cpp\
	  post.cpp\
	  response.cpp\
	  resp_uti.cpp\
	  response_reason.cpp\
	  set_get.cpp\
	  Delete.cpp\
	  Get.cpp\
	  resp_location.cpp\
	  decrypt_uri.cpp\
	  cgi.cpp\
	  send_resp.cpp\

INCLUDE=conf_server.hpp\
		configfile.hpp\
		location.hpp\
		Multiplexing.hpp\
		request.hpp\
		post.hpp\
		response.hpp\
		cgi.hpp\
	
      

OBJECT = ${SRC:.cpp=.o}



all: $(NAME)

$(NAME) : $(OBJECT)
		$(CXX)  $(OBJECT) $(CXXFLAGS)  -o $(NAME) 
$(OBJECT):$(INCLUDE)
clean:
	$(RM) $(OBJECT)
	
fclean:
	$(RM) $(OBJECT) 
	$(RM) $(NAME)

re: fclean all