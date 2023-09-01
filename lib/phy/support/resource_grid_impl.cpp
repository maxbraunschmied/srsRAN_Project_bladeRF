/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "resource_grid_impl.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

resource_grid_impl::resource_grid_impl(unsigned                          nof_ports_,
                                       unsigned                          nof_symb_,
                                       unsigned                          nof_subc_,
                                       std::unique_ptr<channel_precoder> precoder_) :
  empty(nof_ports_),
  nof_ports(nof_ports_),
  nof_symb(nof_symb_),
  nof_subc(nof_subc_),
  precoder(std::move(precoder_)),
  layer_mapping_buffer(nof_ports, nof_subc * nof_symb),
  precoding_buffer(nof_ports, nof_subc * nof_symb),
  writer(rg_buffer, empty),
  reader(rg_buffer, empty)
{
  // Reserve memory for the internal buffer.
  rg_buffer.reserve({nof_subc, nof_symb, nof_ports});

  // Set all the resource elements to zero.
  for (unsigned port = 0; port != nof_ports; ++port) {
    srsvec::zero(rg_buffer.get_view<static_cast<unsigned>(resource_grid_dimensions::port)>({port}));
    empty[port] = true;
  }
}

void resource_grid_impl::set_all_zero()
{
  // For each non-empty port, set the underlying resource elements to zero.
  for (unsigned port = 0; port != nof_ports; ++port) {
    if (!empty[port]) {
      srsvec::zero(rg_buffer.get_view<static_cast<unsigned>(resource_grid_dimensions::port)>({port}));
      empty[port] = true;
    }
  }
}

