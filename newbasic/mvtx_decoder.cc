#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>
#include <oncsSub_idmvtxv3.h>
using namespace std;

void readMSD(vector<uint8_t> sensor_data)
{
    cout << hex;
  // Delete the identifier (20, 21, 22, etc)
  for(int i = 0; i < (int)(sensor_data.size()); i++){
    //if((sensor_data.at(i)>>4) == 0x2){ sensor_data.erase(sensor_data.begin()+i); i--; }
    if(sensor_data.at(i) >= 0x20 && sensor_data.at(i) <= 0x29){ sensor_data.erase(sensor_data.begin()+i); i--; }
  }

  for(int i = 0; i < (int)(sensor_data.size()); i++){
    if(sensor_data.at(i) == 0xff) cout << "IDLE:             " << (unsigned int)sensor_data.at(i) << " ";
    if(sensor_data.at(i) == 0xf1) cout << "BUSY ON:          " << (unsigned int)sensor_data.at(i) << " ";
    if(sensor_data.at(i) == 0xf0) cout << "BUSY OFF:         " << (unsigned int)sensor_data.at(i) << " ";
    if((sensor_data.at(i)>>4) == 0xb){
      cout << "Chip Trailer:     " << (unsigned int)sensor_data.at(i) << "     -> ";
      cout << "Readout Flags: " << (unsigned int)(sensor_data.at(i) & 0xf) << " " << endl;
      break;
    }
    if((sensor_data.at(i)>>5) == 0x6){
      cout << "Region Header:    " << (unsigned int)sensor_data.at(i) << "     -> ";
      cout << "Region ID:     " << (unsigned int)(sensor_data.at(i) & 0x1f) << " " << endl;
      continue;
    }
    if((sensor_data.at(i)>>4) == 0xa){
      if(i >= (int)(sensor_data.size())-2) continue;
      cout << "Chip Header:      " << (unsigned int)sensor_data.at(i) << " " << (unsigned int)sensor_data.at(i+1) << "  -> ";
      cout << "Chip ID: " << (unsigned int)(sensor_data.at(i) & 0xf) << " // ";
      cout << "Bunch Counter: " << (unsigned int)sensor_data.at(i+1) << " " << endl;
      if(i < (int)(sensor_data.size())-2) i++;
      continue;
    }
    if((sensor_data.at(i)>>4) == 0xe){
      if(i >= (int)(sensor_data.size())-2) continue;
      cout << "Chip Empty Frame: " << (unsigned int)sensor_data.at(i) << " " << (unsigned int)sensor_data.at(i+1) << "     -> ";
      cout << "Chip ID: " << (unsigned int)(sensor_data.at(i) & 0xf) << " // ";
      cout << "Bunch Counter: " << (unsigned int)sensor_data.at(i+1) << " " << endl;
      if(i < (int)(sensor_data.size())-2) i++;
      break;
    }
    if((sensor_data.at(i)>>6) == 0x1){
      if(i >= (int)(sensor_data.size())-2) continue;
      cout << "Data Short:       " << (unsigned int)sensor_data.at(i) << " " << (unsigned int)sensor_data.at(i+1) << "     -> ";
      uint16_t dsBin = (sensor_data.at(i)<<8) + sensor_data.at(i+1);
      cout << "Encoder ID:    " << (unsigned int)((dsBin>>10) & 0xf) << " // ";
      cout << "Address:       " << (unsigned int)(dsBin & 0x3ff) << " " << endl;
      if(i < (int)(sensor_data.size())-2) i++;
      continue;
    }
    if((sensor_data.at(i)>>6) == 0x0){
      if(i >= (int)(sensor_data.size())-3) continue;
      cout << "Data Long:        " << (unsigned int)sensor_data.at(i) << " " << (unsigned int)sensor_data.at(i+1) << " " << (unsigned int)sensor_data.at(i+2) << " -> ";
      uint32_t dlBin = (sensor_data.at(i)<<16) + (sensor_data.at(i+1)<<8) + sensor_data.at(i+2);
      cout << "Encoder ID:    " << (unsigned int)((dlBin>>18) & 0xf) << " // ";
      cout << "Address: " << (unsigned int)((dlBin>>8) & 0x3ff) << " // ";
      cout << "Hit Map: " << (unsigned int)(dlBin & 0x7f) << " " << endl;
      if(i < (int)(sensor_data.size())-3) i+=2;
      continue;
    }
  }
}

