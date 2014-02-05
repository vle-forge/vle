
all: collection

collection:
	@echo "==== Generate QHC collection ===="
	qcollectiongenerator gvle2.qhcp -o gvle2.qhc 

app:
	@echo "==== Generate QHC file for main app =="
	qhelpgenerator gvle2.qhp -o gvle2.qch

clean:
	rm -f *~ *.qhc *.qch