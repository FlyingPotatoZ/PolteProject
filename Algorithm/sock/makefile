###########define################
CC = g++

TargetC = ./out/zsock
Target = ./out/zsockd
Tar_ShareLib = 

Include = $(shell find ./ -name "*.h")
INC_PATH = -I ./include/
INC_PATH += -I ../include/

SrcC = $(shell find ./zsock -name "*.cpp")
SrcC += $(shell find ./zsock -name "*.c")
SrcC += z_local_sock_common.cpp

Src = $(shell find ./zsockd -name "*.cpp")
Src += $(shell find ./zsockd -name "*.c")
Src += z_local_sock_common.cpp

obj = $(Src:%.cpp=%.o)

LibPath = -L../out
Lib = -lztool

###########export##############
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/zhuzhiyu/PolteZhu/PolteProject/Algorithm/out

###########Rule################
all : $(Target) $(TargetC)
.PHONY :clean zexport


$(Target):$(Src) $(Tar_ShareLib)
	$(CC) $(Src) -o $@ $(LibPath) $(Lib) $(INC_PATH)

$(TargetC):$(SrcC) $(Tar_ShareLib)
	$(CC) $(SrcC) -o $@ $(LibPath) $(Lib) $(INC_PATH)


clean:
	rm  -rf $(Target) 
	rm  -rf $(TargetC)
	rm  *.o

