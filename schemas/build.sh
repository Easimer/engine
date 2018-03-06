#!/bin/bash

OUTPATH=../public/include/schemas

for src in `find . -name "*.fbs"`; do
	noext=${src%.fbs}
	outfile=${OUTPATH}/${noext}_generated.h

	if [[ -e ${src} ]]; then
		outfile_lastmod=`date -r ${outfile} +%s`
		src_lastmod=`date -r ${src} +%s`

		if [[ ${outfile_lastmod} -gt ${src_lastmod} ]]; then
			echo -e "`basename ${src}`:\tnot modified"
			continue
		fi
	fi
	echo "Building ${src}"
	./flatc.exe -c -o ${OUTPATH} ${src}
done
