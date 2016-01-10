all:
	$(MAKE) -C src/ipf_unpack

clean:
	$(MAKE) clean -C src/ipf_unpack

release:
	$(MAKE) release -C src/ipf_unpack
