all: report.pdf

report.pdf: main.tex
	pdflatex main.tex
	bibtex main
	pdflatex main.tex
	mv main.pdf report.pdf
	evince report.pdf &

clean:
	rm -fr report.pdf *.log *.aux *.bbl *.toc *.blg
