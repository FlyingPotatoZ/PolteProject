###########define################
CC = g++
TargetC = ./out/zsock
Target = ./out/zsockd
Tar_ShareLib = 

Include = $(shell find ./ -name "*.h")
INC_PATH = -I ./include/

SrcC = $(shell find ./zsock -name "*.cpp")
SrcC += $(shell find ./zsock -name "*.c")
Src = $(shell find ./zsockd -name "*.cpp")
Src += $(shell find ./zsockd -name "*.c")

obj = $(Src:%.cpp=%.o)

Lib = 

###########export##############
export LD_LIBRARY_PATH=

###########Rule################
all : $(Target) $(TargetC)
.PHONY :clean mkdir


$(Target):$(Src) $(Tar_ShareLib)
	$(CC) $(Src) -o $@ $(INC_PATH)

$(TargetC):$(SrcC) $(Tar_ShareLib)
	$(CC) $(SrcC) -o $@ $(INC_PATH)


clean:
	rm  -rf $(Target) 
	rm  -rf $(TargetC)
	rm  *.o
