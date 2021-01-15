###########define################
CC = g++
Target = Algorithm
Include = $(shell find ./ -name "*.h")
Src = $(shell find ./ -name "*.cpp")
Src += $(shell find ./ -name "*.c")
obj = $(Src:%.cpp=%.o)
Lib = -lpthread -lrt
###########Rule################
.PHONY:$(Target)
$(Target):$(Src)
	$(CC) $(Src) -o $@ $(Lib)


