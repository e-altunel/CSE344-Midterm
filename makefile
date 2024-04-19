# Compiler 
CC = gcc
CCFLAGS = #-Wall -Werror -Wextra -pedantic -std=c99 -g
AR = ar
ARFLAGS = rcs

# Directories
SRC = src
INC = inc
OBJ = obj
LIB = lib
BIN = bin

# Names
PROJECT = cse344midterm
SERVER = server
CLIENT = client

# Files
SRCS = $(wildcard $(SRC)/*.c)
SERVER_SRC = $(SERVER).c
CLIENT_SRC = $(CLIENT).c
INCS = $(wildcard $(INC)/*.h)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
LIBS = $(LIB)/lib$(PROJECT).a
SERVER_BIN = $(BIN)/$(SERVER).out
CLIENT_BIN = $(BIN)/$(CLIENT).out

# Rules
all: $(SERVER_BIN) $(CLIENT_BIN)

run_server: $(SERVER_BIN)
	@echo "\033[1m\033[32mRunning \033[0m$(SERVER_BIN)..."
	@./$(SERVER_BIN)

run_client: $(CLIENT_BIN)
	@echo "\033[1m\033[32mRunning \033[0m$(CLIENT_BIN)..."
	@./$(CLIENT_BIN)

$(SERVER_BIN): $(OBJ)/$(SERVER_OBJ) $(LIBS)
	@mkdir -p $(BIN)
	@echo "\033[1m\033[32mLinking \033[0m$(SERVER_BIN)..."
	@$(CC) $(CCFLAGS) -o $@ $< -I$(INC) -L$(LIB) -l$(PROJECT)

$(CLIENT_BIN): $(OBJ)/$(CLIENT_OBJ) $(LIBS)
	@mkdir -p $(BIN)
	@echo "\033[1m\033[32mLinking \033[0m$(CLIENT_BIN)..."
	@$(CC) $(CCFLAGS) -o $@ $< -I$(INC) -L$(LIB) -l$(PROJECT)

$(LIBS): $(OBJS)
	@mkdir -p $(LIB)
	@echo "\033[1m\033[32mArchiving \033[0m$(LIBS)..."
	@$(AR) $(ARFLAGS) $@ $^

$(OBJ)/$(SERVER_OBJ): $(SERVER_SRC) $(INCS)
	@mkdir -p $(OBJ)
	@echo "\033[1m\033[32mCompiling \033[0m$(SERVER_SRC)..."
	@$(CC) $(CCFLAGS) -c $< -o $@ -I$(INC)

$(OBJ)/$(CLIENT_OBJ): $(CLIENT_SRC) $(INCS)
	@mkdir -p $(OBJ)
	@echo "\033[1m\033[32mCompiling \033[0m$(CLIENT_SRC)..."
	@$(CC) $(CCFLAGS) -c $< -o $@ -I$(INC)

$(OBJ)/%.o: $(SRC)/%.c $(INCS)
	@mkdir -p $(OBJ)
	@echo "\033[1m\033[32mCompiling \033[0m$<..."
	@$(CC) $(CCFLAGS) -c $< -o $@ -I$(INC)

clean:
	@echo "\033[1m\033[31mCleaning...\033[0m"
	@rm -rf $(OBJ) $(LIB) $(BIN)

re: clean all

.PHONY: all clean re