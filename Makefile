make all:
	cd rm && $(MAKE)
	cd dv && $(MAKE)
	cd dump && $(MAKE)
	cd function_time && $(MAKE)
	cd copy_time && $(MAKE)

clean:
	cd rm && $(MAKE) clean
	cd dv && $(MAKE) clean
	cd dump && $(MAKE) clean
	cd function_time && $(MAKE) clean
	cd copy_time && $(MAKE) clean
