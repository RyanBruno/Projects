FROM archlinux

RUN pacman --noconfirm -Syu

RUN pacman --noconfirm -S noto-fonts noto-fonts-emoji git base-devel

RUN git clone https://aur.archlinux.org/zoom.git
WORKDIR /zoom

RUN useradd -m build
RUN usermod -aG wheel build
RUN echo "%wheel ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers
RUN chown -R build:build .
USER build

RUN makepkg -si --noconfirm

RUN sudo pacman --noconfirm -S noto-fonts noto-fonts-emoji firefox
RUN sudo pacman --noconfirm -S nvidia nvidia-utils

ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute

ENTRYPOINT zoom
