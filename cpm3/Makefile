ESPOBJ=atlib.rel mk4.rel
SNIOSOBJ = snios.rel $(ESPOBJ)

BROKEN = ntpdate.com

COMFILES = ndosld.com ndos.com espconf.com esptest.com \
	rsxrm.com rsxls.com tpa.com tr.com \
	netstat.com netdown.com ntpdate.com \
	local.com \
	mail2.com login.com logoff.com endlist.com	

OBJECTS = $(filter-out $(BROKEN),$(COMFILES))

OTHERS = $(SNIOSOBJ) *.rel *.lst *.hex *.rsx *.prl

ESP=$(shell echo $(basename $(ESPOBJ)) | tr ' ' ',')

TOOLS = $(HOME)/src/RomWBW/Tools

include $(TOOLS)/Makefile.inc

$(SNIOSOBJ): $(SNIOSSRC)

esptest.com:	esptest.rel $(ESPOBJ)
	$(ZXCC) $(TOOLS)/cpm/bin/LINK esptest=esptest,$(ESP)[oc]

espconf.com:	espconf.rel $(ESPOBJ)
	$(ZXCC) $(TOOLS)/cpm/bin/LINK espconf=espconf,$(ESP)[oc]

ndosld.com:	ndosld.180

ndos.rsx:	ndos.rel $(SNIOSOBJ)
	$(ZXCC) $(TOOLS)/cpm/bin/LINK -ndos.rsx=ndos.rel,snios,$(ESP)[op]

ndos.rel:	ndos.180

ndos.com:	ndos.rsx ndosld.com
	cp ndosld.com ndos.com
	$(ZXCC) $(TOOLS)/cpm/bin/GENCOM -NDOS -ndos.rsx

%.com: %.180
	$(ZXCC) $(TOOLS)/cpm/bin/SLR180 -$(basename $?)/A
	
rsxrm.com:	rsxrm.180
rsxls.com:	rsxls.180
tpa.com:	tpa.180
tr.com:		tr.180
netstat.com:	netstat.180
netdown.com:	netdown.180
ntpdate.com:	ntpdate.180
mail2.com:	mail2.180
login.com:	login.180
logoff.com:	logoff.180
endlist.com:	endlist.180
local.com:	local.180

hdc: $(OBJECTS)
	cd ~/src/RomWBW/Source/Images ; rm hd_cpnet.img hd_cpm3.img hd_combo.img  ; make
	sudo dd bs=1024k if=$(HOME)/src/RomWBW/Source/Images/hd_combo.img of=/dev/sdc

hdd: $(OBJECTS)
	cd ~/src/RomWBW/Source/Images ; rm hd_cpnet.img hd_cpm3.img hd_combo.img  ; make
	sudo dd bs=1024k if=$(HOME)/src/RomWBW/Source/Images/hd_combo.img of=/dev/sdd