void resource_grid_impl::map(const re_buffer_reader&        input,
                             const re_pattern_list&         pattern,
                             const re_pattern_list&         reserved,
                             const precoding_configuration& precoding)
{
  unsigned nof_layers = precoding.get_nof_layers();

  srsran_assert(input.get_nof_slices() == precoding.get_nof_layers(),
                "The input number of layers (i.e., {}) and the precoding number of layers (i.e., {}) are different.",
                input.get_nof_slices(),
                nof_layers);

  unsigned nof_precoding_ports = precoding.get_nof_ports();
  srsran_assert(nof_precoding_ports <= nof_ports,
                "The precoding number of ports (i.e., {}) exceeds the grid number of ports (i.e., {}).",
                precoding.get_nof_ports(),
                nof_ports);

  // PRG size in number of subcarriers.
  unsigned prg_size = precoding.get_prg_size() * NRE;

  // Counter for the number of RE read from the input and mapped to the grid.
  unsigned i_re_buffer = 0;
  for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    // Get the symbol RE mask.
    bounded_bitset<MAX_RB * NRE> symbol_re_mask(nof_subc);
    pattern.get_inclusion_mask(symbol_re_mask, i_symbol);
    reserved.get_exclusion_mask(symbol_re_mask, i_symbol);

    // Find the highest used subcarrier. Skip symbol if no active subcarrier.
    int i_highest_subc = symbol_re_mask.find_highest();
    if (i_highest_subc < 0) {
      continue;
    }

    // Resize the mask to the highest subcarrier, ceiling to PRB.
    symbol_re_mask.resize(divide_ceil(i_highest_subc + 1, NRE) * NRE);

    // Number of RE to be allocated for the current symbol.
    unsigned nof_re_symbol = symbol_re_mask.count();

    // Bypass precoding if it has no effect on the signal.
    if ((nof_layers == 1) && (nof_precoding_ports == 1) && (precoding.get_nof_prg() == 1) &&
        (precoding.get_coefficient(0, 0, 0) == 1.0F)) {
      // View over the input RE belonging to the current symbol.
      re_buffer_reader_view input_re_symbol(input, i_re_buffer, nof_re_symbol);

      // Map directly to the grid.
      span<const cf_t> unmapped = writer.put(0, i_symbol, 0, symbol_re_mask, input_re_symbol.get_slice(0));
      srsran_assert(unmapped.empty(), "Not all REs have been mapped to the grid.");
      i_re_buffer += nof_re_symbol;
      continue;
    }

    if ((nof_re_symbol != precoding_buffer.get_nof_re()) ||
        (nof_precoding_ports != precoding_buffer.get_nof_slices())) {
      // Resize the output buffer if the input dimensions don't match.
      precoding_buffer.resize(nof_precoding_ports, nof_re_symbol);
    }

    // Counter for the number of precoded REs for the current symbol.
    unsigned i_precoding_buffer = 0;
    for (unsigned i_prg = 0, nof_prg = precoding.get_nof_prg(), i_subc = 0; i_prg != nof_prg; ++i_prg) {
      // Get the precoding matrix for the current PRG.
      const precoding_weight_matrix& prg_weights = precoding.get_prg_coefficients(i_prg);

      // Number of grid RE belonging to the current PRG for the provided allocation pattern dimensions.
      unsigned nof_subc_prg = std::min(prg_size, static_cast<unsigned>(symbol_re_mask.size()) - i_subc);

      // Mask for the RE belonging to the current PRG.
      bounded_bitset<MAX_RB* NRE> prg_re_mask = symbol_re_mask.slice(i_subc, i_subc + nof_subc_prg);

      // Number of allocated RE for the current PRG.
      unsigned nof_re_prg = prg_re_mask.count();

      // Views of the input and precoder buffers for the REs belonging to the current PRG.
      re_buffer_reader_view input_re_prg(input, i_re_buffer, nof_re_prg);
      re_buffer_writer_view output_re_prg(precoding_buffer, i_precoding_buffer, nof_re_prg);

      // Apply precoding.
      precoder->apply_precoding(output_re_prg, input_re_prg, prg_weights);

      // Advance input and output buffers.
      i_re_buffer += nof_re_prg;
      i_precoding_buffer += nof_re_prg;

      // Advance mask slice.
      i_subc += nof_subc_prg;
    }

    // Assert that the precoding buffer has been filled.
    srsran_assert((i_precoding_buffer == precoding_buffer.get_nof_re()),
                  "The number of precoded RE (i.e., {}) does not match the precoding buffer size (i.e., {}).",
                  i_precoding_buffer,
                  precoding_buffer.get_nof_re());

    for (unsigned i_tx_port = 0; i_tx_port != nof_precoding_ports; ++i_tx_port) {
      // Map the precoded REs to each port for the current symbol.
      span<const cf_t> port_data = precoding_buffer.get_slice(i_tx_port);
      span<const cf_t> unmapped  = writer.put(i_tx_port, i_symbol, 0, symbol_re_mask, port_data);
      srsran_assert(unmapped.empty(), "Not all REs have been mapped to the grid.");
    }
  }

  // Assert that all input REs have been processed.
  srsran_assert((i_re_buffer == input.get_nof_re()),
                "The number of total precoded RE (i.e., {}) does not match the number of total input RE (i.e., {}).",
                i_re_buffer,
                input.get_nof_re());
}

resource_grid_writer& resource_grid_impl::get_writer()
{
  return writer;
}

const resource_grid_reader& resource_grid_impl::get_reader() const
{
  return reader;
}

void resource_grid_impl::map(const re_buffer_reader&        input,
                             const re_pattern_list&         pattern,
                             const precoding_configuration& precoding)
{
  // Map with an empty list of reserved RE patterns.
  map(input, pattern, re_pattern_list(), precoding);
}

