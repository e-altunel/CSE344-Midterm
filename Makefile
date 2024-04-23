# Compilers 
CC = gcc
CCFLAGS = -Wall -g -Werror -Wextra -pedantic -ansi
AR = ar
ARFLAGS = rcs

# Directories
SRCDIR = src
INCDIR = inc
OBJDIR = obj
BINDIR = bin
LIBDIR = lib

# Names
LIBNAME = cse344midterm
SERVER_TARGET = $(BINDIR)/server.out
SERVER_SRC = server.c
SERVER_OBJ = $(OBJDIR)/server.o
CLIENT_TARGET = $(BINDIR)/client.out
CLIENT_SRC = client.c
CLIENT_OBJ = $(OBJDIR)/client.o

# Files
SRCS = $(wildcard $(SRCDIR)/*.c)
INCS = $(wildcard $(INCDIR)/*.h)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))
LIBS = $(LIBDIR)/lib$(LIBNAME).a

# Targets
.PHONY: all clean re server client

all: server client

server: $(SERVER_TARGET)

client: $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ) $(LIBS)
	@echo $(GREEN)"Linking server..."$(NC)
	@mkdir -p $(BINDIR)
	@$(CC) $(CCFLAGS) -o $@ $< -L$(LIBDIR) -l$(LIBNAME)

$(CLIENT_TARGET): $(CLIENT_OBJ) $(LIBS)
	@echo $(GREEN)"Linking client..."$(NC)
	@mkdir -p $(BINDIR)
	@$(CC) $(CCFLAGS) -o $@ $< -L$(LIBDIR) -l$(LIBNAME) 

$(SERVER_OBJ): $(SERVER_SRC)
	@echo $(GREEN)"Compiling server..."$(NC)
	@mkdir -p $(OBJDIR)
	@$(CC) $(CCFLAGS) -c -o $@ -I$(INCDIR) $<

$(CLIENT_OBJ): $(CLIENT_SRC)
	@echo $(GREEN)"Compiling client..."$(NC)
	@mkdir -p $(OBJDIR)
	@$(CC) $(CCFLAGS) -c -o $@ -I$(INCDIR) $<

$(LIBS): $(OBJS)
	@echo $(YELLOW)"Creating library..."$(NC)
	@mkdir -p $(LIBDIR)
	@$(AR) $(ARFLAGS) $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo $(GREEN)"Compiling $<..."$(NC)
	@mkdir -p $(OBJDIR)
	@$(CC) $(CCFLAGS) -c -o $@ -I$(INCDIR) $<

clean:
	@echo $(RED)"Cleaning..."$(NC)
	@rm -rf $(BINDIR) $(OBJDIR) $(LIBDIR) 

re: clean all

YELLOW = "\033[1;33m"
GREEN = "\033[1;32m"
RED = "\033[1;31m"
NC = "\033[0m"