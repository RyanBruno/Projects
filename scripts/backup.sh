#!/bin/bash

tar czvf /home/object/Objects/Backups/$(date +%Y.%m.%d).tar.gz /home/ryan/.gnupg/ /home/vault/.gnupg/ /home/vault/Private/ /home/ryan/TheRepository/ /home/ryan/.ssh/
chown -R object:object /home/object/
