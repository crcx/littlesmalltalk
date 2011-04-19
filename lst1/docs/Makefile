TROFF = vtroff
DITROFF = vtroff
FILES = READ_ME Makefile macros books BUGS INDEX \
install userman apndx1 st.1 syntax

INDEX: books
	invert books

installguide:
	$(TROFF) -ms install

manbody: INDEX 
	bib userman | tbl | $(TROFF) -ms 

appendix1: apndx1 macros
	tbl apndx1 | $(TROFF) -ms

appendix2:
	$(TROFF) -man man

appendix3:
	pic syntax | $(DITROFF) -ms

bundle: $(FILES)
	bundle $(FILES) >../docs.bundle

