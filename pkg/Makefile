
PKGs = apache.ryan \
    base.ryan \
    bash.ryan \
    cgit.ryan \
    grab \
    lock \
    nginx.ryan \
    pacman.ryan \
    repo-add.ryan \
    wg-overlay.ryan \
    smartd.ryan \
    vim.ryan


%.ryan: %.ryan/* Makefile
	@cd $@ && makepkg -s --force 1> /dev/null
	@cd $@ && gpg -b *.pkg.tar.xz

all: $(PKGs)

dist:
	-@rm -rf /srv/http/pkg/core.ryan.db*
	@cp */*.pkg.tar* /srv/http/pkg/
	@repo-add.ryan -s /srv/http/pkg/core.ryan.db.tar.xz /srv/http/pkg/*.pkg.tar.xz
