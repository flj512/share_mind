#root make file 


OBJECT_PATH = ..\
			:../include\
			:../sock\
			:../thirdpart\
			:../comm
			
			  
VPATH =$(OBJECT_PATH)

CC_INCLUDE_PATH = $(subst :, -I,$(OBJECT_PATH))
CC      = g++ 

CFLAGS  = -O2 -fabi-version=2 -fno-omit-frame-pointer -Wall

DLLLIB  = -lpthread  -lmysqlclient -lz -lm -lrt -ldl

COMM_SRC = 	SockListen.cpp\
			cJSON.cpp\
			cJSON_Utils.cpp\
			LibMd5.cpp\
			aes.cpp\
			com_util.cpp\
			LogUtil.cpp	

BOOK_DOWNLOAD = main.cpp	  	

ALL_TARGET_SRC = $(TEST_SRC) $(BOOK_SHARE) $(DEAMON_LOADER)
SRC=$(COMM_SRC) $(ALL_TARGET_SRC)

OBJ := $(SRC:.cpp=.o)

define BUILD_EXE_FILE
$(CC) -o $@ $^ $(DLLLIB)
@echo [link ok] $@
endef

all:book-download

book-download:$(BOOK_DOWNLOAD:.cpp=.o) $(COMM_SRC:.cpp=.o)
	$(BUILD_EXE_FILE)
	
%.o:%.cpp
	@$(CC) $(CFLAGS) -I$(CC_INCLUDE_PATH) -c -o $@ $<
	@echo [build ok] $@
	
%.d:%.cpp
	@set -e; rm -f $@; $(CC) -MM -I$(CC_INCLUDE_PATH) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$
    
-include $(OBJ:.o=.d)

.PHONY:all,clean

clean:
	@rm -rf *.o *.d 
