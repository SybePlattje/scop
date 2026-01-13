NAME = scop

CXX = c++
CC = cc
CFLAGS = -Wall -Wextra -Werror -MMD -MP
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -MMD -MP
LFLAGS = -lGL -lglfw -ldl

SRC_DIR = ./src
OBJ_DIR = ./obj
INCLUDE = include

WRAPPER_DIR = ./glAndGlfwWrapper
WRAPPER_SRC_DIR = $(WRAPPER_DIR)/src
WRAPPER_INCLUDE_DIR = $(WRAPPER_DIR)/include
EXTERNAL_DIR = $(WRAPPER_DIR)/externalLib
GLAD_DIR = $(EXTERNAL_DIR)/glad

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
SOURCES += $(wildcard $(WRAPPER_SRC_DIR)/*.cpp)
SOURCES += $(GLAD_DIR)/src/glad.c

OBJECTS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(filter %.cpp,$(SOURCES)))
OBJECTS += $(patsubst %.c,$(OBJ_DIR)/%.o,$(filter %.c,$(SOURCES)))

DEPS = $(OBJECTS:.o=.d)
INCLUDES = -I$(INCLUDE) -I$(WRAPPER_INCLUDE_DIR) -I$(GLAD_DIR)/include -I$(EXTERNAL_DIR)

ifdef DEBUG
CXXFLAGS += -g
endif

ifdef FSAN
CXXFLAGS += -g -fsanitize=address
endif

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug:
	$(MAKE) DEBUG=1

rebug: fclean debug

fsan:
	$(MAKE) FSAN=1

resan: fclean fsan

.PHONY: all clean fclean re debug rebug fsan resan