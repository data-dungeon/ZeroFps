include config


all:
	cd zerofpsv2/; $(MAKE) $@
	cd mcommon/; $(MAKE) $@
#	cd mistclient/; $(MAKE) $@
	cd mistclient2/; $(MAKE) $@
	cd mistserver/; $(MAKE) $@
#	cd dark_metropolis/; $(MAKE) $@
#	cd dm_arcad/; $(MAKE) $@
	cd zgresedit; $(MAKE) $@
	cd zeroed/; $(MAKE) $@
#	cd fps_test/; $(MAKE) $@
	cd tools/tool/madview/; $(MAKE) $@
	./gengdbinit.sh

clean:
	cd zerofpsv2/; $(MAKE) $@
	cd mcommon/; $(MAKE) $@
#	cd mistclient/; $(MAKE) $@
	cd mistclient2/; $(MAKE) $@
	cd mistserver/; $(MAKE) $@
#	cd dark_metropolis/; $(MAKE) $@
#	cd dm_arcad/; $(MAKE) $@
	cd zgresedit/; $(MAKE) $@	
	cd zeroed/; $(MAKE) $@
#	cd fps_test/; $(MAKE) $@
	cd tools/tool/madview/; $(MAKE) $@

clean_engine:
	cd zerofpsv2/; $(MAKE) clean	

clean_apps:
	cd mcommon/; $(MAKE) clean
	cd zeroed/; $(MAKE) clean
	cd mistserver/; $(MAKE) clean
	cd mistclient2/; $(MAKE) clean
	cd tools/tool/madview/; $(MAKE) clean	
	cd zgresedit/; $(MAKE) clean
		
	
