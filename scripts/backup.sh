#!/bin/bash


cd /home/ryan/TheRepository
git bundle create /home/object/Objects/Backups/public-$(date +%Y.%m.%d).bundle --all
cd /home/vault/Private
git bundle create /home/object/Objects/Backups/private-$(date +%Y.%m.%d).bundle --all
chown -R object:object /home/object/
