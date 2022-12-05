TYPE = Release
NAME = Final

.PHONY: info run clean gtest fsm

info:
	@echo "Type: $(TYPE)"
	@echo "If you want to change the type, use 'make run TYPE=Release'"

run:
	./x64/$(TYPE)/$(NAME).exe

gtest:
	./x64/$(TYPE)/test.exe

clean:
	rm -rf x64

cloc:
	cloc . --exclude-dir=packages