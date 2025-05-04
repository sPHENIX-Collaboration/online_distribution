#include "oncsSubConstants.h"


// ---------------------------------------------------------------------

const char *oncs_get_mnemonic (const int structure, const int format)
{
  // if we are not "Unformatted", we return nothing for now.
  // later we will also return the hit formats.

  if (structure) return "";

  switch (format)
    {
    case(IDCRAW): return "IDCRAW";
    case(IDDGEN): return "IDDGEN";
    case(IDHCPY): return "IDHCPY";
    case(ID1STR): return "ID1STR";
    case(IDCSTR): return "IDCSTR";
    case(ID2EVT): return "ID2EVT";
    case(ID4EVT): return "ID4EVT";
    case(ID2SUP): return "ID2SUP";
    case(IDHAMMOND): return "IDHAMMOND";
    case(IDSAM): return "IDSAM";
    case(IDDCFEM): return "IDDCFEM";
    case(IDMIZNHC): return "IDMIZNHC";
    case(IDSIS3300): return "IDSIS3300";
    case(IDSIS3300R): return "IDSIS3300R";
    case(IDCAENV792): return  "IDCAENV792";
    case(IDCAENV785N): return "IDCAENV785N";
    case(IDFIFOBOARD): return "IDFIFOBOARD";
    case(IDBSPETDATA): return "IDBSPETDATA";
    case(IDRCPETDATA): return "IDRCPETDATA";
    case(IDUPPETDATA): return "IDUPPETDATA";
    case(IDUPPETDATA_V104): return "IDUPPETDATA_V104";
    case(IDUPPETPARAMS): return "IDUPPETPARAMS";
    case(IDSRSV01): return "IDSRSV01";
    case(IDFNALMWPC): return "IDFNALMWPC";
    case(IDFNALMWPCV2): return "IDFNALMWPCV2";
    case(IDDRS4V1): return "IDDRS4V1";
    case(IDCAENV1742): return  "IDCAENV1742";
    case(IDMVTXV0): return  "IDMVTXV0";
    case(IDDIGITIZERV1): return  "IDDIGITIZERV1";
    case(IDTPCFEEV1): return  "IDTPCFEEV1";
    case(IDTPCFEEV2): return "IDTPCFEEV2"; 
    case(IDTPCFEEV3): return "IDTPCFEEV3"; 
    case(IDTPCFEEV4): return "IDTPCFEEV4"; 
    case(IDVMM3V1):return "IDVMM3V1";
    case(IDDREAMV0): return "IDDREAMV0";
    case(IDMVTXV1): return  "IDMVTXV1";
    case(IDMVTXV2): return  "IDMVTXV2";
    case(IDMVTXV3): return  "IDMVTXV3";
    case(IDINTTV0): return  "IDINTTV0";
    case(IDGL1V0):  return  "IDGL1V0";
    case(IDGL1V1):  return  "IDGL1V1";
    case(IDH2GCROC3):  return  "IDH2GCROC3";
  
  }
  return "UNKNOWN";
}

