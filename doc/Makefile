.SUFFIXES: .txt .html

.txt.html:
	asciidoc $<

all: upplay.html searching.html upplay-releases.html upplay-mingw.html \
     install.html songcast.html

clean:
	rm -f *.html
