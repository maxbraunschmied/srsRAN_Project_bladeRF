/*
 *
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/f1u/cu_up/split_connector/f1u_split_connector.h"
#include "srsran/gtpu/gtpu_tunnel_nru_factory.h"
#include "srsran/ran/lcid.h"

using namespace srsran;
using namespace srs_cu_up;

std::unique_ptr<f1u_cu_up_gateway_bearer>
f1u_split_connector::create_cu_bearer(uint32_t                              ue_index,
                                      drb_id_t                              drb_id,
                                      const srs_cu_up::f1u_config&          config,
                                      const up_transport_layer_info&        ul_up_tnl_info,
                                      f1u_cu_up_gateway_bearer_rx_notifier& rx_notifier,
                                      task_executor&                        ul_exec,
                                      timer_factory                         ue_dl_timer_factory,
                                      unique_timer&                         ue_inactivity_timer)
{
  logger_cu.info("Creating CU gateway local bearer with UL GTP Tunnel={}", ul_up_tnl_info);
  std::unique_lock<std::mutex> lock(map_mutex);
  srsran_assert(cu_map.find(ul_up_tnl_info) == cu_map.end(),
                "Cannot create CU gateway local bearer with already existing UL GTP Tunnel={}",
                ul_up_tnl_info);
  auto cu_bearer = std::make_unique<f1u_split_gateway_cu_bearer>(
      ue_index, drb_id, ul_up_tnl_info, rx_notifier, *udp_session, ul_exec, *this);
  cu_map.insert({ul_up_tnl_info, cu_bearer.get()});
  return cu_bearer;
}

void f1u_split_connector::attach_dl_teid(const up_transport_layer_info& ul_up_tnl_info,
                                         const up_transport_layer_info& dl_up_tnl_info)
{
  std::unique_lock<std::mutex> lock(map_mutex);
  if (cu_map.find(ul_up_tnl_info) == cu_map.end()) {
    logger_cu.warning("Could not find UL GTP Tunnel at CU-CP to connect. UL GTP Tunnel={}, DL GTP Tunnel={}",
                      ul_up_tnl_info,
                      dl_up_tnl_info);
    return;
  }
  f1u_split_gateway_cu_bearer* cu_bearer = cu_map.at(ul_up_tnl_info);
  logger_cu.debug("Connecting CU F1-U bearer. UL GTP Tunnel={}, DL GTP Tunnel={}", ul_up_tnl_info, dl_up_tnl_info);

  // create GTP-U tunnel
  // TODO create RX earlier
  gtpu_tunnel_nru_creation_message msg{};
  // msg.ue_index                            = 0; TODO
  msg.cfg.rx.local_teid                   = ul_up_tnl_info.gtp_teid;
  msg.cfg.tx.peer_teid                    = dl_up_tnl_info.gtp_teid;
  msg.cfg.tx.peer_addr                    = dl_up_tnl_info.tp_address.to_string();
  msg.cfg.tx.peer_port                    = GTPU_PORT;
  msg.gtpu_pcap                           = &gtpu_pcap;
  msg.tx_upper                            = &cu_bearer->gtpu_network_adapter;
  std::unique_ptr<gtpu_tunnel_nru> tunnel = srsran::create_gtpu_tunnel_nru(msg);

  // attach it to the F1-U bearer
  cu_bearer->attach_tunnel(std::move(tunnel));

  // attach RX to DEMUX
}

void f1u_split_connector::disconnect_cu_bearer(const up_transport_layer_info& ul_up_tnl_info)
{
  std::unique_lock<std::mutex> lock(map_mutex);
  // TODO
  // Remove DL path
  // cu_map.erase(bearer_it);
  logger_cu.debug("Removed CU F1-U bearer with UL GTP Tunnel={}.", ul_up_tnl_info);
}
