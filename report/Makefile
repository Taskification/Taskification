CONV=pandoc

FILES=intro.md description.md example.md

all: report.pdf

report.pdf: $(FILES)
	$(CONV) -o report.pdf $(FILES)

clean:
	rm -fr report.pdf