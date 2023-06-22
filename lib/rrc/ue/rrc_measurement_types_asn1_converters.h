/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/rrc_nr/common.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg.h"
#include "srsran/asn1/rrc_nr/msg_common.h"
#include "srsran/cu_cp/meas_types.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/srslog/srslog.h"
#include <string>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

/// \brief Converts type \c subcarrier_spacing to an RRC NR ASN.1 type.
/// \param sc_spacing subcarrier spacing object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
inline asn1::rrc_nr::subcarrier_spacing_e subcarrier_spacing_to_rrc_asn1(subcarrier_spacing sc_spacing)
{
  asn1::rrc_nr::subcarrier_spacing_e asn1_sc_spacing;

  switch (sc_spacing) {
    case srsran::subcarrier_spacing::kHz15:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::khz15;
      break;
    case srsran::subcarrier_spacing::kHz30:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::khz30;
      break;
    case srsran::subcarrier_spacing::kHz60:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::khz60;
      break;
    case srsran::subcarrier_spacing::kHz120:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::khz120;
      break;
    case srsran::subcarrier_spacing::kHz240:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::khz240;
      break;
    default:
      asn1_sc_spacing = asn1::rrc_nr::subcarrier_spacing_opts::options::nulltype;
  }

  return asn1_sc_spacing;
}

/// \brief Converts type \c ssb_mtc to an RRC NR ASN.1 type.
/// \param ssb_mtc ssb mtc object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
inline asn1::rrc_nr::ssb_mtc_s ssb_mtc_to_rrc_asn1(cu_cp_ssb_mtc ssb_mtc)
{
  asn1::rrc_nr::ssb_mtc_s asn1_ssb_mtc;

  if (ssb_mtc.periodicity_and_offset.sf5.has_value()) {
    asn1_ssb_mtc.periodicity_and_offset.set_sf5();
    asn1_ssb_mtc.periodicity_and_offset.sf5() = ssb_mtc.periodicity_and_offset.sf5.value();
  } else if (ssb_mtc.periodicity_and_offset.sf10.has_value()) {
    asn1_ssb_mtc.periodicity_and_offset.set_sf10();
    asn1_ssb_mtc.periodicity_and_offset.sf10() = ssb_mtc.periodicity_and_offset.sf10.value();
  } else if (ssb_mtc.periodicity_and_offset.sf20.has_value()) {
    asn1_ssb_mtc.periodicity_and_offset.set_sf20();
    asn1_ssb_mtc.periodicity_and_offset.sf20() = ssb_mtc.periodicity_and_offset.sf20.value();
  } else if (ssb_mtc.periodicity_and_offset.sf40.has_value()) {
    asn1_ssb_mtc.periodicity_and_offset.set_sf40();
    asn1_ssb_mtc.periodicity_and_offset.sf40() = ssb_mtc.periodicity_and_offset.sf40.value();
  } else if (ssb_mtc.periodicity_and_offset.sf80.has_value()) {
    asn1_ssb_mtc.periodicity_and_offset.set_sf10();
    asn1_ssb_mtc.periodicity_and_offset.sf80() = ssb_mtc.periodicity_and_offset.sf80.value();
  } else if (ssb_mtc.periodicity_and_offset.sf160.has_value()) {
    asn1_ssb_mtc.periodicity_and_offset.set_sf160();
    asn1_ssb_mtc.periodicity_and_offset.sf160() = ssb_mtc.periodicity_and_offset.sf160.value();
  } else {
    srslog::fetch_basic_logger("RRC").error("Invalid SSB MTC configuration.");
  }

  asn1::number_to_enum(asn1_ssb_mtc.dur, ssb_mtc.dur);

  return asn1_ssb_mtc;
}

inline asn1::rrc_nr::ssb_cfg_mob_s ssb_cfg_mob_to_rrc_asn1(const cu_cp_ssb_cfg_mob& ssb_cfg_mob)
{
  asn1::rrc_nr::ssb_cfg_mob_s asn1_ssb_cfg_mob;

  // ssb to measure
  if (ssb_cfg_mob.ssb_to_measure.has_value()) {
    const auto& ssb_to_measure              = ssb_cfg_mob.ssb_to_measure.value();
    asn1_ssb_cfg_mob.ssb_to_measure_present = true;
    // release
    if (ssb_to_measure.is_release) {
      asn1_ssb_cfg_mob.ssb_to_measure.set_release();
    } else if (ssb_to_measure.setup.has_value()) {
      // setup
      asn1_ssb_cfg_mob.ssb_to_measure.set_setup();
      // short bitmap
      if (ssb_to_measure.setup.value().short_bitmap.has_value()) {
        asn1_ssb_cfg_mob.ssb_to_measure.setup().set_short_bitmap();
        asn1_ssb_cfg_mob.ssb_to_measure.setup().short_bitmap().from_number(
            ssb_to_measure.setup.value().short_bitmap.value());
      } else if (ssb_to_measure.setup.value().medium_bitmap.has_value()) {
        // medium bitmap
        asn1_ssb_cfg_mob.ssb_to_measure.setup().set_medium_bitmap();
        asn1_ssb_cfg_mob.ssb_to_measure.setup().medium_bitmap().from_number(
            ssb_to_measure.setup.value().medium_bitmap.value());
      } else if (ssb_to_measure.setup.value().long_bitmap.has_value()) {
        // long bitmap
        asn1_ssb_cfg_mob.ssb_to_measure.setup().set_long_bitmap();
        asn1_ssb_cfg_mob.ssb_to_measure.setup().long_bitmap().from_number(
            ssb_to_measure.setup.value().long_bitmap.value());
      }
      // error
      srslog::fetch_basic_logger("RRC").error("Invalid ssb to measure.");
    }
  }

  // derive ssb idx from cell
  asn1_ssb_cfg_mob.derive_ssb_idx_from_cell = ssb_cfg_mob.derive_ssb_idx_from_cell;

  // ss rssi meas
  if (ssb_cfg_mob.ss_rssi_meas.has_value()) {
    const auto& ss_rssi_meas              = ssb_cfg_mob.ss_rssi_meas.value();
    asn1_ssb_cfg_mob.ss_rssi_meas_present = true;
    asn1_ssb_cfg_mob.ss_rssi_meas.meas_slots.from_number(ss_rssi_meas.meas_slots);
    asn1_ssb_cfg_mob.ss_rssi_meas.end_symbol = ss_rssi_meas.end_symbol;
  }

  return asn1_ssb_cfg_mob;
}

