#include "packetConstants.h"



const char *get_mnemonic (const int structure, const int format)
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
    case(ID4SCALER): return "ID4SCALER";

    case(IDDIGITIZER_12S): return "IDDIGITIZER_12S";
    case(IDDIGITIZER_16S): return "IDDIGITIZER_16S";
    case(IDDIGITIZER_31S): return "IDDIGITIZER_31S";
      
    case(IDDIGITIZERV3_2S): return "IDDIGITIZERV3_2S";
    case(IDDIGITIZERV3_4S): return "IDDIGITIZERV3_4S";
    case(IDDIGITIZERV3_6S): return "IDDIGITIZERV3_6S";
    case(IDDIGITIZERV3_8S): return "IDDIGITIZERV3_8S";

    case(IDDIGITIZERV3_10S): return "IDDIGITIZERV3_10S";
    case(IDDIGITIZERV3_12S): return "IDDIGITIZERV3_12S";
    case(IDDIGITIZERV3_14S): return "IDDIGITIZERV3_14S";
    case(IDDIGITIZERV3_16S): return "IDDIGITIZERV3_16S";
    case(IDDIGITIZERV3_18S): return "IDDIGITIZERV3_18S";

    case(IDDIGITIZERV3_20S): return "IDDIGITIZERV3_20S";
    case(IDDIGITIZERV3_22S): return "IDDIGITIZERV3_22S";
    case(IDDIGITIZERV3_24S): return "IDDIGITIZERV3_24S";
    case(IDDIGITIZERV3_26S): return "IDDIGITIZERV3_26S";
    case(IDDIGITIZERV3_28S): return "IDDIGITIZERV3_28S";

    case(IDDIGITIZERV3_30S): return "IDDIGITIZERV3_30S";

    case(IDHAMMOND): return "IDHAMMOND";
    case(IDSAM): return "IDSAM";
    case(IDDCFEM): return "IDDCFEM";
    case(IDMIZNHC): return "IDMIZNHC";
    case(IDLL1_20S): return "IDLL1_20S";
    case(IDLL1v2_20S): return "IDLL1v2_20S";
    case(IDLL1_MBD): return "IDLL1_MBD";
    case(IDLL1_EMCAL_MON0): return "IDLL1_EMCAL_MON0";
    case(IDLL1_EMCAL_MON1): return "IDLL1_EMCAL_MON1";
    case(IDLL1_EMCAL_MON2): return "IDLL1_EMCAL_MON2";
    case(IDLL1_EMCAL_MON3): return "IDLL1_EMCAL_MON3"; 
    case(IDLL1_JET_HCAL_MON0): return "IDLL1_JET_HCAL_MON0";
    case(IDLL1_JET_HCAL_MON1): return "IDLL1_JET_HCAL_MON1";
    case(IDLL1_JET_HCAL_MON2): return "IDLL1_JET_HCAL_MON2";
    case(IDLL1_JET_HCAL_MON3): return "IDLL1_JET_HCAL_MON3";
    case(IDLL1_JET_HCAL_MON4): return "IDLL1_JET_HCAL_MON4";
    case(IDLL1_JET_EMCAL_MON0): return "IDLL1_JET_EMCAL_MON0";
    case(IDLL1_JET_EMCAL_MON1): return "IDLL1_JET_EMCAL_MON1";
    case(IDLL1_JET_EMCAL_MON2): return "IDLL1_JET_EMCAL_MON2";
    case(IDLL1_JET_EMCAL_MON3): return "IDLL1_JET_EMCAL_MON3";
    case(IDLL1_JET_EMCAL_MON4): return "IDLL1_JET_EMCAL_MON4";
    case(IDBBC_DCM0): return "IDBBC_DCM0";
    case(IDMVD_DCM0): return "IDMVD_DCM0";
    case(IDDCH_DCM0): return "IDDCH_DCM0";
    case(IDPC_DCM0): return "IDPC_DCM0";
    case(IDTEC_DCM0): return "IDTEC_DCM0";
    case(IDRICH_DCM0): return "IDRICH_DCM0";
    case(IDTOF_DCM0): return "IDTOF_DCM0";
    case(IDPBSC_DCM0): return "IDPBSC_DCM0";
    case(IDPBGL_DCM0): return "IDPBGL_DCM0";
    case(IDMUTA_DCM0): return "IDMUTA_DCM0";
    case(IDMUTC_DCM0): return "IDMUTC_DCM0";
    case(IDMUID_DCM0): return "IDMUID_DCM0";
    case(IDZDC_DCM0): return "IDZDC_DCM0";

    case(IDBBC_LL1): return "IDBBC_LL1";
    case(IDMVD_LL1): return "IDMVD_LL1";
    case(IDRICH_LL1): return "IDRICH_LL1";
    case(IDTOF_LL1): return "IDTOF_LL1";
    case(IDPBSC_LL1): return "IDPBSC_LL1";
    case(IDPBGL_LL1): return "IDPBGL_LL1";
    case(IDMUIDH_LL1): return "IDMUIDH_LL1";
    case(IDMUIDV_LL1): return "IDMUIDV_LL1";
    case(IDNTCZDC_LL1): return "IDNTCZDC_LL1";
    case(IDBIG_LL1): return "IDBIG_LL1";
    case(IDERT_E_LL1): return "IDERT_E_LL1";
    case(IDERT_W_LL1): return "IDERT_W_LL1";
    case(IDGL1): return "IDGL1";
    case (IDGL1P): return "IDGL1P";
    case(IDGL1_EVCLOCK): return "IDGL1_EVCLOCK";
    case (IDL2DECISION) : return "L2DECISION";
    case (IDL2PRIMITIVE) : return "L2PRIMITIVE";
    case(IDBBC_DCM1): return "IDBBC_DCM1";
    case(IDMVD_DCM1): return "IDMVD_DCM1";
    case(IDDCH_DCM1): return "IDDCH_DCM1";
    case(IDPC_DCM1): return "IDPC_DCM1";
    case(IDTEC_DCM1): return "IDTEC_DCM1";
    case(IDRICH_DCM1): return "IDRICH_DCM1";
    case(IDTOF_DCM1): return "IDTOF_DCM1";
    case(IDPBSC_DCM1): return "IDPBSC_DCM1";
    case(IDPBGL_DCM1): return "IDPBGL_DCM1";
    case(IDMUTA_DCM1): return "IDMUTA_DCM1";
    case(IDMUTC_DCM1): return "IDMUTC_DCM1";
    case(IDMUID_DCM1): return "IDMUID_DCM1";
    case(IDZDC_DCM1): return "IDZDC_DCM1";

    case(IDBBC_DCM2): return "IDBBC_DCM2";
    case(IDMVD_DCM2): return "IDMVD_DCM2";
    case(IDDCH_DCM2): return "IDDCH_DCM2";
    case(IDPC_DCM2): return "IDPC_DCM2";
    case(IDTEC_DCM2): return "IDTEC_DCM2";
    case(IDRICH_DCM2): return "IDRICH_DCM2";
    case(IDTOF_DCM2): return "IDTOF_DCM2";
    case(IDMUTA_DCM2): return "IDMUTA_DCM2";
    case(IDMUTC_DCM2): return "IDMUTC_DCM2";
      //    case(IDMUID_DCM2): return "IDMUID_DCM2";
    case(IDZDC_DCM2): return "IDZDC_DCM2";

      //    case(IDPC_DCM3): return "IDPC_DCM3";

    case(IDEMC_OLDSTYLE): return "IDEMC_OLDSTYLE";

    case(IDEMC_REFERENCE): return "IDEMC_REFERENCE";
    case(IDEMC_REFERENCE0SUP): return "IDEMC_REFERENCE0SUP";

    case(IDEMC_SHORTREFERENCE): return "IDEMC_SHORTREFERENCE";
    case(IDEMC_SHORTREFERENCE0SUP): return "IDEMC_SHORTREFERENCE0SUP";


    case(IDEMC_DCM32): return "IDEMC_DCM32";
    case(IDPBSC_DCMS): return "IDPBSC_DCMS";
    case(IDPBSC_DCMZS): return "IDPBSC_DCMZS";
      //    case(IDPBSC_DCM5): return "IDPBSC_DCM5";
      //    case(IDPBSC_DCM05): return "IDPBSC_DCM05";

    case(IDPBGL_DCM32): return "IDPBGL_DCM32";
    case(IDPBGL_DCMS): return "IDPBGL_DCMS";
    case(IDPBGL_DCMZS): return "IDPBGL_DCMZS";
      //case(IDPBGL_DCM5): return "IDPBGL_DCM5";
      //case(IDPBGL_DCM05): return "IDPBGL_DCM05";

    case(IDTOF_DCM16): return "IDTOF_DCM16";
 
    case(IDPC_FPGA): return "IDPC_FPGA";
    case(IDPC_FPGA0SUP): return "IDPC_FPGA0SUP";

    case(IDRICH_FPGA): return "IDRICH_FPGA";
    case(IDRICH_FPGA0SUP): return "IDRICH_FPGA0SUP";

    case(IDMUTC_FPGA): return "IDMUTC_FPGA";
    case(IDMUTC_FPGA0SUP): return "IDMUTC_FPGA0SUP";
    case(IDMUTC_FPGASHORT): return "IDMUTC_FPGASHORT";
    case(IDMUTC_FPGASHORTSUP): return "IDMUTC_FPGASHORTSUP";
    case(IDMUTC_FPGANEW): return "IDMUTC_FPGANEW";
    case(IDMUTC_FPGANEWSUP): return "IDMUTC_FPGANEWSUP";

    case(IDMUTC_15_FPGA): return "IDMUTC_15_FPGA";
    case(IDMUTC_15_FPGA0SUP): return "IDMUTC_15_FPGA0SUP";

    case(IDMUTC_DCM3): return "IDMUTC_DCM3";

    case(IDMVD_FPGA): return "IDMVD_FPGA";
    case(IDMVD_FPGA0SUP): return "IDMVD_FPGA0SUP";
    case(IDMVD_PED_FPGA0SUP): return "IDMVD_PED_FPGA0SUP";

      // case(IDMUID_FPGA): return "IDMUID_FPGA";
    case(IDMUID_FPGA0SUP): return "IDMUID_FPGA0SUP";

    case(IDBBC_FPGA): return "IDBBC_FPGA";
    case(IDBBC_FPGA0SUP): return "IDBBC_FPGA0SUP";

    case(IDZDC_FPGA): return "IDZDC_FPGA";
    case(IDZDC_FPGA0SUP): return "IDZDC_FPGA0SUP";

    case(IDTOF_FPGA): return "IDTOF_FPGA";
    case(IDTOF_FPGA0SUP): return "IDTOF_FPGA0SUP";

    case(IDTOFW_FPGA): return "IDTOFW_FPGA";
    case(IDTOFW_FPGA0SUP): return "IDTOFW_FPGA0SUP";

    case(IDEMC_FPGA): return "IDEMC_FPGA";
    case(IDEMC_FPGA0SUP): return "IDEMC_FPGA0SUP";
    case(IDEMC_FPGASHORT): return "IDEMC_FPGASHORT";
    case(IDEMC_FPGASHORT0SUP): return "IDEMC_FPGASHORT0SUP";

    case(IDEMC_FPGA3WORDS): return "IDEMC_FPGA3WORDS";
    case(IDEMC_FPGA3WORDS0SUP): return "IDEMC_FPGA3WORDS0SUP";

    case(IDNTCT0_FPGA): return "IDNTCT0_FPGA";
    case(IDNTCT0_FPGA0SUP): return "IDNTCT0_FPGA0SUP";

    case(IDRPC_DCM0): return "IDRPC_DCM0";
    case(IDRPC_FPGA): return "IDRPC_FPGA";
    case(IDRPC_FPGA0SUP): return "IDRPC_FPGA0SUP";


    case(IDEMCRICH_LL1): return "IDEMCRICH_LL1";

    case(IDFCAL_FPGA): return "IDFCAL_FPGA";
    case(IDFCAL_FPGA0SUP): return "IDFCAL_FPGA0SUP";
    case(IDFCAL_FPGA3): return "IDFCAL_FPGA3";
    case(IDFCAL_FPGA0SUP3): return "IDFCAL_FPGA0SUP3";

    case(IDHBD_FPGA): return "IDHBD_FPGA";
    case(IDHBD_FPGA0SUP): return "IDHBD_FPGA0SUP";
    case(IDHBD_FPGASHORT): return "IDHBD_FPGASHORT";
    case(IDHBD_FPGASHORT0SUP): return "IDHBD_FPGASHORT0SUP";

    case(IDFVTX_DCM0): return "IDFVTX_DCM0";
    case(IDFVTX_SIM): return "IDFVTX_SIM";

    case(IDRXNP_FPGASHORT): return "IDRXNP_FPGASHORT";
    case(IDRXNP_FPGASHORT0SUP): return "IDRXNP_FPGASHORT0SUP";


    case(IDCDEVIR):         return "IDCDEVIR";
    case(IDCDEVDVM):         return "IDCDEVDVM";
    case(IDCDEVRING):         return "IDCDEVRING";
    case(IDCDEVRINGPOL):     return "IDCDEVRINGPOL";
    case(IDCDEVRINGFILL):    return "IDCDEVRINGFILL";
    case(IDCDEVWCMHISTORY):  return "IDCDEVWCMHISTORY";
    case(IDCDEVSIS):         return "IDCDEVSIS";
    case(IDCDEVPOLARIMETER): return "IDCDEVPOLARIMETER";
    case(IDCDEVPOLDATA):     return "IDCDEVPOLDATA";
      //ejd ADD cdev packed identifiers
    case(IDCDEVPOLARIMETERTARGET):  return "IDCDEVPOLTARGET";
    case(IDCDEVBPM):       return "IDCDEVBPM";
    case(IDCDEVMADCH):     return "IDCDEVMADCH";
    case(IDCDEVBUCKETS):   return "IDCDEVBUCKETS";
    case(IDCDEVRINGNOPOL): return "IDCDEVRINGNOPOL";

    case (IDGL1PSUM):      return "IDGL1PSUM";
    case (IDGL1PSUMOBS):   return "IDGL1PSUMOBS";
    case (IDCDEVDESCR):    return "IDCDEVDESCR";
    case (IDSTARSCALER):    return "IDSTARSCALER";

    case (IDPXL_DCM0):    return "IDPXL_DCM0";
    case (IDMUTRG_DCM0):    return "IDMUTRG_DCM0";
    case (IDFOCAL_FPGATEST):    return "IDFOCAL_FPGATEST";


  }
  return "UNKNOWN";
}

// ---------------------------------------------------------------------

const char *get_type_mnemonic (const int id)
{
  switch (id)
  {
	case(1): return " 8-bit";
	case(2): return "16-bit";
	case(4): return "32-bit";
  }
  return "UNKNOWN";
}
