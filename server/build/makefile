#root make file 


OBJECT_PATH = ..\
			:../conn_mgr\
			:../include\
			:../test\
			:../sock\
			:../thirdpart\
			:../comm\
			:../db
			  
VPATH =$(OBJECT_PATH)

CC_INCLUDE_PATH = $(subst :, -I,$(OBJECT_PATH))
CC      = g++ 

CFLAGS  = -O2 -fabi-version=2 -fno-omit-frame-pointer -Wall

DLLLIB  = -lpthread  -lmysqlclient -lz -lm -lrt -ldl

COMM_SRC = 	CmdInfo.cpp\
			SessionMgr.cpp\
			SockMgr.cpp\
			Session.cpp\
			SockData.cpp\
			SockListen.cpp\
			SockMonitor.cpp\
			SelectSockMonitor.cpp\
			PollSockMonitor.cpp\
			EpollSockMonitor.cpp\
			SockConnector.cpp\
			cJSON.cpp\
			cJSON_Utils.cpp\
			LibMd5.cpp\
			aes.cpp\
			CmdResolver.cpp\
			com_util.cpp\
			SockSender.cpp\
			CmdReciever.cpp\
			SockSendResponse.cpp\
			DatabaseWraper.cpp\
			AccessDBReciever.cpp\
			LogUtil.cpp\
			KeepAliveDetect.cpp
			

BOOK_SHARE = main.cpp	  	
TEST_SRC = test_main.cpp
DEAMON_LOADER = deamon_loader.cpp


ALL_TARGET_SRC = $(TEST_SRC) $(BOOK_SHARE) $(DEAMON_LOADER)
SRC=$(COMM_SRC) $(ALL_TARGET_SRC)

OBJ := $(SRC:.cpp=.o)

define BUILD_EXE_FILE
$(CC) -o $@ $^ $(DLLLIB)
@echo [link ok] $@
endef

all:book-share-mind test loader

book-share-mind:$(BOOK_SHARE:.cpp=.o) $(COMM_SRC:.cpp=.o)
	$(BUILD_EXE_FILE)

loader:$(DEAMON_LOADER:.cpp=.o)
	$(BUILD_EXE_FILE)
	
test:$(TEST_SRC:.cpp=.o) $(COMM_SRC:.cpp=.o)
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