void wordHandler(int word_number, vector<uint8_t> vec, vector<uint8_t>& lane0data, vector<uint8_t>& lane1data, vector<uint8_t>& lane2data, bool isFDH, vector<uint8_t>& MSDword){
  cout << "   | ";
  cout << hex;

  bool isRDH = (vec[9] == 0 && !isFDH);
  bool isIHW = (vec[9] == 0xe0);
  bool isTDH = (vec[9] == 0xe8);
  bool isTDT = (vec[9] == 0xf0);
  bool isDDW = (vec[9] == 0xe4);
  bool isCDW = (vec[9] == 0xf8);
  bool isMSD = (vec[9] >= 0x20 && vec[9] <= 0x29);

  //Felix data header
  if(isFDH){
    bool isEmpty = true;
    for(int i = 0; i < (int)vec.size(); i++) if(vec[i] != 0xff) isEmpty = false;
    if(!isEmpty) cout << "//////////   Felix Data Header. Reading from lane " << (unsigned int)vec[8] << "   //////////" << endl;
    isFDH = false;
  }

  // Raw Data Header
  if(isRDH && word_number == 0){
    uint8_t  header_version = vec[0];
    uint8_t  header_size    = vec[1];
    uint16_t fee_id         = (vec[3]<<8) + vec[2];
    uint8_t  source_id      = vec[4];
    uint32_t detector_field = (vec[8]<<24) + (vec[7]<<16) + (vec[6]<<8) + vec[5];

    cout << "header_version: " << (unsigned int)header_version << " // ";
    cout << "header_size: "    << (unsigned int)header_size    << " // ";
    cout << "fee_id: "         << (unsigned int)fee_id         << " // ";
    cout << "source_id: "      << (unsigned int)source_id      << " // ";
    cout << "detector_field: " << (unsigned int)detector_field;
  }

  if(isRDH && word_number == 1){
    uint16_t lhc_bc    = ((vec[1] & 0xf)<<8) + vec[0];
    uint64_t gtm_bco_1 = (vec[7]<<24) + (vec[6]<<16) + (vec[5]<<8) + vec[4];
    uint64_t gtm_bco_2 = (vec[8]);
    uint64_t gtm_bco   = (gtm_bco_2<<32) + gtm_bco_1;

    cout << "lhc_bc: "  << (unsigned int)lhc_bc << " // ";
    cout << "gtm_bco: " << (unsigned long)gtm_bco;
  }

  if(isRDH && word_number == 2){
    uint32_t trigger_type   = (vec[3]<<24) + (vec[2]<<16) + (vec[1]<<8) + vec[0];
    uint16_t pages_counter  = (vec[5]<<8) + vec[4];
    uint8_t  stop_bit       = vec[6];
    uint8_t  priority_bit   = vec[7];

    cout << "trigger_type:";

    if(trigger_type        & 0x1) cout << " ORBIT";
    if((trigger_type>>1)   & 0x1) cout << " HB";
    if((trigger_type>>2)   & 0x1) cout << " HBr";
    if((trigger_type>>3)   & 0x1) cout << " HC";
    if((trigger_type>>4)   & 0x1) cout << " PhT";
    if((trigger_type>>5)   & 0x1) cout << " PP";
    if((trigger_type>>6)   & 0x1) cout << " Cal";
    if((trigger_type>>7)   & 0x1) cout << " SOT";
    if((trigger_type>>8)   & 0x1) cout << " EOT";
    if((trigger_type>>9)   & 0x1) cout << " SOC";
    if((trigger_type>>10)  & 0x1) cout << " EOC";
    if((trigger_type>>11)  & 0x1) cout << " TF";
    if((trigger_type>>12)  & 0x1) cout << " FErst";
    if((trigger_type>>13)  & 0x1) cout << " RT";
    if((trigger_type>>14)  & 0x1) cout << " RS";
    if((trigger_type>>15)  & 0x1) cout << " LHCgap1";
    if((trigger_type>>16)  & 0x1) cout << " LHCgap2";
    if((trigger_type>>17)  & 0x1) cout << " ITSrst";
    if((trigger_type>>18)  & 0x1) cout << " TPCrst";

    cout << " // ";
    cout << "pages_counter: "  << (unsigned int)pages_counter << " // ";
    cout << "stop_bit: "       << (unsigned int)stop_bit      << " // ";
    cout << "priority_bit: "   << (unsigned int)priority_bit;
  }

  // ITS Header Word
  if(isIHW){
    uint8_t IHW_id        = vec[9];
    uint32_t active_lanes = ((vec[3] & 0xf)<<24) + (vec[2]<<16) + (vec[1]<<8) + vec[0];

    cout << "IHW_id: "       << (unsigned int)IHW_id << " // ";
    cout << "active_lanes: " << (unsigned int)active_lanes;
  }

  // Trigger Data Header
  if(isTDH){
    uint8_t  TDH_id           = vec[9];
    uint64_t tdh_gtm_bco_1    = (vec[7]<<24) + (vec[6]<<16) + (vec[5]<<8) + vec[4];
    uint64_t tdh_gtm_bco_2    = vec[8];
    uint64_t tdh_gtm_bco      = (tdh_gtm_bco_2<<32) + tdh_gtm_bco_1;
    uint16_t tdh_lhc_bc       = ((vec[3] & 0xf)<<8) + vec[2];
    uint8_t  tdh_status       = ((vec[1]>>4) & 0x7);
    uint16_t tdh_trigger_type = ((vec[1] & 0xf)<<8) + vec[0];

    cout << "TDH_id: "           << (unsigned int)TDH_id      << " // ";
    cout << "tdh_gtm_bco: "      << (unsigned long)tdh_gtm_bco << " // ";
    cout << "tdh_lhc_bc: "       << (unsigned int)tdh_lhc_bc  << " // ";

    cout << "tdh_status:";
    if((tdh_status) & 0x1) cout << " Internal Trigger";
    if((tdh_status) & 0x2) cout << " No Data";
    if((tdh_status) & 0x4) cout << " Continuation";
    cout << " // ";

    cout << "tdh_trigger_type:";
    if(tdh_trigger_type       & 0x1) cout << " ORBIT";
    if((tdh_trigger_type>>1)  & 0x1) cout << " HB";
    if((tdh_trigger_type>>2)  & 0x1) cout << " HBr";
    if((tdh_trigger_type>>3)  & 0x1) cout << " HC";
    if((tdh_trigger_type>>4)  & 0x1) cout << " PhT";
    if((tdh_trigger_type>>5)  & 0x1) cout << " PP";
    if((tdh_trigger_type>>6)  & 0x1) cout << " Cal";
    if((tdh_trigger_type>>7)  & 0x1) cout << " SOT";
    if((tdh_trigger_type>>8)  & 0x1) cout << " EOT";
    if((tdh_trigger_type>>9)  & 0x1) cout << " SOC";
    if((tdh_trigger_type>>10) & 0x1) cout << " EOC";
    if((tdh_trigger_type>>11) & 0x1) cout << " TF";
  }

  // Trigger Data Trailer
  if(isTDT){
    uint8_t  TDT_id           = vec[9];
    uint8_t  tdt_error         = (vec[8] & 0xf);
    uint64_t tdt_lane_status_1 = (vec[3]<<24) + (vec[2]<<16) + (vec[1]<<8) + vec[0];
    uint64_t tdt_lane_status_2 = (vec[6]<<16) + (vec[5]<<8) + vec[4];
    uint64_t tdt_lane_status   = (tdt_lane_status_2<<32) + tdt_lane_status_1;

    cout << "TDT_id: "          << (unsigned int)TDT_id    << " // ";
    cout << "tdt_error: ";
    if(tdt_error & 0x8) cout << " lane_starts_violation";
    if(tdt_error & 0x2) cout << " transmission_timeout";
    if(tdt_error & 0x1) cout << " packet_done";
    cout << " // ";

    cout << "tdt_lane_status: ";
    bool lsError = false;
    for(int i = 0; i < 56; i+=2){
      if((tdt_lane_status>>i) & 0x1){ cout << "WARNING: Lane " << i; lsError = true; }
      if((tdt_lane_status>>i) & 0x2){ cout << "ERROR: Lane " << i; lsError = true; }
      if((tdt_lane_status>>i) & 0x3){ cout << "FAULT: Lane " << i; lsError = true; }
    }
    if(!lsError) cout << "OK";
    else lsError = false;

    if(lane0data.size()){ cout << endl << endl << "Lane 0:" << endl; readMSD(lane0data); cout << endl; }
    if(lane1data.size()){ cout << endl << "Lane 1:" << endl; readMSD(lane1data); cout << endl; }
    if(lane2data.size()){ cout << endl << "Lane 2:" << endl; readMSD(lane2data); }
    if(lane0data.size()){ lane0data.clear(); lane1data.clear(); lane2data.clear(); MSDword.clear();}
  }

  // Diagnostic Data Word
  if(isDDW){
    uint8_t  DDW_id            = vec[9];
    uint8_t  ddw_index         = ((vec[8]>>4) & 0xf);
    uint8_t  ddw_error         = ((vec[8]>>1) & 0x7);
    uint64_t ddw_lane_status_1 = (vec[3]<<24) + (vec[2]<<16) + (vec[1]<<8) + vec[0];
    uint64_t ddw_lane_status_2 = (vec[6]<<16) + (vec[5]<<8) + vec[4];
    uint64_t ddw_lane_status   = (ddw_lane_status_2<<32) + ddw_lane_status_1;

    cout << "DDW_id: "          << (unsigned int)DDW_id                << " // ";
    cout << "ddw_index: "       << (unsigned int)ddw_index << " // ";

    cout << "ddw_error: "       << (unsigned int)ddw_error;
    if(ddw_error & 0x4) cout << " lane_starts_violation";
    if(ddw_error & 0x1) cout << " transmission_timeout";
    cout << " // ";

    cout << "ddw_lane_status: ";
    bool lsError = false;
    for(int i = 0; i < 56; i+=2){
      if((ddw_lane_status>>i) & 0x1){ cout << "WARNING: Lane " << i; lsError = true; }
      if((ddw_lane_status>>i) & 0x2){ cout << "ERROR: Lane " << i; lsError = true; }
      if((ddw_lane_status>>i) & 0x3){ cout << "FAULT: Lane " << i; lsError = true; }
    }
    if(!lsError) cout << "OK";
    else lsError = false;
    cout << endl << endl << "------------------------------------------------------------------" << endl;
  }

  // Calibration Data Word
  if(isCDW){
    uint8_t  CDW_id            = vec[9];
    uint32_t cdw_index         = vec[8] + vec[7] + vec[6];
    uint64_t cdw_user_fields_1 = (vec[3]<<24) + (vec[2]<<16) + (vec[1]<<8) + vec[0];
    uint64_t cdw_user_fields_2 = (vec[5]<<8) + vec[4];
    uint64_t cdw_user_fields   = (cdw_user_fields_2<<32) + cdw_user_fields_1;

    cout << "CDW_id: "          << (unsigned int)CDW_id    << " // ";
    cout << "cdw_index: "       << (unsigned int)cdw_index << " // ";
    cout << "cdw_user_fields: " << (unsigned long)cdw_user_fields;
  }

  // MAP Sensor Data
  if(isMSD){
    cout << "Reading chip data...";
    bool newWord = true;
    if(!MSDword.size()){
        MSDword.push_back(vec[9]);
    }
    for(int i=0; i<(int)(MSDword.size()); i++){
        if(vec[9] == MSDword.at(i)){
            newWord = false;
        }
    }
    if(newWord){
        MSDword.push_back(vec[9]);
    }

    for(int i = 0; i < (int)(vec.size()); i++){
      if(MSDword.size()>0){ if(vec[9] == MSDword.at(0)) lane0data.push_back(vec.at(i)); }
      if(MSDword.size()>1){ if(vec[9] == MSDword.at(1)) lane1data.push_back(vec.at(i)); }
      if(MSDword.size()>2){ if(vec[9] == MSDword.at(2)) lane2data.push_back(vec.at(i)); }
    }
  }
/*
  // Felix data header
  if(isFDH){
      cout << "Is FDH!";
      for(int i = 0; i < 10; i++){
          cout << hex << vec[i].to_ulong();
      }
      stringstream flx_header_code;
      stringstream flx_header_version;
      stringstream flx_header_gbt_id;
      stringstream flx_header_count;
      stringstream flx_header_flxid;
      if(word_number == 0){
          flx_header_code << hex << vec[9].to_ulong();
          flx_header_version << hex << vec[8].to_ulong();
          flx_header_gbt_id << hex << vec[6].to_ulong();
          //cout << "flx_header_code: "          << flx_header_code.str()    << " // ";
          cout << "flx_header_version: "          << flx_header_version.str()    << " // ";
          cout << "flx_header_gbt_id: "          << flx_header_gbt_id.str()    << " // ";
      }
      if(word_number == 1){
          flx_header_count << hex << vec[8].to_ulong() << vec[7].to_ulong();
          flx_header_flxid << hex << vec[5].to_ulong();
      }

      cdw_index       << hex << vec[8].to_ulong() << vec[7].to_ulong() << vec[6].to_ulong();

  }
*/
}

