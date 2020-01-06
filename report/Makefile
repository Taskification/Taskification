CONV=pandoc

FILES=intro.md description.md example.md

all: report.pdf report.docx

report.pdf: $(FILES)
	$(CONV) -o $@ $(FILES)

report.docx: $(FILES)
	$(CONV) -o $@ $(FILES)

clean:
	rm -fr report.pdf