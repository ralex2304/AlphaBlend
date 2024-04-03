.PHONY: all clean

CC = $(if $(COMPILER),$(COMPILER),g++)
CFLAGS = -fdiagnostics-color=always -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef			 \
		 -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs 		 \
		 -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual	 \
		 -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers \
		 -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual 			 \
		 -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel 		 \
		 -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

CFLAGS_SANITIZER = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,$\
				   float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,$\
				   object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,$\
				   undefined,unreachable,vla-bound,vptr

OPTIMISATION = -DNDEBUG -O2 -march=native $(OPTION_FLAGS)

EXTERNAL_DIR =
IFLAGS =
LIBRARIES = -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIR = src
BUILD_DIR = build
NON_CODE_DIRS = $(BUILD_DIR) .vscode .git
TARGET = $(if $(OPTION_NAME),$(OPTION_NAME),main)

CD = $(shell pwd)


NESTED_CODE_DIRS_CD = $(shell find ./$(SRC_DIR) -maxdepth 5 -type d $(NON_CODE_DIRS:%=! -path "*%*"))
NESTED_CODE_DIRS = $(NESTED_CODE_DIRS_CD:.%=%)



FILES_FULL = $(shell find ./$(SRC_DIR) -name "*.cpp")

FILES = $(FILES_FULL:.%=%)

MAKE_DIRS = $(NESTED_CODE_DIRS:%=$(BUILD_DIR)%)
OBJ = $(FILES:%=$(BUILD_DIR)%)
DEPENDS = $(OBJ:%.cpp=%.d)
OBJECTS = $(OBJ:%.cpp=%.o)

target: $(TARGET)

$(TARGET): $(OBJECTS)
	@$(CC) $(OPTIMISATION) $(IFLAGS) $(CFLAGS) $(if $(sanitizer), $(CFLAGS_SANITIZER)) $^ -o $@ $(LIBRARIES)

$(BUILD_DIR):
	@mkdir ./$@

$(MAKE_DIRS): | $(BUILD_DIR)
	@mkdir ./$@

-include $(DEPENDS)

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR) $(MAKE_DIRS)
	@$(CC) $(OPTIMISATION) $(IFLAGS) $(CFLAGS) $(if $(sanitizer), $(CFLAGS_SANITIZER)) -MMD -MP -c $< -o $@

clean:
	@rm -rf ./$(BUILD_DIR)/*
	@rm -f ./$(TARGET)

