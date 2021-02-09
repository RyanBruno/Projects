FROM archlinux

RUN echo -e "[multilib]\nInclude = /etc/pacman.d/mirrorlist" >> /etc/pacman.conf
RUN pacman --noconfirm -Syu

RUN pacman --noconfirm -S noto-fonts noto-fonts-emoji git base-devel

RUN git clone https://aur.archlinux.org/protonmail-bridge-bin.git
WORKDIR /protonmail-bridge-bin

RUN useradd -m build
RUN usermod -aG wheel build
RUN echo "%wheel ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers
RUN chown -R build:build .
USER build

RUN makepkg -si --noconfirm
RUN sudo pacman -S --noconfirm lib32-nvidia-libgl lib32-nvidia-utils nvidia-libgl

ENTRYPOINT protonmail-bridge
