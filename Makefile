TARGET_EXEC := app

BUILD_DIR := ./build
SRC_DIRS := ./src


SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) -I.

CPPFLAGS := $(INC_FLAGS) -MMD -MP -O1

$(BUILD_DIR)/$(TARGET_EXEC): $(VERT_OBJS) $(FRAG_OBJS)

# linking
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS) 
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# c++ compiling
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

run:
	./build/$(TARGET_EXEC)

touch:
	touch $(SRCS)

app:
	$(CXX) $(INC_FLAGS) $(CXXFLAGS) $(SRCS) -O3 -o app



-include $(DEPS)