# Compilers 
CC = gcc
CCFLAGS = -Werror -Wall -g -Wextra -pedantic -std=c99 -D_POSIX_C_SOURCE=200112L
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
TESTMAINDIR = tester
TESTSRCDIR = $(TESTMAINDIR)/src
TESTINCDIR = $(TESTMAINDIR)/inc
TESTOBJDIR = $(TESTMAINDIR)/obj
TESTBINDIR = $(TESTMAINDIR)/bin

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
TESTSRCS = $(wildcard $(TESTSRCDIR)/*.c)
TESTINCS = $(wildcard $(TESTINCDIR)/*.h)
TESTOBJS = $(patsubst $(TESTSRCDIR)/%.c, $(TESTOBJDIR)/%.o, $(TESTSRCS))
.PRECIOUS: $(TESTOBJS)
TESTBINS = $(patsubst $(TESTSRCDIR)/%.c, $(TESTBINDIR)/%, $(TESTSRCS))

# Targets
.PHONY: all clean re test_all

all: $(BINS)

$(BINDIR)/%.out: $(OBJDIR)/%.o $(LIBS)
	@echo $(GREEN)"Linking $<..."$(NC)
	@mkdir -p $(BINDIR)
	@$(CC) $(CCFLAGS) -o $@ -I$(INCDIR) $< $(LIBS)

$(OBJDIR)/%.o: $(MAINDIR)/%.c $(LIBS) $(INCS)
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
	@rm -rf $(BINDIR) $(OBJDIR) $(LIBDIR) $(TESTBINDIR) $(TESTOBJDIR)

test_all: $(TESTBINS) $(LIBS)
	@echo $(YELLOW)"Running tests..."$(NC)"\n" 
	@for test in $(TESTBINS); do \
		echo $(YELLOW)"Running test $$test: "$(NC); \
		if $(MEMCHECK) $$test; then \
			echo $(GREEN)"Test $$test passed."$(NC); \
		else \
			echo $(RED)"Test $$test failed."$(NC); \
		fi; \
		echo ""; \
	done

$(TESTBINDIR)/%: $(TESTOBJDIR)/%.o $(LIBS) 
	@echo $(GREEN)"Linking test $<..."$(NC)
	@mkdir -p $(TESTBINDIR)
	@$(CC) $(CCFLAGS) -o $@ -I$(INCDIR) -I$(TESTINCDIR) $< $(LIBS)

$(TESTOBJDIR)/%.o: $(TESTSRCDIR)/%.c $(LIBS) $(TESTINCS)
	@echo $(GREEN)"Compiling test $<..."$(NC)
	@mkdir -p $(TESTOBJDIR)
	@$(CC) $(CCFLAGS) -c -o $@ -I$(INCDIR) -I$(TESTINCDIR) $<

re: clean all

YELLOW = "\033[1;33m"
GREEN = "\033[1;32m"
RED = "\033[1;31m"
NC = "\033[0m"