inline asn1::setup_release_c<asn1::rrc_nr::csi_rs_res_cfg_mob_s>
csi_res_cfg_mob_to_rrc_asn1(const cu_cp_csi_rs_res_cfg_mob_setup_release& csi_rs_res_cfg_mob)
{
  asn1::setup_release_c<asn1::rrc_nr::csi_rs_res_cfg_mob_s> asn1_csi_rs_res_cfg_mob;

  if (csi_rs_res_cfg_mob.is_release) {
    asn1_csi_rs_res_cfg_mob.set_release();
  } else if (csi_rs_res_cfg_mob.setup.has_value()) {
    asn1_csi_rs_res_cfg_mob.set_setup();
    // subcarrier spacing
    asn1_csi_rs_res_cfg_mob.setup().subcarrier_spacing =
        subcarrier_spacing_to_rrc_asn1(csi_rs_res_cfg_mob.setup.value().sc_spacing);
    // csi rs cell list mob
    for (const auto& csi_rs_cell_mob : csi_rs_res_cfg_mob.setup.value().csi_rs_cell_list_mob) {
      asn1::rrc_nr::csi_rs_cell_mob_s asn1_csi_rs_cell_mob;

      // cell id
      asn1_csi_rs_cell_mob.cell_id = csi_rs_cell_mob.cell_id;
      // csi rs meas bw
      asn1::number_to_enum(asn1_csi_rs_cell_mob.csi_rs_meas_bw.nrof_prbs, csi_rs_cell_mob.csi_rs_meas_bw.nrof_prbs);
      asn1_csi_rs_cell_mob.csi_rs_meas_bw.start_prb = csi_rs_cell_mob.csi_rs_meas_bw.start_prb;
      // density
      if (csi_rs_cell_mob.density.has_value()) {
        asn1_csi_rs_cell_mob.density_present = true;
        asn1::number_to_enum(asn1_csi_rs_cell_mob.density, csi_rs_cell_mob.density.value());
      }
      // csi rs res list mob
      for (const auto& csi_rs_res_mob : csi_rs_cell_mob.csi_rs_res_list_mob) {
        asn1::rrc_nr::csi_rs_res_mob_s asn1_csi_rs_res_mob;

        // csi rs idx
        asn1_csi_rs_res_mob.csi_rs_idx = csi_rs_res_mob.csi_rs_idx;
        // slot cfg
        if (csi_rs_res_mob.slot_cfg.ms4.has_value()) {
          asn1_csi_rs_res_mob.slot_cfg.set_ms4();
          asn1_csi_rs_res_mob.slot_cfg.ms4() = csi_rs_res_mob.slot_cfg.ms4.value();
        } else if (csi_rs_res_mob.slot_cfg.ms5.has_value()) {
          asn1_csi_rs_res_mob.slot_cfg.set_ms5();
          asn1_csi_rs_res_mob.slot_cfg.ms5() = csi_rs_res_mob.slot_cfg.ms5.value();
        } else if (csi_rs_res_mob.slot_cfg.ms10.has_value()) {
          asn1_csi_rs_res_mob.slot_cfg.set_ms10();
          asn1_csi_rs_res_mob.slot_cfg.ms10() = csi_rs_res_mob.slot_cfg.ms10.value();
        } else if (csi_rs_res_mob.slot_cfg.ms20.has_value()) {
          asn1_csi_rs_res_mob.slot_cfg.set_ms20();
          asn1_csi_rs_res_mob.slot_cfg.ms20() = csi_rs_res_mob.slot_cfg.ms20.value();
        } else if (csi_rs_res_mob.slot_cfg.ms40.has_value()) {
          asn1_csi_rs_res_mob.slot_cfg.set_ms40();
          asn1_csi_rs_res_mob.slot_cfg.ms40() = csi_rs_res_mob.slot_cfg.ms40.value();
        } else {
          // error
          srslog::fetch_basic_logger("RRC").error("Invalid slot cfg.");
        }
        // associated ssb
        if (csi_rs_res_mob.associated_ssb.has_value()) {
          asn1_csi_rs_res_mob.associated_ssb_present = true;
          asn1_csi_rs_res_mob.associated_ssb.ssb_idx = csi_rs_res_mob.associated_ssb.value().ssb_idx;
          asn1_csi_rs_res_mob.associated_ssb.is_quasi_colocated =
              csi_rs_res_mob.associated_ssb.value().is_quasi_colocated;
        }
        // freq domain alloc
        if (csi_rs_res_mob.freq_domain_alloc.row1.has_value()) {
          asn1_csi_rs_res_mob.freq_domain_alloc.set_row1();
          asn1_csi_rs_res_mob.freq_domain_alloc.row1().from_number(csi_rs_res_mob.freq_domain_alloc.row1.value());
        } else if (csi_rs_res_mob.freq_domain_alloc.row2.has_value()) {
          asn1_csi_rs_res_mob.freq_domain_alloc.set_row2();
          asn1_csi_rs_res_mob.freq_domain_alloc.row2().from_number(csi_rs_res_mob.freq_domain_alloc.row2.value());
        } else {
          // error
          srslog::fetch_basic_logger("RRC").error("Invalid freq domain alloc.");
        }
        // first ofdm symbol in time domain
        asn1_csi_rs_res_mob.first_ofdm_symbol_in_time_domain = csi_rs_res_mob.first_ofdm_symbol_in_time_domain;
        // seq generation cfg
        asn1_csi_rs_res_mob.seq_generation_cfg = csi_rs_res_mob.seq_generation_cfg;

        asn1_csi_rs_cell_mob.csi_rs_res_list_mob.push_back(asn1_csi_rs_res_mob);
      }

      asn1_csi_rs_res_cfg_mob.setup().csi_rs_cell_list_mob.push_back(asn1_csi_rs_cell_mob);
    }
  } else {
    // error
    srslog::fetch_basic_logger("RRC").error("Invalid csi rs res cfg mob.");
  }

  return asn1_csi_rs_res_cfg_mob;
}

inline asn1::rrc_nr::thres_nr_s thres_nr_to_rrc_asn1(const cu_cp_thres_nr& thres_nr)
{
  asn1::rrc_nr::thres_nr_s asn1_thres_nr;

  // thres rsrp
  if (thres_nr.thres_rsrp.has_value()) {
    asn1_thres_nr.thres_rsrp_present = true;
    asn1_thres_nr.thres_rsrp         = thres_nr.thres_rsrp.value();
  }

  // thres rsrq
  if (thres_nr.thres_rsrq.has_value()) {
    asn1_thres_nr.thres_rsrq_present = true;
    asn1_thres_nr.thres_rsrq         = thres_nr.thres_rsrq.value();
  }

  // thres sinr
  if (thres_nr.thres_sinr.has_value()) {
    asn1_thres_nr.thres_sinr_present = true;
    asn1_thres_nr.thres_sinr         = thres_nr.thres_sinr.value();
  }

  return asn1_thres_nr;
}

inline asn1::rrc_nr::q_offset_range_list_s
q_offset_range_list_to_rrc_asn1(const cu_cp_q_offset_range_list& q_offset_range_list)
{
  asn1::rrc_nr::q_offset_range_list_s asn1_q_offset_range_list;

  // rsrp offset ssb
  if (q_offset_range_list.rsrp_offset_ssb.has_value()) {
    asn1_q_offset_range_list.rsrp_offset_ssb_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.rsrp_offset_ssb, q_offset_range_list.rsrp_offset_ssb.value());
  }
  // rsrq offset ssb
  if (q_offset_range_list.rsrq_offset_ssb.has_value()) {
    asn1_q_offset_range_list.rsrq_offset_ssb_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.rsrq_offset_ssb, q_offset_range_list.rsrq_offset_ssb.value());
  }
  // sinr offset ssb
  if (q_offset_range_list.sinr_offset_ssb.has_value()) {
    asn1_q_offset_range_list.sinr_offset_ssb_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.sinr_offset_ssb, q_offset_range_list.sinr_offset_ssb.value());
  }
  // rsrp offset csi_rs
  if (q_offset_range_list.rsrp_offset_csi_rs.has_value()) {
    asn1_q_offset_range_list.rsrp_offset_csi_rs_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.rsrp_offset_csi_rs, q_offset_range_list.rsrp_offset_csi_rs.value());
  }
  // rsrq offset csi_rs
  if (q_offset_range_list.rsrq_offset_csi_rs.has_value()) {
    asn1_q_offset_range_list.rsrq_offset_csi_rs_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.rsrq_offset_csi_rs, q_offset_range_list.rsrq_offset_csi_rs.value());
  }
  // sinr offset csi_rs
  if (q_offset_range_list.sinr_offset_csi_rs.has_value()) {
    asn1_q_offset_range_list.sinr_offset_csi_rs_present = true;
    asn1::number_to_enum(asn1_q_offset_range_list.sinr_offset_csi_rs, q_offset_range_list.sinr_offset_csi_rs.value());
  }

  return asn1_q_offset_range_list;
}

