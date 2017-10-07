$compiler:=g++
.PYONY:all
all:
	make -C ./test/ all
clean:
	make -C ./test/ clean