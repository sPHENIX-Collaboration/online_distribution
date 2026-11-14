AUTOMAKE_OPTIONS = foreign subdir-objects

if VXWORKS
include_HEADERS = oncsdefs.h \
  phenixTypes.h \
  EvtConstants.h \
  EvtStructures.h \
  oBuffer.h \
  EventTypes.h \
  BufferConstants.h

else
# all definitions

installedheaders = \
  generalDefinitions.h \
  event_io.h \
  ioselect.h \
  Event.h \
  Event_A.h \
  oncsEvtConstants.h \
  oncsEvtStructures.h \
  oncsStructures.h \
  oncsSubConstants.h \
  Packet_A.h \
  EventTypes.h \
  Eventiterator.h \
  EvtConstants.h \
  EvtStructures.h \
  BufferConstants.h \
  phenixTypes.h \
  phenixOnline.h \
  testEventiterator.h \
  fileEventiterator.h \
  listEventiterator.h \
  dpipe_filter.h \
  buffer.h \
  simpleRandom.h \
  eventReceiverClient.h \
  gzbuffer.h \
  lzobuffer.h \
  bz2buffer.h \
  oBuffer.h \
  ospBuffer.h \
  ospEvent.h \
  oncsBuffer.h \
  oncsEventiterator.h \
  rcdaqEventiterator.h \
  packet.h \
  packetConstants.h \
  decoding_routines.h \
  caen_correction.h \
  caen_calibration.h \
  ROC_pool.h \
  h2gcroc3_10G_pool.h \
  caloroc1b_pool.h \
  md5.h

allheaders = \
  ioselect.h \
  event_io.h \
  BoolDefs.h \
  devicedefs.h \
  oncsdefs.h \
  phenixOnline.h \
  phenixTypes.h \
  printmsg.h \
  BufferConstants.h \
  Event.h \
  EventTypes.h \
  Eventiterator.h \
  EvtConstants.h \
  EvtStructures.h \
  simpleRandom.h \
  testEventiterator.h \
  fileEventiterator.h \
  listEventiterator.h \
  md5.h \
  PHmd5Utils.h \
  gen_utilities.h \
  dpipe_filter.h \
  PHmd5Value.h \
  buffer.h \
  decoding_routines.h \
  generalDefinitions.h \
  gzbuffer.h \
  lzobuffer.h \
  oBuffer.h \
  ospBuffer.h \
  ospEvent.h \
  oncsBuffer.h \
  packet_collection.h \
  Event_A.h \
  oncsEventiterator.h \
  rcdaqEventiterator.h \
  oncsEvtConstants.h \
  oncsEvtStructures.h \
  oncsStructures.h \
  oncsSubConstants.h \
  Packet_id2evt.h \
  Packet_id4evt.h \
  Packet_id8evt.h \
  Packet_idrtclock.h \
  Packet_idsis3300.h \
  Packet_idsis3300r.h \
  Packet_idcaenv792.h \
  Packet_idcaenv1742.h \
  Packet_idcaenv1742calib.h \
  Packet_idbspetdata.h \
  Packet_idrcpetdata.h \
  Packet_iduppetdata.h \
  Packet_iduppetdata_v104.h \
  Packet_iduppetparams.h \
  Packet_idsrs_v01.h \
  Packet_idfnalmwpc.h \
  Packet_idfnalmwpcv2.h \
  Packet_iddrs4v1.h \
  Packet_iddigitizerv1.h \
  Packet_idh2gcroc3.h \
  Packet_idh2gcroc3_10G.h \
  Packet_idcaloroc1b.h \
  Packet_idastrov1.h \
  Packet_A.h \
  packet.h \
  packetConstants.h \
  caen_correction.h \
  caen_calibration.h \
  ROC_pool.h \
  h2gcroc3_10G_pool.h \
  caloroc1b_pool.h