void mvtx_decoder(std::vector<oncsSub_idmvtxv3::data32_mvtx*> data_vec)
{
  vector<uint8_t> vBuf;
  vector<uint8_t> lane0data;
  vector<uint8_t> lane1data;
  vector<uint8_t> lane2data;
  vector<uint8_t> MSDword;

  for(int dword = 0; dword < (int)(data_vec.size()); dword++){
    for(int i = 0; i < 3; i++){
      for(int j = 0; j <10; j++){
        vBuf.push_back(data_vec[dword]->d0[i][j]);
      }
    }
    vBuf.push_back(data_vec[dword]->counter);
    vBuf.push_back(data_vec[dword]->ruid);

    bool isFDH = false;
    if( (vBuf[vBuf.size()-1] & 0xab) == 0xab ){ cout << endl; isFDH = true; }

    vector<uint8_t> eightbitVec;
    bool goodLine = false;
    for(int word_number = 1; word_number < (int)(vBuf.size()+1); word_number++){
      eightbitVec.push_back(vBuf[word_number-1]);
      cout.fill('0');
      cout.width(2);
      if(word_number % 10 == 0){
        // skip any empty lines
        for(int i = 0; i < (int)(eightbitVec.size()); i++) if(eightbitVec.at(i) != 0) goodLine = true;
        if(!goodLine){eightbitVec.clear(); continue;}
        else goodLine = false;
        // print the line
        for(int i = (int)(eightbitVec.size()-1); i >= 0; i--){cout.width(2); cout << hex << (unsigned int)(eightbitVec.at(i)) << " ";}
        // decode
        wordHandler(word_number/10-1, eightbitVec, lane0data, lane1data, lane2data, isFDH, MSDword);
        eightbitVec.clear();
        cout << endl;
      }
    }
    cout << endl;
    vBuf.clear();
  }

  return;
}
