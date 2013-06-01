MAN1 =
MAN3 = zbeacon.3 \
    zclock.3 \
    zconfig.3 \
    zctx.3 \
    zfile.3 \
    zframe.3 \
    zhash.3 \
    zlist.3 \
    zloop.3 \
    zmsg.3 \
    zmutex.3 \
    zsocket.3 \
    zsockopt.3 \
    zstr.3 \
    zsys.3 \
    zthread.3 \
    ztree.3
MAN7 = czmq.7
MAN_DOC = $(MAN1) $(MAN3) $(MAN7)

MAN_TXT = $(MAN1:%.1=%.txt)
MAN_TXT += $(MAN3:%.3=%.txt)
MAN_TXT += $(MAN7:%.7=%.txt)

if INSTALL_MAN
dist_man_MANS = $(MAN_DOC)
endif

EXTRA_DIST = asciidoc.conf mkman $(MAN_TXT)

MAINTAINERCLEANFILES = $(MAN_DOC)

dist-hook : $(MAN_DOC)

if BUILD_DOC
SUFFIXES=.txt .xml .1 .3 .7

.txt.xml:
	$(srcdir)/mkman $<
	asciidoc -d manpage -b docbook -f  $(srcdir)/asciidoc.conf \
		-aczmq_version=@PACKAGE_VERSION@ -o$@ $<
.xml.1:
	xmlto man $<
.xml.3:
	xmlto man $<
.xml.7:
	xmlto man $<
endif
