CC=gcc
TARGET  = demo
CFLAGS+= -g -Wall $(JMP) 
TARS2C =../../../cpp/build/tools/tars2c/tars2c
INCLUDE=-I../

TARS_SRC := $(wildcard *.tars)
TARS_H   := $(patsubst %.tars,%.h, $(TARS_SRC))
TARS_C    := $(patsubst %.tars,%.c, $(TARS_SRC))
LOCAL_SRC   	+= $(sort $(wildcard *.cpp *.c) $(TARS_C))
LOCAL_SRC   	+= $(sort $(wildcard ../*.cpp ../*.c))
LOCAL_OBJ   	+= $(patsubst %.cpp,%.o, $(patsubst %.c,%.o, $(LOCAL_SRC)))

$(TARGET): $(TARS_H) $(TARS_C) $(LOCAL_OBJ)
	$(CC) -o $@  $(LOCAL_OBJ)

ifneq ($(TARS_SRC),)
$(TARS_H) $(TARS_C) : $(TARS_SRC)
	rm -vf $(TARS_H) $(TARS_C) $(LOCAL_OBJ)
	$(TARS2C) ${TARSFLAGS} $(TARS_SRC)
endif

.%.d: %.cpp
	@echo "update $@ ..."; \
	echo -n $< | sed s/\.cpp/\.o:/ > $@; \
	$(CC) $(INCLUDE) -MM $< | sed '1s/.*.://' >> $@;

%.o: %.cpp
	$(CXX) -m$(MFLAGS) $(CFLAGS) $(INCLUDE) -o $@ -c $<

.%.d: %.c
	@echo "update $@ ..."; \
	echo -n $< | sed s/\.c/\.o:/ > $@; \
	$(CC) $(INCLUDE) -MM $< | sed '1s/.*.://' >> $@;

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	@echo "cleanall"; \
	rm -vf $(TARGET) *.o .*.d.tmp .*.d gmon.out core.* *.log $(TARS_H) $(TARS_C) $(LOCAL_OBJ)
