NAME:=ircserv

DEBUG:=true


CXX:=c++
CXXFLAGS:=-std=c++98 -g -Wall -Wextra -Werror -DDEBUG=$(DEBUG) -I./includes

SRC = ./src/Channel.cpp \
	./src/Client.cpp \
	./src/IRCUtils.cpp \
	./src/ResponseMsg.cpp \
	./src/Server.cpp \
	./src/cmd/CmdInterface.cpp \
	./src/cmd/InviteCmd.cpp \
	./src/cmd/JoinCmd.cpp \
	./src/cmd/KickCmd.cpp \
	./src/cmd/ModeCmd.cpp \
	./src/cmd/NickCmd.cpp \
	./src/cmd/PassCmd.cpp \
	./src/cmd/PingCmd.cpp \
	./src/cmd/PrivMsgCmd.cpp \
	./src/cmd/TopicCmd.cpp \
	./src/cmd/UserCmd.cpp \
	./src/cmd/WhoCmd.cpp \
	./src/main.cpp


all: $(NAME)

debug: DEBUG:=true
debug: all

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
re-valgrind: fclean all valgrind

.PHONY: all clean fclean re getsrc
.SILENT:
