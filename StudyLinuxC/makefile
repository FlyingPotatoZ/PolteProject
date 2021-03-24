###########define################
CC = g++
Target = ./out/main
Tar_ShareLib = ./out/libztool.so
Include = $(shell find ./ -name "*.h")
INC_PATH = -I ./include/
Src = $(shell find ./src/ -name "*.cpp")
Src += $(shell find ./src/ -name "*.c")
obj = $(Src:%.cpp=%.o)
Lib = -lpthread -lrt -L./lib -lztool

###########export##############
export LD_LIBRARY_PATH="./lib":$LD_LIBRARY_PATH

###########Rule################
.PHONY:$(Tar_ShareLib) $(Target)
$(Target):$(Src) $(Tar_ShareLib)
	$(CC) $(Src) -o $@  $(Lib) $(INC_PATH)
