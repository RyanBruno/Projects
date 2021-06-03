
.PHONY: all dev clean

all:
	npx webpack

dev: clean all
	cp dist/*.html dist/index.html

clean:
	rm -r dist/*