inline asn1::rrc_nr::meas_obj_nr_s meas_obj_nr_to_rrc_asn1(const cu_cp_meas_obj_nr& meas_obj_nr)
{
  asn1::rrc_nr::meas_obj_nr_s asn1_meas_obj_nr;

  // ssb freq
  if (meas_obj_nr.ssb_freq.has_value()) {
    asn1_meas_obj_nr.ssb_freq_present = true;
    asn1_meas_obj_nr.ssb_freq         = meas_obj_nr.ssb_freq.value();
  }

  // ssb subcarrier spacing
  if (meas_obj_nr.ssb_subcarrier_spacing.has_value()) {
    asn1_meas_obj_nr.ssb_subcarrier_spacing_present = true;
    asn1_meas_obj_nr.ssb_subcarrier_spacing =
        subcarrier_spacing_to_rrc_asn1(meas_obj_nr.ssb_subcarrier_spacing.value());
  }

  // smtc1
  if (meas_obj_nr.smtc1.has_value()) {
    asn1_meas_obj_nr.smtc1_present = true;
    asn1_meas_obj_nr.smtc1         = ssb_mtc_to_rrc_asn1(meas_obj_nr.smtc1.value());
  }

  // smtc2
  if (meas_obj_nr.smtc2.has_value()) {
    asn1_meas_obj_nr.smtc2_present = true;
    // pci list
    for (const auto& pci : meas_obj_nr.smtc2.value().pci_list) {
      asn1_meas_obj_nr.smtc2.pci_list.push_back(pci);
    }
    // periodicity
    asn1::number_to_enum(asn1_meas_obj_nr.smtc2.periodicity, meas_obj_nr.smtc2.value().periodicity);
  }

  // ref freq csi rs
  if (meas_obj_nr.ref_freq_csi_rs.has_value()) {
    asn1_meas_obj_nr.ref_freq_csi_rs_present = true;
    asn1_meas_obj_nr.ref_freq_csi_rs         = meas_obj_nr.ref_freq_csi_rs.value();
  }

  // ref sig cfg
  // ssb cfg mob
  if (meas_obj_nr.ref_sig_cfg.ssb_cfg_mob.has_value()) {
    asn1_meas_obj_nr.ref_sig_cfg.ssb_cfg_mob_present = true;
    asn1_meas_obj_nr.ref_sig_cfg.ssb_cfg_mob = ssb_cfg_mob_to_rrc_asn1(meas_obj_nr.ref_sig_cfg.ssb_cfg_mob.value());
  }
  // csi rs res cfg mob
  if (meas_obj_nr.ref_sig_cfg.csi_rs_res_cfg_mob.has_value()) {
    asn1_meas_obj_nr.ref_sig_cfg.csi_rs_res_cfg_mob_present = true;
    asn1_meas_obj_nr.ref_sig_cfg.csi_rs_res_cfg_mob =
        csi_res_cfg_mob_to_rrc_asn1(meas_obj_nr.ref_sig_cfg.csi_rs_res_cfg_mob.value());
  }

  // abs thresh ss blocks consolidation
  if (meas_obj_nr.abs_thresh_ss_blocks_consolidation.has_value()) {
    asn1_meas_obj_nr.abs_thresh_ss_blocks_consolidation_present = true;
    asn1_meas_obj_nr.abs_thresh_ss_blocks_consolidation =
        thres_nr_to_rrc_asn1(meas_obj_nr.abs_thresh_ss_blocks_consolidation.value());
  }

  // abs thresh csi rs consolidation
  if (meas_obj_nr.abs_thresh_csi_rs_consolidation.has_value()) {
    asn1_meas_obj_nr.abs_thresh_csi_rs_consolidation_present = true;
    asn1_meas_obj_nr.abs_thresh_csi_rs_consolidation =
        thres_nr_to_rrc_asn1(meas_obj_nr.abs_thresh_csi_rs_consolidation.value());
  }

  // nrof ss blocks to average
  if (meas_obj_nr.nrof_ss_blocks_to_average.has_value()) {
    asn1_meas_obj_nr.nrof_ss_blocks_to_average_present = true;
    asn1_meas_obj_nr.nrof_ss_blocks_to_average         = meas_obj_nr.nrof_ss_blocks_to_average.value();
  }

  // nrof csi rs res to average
  if (meas_obj_nr.nrof_csi_rs_res_to_average.has_value()) {
    asn1_meas_obj_nr.nrof_csi_rs_res_to_average_present = true;
    asn1_meas_obj_nr.nrof_csi_rs_res_to_average         = meas_obj_nr.nrof_csi_rs_res_to_average.value();
  }

  // quant cfg idx
  asn1_meas_obj_nr.quant_cfg_idx = meas_obj_nr.quant_cfg_idx;

  // offset mo
  asn1_meas_obj_nr.offset_mo = q_offset_range_list_to_rrc_asn1(meas_obj_nr.offset_mo);

  // cells to rem list
  srsran_assert(meas_obj_nr.cells_to_rem_list.size() <= 32,
                "Too many cells to remove ({}>{}).",
                meas_obj_nr.cells_to_rem_list.size(),
                32);
  for (const auto& cell_to_rem : meas_obj_nr.cells_to_rem_list) {
    asn1_meas_obj_nr.cells_to_rem_list.push_back(cell_to_rem);
  }

  // cells to add mod list
  for (const auto& cell_to_add_mod : meas_obj_nr.cells_to_add_mod_list) {
    asn1::rrc_nr::cells_to_add_mod_s asn1_cells_to_add_mod;

    asn1_cells_to_add_mod.pci = cell_to_add_mod.pci;
    asn1_cells_to_add_mod.cell_individual_offset =
        q_offset_range_list_to_rrc_asn1(cell_to_add_mod.cell_individual_offset);

    asn1_meas_obj_nr.cells_to_add_mod_list.push_back(asn1_cells_to_add_mod);
  }

  // excluded cells to rem list
  srsran_assert(meas_obj_nr.excluded_cells_to_rem_list.size() <= 8,
                "Too many excluded cells to remove ({}>{}).",
                meas_obj_nr.excluded_cells_to_rem_list.size(),
                8);
  for (const auto& excluded_cell : meas_obj_nr.excluded_cells_to_rem_list) {
    asn1_meas_obj_nr.excluded_cells_to_rem_list.push_back(excluded_cell);
  }

  // excluded cells to add mod list
  for (const auto& excluded_cell : meas_obj_nr.excluded_cells_to_add_mod_list) {
    asn1::rrc_nr::pci_range_elem_s asn1_pci_range_elem;
    asn1_pci_range_elem.pci_range_idx = excluded_cell.pci_range_idx;

    asn1_pci_range_elem.pci_range.start = excluded_cell.pci_range.start;
    if (excluded_cell.pci_range.range.has_value()) {
      asn1_pci_range_elem.pci_range.range_present = true;
      asn1::number_to_enum(asn1_pci_range_elem.pci_range.range, excluded_cell.pci_range.range.value());
    }
    asn1_meas_obj_nr.excluded_cells_to_add_mod_list.push_back(asn1_pci_range_elem);
  }

  // allowed cells to rem list
  srsran_assert(meas_obj_nr.allowed_cells_to_rem_list.size() <= 8,
                "Too many allowed cells to remove ({}>{}).",
                meas_obj_nr.allowed_cells_to_rem_list.size(),
                8);
  for (const auto& allowed_cell : meas_obj_nr.allowed_cells_to_rem_list) {
    asn1_meas_obj_nr.allowed_cells_to_rem_list.push_back(allowed_cell);
  }

  // allowed cells to add mod list
  for (const auto& allowed_cell : meas_obj_nr.allowed_cells_to_add_mod_list) {
    asn1::rrc_nr::pci_range_elem_s asn1_pci_range_elem;
    asn1_pci_range_elem.pci_range_idx = allowed_cell.pci_range_idx;

    asn1_pci_range_elem.pci_range.start = allowed_cell.pci_range.start;
    if (allowed_cell.pci_range.range.has_value()) {
      asn1_pci_range_elem.pci_range.range_present = true;
      asn1::number_to_enum(asn1_pci_range_elem.pci_range.range, allowed_cell.pci_range.range.value());
    }
    asn1_meas_obj_nr.allowed_cells_to_add_mod_list.push_back(asn1_pci_range_elem);
  }

  // group 0
  // freq band ind nr
  if (meas_obj_nr.freq_band_ind_nr.has_value()) {
    asn1_meas_obj_nr.ext                      = true;
    asn1_meas_obj_nr.freq_band_ind_nr_present = true;
    asn1_meas_obj_nr.freq_band_ind_nr         = meas_obj_nr.freq_band_ind_nr.value();
  }
  // meas cycle scell
  if (meas_obj_nr.meas_cycle_scell.has_value()) {
    asn1_meas_obj_nr.ext                      = true;
    asn1_meas_obj_nr.meas_cycle_scell_present = true;
    asn1::number_to_enum(asn1_meas_obj_nr.meas_cycle_scell, meas_obj_nr.meas_cycle_scell.value());
  }

  return asn1_meas_obj_nr;
}

