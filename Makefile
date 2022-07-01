export PROJNAME := loader
export RESULT := loader.elf

# Go read kernel's <tasks/loader/bootstrap.cpp> about this
export LINKER_FLAGS := -Ttext-segment=0x1000

# stdlib needs to be statically linked
export static := true

.PHONY: all

all: loader.sus
	@

loader.sus: $(RESULT)
	@echo "[Loader] Creating SUS..."
	@./elf2sus.py $<

clean:
	rm -rf obj loader.elf loader.sus

%: force
	@$(MAKE) -f $(STRIFE_HELPER)/Makefile $@ --no-print-directory
force: ;
