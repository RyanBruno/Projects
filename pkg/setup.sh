
# Join overlay01
sudo pacman-key --init
sudo pacman -Syu wireguard-tools
sudo wg-quick up overlay01.temp.conf


KEYID="915EBB9C9889E4AEB983CD96F15F942AA25D5A6A"
pacman-key --recv-key $KEYID --keyserver keyserver.ubuntu.com
pacman-key --lsign-key $KEYID

cat <<EOF > /tmp/pacman.conf.ryan
[options]
HoldPkg     = pacman glibc
Architecture = auto
Color
CheckSpace
ILoveCandy
SigLevel    = Required DatabaseOptional
LocalFileSigLevel = Optional

[core.ryan]
Server = http://10.0.1.1/pkg/

[core]
Server = http://10.0.1.1/archlinux/\$repo/os/\$arch

[extra]
Server = http://10.0.1.1/archlinux/\$repo/os/\$arch

#[community-testing]
#Include = /etc/pacman.d/mirrorlist

[community]
Server = http://10.0.1.1/archlinux/\$repo/os/\$arch

# If you want to run 32 bit applications on your x86_64 system,
# enable the multilib repositories as required here.

#[multilib-testing]
#Include = /etc/pacman.d/mirrorlist

[multilib]
Server = http://10.0.1.1/archlinux/\$repo/os/\$arch

# An example of a custom package repository.  See the pacman manpage for
# tips on creating your own repositories.
#[custom]
#SigLevel = Optional TrustAll
#Server = file:///home/custompkgs
EOF


pacman --config /tmp/pacman.conf.ryan -Syu pacman.ryan
pacman.ryan -Syu base.ryan
