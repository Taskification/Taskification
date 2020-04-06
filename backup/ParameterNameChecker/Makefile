# The name of the plugin.
PLUGIN = ParameterNameChecker

# LLVM paths. Note: you probably need to update these.
LLVM_DIR = /home/hans/llvm
LLVM_BUILD_DIR = $(LLVM_DIR)/build
CLANG_DIR = $(LLVM_DIR)/tools/clang
CLANG = $(LLVM_BUILD_DIR)/bin/clang

# Compiler flags.
CXXFLAGS  = -I$(LLVM_DIR)/include -I$(CLANG_DIR)/include
CXXFLAGS += -I$(LLVM_BUILD_DIR)/include -I$(LLVM_BUILD_DIR)/tools/clang/include
CXXFLAGS += -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -Wno-long-long
CXXFLAGS += -fPIC -fvisibility-inlines-hidden
CXXFLAGS += -fno-exceptions -fno-rtti -std=c++11
CXXFLAGS += -Wall

# Linker flags.
LDFLAGS = -shared -Wl,-undefined,dynamic_lookup

$(PLUGIN).so : $(PLUGIN).o
	$(CXX) $(LDFLAGS) -o $(PLUGIN).so $(PLUGIN).o

$(PLUGIN).o : $(PLUGIN).cc
	$(CXX) $(CXXFLAGS) -c $(PLUGIN).cc -o $(PLUGIN).o

check : $(PLUGIN).so
	$(CLANG) -c -Xclang -load -Xclang ./$(PLUGIN).so \
                -Xclang -add-plugin -Xclang check-parameter-names test.c

clean :
	rm -fv $(PLUGIN).o $(PLUGIN).so test.o
