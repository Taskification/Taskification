all: report.pdf

report.pdf: main.tex
	pdflatex main.tex
	mv main.pdf report.pdf

clean:
	rm -fr report.pdf *.log *.aux
