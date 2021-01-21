###########define################
CC = g++
Target = ./out/Algorithm
Include = $(shell find ./ -name "*.h")
INC_PATH = -I ./include/
Src = $(shell find ./ -name "*.cpp")
Src += $(shell find ./ -name "*.c")
obj = $(Src:%.cpp=%.o)
Lib = -lpthread -lrt
###########Rule################
.PHONY:$(Target)
$(Target):$(Src)
	$(CC) $(Src) -o $@ $(Lib) $(INC_PATH)


