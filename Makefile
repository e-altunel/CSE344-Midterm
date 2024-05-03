# Compilers 
CC = gcc
CCFLAGS = -Werror -Wall -g -Wextra -pedantic -std=c89 -D_POSIX_C_SOURCE=200112L
AR = ar
ARFLAGS = rcs
MEMCHECK = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes -q

# Directories
SRCDIR = src
INCDIR = inc
OBJDIR = obj
BINDIR = bin
LIBDIR = lib
MAINDIR = main

# Names
LIBNAME = cse344midterm

# Files
SRCS = $(wildcard $(SRCDIR)/*.c)
INCS = $(wildcard $(INCDIR)/*.h)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))
LIBS = $(LIBDIR)/lib$(LIBNAME).a
MAINS = $(wildcard $(MAINDIR)/*.c)
MAIN_O = $(patsubst $(MAINDIR)/%.c, $(OBJDIR)/%.o, $(MAINS))
.PRECIOUS: $(MAIN_O)
BINS = $(patsubst $(MAINDIR)/%.c, $(BINDIR)/%.out, $(MAINS))

# Targets
.PHONY: all clean re 

all: $(BINS)

$(BINDIR)/%.out: $(OBJDIR)/%.o $(LIBS)
	@echo $(GREEN)"Linking $<..."$(NC)
	@mkdir -p $(BINDIR)
	@$(CC) $(CCFLAGS) -o $@ -I$(INCDIR) $< $(LIBS)

$(OBJDIR)/%.o: $(MAINDIR)/%.c $(LIBS)
	@echo $(GREEN)"Compiling main files $<..."$(NC)
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