inline asn1::rrc_nr::meas_obj_eutra_s meas_obj_eutra_to_rrc_asn1(const cu_cp_meas_obj_eutra& meas_obj_eutra)
{
  asn1::rrc_nr::meas_obj_eutra_s asn1_meas_obj_eutra;

  // carrier freq
  asn1_meas_obj_eutra.carrier_freq = meas_obj_eutra.carrier_freq;

  // allowed meas bw
  asn1::number_to_enum(asn1_meas_obj_eutra.allowed_meas_bw, meas_obj_eutra.allowed_meas_bw);

  // cells to rem list eutran
  srsran_assert(meas_obj_eutra.cells_to_rem_list_eutran.size() <= 32,
                "Too many eutran cells to remove ({}>{}).",
                meas_obj_eutra.cells_to_rem_list_eutran.size(),
                32);
  for (const auto& cell : meas_obj_eutra.cells_to_rem_list_eutran) {
    asn1_meas_obj_eutra.cells_to_rem_list_eutran.push_back(cell);
  }

  // cells to add mod list eutran
  for (const auto& cell : meas_obj_eutra.cells_to_add_mod_list_eutran) {
    asn1::rrc_nr::eutra_cell_s asn1_cell;

    asn1_cell.cell_idx_eutra = cell.cell_idx_eutra;
    asn1_cell.pci            = cell.pci;
    asn1::number_to_enum(asn1_cell.cell_individual_offset, cell.cell_individual_offset);

    asn1_meas_obj_eutra.cells_to_add_mod_list_eutran.push_back(asn1_cell);
  }

  // excluded cells to rem list eutran
  srsran_assert(meas_obj_eutra.excluded_cells_to_rem_list_eutran.size() <= 32,
                "Too many excluded eutran cells to remove ({}>{}).",
                meas_obj_eutra.excluded_cells_to_rem_list_eutran.size(),
                32);
  for (const auto& excluded_cell : meas_obj_eutra.excluded_cells_to_rem_list_eutran) {
    asn1_meas_obj_eutra.excluded_cells_to_rem_list_eutran.push_back(excluded_cell);
  }

  // excluded cells to add mod list eutran
  for (const auto& excluded_cell : meas_obj_eutra.excluded_cells_to_add_mod_list_eutran) {
    asn1::rrc_nr::eutra_excluded_cell_s asn1_excluded_cell;

    asn1_excluded_cell.cell_idx_eutra  = excluded_cell.cell_idx_eutra;
    asn1_excluded_cell.pci_range.start = excluded_cell.pci_range.start;
    if (excluded_cell.pci_range.range.has_value()) {
      asn1_excluded_cell.pci_range.range_present = true;
      asn1::number_to_enum(asn1_excluded_cell.pci_range.range, excluded_cell.pci_range.range.value());
    }

    asn1_meas_obj_eutra.excluded_cells_to_add_mod_list_eutran.push_back(asn1_excluded_cell);
  }

  // eutra presence ant port1
  asn1_meas_obj_eutra.eutra_presence_ant_port1 = meas_obj_eutra.eutra_presence_ant_port1;

  // eutra q offset range
  if (meas_obj_eutra.eutra_q_offset_range.has_value()) {
    asn1_meas_obj_eutra.eutra_q_offset_range_present = true;
    asn1::number_to_enum(asn1_meas_obj_eutra.eutra_q_offset_range, meas_obj_eutra.eutra_q_offset_range.value());
  }

  // wideband rsrq meas
  asn1_meas_obj_eutra.wideband_rsrq_meas = meas_obj_eutra.wideband_rsrq_meas;

  return asn1_meas_obj_eutra;
}

template <class asn1_srs_periodicity_and_offset>
void srs_periodicity_and_offset_to_rrc_asn1(asn1_srs_periodicity_and_offset&        asn1_srs_period_and_offset,
                                            const cu_cp_srs_periodicity_and_offset& srs_period_and_offset)
{
  if (srs_period_and_offset.is_sl1) {
    asn1_srs_period_and_offset.set_sl1();
  } else if (srs_period_and_offset.sl2.has_value()) {
    asn1_srs_period_and_offset.set_sl2();
    asn1_srs_period_and_offset.sl2() = srs_period_and_offset.sl2.value();
  } else if (srs_period_and_offset.sl4.has_value()) {
    asn1_srs_period_and_offset.set_sl4();
    asn1_srs_period_and_offset.sl4() = srs_period_and_offset.sl4.value();
  } else if (srs_period_and_offset.sl5.has_value()) {
    asn1_srs_period_and_offset.set_sl5();
    asn1_srs_period_and_offset.sl5() = srs_period_and_offset.sl5.value();
  } else if (srs_period_and_offset.sl8.has_value()) {
    asn1_srs_period_and_offset.set_sl8();
    asn1_srs_period_and_offset.sl8() = srs_period_and_offset.sl8.value();
  } else if (srs_period_and_offset.sl10.has_value()) {
    asn1_srs_period_and_offset.set_sl10();
    asn1_srs_period_and_offset.sl10() = srs_period_and_offset.sl10.value();
  } else if (srs_period_and_offset.sl16.has_value()) {
    asn1_srs_period_and_offset.set_sl16();
    asn1_srs_period_and_offset.sl16() = srs_period_and_offset.sl16.value();
  } else if (srs_period_and_offset.sl20.has_value()) {
    asn1_srs_period_and_offset.set_sl20();
    asn1_srs_period_and_offset.sl20() = srs_period_and_offset.sl20.value();
  } else if (srs_period_and_offset.sl32.has_value()) {
    asn1_srs_period_and_offset.set_sl32();
    asn1_srs_period_and_offset.sl32() = srs_period_and_offset.sl32.value();
  } else if (srs_period_and_offset.sl40.has_value()) {
    asn1_srs_period_and_offset.set_sl40();
    asn1_srs_period_and_offset.sl40() = srs_period_and_offset.sl40.value();
  } else if (srs_period_and_offset.sl64.has_value()) {
    asn1_srs_period_and_offset.set_sl64();
    asn1_srs_period_and_offset.sl64() = srs_period_and_offset.sl64.value();
  } else if (srs_period_and_offset.sl80.has_value()) {
    asn1_srs_period_and_offset.set_sl80();
    asn1_srs_period_and_offset.sl80() = srs_period_and_offset.sl80.value();
  } else if (srs_period_and_offset.sl160.has_value()) {
    asn1_srs_period_and_offset.set_sl160();
    asn1_srs_period_and_offset.sl160() = srs_period_and_offset.sl160.value();
  } else if (srs_period_and_offset.sl320.has_value()) {
    asn1_srs_period_and_offset.set_sl320();
    asn1_srs_period_and_offset.sl320() = srs_period_and_offset.sl320.value();
  } else if (srs_period_and_offset.sl640.has_value()) {
    asn1_srs_period_and_offset.set_sl640();
    asn1_srs_period_and_offset.sl640() = srs_period_and_offset.sl640.value();
  } else if (srs_period_and_offset.sl1280.has_value()) {
    asn1_srs_period_and_offset.set_sl1280();
    asn1_srs_period_and_offset.sl1280() = srs_period_and_offset.sl1280.value();
  } else if (srs_period_and_offset.sl2560.has_value()) {
    asn1_srs_period_and_offset.set_sl2560();
    asn1_srs_period_and_offset.sl2560() = srs_period_and_offset.sl2560.value();
  } else {
    // error
    srslog::fetch_basic_logger("RRC").error("Invalid srs periodicity and offset.");
  }
};

