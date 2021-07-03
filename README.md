# TheRepository

My solution to the problem of keeping track of mulitple repositories for muliple personal projects on both my laptop and PC:
Just put it all in one repository. TheRepository.

## Directories

**pkg/**

The `pkg/` directory contains build packages maintained by Ryan Bruno
\<ryan@rbruno.com\>. The packages are targeted for Arch Linux and use
PKGBUILD files. Packages that are names with the `.ryan` suffix are
personally configured for my machines. Other packages are actual
software packages. A special meta-package is the `base.ryan` which
lists, as dependencies, packages that I want installed by default on my
machines.

**src/**

The `src/` directory contains software developed by Ryan Bruno
\<ryan@rbruno.com\>. All software is written in C.

**web/**

The `web/` directory contains web applications developed by Ryan Bruno
\<ryan@rbruno.com\>. All software is written in Vue.js with Webpack.
