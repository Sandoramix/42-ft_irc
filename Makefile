NAME:=ircserv

CXX:=clang++
CXXFLAGS:=-std=c++98 -Wall -Wextra -Werror -g -I./includes

SRC = ./src/Client.cpp \
	./src/Server.cpp \
	./src/cmd/CmdInterface.cpp \
	./src/cmd/JoinCmd.cpp \
	./src/main.cpp



all: $(NAME)

$(NAME): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "[${NAME}] Compiled successfully."

PORT:=6667
PASSW:=password

valgrind: all
	@valgrind --leak-check=full --show-leak-kinds=all --quiet ./$(NAME) "$(PORT)" "$(PASSW)"

clean:
fclean:
	rm -f $(NAME)

# Automatically get source files
getsrc:
	@echo "[getsrc] Automatic lookup of source files..."
	@curl 'https://raw.githubusercontent.com/Sandoramix/42cursus/master/utils/getSrc.py' | python3 - -ext cpp

re: fclean all

.PHONY: all clean fclean re getsrc
.SILENT: