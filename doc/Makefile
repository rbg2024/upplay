.SUFFIXES: .txt .html

.txt.html:
	asciidoc $<

all: upplay.html searching.html upplay-releases.html upplay-mingw.html

clean:
	rm -f *.html
