#################################################################
#   GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY    #
#   Please read the README.txt file in the model directory.     #
#################################################################
MAN1 =
MAN3 = zactor.3 zauth.3 zarmour.3 zbeacon.3 zcert.3 zcertstore.3 zchunk.3 zclock.3 zconfig.3 zdigest.3 zdir.3 zdir_patch.3 zfile.3 zframe.3 zgossip.3 zhashx.3 ziflist.3 zlistx.3 zloop.3 zmonitor.3 zmsg.3 zpoller.3 zproxy.3 zrex.3 zsock.3 zsock_option.3 zstr.3 zsys.3 zuuid.3 zauth_v2.3 zbeacon_v2.3 zctx.3 zhash.3 zlist.3 zmonitor_v2.3 zmutex.3 zproxy_v2.3 zsocket.3 zsockopt.3 zthread.3
MAN7 = czmq.7
MAN_DOC = $(MAN1) $(MAN3) $(MAN7)

MAN_TXT = $(MAN1:%.1=%.txt)
MAN_TXT += $(MAN3:%.3=%.txt)
MAN_TXT += $(MAN7:%.7=%.txt)

EXTRA_DIST = asciidoc.conf mkman $(MAN_TXT)

if BUILD_DOC
if INSTALL_MAN
dist_man_MANS = $(MAN_DOC)
endif

MAINTAINERCLEANFILES = $(MAN_DOC)

dist-hook : $(MAN_DOC)

SUFFIXES=.txt .xml .1 .3 .7

.txt.xml:
	$(srcdir)/mkman $<
	asciidoc -d manpage -b docbook -f $(srcdir)/asciidoc.conf \
		-aczmq_version=@PACKAGE_VERSION@ -o$@ $<
.xml.1:
	xmlto man $<
.xml.3:
	xmlto man $<
.xml.7:
	xmlto man $<
endif
#################################################################
#   GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY    #
#   Please read the README.txt file in the model directory.     #
#################################################################
