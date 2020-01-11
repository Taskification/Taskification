all: presentation.pdf

presentation.pdf: main.tex
	pdflatex main.tex
	pdflatex main.tex
	mv main.pdf presentation.pdf
	evince presentation.pdf &

clean:
	rm -fr presentation.pdf *.log *.aux