inline asn1::rrc_nr::srs_res_s srs_res_to_rrc_asn1(const cu_cp_srs_res& srs_res)
{
  asn1::rrc_nr::srs_res_s asn1_srs_res;

  // srs res id
  asn1_srs_res.srs_res_id = srs_res.srs_res_id;

  // nrof srs ports
  asn1::number_to_enum(asn1_srs_res.nrof_srs_ports, srs_res.nrof_srs_ports);

  // ptrs port idx
  if (srs_res.ptrs_port_idx.has_value()) {
    asn1_srs_res.ptrs_port_idx_present = true;
    asn1::number_to_enum(asn1_srs_res.ptrs_port_idx, srs_res.ptrs_port_idx.value());
  }

  // tx comb
  if (srs_res.tx_comb.n2.has_value()) {
    asn1_srs_res.tx_comb.set_n2();
    asn1_srs_res.tx_comb.n2().comb_offset_n2  = srs_res.tx_comb.n2.value().comb_offset_n2;
    asn1_srs_res.tx_comb.n2().cyclic_shift_n2 = srs_res.tx_comb.n2.value().cyclic_shift_n2;
  } else if (srs_res.tx_comb.n4.has_value()) {
    asn1_srs_res.tx_comb.set_n4();
    asn1_srs_res.tx_comb.n4().comb_offset_n4  = srs_res.tx_comb.n4.value().comb_offset_n4;
    asn1_srs_res.tx_comb.n4().cyclic_shift_n4 = srs_res.tx_comb.n4.value().cyclic_shift_n4;
  } else {
    // error
    srslog::fetch_basic_logger("RRC").error("Invalid tx comb.");
  }

  // res map
  asn1_srs_res.res_map.start_position = srs_res.res_map.start_position;
  asn1::number_to_enum(asn1_srs_res.res_map.nrof_symbols, srs_res.res_map.nrof_symbols);
  asn1::number_to_enum(asn1_srs_res.res_map.repeat_factor, srs_res.res_map.repeat_factor);

  // freq domain position
  asn1_srs_res.freq_domain_position = srs_res.freq_domain_position;

  // freq domain shift
  asn1_srs_res.freq_domain_shift = srs_res.freq_domain_shift;

  // freq hop
  asn1_srs_res.freq_hop.c_srs = srs_res.freq_hop.c_srs;
  asn1_srs_res.freq_hop.b_srs = srs_res.freq_hop.b_srs;
  asn1_srs_res.freq_hop.b_hop = srs_res.freq_hop.b_hop;

  // group or seq hop
  asn1::string_to_enum(asn1_srs_res.group_or_seq_hop, srs_res.group_or_seq_hop);

  // res type
  if (srs_res.res_type.is_aperiodic) {
    asn1_srs_res.res_type.set_aperiodic();
  } else if (srs_res.res_type.semi_persistent.has_value()) {
    asn1_srs_res.res_type.set_semi_persistent();
    srs_periodicity_and_offset_to_rrc_asn1(asn1_srs_res.res_type.semi_persistent().periodicity_and_offset_sp,
                                           srs_res.res_type.semi_persistent.value().periodicity_and_offset_sp_p);
  } else if (srs_res.res_type.periodic.has_value()) {
    asn1_srs_res.res_type.set_periodic();
    srs_periodicity_and_offset_to_rrc_asn1(asn1_srs_res.res_type.periodic().periodicity_and_offset_p,
                                           srs_res.res_type.periodic.value().periodicity_and_offset_sp_p);
  } else {
    // error
    srslog::fetch_basic_logger("RRC").error("Invalid res type.");
  }

  // seq id
  asn1_srs_res.seq_id = srs_res.seq_id;

  // spatial relation info
  if (srs_res.spatial_relation_info.has_value()) {
    // serving cell id
    if (srs_res.spatial_relation_info.value().serving_cell_id.has_value()) {
      asn1_srs_res.spatial_relation_info.serving_cell_id_present = true;
      asn1_srs_res.spatial_relation_info.serving_cell_id =
          srs_res.spatial_relation_info.value().serving_cell_id.value();
    }
    // ref sig
    if (srs_res.spatial_relation_info.value().ref_sig.ssb_idx.has_value()) {
      asn1_srs_res.spatial_relation_info.ref_sig.set_ssb_idx() =
          srs_res.spatial_relation_info.value().ref_sig.ssb_idx.value();
    } else if (srs_res.spatial_relation_info.value().ref_sig.csi_rs_idx.has_value()) {
      asn1_srs_res.spatial_relation_info.ref_sig.set_csi_rs_idx() =
          srs_res.spatial_relation_info.value().ref_sig.csi_rs_idx.value();
    } else if (srs_res.spatial_relation_info.value().ref_sig.srs.has_value()) {
      asn1_srs_res.spatial_relation_info.ref_sig.set_srs();
      asn1_srs_res.spatial_relation_info.ref_sig.srs().res_id =
          srs_res.spatial_relation_info.value().ref_sig.srs.value().res_id;
      asn1_srs_res.spatial_relation_info.ref_sig.srs().ul_bwp =
          srs_res.spatial_relation_info.value().ref_sig.srs.value().ul_bwp;
    } else {
      // error
      srslog::fetch_basic_logger("RRC").error("Invalid ref sig.");
    }
  }

  return asn1_srs_res;
}

inline asn1::rrc_nr::meas_obj_to_add_mod_s
meas_obj_to_add_mod_to_rrc_asn1(const cu_cp_meas_obj_to_add_mod& meas_obj_to_add_mod)
{
  asn1::rrc_nr::meas_obj_to_add_mod_s asn1_meas_obj_to_add_mod;

  // meas obj id
  asn1_meas_obj_to_add_mod.meas_obj_id = meas_obj_to_add_mod.meas_obj_id;

  // meas obj
  if (meas_obj_to_add_mod.meas_obj_nr.has_value()) {
    // meas obj nr
    asn1_meas_obj_to_add_mod.meas_obj.set_meas_obj_nr();
    asn1_meas_obj_to_add_mod.meas_obj.meas_obj_nr() = meas_obj_nr_to_rrc_asn1(meas_obj_to_add_mod.meas_obj_nr.value());
  } else if (meas_obj_to_add_mod.meas_obj_eutra.has_value()) {
    // meas obj eutra
    asn1_meas_obj_to_add_mod.meas_obj.set_meas_obj_eutra();
    asn1_meas_obj_to_add_mod.meas_obj.meas_obj_eutra() =
        meas_obj_eutra_to_rrc_asn1(meas_obj_to_add_mod.meas_obj_eutra.value());
  } else {
    // error
    srslog::fetch_basic_logger("RRC").error("Invalid meas obj.");
  }

  return asn1_meas_obj_to_add_mod;
}

inline asn1::rrc_nr::meas_report_quant_s meas_report_quant_to_rrc_asn1(const cu_cp_meas_report_quant& meas_report_quant)
{
  asn1::rrc_nr::meas_report_quant_s asn1_meas_report_quant;

  asn1_meas_report_quant.rsrp = meas_report_quant.rsrp;
  asn1_meas_report_quant.rsrq = meas_report_quant.rsrq;
  asn1_meas_report_quant.sinr = meas_report_quant.sinr;

  return asn1_meas_report_quant;
}

inline asn1::rrc_nr::periodical_report_cfg_s
periodical_report_cfg_to_rrc_asn1(const cu_cp_periodical_report_cfg& periodical_report_cfg)
{
  asn1::rrc_nr::periodical_report_cfg_s asn1_periodical_report_cfg;

  // rs type
  asn1::string_to_enum(asn1_periodical_report_cfg.rs_type, periodical_report_cfg.rs_type);
  // report interv
  asn1::number_to_enum(asn1_periodical_report_cfg.report_interv, periodical_report_cfg.report_interv);
  // report amount
  asn1::number_to_enum(asn1_periodical_report_cfg.report_amount, periodical_report_cfg.report_amount);
  // report quant cell
  asn1_periodical_report_cfg.report_quant_cell = meas_report_quant_to_rrc_asn1(periodical_report_cfg.report_quant_cell);
  // max report cells
  asn1_periodical_report_cfg.max_report_cells = periodical_report_cfg.max_report_cells;
  // report quant rx idxes
  if (periodical_report_cfg.report_quant_rs_idxes.has_value()) {
    asn1_periodical_report_cfg.report_quant_rs_idxes_present = true;
    asn1_periodical_report_cfg.report_quant_rs_idxes =
        meas_report_quant_to_rrc_asn1(periodical_report_cfg.report_quant_rs_idxes.value());
  }
  // max nrof rs idxes to report
  if (periodical_report_cfg.max_nrof_rs_idxes_to_report.has_value()) {
    asn1_periodical_report_cfg.max_nrof_rs_idxes_to_report_present = true;
    asn1_periodical_report_cfg.max_nrof_rs_idxes_to_report = periodical_report_cfg.max_nrof_rs_idxes_to_report.value();
  }
  // include beam meass
  asn1_periodical_report_cfg.include_beam_meass = periodical_report_cfg.include_beam_meass;
  // use allowed cell list
  asn1_periodical_report_cfg.use_allowed_cell_list = periodical_report_cfg.use_allowed_cell_list;

  return asn1_periodical_report_cfg;
}

template <class asn1_meas_trigger_quant_quant_offset>
void meas_trigger_quant_to_rrc_asn1(asn1_meas_trigger_quant_quant_offset& asn1_meas_trigger_quant_offset,
                                    const cu_cp_meas_trigger_quant&       meas_trigger_quant)
{
  if (meas_trigger_quant.rsrp.has_value()) {
    asn1_meas_trigger_quant_offset.set_rsrp() = meas_trigger_quant.rsrp.value();
  } else if (meas_trigger_quant.rsrq.has_value()) {
    asn1_meas_trigger_quant_offset.set_rsrq() = meas_trigger_quant.rsrq.value();
  } else if (meas_trigger_quant.sinr.has_value()) {
    asn1_meas_trigger_quant_offset.set_sinr() = meas_trigger_quant.sinr.value();
  } else {
    // error
    srslog::fetch_basic_logger("RRC").error("Invalid meas trigger quant.");
  }
}

