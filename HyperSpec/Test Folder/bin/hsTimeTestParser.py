import glob
import os

counter = 0

headerName = os.getcwd()
docName = "TimeTest_%s" %headerName.split('/')[(len(headerName.split('/'))-1)]
if glob.glob('%s.txt' %docName):
	if glob.glob('%sv*.txt'):
		versionList = glob.glob('%sv*.txt' %docName)
		versionListLength = len(versionList)
		docName = '%sv%s' %(docName, (versionListLength + 2))
	else:
		docName = '%sv2' %docName

headers = glob.glob('*.hdr')
for files in headers:
	f = open(files, 'r')
	for line in f:
		counter++
		if "File created at" in line:
			fileTime = line.split(' ')[3]
		if "First line acquired" in line:
			firstLineTime = line.split('at')[1]
			firstLineTime = firstLineTime.split(' ')[1]
			doc = open('%s.txt' %docName, 'a')
			doc.write('%s,%s,%d' %(fileTime, firstLineTime, counter))
			doc.close()