allsources = \
  buffer.cc \
  decoding_routines.cc \
  evt_mnemonic.cc \
  simpleRandom.cc \
  eventReceiverClient.cc \
  testEventiterator.cc \
  fileEventiterator.cc \
  listEventiterator.cc \
  md5.cc \
  PHmd5Utils.cc \
  PHmd5Value.cc \
  gzbuffer.cc \
  lzobuffer.cc \
  bz2buffer.cc \
  ospEvent.cc \
  oBuffer.cc \
  ospBuffer.cc \
  oncsBuffer.cc \
  Event_A.cc \
  oncsEventiterator.cc \
  rcdaqEventiterator.cc \
  Packet_idcstr.cc \
  Packet_id2evt.cc \
  Packet_id4evt.cc \
  Packet_id8evt.cc \
  Packet_idrtclock.cc \
  Packet_idsis3300.cc \
  Packet_idsis3300r.cc \
  Packet_idcaenv792.cc \
  Packet_idcaenv1742.cc \
  Packet_idcaenv1742calib.cc \
  Packet_idbspetdata.cc \
  Packet_idrcpetdata.cc \
  Packet_iduppetdata.cc \
  Packet_iduppetdata_v104.cc \
  Packet_iduppetparams.cc \
  Packet_idsrs_v01.cc \
  Packet_idfnalmwpc.cc \
  Packet_idfnalmwpcv2.cc \
  Packet_iddrs4v1.cc \
  Packet_idh2gcroc3.cc \
  Packet_idh2gcroc3_10G.cc \
  Packet_idcaloroc1b.cc \
  Packet_idastrov1.cc \
  Packet_A.cc \
  oncs_mnemonic.cc \
  packet_mnemonic.cc \
  caen_correction.cc \
  caen_calibration.cc \
  ROC_pool.cc \
  h2gcroc3_10G_pool.cc \
  caloroc1b_pool.cc


AM_LDFLAGS = -L$(libdir) -L$(ONLINE_MAIN)/lib

BUILT_SOURCES = ioselect.h


msgheaders = \
  msg_control.h \
  msg_profile.h \
  msg_buffer.h \
  date_filter_msg_buffer.h \
  filter_msg_buffer.h \
  remote_msg_buffer.h \
  strnstr.h

AM_CPPFLAGS =  -I$(includedir)  -I@ROOTINC@

noinst_HEADERS = \
  eventLinkDef.h \
  rangeParser.h \
  all.h

pkginclude_HEADERS = $(installedheaders) $(msgheaders)

# NOTE: previously gated "if ! MAKEROOT6" (ROOT5 only). The rootcint
# recipes for msg_Dict.C/event_Dict.C below work fine on ROOT6 too (its
# rootcint is a rootcling wrapper) - the guard just meant
# libRootmessage_la_SOURCES/libEvent_la_SOURCES stayed empty on any
# ROOT6 host, so those libraries built as dictionary-less shells even
# though nobase_dist_pcm_DATA still installed their .pcm files (that
# install rule was never gated, so rootcint still ran as a side effect
# of the %_Dict_rdict.pcm: %_Dict.C ; dependency - it just never got
# compiled into the library). Confirmed on a ROOT 6.40 build:
# TClass::GetClass("Event")->HasDictionary() was false before this fix.
ROOT5_MSGDICTS = \
  msg_Dict.C
ROOT5_EVENTDICTS = \
  event_Dict.C

CLEANFILES = \
  ioselect.h \
  event_Dict.C \
  event_Dict.h \
  setup.com \
  msg_Dict.C \
  msg_Dict.h

lib_LTLIBRARIES = \
  libmessage.la \
  libRootmessage.la \
  libNoRootEvent.la \
  libEvent.la

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  event_Dict_rdict.pcm \
  msg_Dict_rdict.pcm

bin_PROGRAMS = \
  dpipe \
  ddump \
  dlist \
  eventcombiner \
  prdfcheck \
  prdfsplit \
  eventServer \
  eventServer_classic \
  eventClient \
  lastEvent \
  pool_validate



dpipe_SOURCES = dpipe.cc
dlist_SOURCES = dlist.cc
ddump_SOURCES = ddump.cc
eventcombiner_SOURCES = eventcombiner.cc

if LINUX
dpipe_LDFLAGS = -Wl,-export-dynamic
endif

