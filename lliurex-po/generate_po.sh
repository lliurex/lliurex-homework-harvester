#!/bin/bash

cp /srv/svn/leia/llx-apps/trunk/lliurex-homework-harvester/install-files/usr/bin/lliurex-homework-harvester 1.py
cp /srv/svn/leia/llx-apps/trunk/lliurex-homework-harvester/install-files/usr/bin/lliurex-homework-sender 2.py

xgettext 1.py 2.py /srv/svn/leia/llx-apps/trunk/lliurex-homework-harvester/install-files/usr/lib/nautilus/extensions-2.0/python/lliurex-homework-harverster-extension.py /srv/svn/leia/llx-apps/trunk/lliurex-homework-harvester/install-files/usr/share/lliurex-homework-harvester/rsrc/*.glade -o lliurex-homework-harvester/lliurex-homework-harvester.pot

rm 1.py
rm 2.py


