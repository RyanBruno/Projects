

%.ryan: %.ryan/* Makefile
	@cd $@ && makepkg -s --force 1> /dev/null
	@cd $@ && gpg -b *.pkg.tar.xz


all: vim.ryan repo-add.ryan base.ryan vim.ryan lock grab apache.ryan bash.ryan


dist:
	-@rm -rf /srv/http/pkg/core.ryan.db*
	@cp */*.pkg.tar* /srv/http/pkg/
	@repo-add.ryan -s /srv/http/pkg/core.ryan.db.tar.xz /srv/http/pkg/*.pkg.tar.xz
