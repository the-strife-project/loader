export PROJNAME := loader
export RESULT := loader.elf
# There's no stdlib yet
export nostdlib := true

all: loader.sus

loader.sus: $(RESULT)
	@echo "[Loader] Creating SUS..."
	@./elf2sus.py $<

clean:
	rm -rf obj loader.elf loader.sus

%: force
	@$(MAKE) -f ../helper/Makefile $@ --no-print-directory
force: ;