inline asn1::rrc_nr::event_trigger_cfg_s
event_triggered_report_cfg_to_rrc_asn1(const cu_cp_event_trigger_cfg& event_triggered_cfg)
{
  asn1::rrc_nr::event_trigger_cfg_s asn1_event_triggered_cfg;

  // report add neigh meas present
  asn1_event_triggered_cfg.report_add_neigh_meas_present = event_triggered_cfg.report_add_neigh_meas_present;

  // event id
  if (event_triggered_cfg.event_id.event_a1.has_value()) {
    // event a1
    auto& asn1_event_a1 = asn1_event_triggered_cfg.event_id.set_event_a1();
    // a1 thres
    meas_trigger_quant_to_rrc_asn1(asn1_event_a1.a1_thres, event_triggered_cfg.event_id.event_a1.value().a1_thres);
    // report on leave
    asn1_event_a1.report_on_leave = event_triggered_cfg.event_id.event_a1.value().report_on_leave;
    // hysteresis
    asn1_event_a1.hysteresis = event_triggered_cfg.event_id.event_a1.value().hysteresis;
    // time to trigger
    asn1::number_to_enum(asn1_event_a1.time_to_trigger, event_triggered_cfg.event_id.event_a1.value().time_to_trigger);
  } else if (event_triggered_cfg.event_id.event_a2.has_value()) {
    // event a2
    auto& asn1_event_a2 = asn1_event_triggered_cfg.event_id.set_event_a2();
    // a2 thres
    meas_trigger_quant_to_rrc_asn1(asn1_event_a2.a2_thres, event_triggered_cfg.event_id.event_a2.value().a2_thres);
    // report on leave
    asn1_event_a2.report_on_leave = event_triggered_cfg.event_id.event_a2.value().report_on_leave;
    // hysteresis
    asn1_event_a2.hysteresis = event_triggered_cfg.event_id.event_a2.value().hysteresis;
    // time to trigger
    asn1::number_to_enum(asn1_event_a2.time_to_trigger, event_triggered_cfg.event_id.event_a2.value().time_to_trigger);
  } else if (event_triggered_cfg.event_id.event_a3.has_value()) {
    // event a3
    auto& asn1_event_a3 = asn1_event_triggered_cfg.event_id.set_event_a3();
    // a3 offset
    meas_trigger_quant_to_rrc_asn1(asn1_event_a3.a3_offset, event_triggered_cfg.event_id.event_a3.value().a3_offset);
    // report on leave
    asn1_event_a3.report_on_leave = event_triggered_cfg.event_id.event_a3.value().report_on_leave;
    // hysteresis
    asn1_event_a3.hysteresis = event_triggered_cfg.event_id.event_a3.value().hysteresis;
    // time to trigger
    asn1::number_to_enum(asn1_event_a3.time_to_trigger, event_triggered_cfg.event_id.event_a3.value().time_to_trigger);
    // use allowed cell list
    asn1_event_a3.use_allowed_cell_list = event_triggered_cfg.event_id.event_a3.value().use_allowed_cell_list;
  } else if (event_triggered_cfg.event_id.event_a4.has_value()) {
    // event a4
    auto& asn1_event_a4 = asn1_event_triggered_cfg.event_id.set_event_a4();
    // a4 thres
    meas_trigger_quant_to_rrc_asn1(asn1_event_a4.a4_thres, event_triggered_cfg.event_id.event_a4.value().a4_thres);
    // report on leave
    asn1_event_a4.report_on_leave = event_triggered_cfg.event_id.event_a4.value().report_on_leave;
    // hysteresis
    asn1_event_a4.hysteresis = event_triggered_cfg.event_id.event_a4.value().hysteresis;
    // time to trigger
    asn1::number_to_enum(asn1_event_a4.time_to_trigger, event_triggered_cfg.event_id.event_a4.value().time_to_trigger);
    // use allowed cell list
    asn1_event_a4.use_allowed_cell_list = event_triggered_cfg.event_id.event_a4.value().use_allowed_cell_list;
  } else if (event_triggered_cfg.event_id.event_a5.has_value()) {
    // event a5
    auto& asn1_event_a5 = asn1_event_triggered_cfg.event_id.set_event_a5();
    // a5 thres 1
    meas_trigger_quant_to_rrc_asn1(asn1_event_a5.a5_thres1, event_triggered_cfg.event_id.event_a5.value().a5_thres_1);
    // a5 thres 2
    meas_trigger_quant_to_rrc_asn1(asn1_event_a5.a5_thres2, event_triggered_cfg.event_id.event_a5.value().a5_thres_2);
    // report on leave
    asn1_event_a5.report_on_leave = event_triggered_cfg.event_id.event_a5.value().report_on_leave;
    // hysteresis
    asn1_event_a5.hysteresis = event_triggered_cfg.event_id.event_a5.value().hysteresis;
    // time to trigger
    asn1::number_to_enum(asn1_event_a5.time_to_trigger, event_triggered_cfg.event_id.event_a5.value().time_to_trigger);
    // use allowed cell list
    asn1_event_a5.use_allowed_cell_list = event_triggered_cfg.event_id.event_a5.value().use_allowed_cell_list;
  } else if (event_triggered_cfg.event_id.event_a6.has_value()) {
    // event a6
    auto& asn1_event_a6 = asn1_event_triggered_cfg.event_id.set_event_a6();
    // a6 offset
    meas_trigger_quant_to_rrc_asn1(asn1_event_a6.a6_offset, event_triggered_cfg.event_id.event_a6.value().a6_offset);
    // report on leave
    asn1_event_a6.report_on_leave = event_triggered_cfg.event_id.event_a6.value().report_on_leave;
    // hysteresis
    asn1_event_a6.hysteresis = event_triggered_cfg.event_id.event_a6.value().hysteresis;
    // time to trigger
    asn1::number_to_enum(asn1_event_a6.time_to_trigger, event_triggered_cfg.event_id.event_a6.value().time_to_trigger);
    // use allowed cell list
    asn1_event_a6.use_allowed_cell_list = event_triggered_cfg.event_id.event_a6.value().use_allowed_cell_list;
  } else {
    // error
    srslog::fetch_basic_logger("RRC").error("Invalid event id.");
  }

  // rs type
  asn1::string_to_enum(asn1_event_triggered_cfg.rs_type, event_triggered_cfg.rs_type);

  // report interv
  asn1::number_to_enum(asn1_event_triggered_cfg.report_interv, event_triggered_cfg.report_interv);

  // report amount
  asn1::number_to_enum(asn1_event_triggered_cfg.report_amount, event_triggered_cfg.report_amount);

  // report quant cell
  asn1_event_triggered_cfg.report_quant_cell = meas_report_quant_to_rrc_asn1(event_triggered_cfg.report_quant_cell);

  // max report cells
  asn1_event_triggered_cfg.max_report_cells = event_triggered_cfg.max_report_cells;

  // report quant rx idxes
  if (event_triggered_cfg.report_quant_rs_idxes.has_value()) {
    asn1_event_triggered_cfg.report_quant_rs_idxes_present = true;
    asn1_event_triggered_cfg.report_quant_rs_idxes =
        meas_report_quant_to_rrc_asn1(event_triggered_cfg.report_quant_rs_idxes.value());
  }

  // max nrof rs idxes to report
  if (event_triggered_cfg.max_nrof_rs_idxes_to_report.has_value()) {
    asn1_event_triggered_cfg.max_nrof_rs_idxes_to_report_present = true;
    asn1_event_triggered_cfg.max_nrof_rs_idxes_to_report = event_triggered_cfg.max_nrof_rs_idxes_to_report.value();
  }

  // include beam meass
  asn1_event_triggered_cfg.include_beam_meass = event_triggered_cfg.include_beam_meass;

  return asn1_event_triggered_cfg;
}

inline asn1::rrc_nr::report_cfg_nr_s report_cfg_nr_to_rrc_asn1(const cu_cp_report_cfg_nr& report_cfg_nr)
{
  asn1::rrc_nr::report_cfg_nr_s asn1_report_cfg_nr;

  if (report_cfg_nr.periodical.has_value()) {
    // periodical
    asn1_report_cfg_nr.report_type.set_periodical() =
        periodical_report_cfg_to_rrc_asn1(report_cfg_nr.periodical.value());
  } else if (report_cfg_nr.event_triggered.has_value()) {
    // event triggered
    asn1_report_cfg_nr.report_type.set_event_triggered() =
        event_triggered_report_cfg_to_rrc_asn1(report_cfg_nr.event_triggered.value());
  } else if (report_cfg_nr.report_cgi.has_value()) {
    // report cgi
    asn1_report_cfg_nr.report_type.set_report_cgi();
    asn1_report_cfg_nr.report_type.report_cgi().cell_for_which_to_report_cgi =
        report_cfg_nr.report_cgi.value().cell_for_which_to_report_cgi;
  } else if (report_cfg_nr.report_sftd.has_value()) {
    // report sftd
    asn1_report_cfg_nr.report_type.set_report_sftd();
    asn1_report_cfg_nr.report_type.report_sftd().report_sftd_meas = report_cfg_nr.report_sftd.value().report_sftd_meas;
    asn1_report_cfg_nr.report_type.report_sftd().report_rsrp      = report_cfg_nr.report_sftd.value().report_rsrp;
  } else {
    // error
    srslog::fetch_basic_logger("RRC").error("Invalid report cfg nr.");
  }

  return asn1_report_cfg_nr;
}

