
EXAMPLES=$(shell find . -maxdepth 1 -mindepth 1 -type d)

all:
	@for EXAMPLE in $(EXAMPLES); do $(MAKE) -C "$$EXAMPLE"; done

clean:
	@for EXAMPLE in $(EXAMPLES); do $(MAKE) -C "$$EXAMPLE" clean || true; done
