.PHONY: help
help: default

.PHONY: default
default:
	@echo "Usage: make RULE"
	@echo ""
	@echo "  clean                  - Clean the build folder"
	@echo "  generate-vs-project    - Generate a Visual Studio project"

.PHONY: clean
clean:
	@rm -rf .build/
	@echo Cleaned build folder

.PHONY: generate-vs-project
generate-vs-project:
	@cmake -S . -B .build/Win64 -G "Visual Studio 17 2022" -A x64
	@echo "Generated VS project"