FROM archlinux

RUN pacman --noconfirm -Sy
RUN pacman --noconfirm -S noto-fonts noto-fonts-emoji git base-devel

RUN git clone https://aur.archlinux.org/icecat-bin.git
WORKDIR /icecat-bin

RUN useradd -m build
RUN usermod -aG wheel build
RUN echo "%wheel ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers
RUN chown -R build:build .
USER build

RUN makepkg -si --skippgpcheck --noconfirm
RUN sudo pacman --noconfirm -S noto-fonts noto-fonts-emoji
RUN sudo pacman --noconfirm -S gtk3

ENTRYPOINT icecat
