RTF = $(wildcard *.rtf)
megatune.hlp:	megatune.cnt megatune.hpj $(RTF)
	"c:/Program Files/Microsoft Visual Studio/Common/Tools/HCW.EXE" megatune.hpj

release: megatune.hlp
	@(cd ../../Release/ ; rm -fr ./MEGATUNE.HLP ./megatune.GID ./megatune.cnt)
	cp -p ./MEGATUNE.HLP ./megatune.GID ./megatune.cnt ../../Release/
	@(cd ../Release/ ; rm -fr ./MEGATUNE.HLP ./megatune.GID ./megatune.cnt)
	cp -p ./MEGATUNE.HLP ./megatune.GID ./megatune.cnt ../Release/
	@(cd ../Debug/ ; rm -fr ./MEGATUNE.HLP ./megatune.GID ./megatune.cnt)
	cp -p ./MEGATUNE.HLP ./megatune.GID ./megatune.cnt ../Debug/

zip: hlp.zip
	zip -9 hlp.zip megatune.hpj megatune.cnt $(RTF)