inline asn1::rrc_nr::event_trigger_cfg_inter_rat_s
event_triggered_cfg_inter_rat_to_rrc_asn1(const cu_cp_event_trigger_cfg_inter_rat& event_trigger_cfg_inter_rat)
{
  asn1::rrc_nr::event_trigger_cfg_inter_rat_s asn1_event_trigger_cfg_inter_rat;

  // event id
  if (event_trigger_cfg_inter_rat.event_id.event_b1.has_value()) {
    auto& asn1_event_b1 = asn1_event_trigger_cfg_inter_rat.event_id.set_event_b1();
    // b1 thres eutra
    meas_trigger_quant_to_rrc_asn1(asn1_event_b1.b1_thres_eutra,
                                   event_trigger_cfg_inter_rat.event_id.event_b1.value().b1_thres_eutra);
    // report on leave
    asn1_event_b1.report_on_leave = event_trigger_cfg_inter_rat.event_id.event_b1.value().report_on_leave;
    // hyteresis
    asn1_event_b1.hysteresis = event_trigger_cfg_inter_rat.event_id.event_b1.value().hysteresis;
    // time to trigger
    asn1::number_to_enum(asn1_event_b1.time_to_trigger,
                         event_trigger_cfg_inter_rat.event_id.event_b1.value().time_to_trigger);
  } else if (event_trigger_cfg_inter_rat.event_id.event_b2.has_value()) {
    auto& asn1_event_b2 = asn1_event_trigger_cfg_inter_rat.event_id.set_event_b2();
    // b2 thres 1
    meas_trigger_quant_to_rrc_asn1(asn1_event_b2.b2_thres1,
                                   event_trigger_cfg_inter_rat.event_id.event_b2.value().b2_thres1);
    // b2 thres 2 eutra
    meas_trigger_quant_to_rrc_asn1(asn1_event_b2.b2_thres2_eutra,
                                   event_trigger_cfg_inter_rat.event_id.event_b2.value().b2_thres2_eutra);
    // report on leave
    asn1_event_b2.report_on_leave = event_trigger_cfg_inter_rat.event_id.event_b2.value().report_on_leave;
    // hyteresis
    asn1_event_b2.hysteresis = event_trigger_cfg_inter_rat.event_id.event_b2.value().hysteresis;
    // time to trigger
    asn1::number_to_enum(asn1_event_b2.time_to_trigger,
                         event_trigger_cfg_inter_rat.event_id.event_b2.value().time_to_trigger);
  } else {
    // error
    srslog::fetch_basic_logger("RRC").error("Invalid event id.");
  }

  // rs type
  asn1::string_to_enum(asn1_event_trigger_cfg_inter_rat.rs_type, event_trigger_cfg_inter_rat.rs_type);

  // report interv
  asn1::number_to_enum(asn1_event_trigger_cfg_inter_rat.report_interv, event_trigger_cfg_inter_rat.report_interv);

  // report amount
  asn1::number_to_enum(asn1_event_trigger_cfg_inter_rat.report_amount, event_trigger_cfg_inter_rat.report_amount);

  // report quant
  asn1_event_trigger_cfg_inter_rat.report_quant =
      meas_report_quant_to_rrc_asn1(event_trigger_cfg_inter_rat.report_quant);

  // max report cells
  asn1_event_trigger_cfg_inter_rat.max_report_cells = event_trigger_cfg_inter_rat.max_report_cells;

  return asn1_event_trigger_cfg_inter_rat;
}

inline asn1::rrc_nr::report_cfg_inter_rat_s
report_cfg_inter_rat_to_rrc_asn1(const cu_cp_report_cfg_inter_rat& report_cfg_inter_rat)
{
  asn1::rrc_nr::report_cfg_inter_rat_s asn1_report_cfg_inter_rat;

  if (report_cfg_inter_rat.periodical.has_value()) {
    // periodical
    auto& asn1_periodical = asn1_report_cfg_inter_rat.report_type.set_periodical();

    // report interv
    asn1::number_to_enum(asn1_periodical.report_interv, report_cfg_inter_rat.periodical.value().report_interv);
    // report amount
    asn1::number_to_enum(asn1_periodical.report_amount, report_cfg_inter_rat.periodical.value().report_amount);
    // report quant
    asn1_periodical.report_quant = meas_report_quant_to_rrc_asn1(report_cfg_inter_rat.periodical.value().report_quant);
    // max report cells
    asn1_periodical.max_report_cells = report_cfg_inter_rat.periodical.value().max_report_cells;

  } else if (report_cfg_inter_rat.event_triggered.has_value()) {
    // event triggered
    asn1_report_cfg_inter_rat.report_type.set_event_triggered() =
        event_triggered_cfg_inter_rat_to_rrc_asn1(report_cfg_inter_rat.event_triggered.value());
  } else if (report_cfg_inter_rat.report_cgi.has_value()) {
    // report cgi
    asn1_report_cfg_inter_rat.report_type.set_report_cgi();
    asn1_report_cfg_inter_rat.report_type.report_cgi().cell_for_which_to_report_cgi =
        report_cfg_inter_rat.report_cgi.value().cell_for_which_to_report_cgi;
  } else if (report_cfg_inter_rat.report_sftd.has_value()) {
    // report sftd
    asn1_report_cfg_inter_rat.report_type.set_report_sftd();
    asn1_report_cfg_inter_rat.report_type.report_sftd().report_sftd_meas =
        report_cfg_inter_rat.report_sftd.value().report_sftd_meas;
    asn1_report_cfg_inter_rat.report_type.report_sftd().report_rsrp =
        report_cfg_inter_rat.report_sftd.value().report_rsrp;
  } else {
    // error
    srslog::fetch_basic_logger("RRC").error("Invalid report cfg nr.");
  }

  return asn1_report_cfg_inter_rat;
}

inline asn1::rrc_nr::report_cfg_to_add_mod_s
report_cfg_to_add_mod_to_rrc_asn1(const cu_cp_report_cfg_to_add_mod& report_cfg_to_add_mod)
{
  asn1::rrc_nr::report_cfg_to_add_mod_s asn1_report_cfg_to_add_mod;

  // report cfg id
  asn1_report_cfg_to_add_mod.report_cfg_id = report_cfg_to_add_mod.report_cfg_id;

  // report cfg
  if (report_cfg_to_add_mod.report_cfg.report_cfg_nr.has_value()) {
    asn1_report_cfg_to_add_mod.report_cfg.set_report_cfg_nr() =
        report_cfg_nr_to_rrc_asn1(report_cfg_to_add_mod.report_cfg.report_cfg_nr.value());
  } else if (report_cfg_to_add_mod.report_cfg.report_cfg_inter_rat.has_value()) {
    asn1_report_cfg_to_add_mod.report_cfg.set_report_cfg_inter_rat() =
        report_cfg_inter_rat_to_rrc_asn1(report_cfg_to_add_mod.report_cfg.report_cfg_inter_rat.value());
  } else {
    // error
    srslog::fetch_basic_logger("RRC").error("Invalid report cfg.");
  }

  return asn1_report_cfg_to_add_mod;
}

inline asn1::rrc_nr::meas_id_to_add_mod_s
meas_id_to_add_mod_to_rrc_asn1(const cu_cp_meas_id_to_add_mod& meas_id_to_add_mod)
{
  asn1::rrc_nr::meas_id_to_add_mod_s asn1_meas_id_to_add_mod;

  // meas id
  asn1_meas_id_to_add_mod.meas_id = meas_id_to_add_mod.meas_id;
  // meas obj id
  asn1_meas_id_to_add_mod.meas_obj_id = meas_id_to_add_mod.meas_obj_id;
  // report cfg ind
  asn1_meas_id_to_add_mod.report_cfg_id = meas_id_to_add_mod.report_cfg_id;

  return asn1_meas_id_to_add_mod;
}

inline asn1::rrc_nr::filt_cfg_s filt_cfg_to_rrc_asn1(const cu_cp_filt_cfg& filt_cfg)
{
  asn1::rrc_nr::filt_cfg_s asn1_filt_cfg;

  // filt coef rsrp
  if (filt_cfg.filt_coef_rsrp.has_value()) {
    asn1_filt_cfg.filt_coef_rsrp_present = true;
    asn1::number_to_enum(asn1_filt_cfg.filt_coef_rsrp, filt_cfg.filt_coef_rsrp.value());
  }

  // filt coef rsrq
  if (filt_cfg.filt_coef_rsrq.has_value()) {
    asn1_filt_cfg.filt_coef_rsrq_present = true;
    asn1::number_to_enum(asn1_filt_cfg.filt_coef_rsrq, filt_cfg.filt_coef_rsrq.value());
  }

  // filt coef rs sinr
  if (filt_cfg.filt_coef_rs_sinr.has_value()) {
    asn1_filt_cfg.filt_coef_rs_sinr_present = true;
    asn1::number_to_enum(asn1_filt_cfg.filt_coef_rs_sinr, filt_cfg.filt_coef_rs_sinr.value());
  }

  return asn1_filt_cfg;
}

