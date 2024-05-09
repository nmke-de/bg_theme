
build: bg_theme

config.h: config.def.h
	cp -i $< $@

bg_theme.o: bg_theme.c config.h
	$(CC) $(CCFLAGS) -c -o $@ $<

bg_theme: bg_theme.o
	$(CC) $(CCFLAGS) -o $@ $<

clean:
	rm -f bg_theme
	rm -i config.h

install: build
	install bg_theme $(PREFIX)/usr/local/bin

.PHONY: build clean install
