/*
 * smbios_types.h - data structure definitions for Loongson PMON SMBIOS support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright (C) IBM Corporation, 2006
 * Authors: Andrew D. Ball <aball@us.ibm.com>
 *
 * Adapated for Loongson PMON
 * (C) Copyright 2012 <meiwenbin@loongson.cn> and <fandongdong@loongson.cn>
 *
 * See the SMBIOS 2.6 spec for more detail:
 *   http://www.dmtf.org/standards/smbios/
 */

#ifndef SMBIOS_TYPES_H
#define SMBIOS_TYPES_H

/* SMBIOS entry point -- must be written to a 16-bit aligned address
   between 0xf0000 and 0xfffff. 
 */
struct smbios_entry_point {
	char anchor_string[4];
	uint8_t checksum;
	uint8_t length;
	uint8_t smbios_major_version;
	uint8_t smbios_minor_version;
	uint16_t max_structure_size;
	uint8_t entry_point_revision;
	uint8_t formatted_area[5];
	char intermediate_anchor_string[5];
	uint8_t intermediate_checksum;
	uint16_t structure_table_length;
	uint32_t structure_table_address;
	uint16_t number_of_structures;
	uint8_t smbios_bcd_revision;
} __attribute__ ((packed));

/* This goes at the beginning of every SMBIOS structure. */
struct smbios_structure_header {
	uint8_t type;
	uint8_t length;
	uint16_t handle;
} __attribute__ ((packed));

/* SMBIOS type 0 - BIOS Information */
struct smbios_type_0 {
	struct smbios_structure_header header;
	uint8_t vendor_str;
	uint8_t version_str;
	uint16_t starting_address_segment;
	uint8_t release_date_str;
	uint8_t rom_size; 
	uint8_t characteristics[8];
	uint8_t characteristics_extension_bytes[2];
	uint8_t major_release;
	uint8_t minor_release;
	uint8_t embedded_controller_major;
	uint8_t embedded_controller_minor;
} __attribute__ ((packed));

/* SMBIOS type 1 - System Information */
struct smbios_type_1 {
	struct smbios_structure_header header;
	uint8_t manufacturer_str;
	uint8_t product_name_str;
	uint8_t version_str;
	uint8_t serial_number_str;
	uint8_t uuid[16];
	uint8_t wake_up_type;
	uint8_t sku_str;
	uint8_t family_str;
} __attribute__ ((packed));

/* SMBIOS type 2 - Board Information */
struct smbios_type_2{
	struct smbios_structure_header header;
	uint8_t manufacturer_str;
	uint8_t product_name_str;
	uint8_t version_str;
	uint8_t serial_number_str;
	uint8_t asset_tag_str;
	uint8_t feature_flags;
	uint8_t location_in_chassis_str;
	uint16_t chassis_handle;
	uint8_t board_type;
	uint8_t number_contained_object_handles;
	uint16_t contained_object_handles;
}__attribute__ ((packed));

/* SMBIOS type 3 - System Enclosure */
struct smbios_type_3 {
	struct smbios_structure_header header;
	uint8_t manufacturer_str;
	uint8_t type;
	uint8_t version_str;
	uint8_t serial_number_str;
	uint8_t asset_tag_str;
	uint8_t boot_up_state;
	uint8_t power_supply_state;
	uint8_t thermal_state;
	uint8_t security_status;
} __attribute__ ((packed));

/* SMBIOS type 4 - Processor Information */
typedef struct cpuid{
  uint8_t  ProcessorSteppingId: 4;
  uint8_t  ProcessorModel:     4;
  uint8_t  ProcessorFamily:    4;
  uint8_t  ProcessorType:      2;
  uint8_t  ProcessorReserved1: 2;
  uint8_t  ProcessorXModel:    4;
  uint8_t  ProcessorXFamily:   8;
  uint8_t  ProcessorReserved2: 4;
  uint32_t  FeatureFlags;
} __attribute__ ((packed));

struct smbios_type_4 {
        struct smbios_structure_header header;
        uint8_t socket_designation_str;
        uint8_t processor_type;
        uint8_t processor_family;
        uint8_t manufacturer_str;
        struct cpuid cpuid;
        uint8_t version_str;
        uint8_t voltage;
        uint16_t external_clock;
        uint16_t max_speed;
        uint16_t current_speed;
        uint8_t status;
        uint8_t upgrade;
        uint16_t l1_cache_handle;
        uint16_t l2_cache_handle;
        uint16_t l3_cache_handle;
        uint8_t  serial_number_str;
        uint8_t  assert_tag_str;
        uint8_t  part_number_str;
        uint8_t core_count;
        uint8_t core_enable;
        uint8_t thread_count;
        uint16_t processor_characteristics;
        uint16_t processor_family2;
} __attribute__ ((packed));

/* SMBIOS type 16 - Physical Memory Array
 *   Associated with one type 17 (Memory Device).
 */
struct smbios_type_16 {
	struct smbios_structure_header header;
	uint8_t location;
	uint8_t use;
	uint8_t error_correction;
	uint32_t maximum_capacity;
	uint16_t memory_error_information_handle;
	uint16_t number_of_memory_devices;
} __attribute__ ((packed));

/* SMBIOS type 17 - Memory Device 
 *   Associated with one type 19
*/
struct smbios_type_17 {
        struct smbios_structure_header header;
        uint16_t physical_memory_array_handle;
        uint16_t memory_error_information_handle;
        uint16_t total_width;
        uint16_t data_width;
        uint16_t size;
        uint8_t form_factor;
        uint8_t device_set;
        uint8_t device_locator_str;
        uint8_t bank_locator_str;
        uint8_t memory_type;
        uint16_t type_detail;
        uint16_t speed;
        uint8_t manufacturer;
        uint8_t serial_number;
        uint8_t asset_tag;
        uint8_t part_number;
        uint8_t attributes;
} __attribute__ ((packed));

 


/* SMBIOS type 19 - Memory Array Mapped Address */
struct smbios_type_19 {
	struct smbios_structure_header header;
	uint32_t starting_address;
	uint32_t ending_address;
	uint16_t memory_array_handle;
	uint8_t partition_width;
} __attribute__ ((packed));

/* SMBIOS type 20 - Memory Device Mapped Address */
struct smbios_type_20 {
	struct smbios_structure_header header;
	uint32_t starting_address;
	uint32_t ending_address;
	uint16_t memory_device_handle;
	uint16_t memory_array_mapped_address_handle;
	uint8_t partition_row_position;
	uint8_t interleave_position;
	uint8_t interleaved_data_depth;
} __attribute__ ((packed));

/* SMBIOS type 28 - Temperature Probe*/
struct smbios_type_28{
	struct smbios_structure_header header;
	uint8_t description;
	uint8_t location_status;
	uint16_t maximum_value;
	uint16_t minimum_value;
	uint16_t resolution;
	uint16_t tolerance;
	uint16_t accuracy;
	uint32_t OEM_defined;
	uint16_t nominal_value;
} __attribute__ ((packed));
/* SMBIOS type 32 - System Boot Information */
struct smbios_type_32 {
	struct smbios_structure_header header;
	uint8_t reserved[6];
	uint8_t boot_status;
} __attribute__ ((packed));

/* SMBIOS type 127 -- End-of-table */
struct smbios_type_127 {
	struct smbios_structure_header header;
} __attribute__ ((packed));
#endif /* SMBIOS_TYPES_H */
