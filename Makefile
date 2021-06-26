export PROJNAME := loader
export RESULT := loader.elf
# There is no stdlib yet :^)
export nostdlib := true
export nostdlibh := true

all: loader.sus

loader.sus: $(RESULT)
	@echo "[Loader] Creating USU..."
	@./elf2sus.py $<

clean:
	rm -rf obj ELF.elf ELF.sus

%: force
	@$(MAKE) -f ../helper/Makefile $@ --no-print-directory
force: ;
