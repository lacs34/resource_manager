include ../common.mk
source_extensions:=cc cpp

project_include_directory:=$(foreach directory,$(shell find ../project/include -type d),-I $(directory))
test_include_directory:=$(foreach directory,$(shell find ./include -type d),-I $(directory))
test_framework_include_directory:=-I ../test_framework/include/gtest -I ../test_framework/include

include_directory:=$(shell find ./ -type d -name *.cc)

define make_project_source_file
../project/target/$(basename $(notdir $(1))).o: $(1)
	$(compiler) $(1) $(project_include_directory) -o ../project/target/$(basename $(notdir $(1))).o $(compiler_flags)
endef
define make_test_source_file
./target/$(basename $(notdir $(1))).o: $(1)
	$(compiler) $(1) $(project_include_directory) $(test_include_directory) $(test_framework_include_directory) -o ./target/$(basename $(notdir $(1))).o $(compiler_flags)
endef

project_source_files:=$(foreach extension,$(source_extensions),$(shell find ../project/source -type f -name *.$(extension)))
test_source_files:=$(foreach extension,$(source_extensions),$(shell find ./source -type f -name *.$(extension)))

project_object_files:=$(foreach source,$(project_source_files),../project/target/$(basename $(notdir $(source))).o)
test_object_files:=$(foreach source,$(test_source_files),./target/$(basename $(notdir $(source))).o)

$(foreach source,$(project_source_files),$(eval $(call make_project_source_file,$(source))))
$(foreach source,$(test_source_files),$(eval $(call make_test_source_file,$(source))))

./target/gtest.o: ../test_framework/src/gtest-all.cc \
 ../test_framework/include/gtest/gtest.h \
 ../test_framework/include/gtest/internal/gtest-internal.h \
 ../test_framework/include/gtest/internal/gtest-port.h \
 ../test_framework/include/gtest/gtest-message.h \
 ../test_framework/include/gtest/internal/gtest-string.h \
 ../test_framework/include/gtest/internal/gtest-filepath.h \
 ../test_framework/include/gtest/internal/gtest-type-util.h \
 ../test_framework/include/gtest/gtest-death-test.h \
 ../test_framework/include/gtest/internal/gtest-death-test-internal.h \
 ../test_framework/include/gtest/gtest-param-test.h \
 ../test_framework/include/gtest/internal/gtest-param-util.h \
 ../test_framework/include/gtest/internal/gtest-linked_ptr.h \
 ../test_framework/include/gtest/gtest-printers.h \
 ../test_framework/include/gtest/internal/gtest-param-util-generated.h \
 ../test_framework/include/gtest/gtest_prod.h \
 ../test_framework/include/gtest/gtest-test-part.h \
 ../test_framework/include/gtest/gtest-typed-test.h \
 ../test_framework/include/gtest/gtest_pred_impl.h \
 ../test_framework/src/gtest.cc \
 ../test_framework/include/gtest/gtest-spi.h \
 ../test_framework/src/gtest-internal-inl.h \
 ../test_framework/src/gtest-death-test.cc \
 ../test_framework/src/gtest-filepath.cc \
 ../test_framework/src/gtest-port.cc \
 ../test_framework/src/gtest-printers.cc \
 ../test_framework/src/gtest-test-part.cc \
 ../test_framework/src/gtest-typed-test.cc
	$(compiler) ../test_framework/src/gtest-all.cc -I ../test_framework/include -I ../test_framework/ -o ./target/gtest.o $(compiler_flags)
	
./target/gtest_main.o: ./gtest_main.cc
	$(compiler) ./gtest_main.cc -I ../test_framework/include $(project_include_directory) $(test_include_directory) $(test_framework_include_directory) -o ./target/gtest_main.o $(compiler_flags)

./executable/program.exe: ./target/gtest.o ./target/gtest_main.o $(project_object_files) $(test_object_files)
	$(linker) ./target/gtest.o ./target/gtest_main.o $(project_object_files) $(test_object_files) -pthread -o ./executable/program.exe
	
all: ./executable/program.exe

clean:
	rm -f ./target/*
	rm -f ./executable/*
	rm -f ../project/target/*

.PYONY: all clean