void resource_grid_impl::map(symbol_buffer&                 buffer,
                             const re_pattern_list&         pattern,
                             const re_pattern_list&         reserved,
                             const precoding_configuration& precoding)
{
  unsigned max_block_size = buffer.get_max_block_size();

  // The number of layers is equal to the number of ports.
  unsigned nof_layers = precoding.get_nof_layers();

  // Extract number of antennas.
  unsigned nof_antennas = precoding.get_nof_ports();

  // Verify that the number of layers is consistent with the number of ports.
  interval<unsigned, true> nof_antennas_range(1, nof_ports);
  srsran_assert(nof_antennas_range.contains(nof_antennas),
                "The number of antennas (i.e., {}) must be in range {}",
                nof_antennas,
                nof_antennas_range);
  interval<unsigned, true> nof_layers_range(1, nof_antennas);
  srsran_assert(nof_layers_range.contains(nof_layers),
                "The number of layers (i.e., {}) must be in range {}",
                nof_layers,
                nof_layers_range);

  for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    // Get the symbol RE mask.
    bounded_bitset<MAX_RB * NRE> symbol_re_mask(MAX_RB * NRE);
    pattern.get_inclusion_mask(symbol_re_mask, i_symbol);
    reserved.get_exclusion_mask(symbol_re_mask, i_symbol);

    // Find the highest used subcarrier. Skip symbol if no active subcarrier.
    int i_highest_subc = symbol_re_mask.find_highest();
    if (i_highest_subc < 0) {
      continue;
    }

    // Iterate all precoding PRGs.
    unsigned prg_size = precoding.get_prg_size() * NRE;
    for (unsigned i_prg = 0, nof_prg = precoding.get_nof_prg(); i_prg != nof_prg; ++i_prg) {
      // Get the precoding matrix for the current PRG.
      const precoding_weight_matrix& prg_weights = precoding.get_prg_coefficients(i_prg);

      // Get the subcarrier interval for the PRG.
      unsigned i_subc = i_prg * prg_size;

      // Number of grid RE belonging to the current PRG for the provided allocation pattern dimensions.
      unsigned nof_subc_prg = std::min(prg_size, static_cast<unsigned>(i_highest_subc + 1) - i_subc);

      // Mask for the RE belonging to the current PRG.
      bounded_bitset<MAX_RB* NRE> prg_re_mask = symbol_re_mask.slice(i_subc, i_subc + nof_subc_prg);

      // Skip PRG if no RE is selected.
      if (prg_re_mask.none()) {
        continue;
      }

      // Process PRG in blocks smaller than or equal to max_block_size subcarriers.
      unsigned subc_offset = prg_re_mask.find_lowest();
      while (subc_offset != nof_subc_prg) {
        // Calculate the maximum number of subcarriers that can be processed in one block.
        unsigned max_nof_subc_block = max_block_size / nof_layers;

        // Calculate the number of pending subcarriers to process.
        unsigned nof_subc_pending = nof_subc_prg - subc_offset;
        srsran_assert(nof_subc_pending != 0, "The number of pending subcarriers cannot be zero.");

        // Select the number of subcarriers to process in a block.
        unsigned nof_subc_block = std::min(nof_subc_pending, max_nof_subc_block);

        // Get the allocation mask for the block.
        bounded_bitset<MAX_RB* NRE> block_mask = prg_re_mask.slice(subc_offset, subc_offset + nof_subc_block);

        // Count the number of resource elements to map in the block.
        unsigned nof_re_block = block_mask.count();

        // Calculate the number of symbols to generate.
        unsigned nof_symbols_block = nof_re_block * nof_layers;

        // Prepare destination of the modulation buffer.
        span<const cf_t> block = buffer.pop_symbols(nof_symbols_block);

        // Skip layer mapping and precoding for one layer.
        if ((nof_layers == 1) && (nof_antennas == 1)) {
          // Prepare a temporary buffer for precoding.
          precoding_buffer.resize(nof_antennas, nof_re_block);
          span<cf_t> temp_precoding = precoding_buffer.get_slice(0);

          // Apply only one coefficient.
          srsvec::sc_prod(block, prg_weights.get_port_coefficients(0)[0], temp_precoding);

          writer.put(0, i_symbol, i_subc + subc_offset, block_mask, temp_precoding);
        } else {
          // Prepare buffers.
          layer_mapping_buffer.resize(nof_layers, nof_re_block);
          precoding_buffer.resize(nof_antennas, nof_re_block);

          // Layer map.
          for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
            span<cf_t> layer_data = layer_mapping_buffer.get_slice(i_layer);
            for (unsigned i_re = 0; i_re != nof_re_block; ++i_re) {
              layer_data[i_re] = block[i_re * nof_layers + i_layer];
            }
          }

          // Apply precoding and map for each port.
          for (unsigned i_port = 0; i_port != nof_layers; ++i_port) {
            precoder->apply_precoding_port(
                precoding_buffer.get_slice(0), layer_mapping_buffer, prg_weights.get_port_coefficients(i_port));

            writer.put(i_port, i_symbol, i_subc + subc_offset, block_mask, precoding_buffer.get_slice(0));
          }
        }

        // Increment the subcarrier offset.
        subc_offset += nof_subc_block;
      }
    }
  }
}