prdfcheck_SOURCES = prdfcheck.cc
prdfsplit_SOURCES = prdfsplit.cc
eventServer_SOURCES = eventServer.cc
eventServer_classic_SOURCES = eventServer_classic.cc
eventClient_SOURCES = eventClient.cc
lastEvent_SOURCES = lastEvent.cc
pool_validate_SOURCES = pool_validate.cc


dpipe_LDADD = libNoRootEvent.la libmessage.la  -ldl
dlist_LDADD = libNoRootEvent.la libmessage.la  -ldl
ddump_LDADD = libNoRootEvent.la libmessage.la  -ldl
eventcombiner_LDADD = libNoRootEvent.la libmessage.la -ldl


prdfcheck_LDADD = libNoRootEvent.la -ldl
prdfsplit_LDADD = libNoRootEvent.la -ldl

eventServer_LDADD = libNoRootEvent.la -ldl -lpthread
eventServer_classic_LDADD = libNoRootEvent.la -ldl -lpthread
eventClient_LDADD = libNoRootEvent.la -ldl
lastEvent_LDADD = libNoRootEvent.la -ldl
pool_validate_LDADD = libNoRootEvent.la -ldl


libmessage_la_SOURCES = \
  date_filter_msg_buffer.cc \
  filter_msg_buffer.cc \
  remote_msg_buffer.cc \
  msg_buffer.cc \
  msg_control.cc \
  strnstr.cc


libEvent_la_SOURCES =  \
  $(ROOT5_EVENTDICTS)

libEvent_la_LIBADD = libNoRootEvent.la libRootmessage.la  @ROOTGLIBS@  -lz @LZOLIB@ -lbz2

libNoRootEvent_la_SOURCES = $(allsources)
libNoRootEvent_la_LIBADD = libmessage.la  -lz @LZOLIB@ -lbz2


# because this if statement contains dependencies, no more definitions after
# this

if NEWIO
ioselect.h:
	echo "// *** this is a generated file. Do not commit, do not edit" > ioselect.h
	echo "#ifndef __IOSELECT_H__" >> ioselect.h
	echo "#define __IOSELECT_H__" >> ioselect.h
	echo  >> ioselect.h
	echo "#define STREAMBUF_NEW_IOSTREAM" >> ioselect.h
	echo  >> ioselect.h
	echo "#endif" >> ioselect.h
else
ioselect.h:
	echo "// *** this is a generated file. Do not commit, do not edit" > ioselect.h
	echo "#ifndef __IOSELECT_H__" >> ioselect.h
	echo "#define __IOSELECT_H__" >> ioselect.h
	echo  >> ioselect.h
	echo "#endif" >> ioselect.h

endif

if LINUX

libRootmessage_la_SOURCES = \
  $(ROOT5_MSGDICTS)

#  msg_dict.C

msg_Dict.C : \
  ioselect.h \
  msg_control.h \
  date_filter_msg_buffer.h \
  filter_msg_buffer.h \
  remote_msg_buffer.h \
  msgLinkDef.h
	rootcint -f $@ @CINTDEFS@ -c $(DEFS) $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

libRootmessage_la_LIBADD = libmessage.la

else

libRootmessage_la_SOURCES = \
  date_filter_msg_buffer.cc \
  filter_msg_buffer.cc \
  msg_buffer.cc \
  msg_control.cc \
  remote_msg_buffer.cc \
  strnstr.cc

endif



event_Dict.C : \
  ioselect.h \
  Event.h  \
  Eventiterator.h \
  fileEventiterator.h \
  listEventiterator.h \
  oncsEventiterator.h \
  rcdaqEventiterator.h \
  packet.h \
  testEventiterator.h \
  eventReceiverClient.h \
  ROC_pool.h \
  h2gcroc3_10G_pool.h \
  caloroc1b_pool.h \
  eventLinkDef.h
	rootcint -f $@ @CINTDEFS@ -c $(DEFS) $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^


%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

# get the dependency for the pcm files
%_Dict_rdict.pcm: %_Dict.C ;


# clean cache dir on Solaris 5.8
clean-local:
	rm -rf SunWS_cache
	rm -f *Dict* $(BUILT_SOURCES) *.pcm

#if VXWORKS
endif