inline asn1::rrc_nr::quant_cfg_rs_s quant_cfg_rs_to_rrc_asn1(const cu_cp_quant_cfg_rs& quant_cfg_rs)
{
  asn1::rrc_nr::quant_cfg_rs_s asn1_quant_cfg_rs;

  // ssb filt cfg
  asn1_quant_cfg_rs.ssb_filt_cfg = filt_cfg_to_rrc_asn1(quant_cfg_rs.ssb_filt_cfg);
  // csi rs filt cfg
  asn1_quant_cfg_rs.csi_rs_filt_cfg = filt_cfg_to_rrc_asn1(quant_cfg_rs.csi_rs_filt_cfg);

  return asn1_quant_cfg_rs;
}

/// \brief Converts type \c cu_cp_meas_cfg to an RRC NR ASN.1 type.
/// \param meas_cfg Meas config object.
/// \return The RRC NR ASN.1 object where the result of the conversion is stored.
inline asn1::rrc_nr::meas_cfg_s meas_config_to_rrc_asn1(const cu_cp_meas_cfg& meas_cfg)
{
  asn1::rrc_nr::meas_cfg_s asn1_meas_cfg;

  // meas obj to rem list
  for (const auto& meas_obj_to_rem : meas_cfg.meas_obj_to_rem_list) {
    asn1_meas_cfg.meas_obj_to_rem_list.push_back(meas_obj_to_rem);
  }

  // meas obj to add mod list
  for (const auto& meas_obj_to_add_mod : meas_cfg.meas_obj_to_add_mod_list) {
    asn1::rrc_nr::meas_obj_to_add_mod_s asn1_meas_obj_to_add_mod = meas_obj_to_add_mod_to_rrc_asn1(meas_obj_to_add_mod);
    asn1_meas_cfg.meas_obj_to_add_mod_list.push_back(asn1_meas_obj_to_add_mod);
  }

  // report cfg to rem list
  for (const auto& report_cfg_to_rem : meas_cfg.report_cfg_to_rem_list) {
    asn1_meas_cfg.report_cfg_to_rem_list.push_back(report_cfg_to_rem);
  }

  // report cfg to add mod list
  for (const auto& report_cfg_to_add_mod : meas_cfg.report_cfg_to_add_mod_list) {
    asn1::rrc_nr::report_cfg_to_add_mod_s asn1_report_cfg_to_add_mod =
        report_cfg_to_add_mod_to_rrc_asn1(report_cfg_to_add_mod);
    asn1_meas_cfg.report_cfg_to_add_mod_list.push_back(asn1_report_cfg_to_add_mod);
  }

  // meas id to rem list
  for (const auto& meas_id_to_rem : meas_cfg.meas_id_to_rem_list) {
    asn1_meas_cfg.meas_id_to_rem_list.push_back(meas_id_to_rem);
  }

  // meas id to add mod list
  for (const auto& meas_id_to_add_mod : meas_cfg.meas_id_to_add_mod_list) {
    asn1::rrc_nr::meas_id_to_add_mod_s asn1_meas_id_to_add_mod = meas_id_to_add_mod_to_rrc_asn1(meas_id_to_add_mod);
    asn1_meas_cfg.meas_id_to_add_mod_list.push_back(asn1_meas_id_to_add_mod);
  }

  // s measure cfg
  if (meas_cfg.s_measure_cfg.has_value()) {
    asn1_meas_cfg.s_measure_cfg_present = true;
    if (meas_cfg.s_measure_cfg.value().ssb_rsrp.has_value()) {
      asn1_meas_cfg.s_measure_cfg.set_ssb_rsrp() = meas_cfg.s_measure_cfg.value().ssb_rsrp.value();
    } else if (meas_cfg.s_measure_cfg.value().csi_rsrp.has_value()) {
      asn1_meas_cfg.s_measure_cfg.set_csi_rsrp() = meas_cfg.s_measure_cfg.value().csi_rsrp.value();
    } else {
      // error
      srslog::fetch_basic_logger("RRC").error("Invalid s measure cfg.");
    }
  }

  // quant cfg
  if (meas_cfg.quant_cfg.has_value()) {
    asn1_meas_cfg.quant_cfg_present = true;
    for (const auto& quant_cfg_nr : meas_cfg.quant_cfg.value().quant_cfg_nr_list) {
      asn1::rrc_nr::quant_cfg_nr_s asn1_quant_cfg_nr;

      // quant cfg cell
      asn1_quant_cfg_nr.quant_cfg_cell = quant_cfg_rs_to_rrc_asn1(quant_cfg_nr.quant_cfg_cell);

      // quant cfg rs idx
      if (quant_cfg_nr.quant_cfg_rs_idx.has_value()) {
        asn1_quant_cfg_nr.quant_cfg_rs_idx_present = true;
        asn1_quant_cfg_nr.quant_cfg_rs_idx         = quant_cfg_rs_to_rrc_asn1(quant_cfg_nr.quant_cfg_rs_idx.value());
      }

      asn1_meas_cfg.quant_cfg.quant_cfg_nr_list.push_back(asn1_quant_cfg_nr);
    }
  }

  // meas gap cfg
  if (meas_cfg.meas_gap_cfg.has_value()) {
    asn1_meas_cfg.meas_gap_cfg_present = true;
    if (meas_cfg.meas_gap_cfg.value().gap_fr2.has_value()) {
      asn1_meas_cfg.meas_gap_cfg.gap_fr2_present = true;
      if (meas_cfg.meas_gap_cfg.value().gap_fr2.value().is_release) {
        asn1_meas_cfg.meas_gap_cfg.gap_fr2.set_release();
      } else if (meas_cfg.meas_gap_cfg.value().gap_fr2.value().setup.has_value()) {
        asn1_meas_cfg.meas_gap_cfg.gap_fr2.set_setup();
        // gap offset
        asn1_meas_cfg.meas_gap_cfg.gap_fr2.setup().gap_offset =
            meas_cfg.meas_gap_cfg.value().gap_fr2.value().setup.value().gap_offset;
        // mgl
        asn1::number_to_enum(asn1_meas_cfg.meas_gap_cfg.gap_fr2.setup().mgl,
                             meas_cfg.meas_gap_cfg.value().gap_fr2.value().setup.value().mgl);
        // mgrp
        asn1::number_to_enum(asn1_meas_cfg.meas_gap_cfg.gap_fr2.setup().mgrp,
                             meas_cfg.meas_gap_cfg.value().gap_fr2.value().setup.value().mgrp);
        // mgta
        asn1::number_to_enum(asn1_meas_cfg.meas_gap_cfg.gap_fr2.setup().mgta,
                             meas_cfg.meas_gap_cfg.value().gap_fr2.value().setup.value().mgta);
      } else {
        // error
        srslog::fetch_basic_logger("RRC").error("Invalid gap fr2.");
      }
    }
  }

  // meas gap sharing cfg
  if (meas_cfg.meas_gap_sharing_cfg.has_value()) {
    asn1_meas_cfg.meas_gap_sharing_cfg_present = true;
    if (meas_cfg.meas_gap_sharing_cfg.value().gap_sharing_fr2.has_value()) {
      asn1_meas_cfg.meas_gap_sharing_cfg.gap_sharing_fr2_present = true;
      if (meas_cfg.meas_gap_sharing_cfg.value().gap_sharing_fr2.value().is_release) {
        asn1_meas_cfg.meas_gap_sharing_cfg.gap_sharing_fr2.set_release();
      } else if (!meas_cfg.meas_gap_sharing_cfg.value().gap_sharing_fr2.value().setup.value().empty()) {
        asn1_meas_cfg.meas_gap_sharing_cfg.gap_sharing_fr2.set_setup();
        asn1::string_to_enum(asn1_meas_cfg.meas_gap_sharing_cfg.gap_sharing_fr2.setup(),
                             meas_cfg.meas_gap_sharing_cfg.value().gap_sharing_fr2.value().setup.value());
      } else {
        // error
        srslog::fetch_basic_logger("RRC").error("Invalid gap sharing fr2.");
      }
    }
  }

  return asn1_meas_cfg;
}

} // namespace srs_cu_cp
} // namespace srsran
