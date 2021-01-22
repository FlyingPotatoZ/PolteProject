###########define################
CC = g++
Target = ./out/Demo
Tar_ShareLib = ./out/libztool.so
Include = $(shell find ./ -name "*.h")
INC_PATH = -I ./include/
Src = $(shell find ./src/ -name "*.cpp")
Src += $(shell find ./src/ -name "*.c")
obj = $(Src:%.cpp=%.o)
Lib = -lpthread -lrt

###########export##############
export LD_LIBRARY_PATH="./out":$LD_LIBRARY_PATH

###########Rule################
.PHONY:$(Tar_ShareLib) $(Target)
$(Target):./test/main.cpp $(Tar_ShareLib)
	$(CC) ./test/main.cpp -o $@ -L./out -lztool $(INC_PATH)

$(Tar_ShareLib):$(Src)
	$(CC) $(Src) -fPIC -shared -o $@ $(Lib) $(INC_PATH)


